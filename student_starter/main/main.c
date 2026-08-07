#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <ctype.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "esp_log.h"
#include "esp_timer.h"
#include "esp_mac.h"
#include "esp_system.h"
#include "driver/usb_serial_jtag.h"
#include "driver/usb_serial_jtag_vfs.h"

#define INPUT_LINE_MAX 160
#define LAB_MAGIC 0xCE40876AUL
#define STRESS_BUCKETS 4U
#define STRESS_BUCKET_WIDTH 20U

#define MODE_NORMAL  0
#define MODE_SERVICE 1

#define HEALTH_FLAG_COOKIE_BAD       (1UL << 0)
#define HEALTH_FLAG_SAFETY_BAD       (1UL << 1)
#define HEALTH_FLAG_HEARTBEAT_STALE  (1UL << 2)
#define HEALTH_FLAG_UNEXPECTED_MODE  (1UL << 3)
#define HEALTH_FLAG_BUCKET_GUARD_BAD (1UL << 4)

#define STRESS_GUARD_MAGIC       0x5A17C0DEUL
#define BUCKET_GUARD_MAGIC       0xB0C4E7A1UL

static const char *TAG = "E3_JTAG";

volatile uint32_t g_safety_cookie = LAB_MAGIC;
volatile int32_t g_device_mode = MODE_NORMAL;
volatile int32_t g_safety_level = 3;
volatile uint32_t g_event_counter = 0;
volatile uint32_t g_health_flags = 0;
volatile uint32_t g_heartbeat = 0;
volatile uint32_t g_cli_command_count = 0;
volatile uint32_t g_last_health_alert_us = 0;
volatile uint64_t g_last_heartbeat_us = 0;

static char g_device_id[13] = {0};

typedef struct {
    volatile uint32_t buckets[STRESS_BUCKETS];
    volatile uint32_t bucket_guard;   /* NEW: sacrificial canary, sits between buckets[] and sensitive fields */
    volatile int32_t pending_mode;
    volatile uint32_t guard;
} stress_state_t;

volatile stress_state_t g_stress_state = {
    .buckets = {0, 0, 0, 0},
    .bucket_guard = BUCKET_GUARD_MAGIC,
    .pending_mode = 0,
    .guard = STRESS_GUARD_MAGIC,
};

typedef enum {
    SENSOR_REQUEST_SAMPLE = 1,
    SENSOR_REQUEST_STRESS = 2,
} sensor_request_kind_t;

typedef struct {
    sensor_request_kind_t kind;
    uint32_t value;
} sensor_request_t;

static QueueHandle_t s_sensor_queue = NULL;

static void make_device_id(void)
{
    uint8_t mac[6] = {0};
    ESP_ERROR_CHECK(esp_efuse_mac_get_default(mac));
    snprintf(g_device_id, sizeof(g_device_id), "%02x%02x%02x%02x%02x%02x",
             mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
}

static const char *mode_name(int32_t mode)
{
    switch (mode) {
    case MODE_NORMAL:
        return "normal";
    case MODE_SERVICE:
        return "service";
    default:
        return "unknown";
    }
}

static void print_help(void)
{
    printf("\nCommands:\n");
    printf("  help\n");
    printf("  status\n");
    printf("  sample\n");
    printf("  setmode normal\n");
    printf("  setmode service\n");
    printf("  stress <n>\n");
    printf("  log\n");
    printf("  heartbeat\n");
    printf("  reset\n\n");
}

static void reset_state(void)
{
    g_safety_cookie = LAB_MAGIC;
    g_device_mode = MODE_NORMAL;
    g_safety_level = 3;
    g_event_counter = 0;
    g_health_flags = 0;
    g_heartbeat = 0;
    g_cli_command_count = 0;
    g_last_health_alert_us = 0;
    g_last_heartbeat_us = (uint64_t)esp_timer_get_time();

    for (size_t i = 0; i < STRESS_BUCKETS; ++i) {
        g_stress_state.buckets[i] = 0;
    }
    g_stress_state.bucket_guard = BUCKET_GUARD_MAGIC;
    g_stress_state.pending_mode = 0;
    g_stress_state.guard = STRESS_GUARD_MAGIC;
}

static void print_status(void)
{
    printf("device_id=%s\n", g_device_id);
    printf("mode=%s\n", mode_name(g_device_mode));
    printf("safety_level=%ld\n", (long)g_safety_level);
    printf("events=%lu\n", (unsigned long)g_event_counter);
    printf("health_flags=0x%08lx\n", (unsigned long)g_health_flags);
    printf("heartbeat=%lu\n", (unsigned long)g_heartbeat);

    if (g_safety_cookie == LAB_MAGIC &&
        g_safety_level >= 0 && g_safety_level <= 5 &&
        g_health_flags == 0) {
        printf("health=ok\n");
    } else {
        printf("health=alert\n");
    }
}

static void print_log(void)
{
    printf("cookie=0x%08lx mode=%ld safety=%ld events=%lu flags=0x%08lx heartbeat=%lu\n",
           (unsigned long)g_safety_cookie,
           (long)g_device_mode,
           (long)g_safety_level,
           (unsigned long)g_event_counter,
           (unsigned long)g_health_flags,
           (unsigned long)g_heartbeat);

    printf("stress buckets=[%lu,%lu,%lu,%lu] bucket_guard=0x%08lx pending_mode=%ld guard=0x%08lx\n",
           (unsigned long)g_stress_state.buckets[0],
           (unsigned long)g_stress_state.buckets[1],
           (unsigned long)g_stress_state.buckets[2],
           (unsigned long)g_stress_state.buckets[3],
           (unsigned long)g_stress_state.bucket_guard,
           (long)g_stress_state.pending_mode,
           (unsigned long)g_stress_state.guard);
}

static bool parse_u32(const char *text, uint32_t *value)
{
    if (text == NULL || value == NULL) {
        return false;
    }

    char *end = NULL;
    unsigned long parsed = strtoul(text, &end, 10);
    if (end == text) {
        return false;
    }

    while (*end != '\0' && isspace((unsigned char)*end)) {
        ++end;
    }

    if (*end != '\0' || parsed > 200UL) {
        return false;
    }

    *value = (uint32_t)parsed;
    return true;
}

static bool queue_sensor_request(sensor_request_kind_t kind, uint32_t value)
{
    sensor_request_t request = {
        .kind = kind,
        .value = value,
    };

    return xQueueSend(s_sensor_queue, &request, pdMS_TO_TICKS(250)) == pdTRUE;
}

static bool handle_setmode(const char *argument)
{
    if (argument == NULL || *argument == '\0') {
        printf("ERR missing_mode\n");
        return false;
    }

    if (strcmp(argument, "normal") == 0) {
        g_device_mode = MODE_NORMAL;
        g_safety_level = 3;
        printf("OK mode_set normal\n");
        return true;
    }

    if (strcmp(argument, "service") == 0) {
        g_device_mode = MODE_SERVICE;
        g_safety_level = 1;
        printf("OK mode_set service\n");
        return true;
    }

    printf("ERR invalid_mode\n");
    return false;
}

static char *skip_spaces(char *text)
{
    while (*text != '\0' && isspace((unsigned char)*text)) {
        ++text;
    }
    return text;
}

static void trim_right(char *text)
{
    size_t length = strlen(text);
    while (length > 0 && isspace((unsigned char)text[length - 1])) {
        text[length - 1] = '\0';
        --length;
    }
}

static bool read_line_echo(char *buffer, size_t buffer_size)
{
    size_t index = 0;

    while (true) {
        int c = getchar();

        if (c == EOF) {
            vTaskDelay(pdMS_TO_TICKS(20));
            continue;
        }

        if (c == '\r' || c == '\n') {
            putchar('\n');
            buffer[index] = '\0';
            return true;
        }

        if (c == '\b' || c == 0x7F) {  // backspace / DEL
            if (index > 0) {
                --index;
                printf("\b \b");  // erase char on terminal
                fflush(stdout);
            }
            continue;
        }

        if (index < buffer_size - 1) {
            buffer[index++] = (char)c;
            putchar(c);       // echo
            fflush(stdout);
        }
    }
}


static void handle_command(char *line)
{
    trim_right(line);
    char *command = skip_spaces(line);

    if (*command == '\0') {
        return;
    }

    ++g_cli_command_count;

    if (strcmp(command, "help") == 0) {
        print_help();
        return;
    }

    if (strcmp(command, "status") == 0) {
        print_status();
        return;
    }

    if (strcmp(command, "sample") == 0) {
        if (queue_sensor_request(SENSOR_REQUEST_SAMPLE, 1)) {
            printf("OK sample_queued\n");
        } else {
            printf("ERR sensor_queue_full\n");
        }
        return;
    }

    if (strcmp(command, "log") == 0) {
        print_log();
        return;
    }

    if (strcmp(command, "heartbeat") == 0) {
        printf("heartbeat=%lu last_heartbeat_us=%llu\n",
               (unsigned long)g_heartbeat,
               (unsigned long long)g_last_heartbeat_us);
        return;
    }

    if (strcmp(command, "reset") == 0) {
        reset_state();
        printf("OK reset\n");
        return;
    }

    if (strncmp(command, "setmode", 7) == 0 &&
        isspace((unsigned char)command[7])) {
        char *argument = skip_spaces(command + 7);
        (void)handle_setmode(argument);
        return;
    }

    if (strncmp(command, "stress", 6) == 0 &&
        isspace((unsigned char)command[6])) {
        char *argument = skip_spaces(command + 6);
        uint32_t count = 0;

        if (!parse_u32(argument, &count)) {
            printf("ERR invalid_stress\n");
            return;
        }

        if (queue_sensor_request(SENSOR_REQUEST_STRESS, count)) {
            printf("OK stress_queued count=%lu\n", (unsigned long)count);
        } else {
            printf("ERR sensor_queue_full\n");
        }
        return;
    }

    printf("%s ", command);
    printf("ERR unknown_command\n");
}

void uart_task(void *argument)
{
    (void)argument;
    char line[INPUT_LINE_MAX];

    printf("\n=== CE-40876 E3 JTAG Runtime Forensics ===\n");
    printf("device_id=%s\n", g_device_id);
    printf("Type 'help'.\n\n");
    printf("> ");
    fflush(stdout);

    while (true) {
        if (!read_line_echo(line, sizeof(line))) {
            continue;
        }

        handle_command(line);
        printf("> ");
        fflush(stdout);
    }
}

void sensor_task(void *argument)
{
    (void)argument;
    sensor_request_t request;

    while (true) {
        if (xQueueReceive(s_sensor_queue, &request, portMAX_DELAY) != pdTRUE) {
            continue;
        }

        if (request.kind == SENSOR_REQUEST_SAMPLE) {
            ++g_event_counter;
            printf("\nSENSOR sample event=%lu\n", (unsigned long)g_event_counter);
            continue;
        }

        if (request.kind == SENSOR_REQUEST_STRESS) {
            volatile uint32_t *legacy_buckets = g_stress_state.buckets;

            for (uint32_t i = 0; i < request.value; ++i) {
                uint32_t bucket_index = i / STRESS_BUCKET_WIDTH;

                if (bucket_index >= STRESS_BUCKETS) {
                    bucket_index = STRESS_BUCKETS - 1U;
                }
                legacy_buckets[bucket_index]++;
                ++g_event_counter;

                if ((i % 17U) == 0U) {
                    vTaskDelay(pdMS_TO_TICKS(1));
                }
            }

            printf("\nSENSOR stress_complete count=%lu events=%lu\n",
                   (unsigned long)request.value,
                   (unsigned long)g_event_counter);
        }
    }
}

void control_task(void *argument)
{
    (void)argument;

    while (true) {
        if (g_stress_state.pending_mode != 0) {
            g_device_mode = MODE_SERVICE;
            g_safety_level = 1;
            g_health_flags |= HEALTH_FLAG_UNEXPECTED_MODE;
            g_stress_state.pending_mode = 0;
        }

        vTaskDelay(pdMS_TO_TICKS(10));
    }
}

void heartbeat_task(void *argument)
{
    (void)argument;

    while (true) {
        ++g_heartbeat;
        g_last_heartbeat_us = (uint64_t)esp_timer_get_time();
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

void health_monitor_task(void *argument)
{
    (void)argument;

    while (true) {
        vTaskDelay(pdMS_TO_TICKS(2000));

        uint32_t current_flags = g_health_flags & HEALTH_FLAG_UNEXPECTED_MODE;
        const uint64_t now_us = (uint64_t)esp_timer_get_time();

        if (g_safety_cookie != LAB_MAGIC) {
            current_flags |= HEALTH_FLAG_COOKIE_BAD;
        }

        if (g_safety_level < 0 || g_safety_level > 5) {
            current_flags |= HEALTH_FLAG_SAFETY_BAD;
        }

        if (now_us > g_last_heartbeat_us &&
            (now_us - g_last_heartbeat_us) > 3000000ULL) {
            current_flags |= HEALTH_FLAG_HEARTBEAT_STALE;
        }

        /* NEW: detect any adjacent-memory corruption near stress_state buckets */
        if (g_stress_state.bucket_guard != BUCKET_GUARD_MAGIC ||
            g_stress_state.guard != STRESS_GUARD_MAGIC) {
            current_flags |= HEALTH_FLAG_BUCKET_GUARD_BAD;
            }

        g_health_flags = current_flags;

        if (current_flags != 0) {
            g_last_health_alert_us = (uint32_t)now_us;
            printf("\nALERT health flags=0x%08lx\n",
                   (unsigned long)current_flags);
        }
    }
}

void app_main(void)
{
    esp_log_level_set("*", ESP_LOG_WARN);
    setvbuf(stdin, NULL, _IONBF, 0);
    setvbuf(stdout, NULL, _IONBF, 0);

    usb_serial_jtag_driver_config_t usj_cfg = USB_SERIAL_JTAG_DRIVER_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(usb_serial_jtag_driver_install(&usj_cfg));

    usb_serial_jtag_vfs_use_driver();
    usb_serial_jtag_vfs_set_rx_line_endings(ESP_LINE_ENDINGS_CR);
    usb_serial_jtag_vfs_set_tx_line_endings(ESP_LINE_ENDINGS_CRLF);

    make_device_id();
    reset_state();

    s_sensor_queue = xQueueCreate(8, sizeof(sensor_request_t));
    if (s_sensor_queue == NULL) {
        ESP_LOGE(TAG, "failed to create sensor queue");
        abort();
    }

    xTaskCreate(uart_task, "uart_task", 4096, NULL, 5, NULL);
    xTaskCreate(sensor_task, "sensor_task", 3072, NULL, 4, NULL);
    xTaskCreate(control_task, "control_task", 2048, NULL, 4, NULL);
    xTaskCreate(heartbeat_task, "heartbeat_task", 2048, NULL, 3, NULL);
    xTaskCreate(health_monitor_task, "health_monitor_task", 3072, NULL, 6, NULL);
}
