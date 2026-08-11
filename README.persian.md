# تمرین سوم — تحلیل پویا و کشف خطای حافظه با JTAG (CE-40876 / E3)

**دانشجو:** دانیال عرفانیان

**شماره دانشجویی:** 402211745

**درس:** امنیت سیستم‌های نهفته و اینترنت اشیا — دانشگاه صنعتی شریف

---

## اطلاعات محیط اجرا

| مورد            | مقدار                                                           |
|-----------------|-----------------------------------------------------------------|
| مدل برد و تراشه | ESP32-S3 (QFN56) (revision v0.2)                                |
| روش اتصال JTAG  | USB داخلی                                                       |
| سیستم‌عامل      | Ubuntu 24.04 LTS                                                |
| نسخه‌ی ESP-IDF  | ESP-IDF v6.0.1-dirty                                            |
| GDB             | GNU gdb (esp-gdb) 16.3_20250913                                 |
| OpenOCD         | Open On-Chip Debugger v0.12.0-esp32-20260304 (2026-03-04-20:07) | 
| پورت سریال      | /dev/ttyACM0                                                    |

---

## مرحله‌ی ۱: راه‌اندازی اولیه و ثبت شواهد

### ساخت و بارگذاری

```
cd student_starter
idf.py set-target esp32s3
idf.py build
idf.py -p /dev/ttyACM0 flash monitor
```

### خروجی help و status (بعد از راه‌اندازی)

```
=== CE-40876 E3 JTAG Runtime Forensics ===
device_id=1cdbd49955e8
Type 'help'.

> help

Commands:
  help
  status
  sample
  setmode normal
  setmode service
  stress <n>
  log
  heartbeat
  reset

> status
device_id=1cdbd49955e8
mode=normal
safety_level=3
events=0
health_flags=0x00000000
heartbeat=214
health=ok
> 
```

### اتصال OpenOCD

```
HW3/student_starter ❯ openocd -f board/esp32s3-builtin.cfg
Open On-Chip Debugger v0.12.0-esp32-20260304 (2026-03-04-20:07)
Licensed under GNU GPL v2
For bug reports, read
        http://openocd.org/doc/doxygen/bugs.html
Info : esp_usb_jtag: VID set to 0x303a and PID to 0x1001
Info : esp_usb_jtag: capabilities descriptor set to 0x2000
Info : Listening on port 6666 for tcl connections
Info : Listening on port 4444 for telnet connections
Info : esp_usb_jtag: serial (1C:DB:D4:99:55:E8)
Info : esp_usb_jtag: Device found. Base speed 40000KHz, div range 1 to 255
Info : clock speed 40000 kHz
Info : JTAG tap: esp32s3.tap0 tap/device found: 0x120034e5 (mfg: 0x272 (Tensilica), part: 0x2003, ver: 0x1)
Info : JTAG tap: esp32s3.tap1 tap/device found: 0x120034e5 (mfg: 0x272 (Tensilica), part: 0x2003, ver: 0x1)
Info : [esp32s3.cpu0] Examination succeed
Info : [esp32s3.cpu1] Examination succeed
Info : [esp32s3.cpu0] starting gdb server on 3333
Info : Listening on port 3333 for gdb connections
Info : [esp32s3.cpu0] Debug controller was reset.
Info : [esp32s3.cpu0] Core was reset.
Info : [esp32s3.cpu1] Debug controller was reset.
Info : [esp32s3.cpu1] Core was reset.
Info : [esp32s3.cpu0] Target halted, PC=0x40378C4A, debug_reason=00000000
Info : [esp32s3.cpu0] Reset cause (21) - (USB UART reset)
Info : [esp32s3.cpu1] Target halted, PC=0x40378C4A, debug_reason=00000000
Info : [esp32s3.cpu1] Reset cause (21) - (USB UART reset)

Info : accepting 'gdb' connection on tcp/3333
Info : [esp32s3.cpu0] Target halted, PC=0x40378C4A, debug_reason=00000000
Info : Set GDB target to 'esp32s3.cpu0'
Info : [esp32s3.cpu1] Target halted, PC=0x40378C4A, debug_reason=00000000
Memory protection is enabled. Reset target to disable it...
Info : JTAG tap: esp32s3.tap0 tap/device found: 0x120034e5 (mfg: 0x272 (Tensilica), part: 0x2003, ver: 0x1)
Info : JTAG tap: esp32s3.tap1 tap/device found: 0x120034e5 (mfg: 0x272 (Tensilica), part: 0x2003, ver: 0x1)
Info : [esp32s3.cpu0] requesting target halt and executing a soft reset
Info : [esp32s3.cpu0] Debug controller was reset.
Info : [esp32s3.cpu0] Core was reset.
Info : [esp32s3.cpu0] Target halted, PC=0x500000EF, debug_reason=00000000
Info : [esp32s3.cpu0] Reset cause (3) - (Software core reset)
Info : [esp32s3.cpu0] Core was reset.
Info : [esp32s3.cpu0] Target halted, PC=0x40000400, debug_reason=00000000
Info : [esp32s3.cpu1] Debug controller was reset.
Info : [esp32s3.cpu1] Core was reset.
Info : [esp32s3.cpu1] Target halted, PC=0x40000400, debug_reason=00000000
Info : [esp32s3.cpu1] Reset cause (3) - (Software core reset)
Info : [esp32s3.cpu0] Reset cause (3) - (Software core reset)
Warn : No symbols for FreeRTOS!
Info : [esp32s3.cpu0] Target halted, PC=0x4038CCA2, debug_reason=00000001
Info : Flash mapping 0: 0x10020 -> 0x3c020020, 40 KB
Info : Flash mapping 1: 0x20020 -> 0x42000020, 70 KB
Info : Auto-detected flash bank 'esp32s3.cpu0.flash' size 16384 KB
Info : Using flash bank 'esp32s3.cpu0.flash' size 16384 KB
Info : [esp32s3.cpu0] Target halted, PC=0x4038CCA2, debug_reason=00000001
Info : Flash mapping 0: 0x10020 -> 0x3c020020, 40 KB
Info : Flash mapping 1: 0x20020 -> 0x42000020, 70 KB
Info : Using flash bank 'esp32s3.cpu0.irom' size 72 KB
Info : [esp32s3.cpu0] Target halted, PC=0x4038CCA2, debug_reason=00000001
Info : Flash mapping 0: 0x10020 -> 0x3c020020, 40 KB
Info : Flash mapping 1: 0x20020 -> 0x42000020, 70 KB
Info : Using flash bank 'esp32s3.cpu0.drom' size 44 KB
Info : New GDB Connection: 1, Target esp32s3.cpu0, state: halted
Warn : Prefer GDB command "target extended-remote :3333" instead of "target remote :3333"
Info : Detected FreeRTOS version: (10.5.1)
```

### اتصال GDB

در تلاش اولیه هیچ اثری از اجرا در gdb مشاهده نمی‌شد. زیرا برای اتصال این قسمت باید در GDB این دستور را اجرا کنیم:

```shell
target remote :3333
```

با اجرای این کار همانطور که در لاگ بالا مشاهده می‌شود، OpenOCD متوجه حضور gdb ما می‌شود و ارتباط این دو باهم امکان
خواندن register-ها یا استک زنده در بورد را می‌دهد.

```text
Info : accepting 'gdb' connection on tcp/3333
```

### info registers

```
(gdb) info registers
pc             0x40000400          0x40000400
lbeg           0x0                 0
lend           0x0                 0
lcount         0x0                 0
sar            0x0                 0
ps             0x60225             393765
threadptr      0x0                 0
br             0x0                 0
scompare1      0x0                 0
acclo          0x0                 0
acchi          0x0                 0
m0             0x0                 0
m1             0x0                 0
m2             0x0                 0
m3             0x0                 0
fcr            0x0                 0
fsr            0x0                 0
pwrctl         0x0                 0
pwrstat        0x0                 0
eristat        0x0                 0
cs_itctrl      0x0                 0
cs_claimset    0x0                 0
cs_claimclr    0x0                 0
cs_lockaccess  0x0                 0
cs_lockstatus  0x0                 0
cs_authstatus  0x0                 0
fault_info     0x0                 0
trax_id        0x0                 0
trax_control   0x0                 0
trax_status    0x0                 0
trax_data      0x0                 0
trax_address   0x0                 0
trax_pctrigger 0x0                 0
trax_pcmatch   0x0                 0
trax_delay     0x0                 0
trax_memstart  0x0                 0
trax_memend    0x0                 0
pmg            0x0                 0
pmpc           0x0                 0
pm0            0x0                 0
pm1            0x0                 0
pmctrl0        0x0                 0
pmctrl1        0x0                 0
pmstat0        0x0                 0
pmstat1        0x0                 0
ocdid          0x0                 0
ocd_dcrclr     0x0                 0
ocd_dcrset     0x0                 0
ocd_dsr        0x0                 0
a0             0x0                 0
a1             0x0                 0
a2             0x0                 0
a3             0x0                 0
a4             0x0                 0
a5             0x0                 0
a6             0x0                 0
a7             0x0                 0
a8             0x0                 0
a9             0x0                 0
a10            0x0                 0
a11            0x0                 0
a12            0x0                 0
a13            0x0                 0
a14            0x0                 0
a15            0x0                 0
```

### bt (نمونه ردیابی پشته)

```
(gdb) bt
#0  0x40378c4a in esp_cpu_wait_for_intr () at /home/divar/.espressif/v6.0.1/esp-idf/components/esp_hw_support/cpu.c:64
#1  0x4200eefd in esp_vApplicationIdleHook () at /home/divar/.espressif/v6.0.1/esp-idf/components/esp_system/freertos_hooks.c:58
#2  0x4200ff0d in prvIdleTask (pvParameters=0x0) at /home/divar/.espressif/v6.0.1/esp-idf/components/freertos/FreeRTOS-Kernel/tasks.c:4350
#3  0x4200f479 in vPortTaskWrapper (pxCode=0x4200ff04 <prvIdleTask>, pvParameters=0x0) at /home/divar/.espressif/v6.0.1/esp-idf/components/freertos/FreeRTOS-Kernel/portable/xtensa/port.c:143
(gdb) info threads
  Id   Target Id                                                     Frame 
* 1    Thread 1070167604 "IDLE0" (Name: IDLE0, State: Running @CPU0) 0x40378c4a in esp_cpu_wait_for_intr () at /home/divar/.espressif/v6.0.1/esp-idf/components/esp_hw_support/cpu.c:64
  2    Thread 1070169312 "IDLE1" (Name: IDLE1, State: Running @CPU1) 0x40378c4a in esp_cpu_wait_for_intr () at /home/divar/.espressif/v6.0.1/esp-idf/components/esp_hw_support/cpu.c:64
  3    Thread 1070517516 "heartbeat_task" (Name: heartbeat_task)     vTaskDelay (xTicksToDelay=100) at /home/divar/.espressif/v6.0.1/esp-idf/components/freertos/FreeRTOS-Kernel/tasks.c:1615
  4    Thread 1070520696 "health_monitor_" (Name: health_monitor_)   health_monitor_task (argument=0x0) at /home/divar/Desktop/Uni/1.ESP/HW3/student_starter/main/main.c:444
  5    Thread 1070515360 "control_task" (Name: control_task)         vTaskDelay (xTicksToDelay=1) at /home/divar/.espressif/v6.0.1/esp-idf/components/freertos/FreeRTOS-Kernel/tasks.c:1615
  6    Thread 1070510024 "uart_task" (Name: uart_task)               0x400559e0 in ?? ()
  7    Thread 1070513204 "sensor_task" (Name: sensor_task)           0x400559e0 in ?? ()
  8    Thread 1070159932 "ipc1" (Name: ipc1)                         0x400559e0 in ?? ()
  9    Thread 1070158544 "ipc0" (Name: ipc0)                         0x400559e0 in ?? ()
(gdb) thread 4
[Switching to thread 4 (Thread 1070520696)]
#0  health_monitor_task (argument=0x0) at /home/divar/Desktop/Uni/1.ESP/HW3/student_starter/main/main.c:444
444             uint32_t current_flags = g_health_flags & HEALTH_FLAG_UNEXPECTED_MODE;
(gdb) bt
#0  health_monitor_task (argument=0x0) at /home/divar/Desktop/Uni/1.ESP/HW3/student_starter/main/main.c:444
#1  0x4200f479 in vPortTaskWrapper (pxCode=0x42009088 <health_monitor_task>, pvParameters=0x0) at /home/divar/.espressif/v6.0.1/esp-idf/components/freertos/FreeRTOS-Kernel/portable/xtensa/port.c:143
(gdb) thread 4
[Switching to thread 4 (Thread 1070520696)]
#0  health_monitor_task (argument=0x0) at /home/divar/Desktop/Uni/1.ESP/HW3/student_starter/main/main.c:444
444             uint32_t current_flags = g_health_flags & HEALTH_FLAG_UNEXPECTED_MODE;
(gdb) bt 
#0  health_monitor_task (argument=0x0) at /home/divar/Desktop/Uni/1.ESP/HW3/student_starter/main/main.c:444
#1  0x4200f479 in vPortTaskWrapper (pxCode=0x42009088 <health_monitor_task>, pvParameters=0x0) at /home/divar/.espressif/v6.0.1/esp-idf/components/freertos/FreeRTOS-Kernel/portable/xtensa/port.c:143
(gdb) 
```

**نکته‌ی مهم:** همان‌طور که در خروجی `info threads` مشخص است، در یک اپلیکیشن مبتنی بر FreeRTOS/ESP-IDF، چندین ترد
به‌صورت هم‌زمان در حال اجرا هستند که تعدادی از آن‌ها مربوط به خود سیستم‌عامل و زیرساخت (مثل `IDLE0`، `IDLE1`، `ipc0`،
`ipc1`) هستند و ربطی به منطق برنامه‌ی ما ندارند. اگر مستقیماً دستور `bt` را اجرا کنیم، GDB به‌طور پیش‌فرض stack trace
مربوط به **ترد جاری (current thread)** — که معمولاً همان ترد `IDLE` است — را نمایش می‌دهد. چیزی که هیچ اطلاعات مفیدی
درباره‌ی محل توقف یا خطای برنامه‌ی خودمان نمی‌دهد.

به همین دلیل، روش صحیح دیباگ کردن به این صورت است:

1. ابتدا با دستور `info threads` لیست کامل تردها را مشاهده می‌کنیم و ترد یا تردهای مرتبط با اپلیکیشن خودمان (مثل
   `health_monitor_task`، `control_task`، `heartbeat_task` و غیره) را شناسایی می‌کنیم.
2. با دستور `thread <num>` به ترد مورد نظر سوییچ می‌کنیم. مثلاً:
   ```
   (gdb) thread 4
   ```
3. سپس دستور `bt` را اجرا می‌کنیم تا پشته‌ی فراخوانی (call stack) مخصوص همان ترد نمایش داده شود و بتوانیم دقیقاً ببینیم
   برنامه در کجای کد ما (مثلاً در فایل `main.c` خط ۴۴۴) متوقف شده یا دچار مشکل شده است.

با این روش، به‌جای بررسی تردهای بی‌ربط سیستم‌عامل، مستقیماً روی رفتار و وضعیت تسک‌های تعریف‌شده در برنامه‌ی خودمان تمرکز
می‌کنیم که برای پیدا کردن ریشه‌ی باگ یا رفتار غیرمنتظره بسیار کاربردی‌تر است.

---

## مرحله‌ی ۲: مشاهده‌ی وضعیت‌های پویای برنامه

### دستورهای اجرا شده در GDB

```
(gdb) info variables g_
(gdb) p/x &g_safety_cookie
(gdb) p/x g_safety_cookie
(gdb) p g_device_mode
(gdb) p g_safety_level
(gdb) x/32wx &g_safety_cookie
```

خروجی:

```
(gdb) info variables g_
All variables matching regular expression "g_":

File /home/divar/.espressif/v6.0.1/esp-idf/components/esp_common/src/esp_err_to_name.c:
86:     static const esp_err_msg_t esp_err_msg_table[215];

File /home/divar/.espressif/v6.0.1/esp-idf/components/esp_driver_usb_serial_jtag/src/usb_serial_jtag.c:
57:     static usb_serial_jtag_obj_t *p_usb_serial_jtag_obj;

File /home/divar/.espressif/v6.0.1/esp-idf/components/esp_driver_usb_serial_jtag/src/usb_serial_jtag_connection_monitor.c:
75:     static esp_system_init_fn_t esp_system_init_fn_usb_serial_jtag_conn_status_init;
29:     static uint32_t remaining_allowed_no_sof_ticks;
18:     static volatile _Bool s_usb_serial_jtag_conn_status;

File /home/divar/.espressif/v6.0.1/esp-idf/components/esp_driver_usb_serial_jtag/src/usb_serial_jtag_vfs.c:
634:    static const esp_vfs_select_ops_t s_vfs_jtag_select;

File /home/divar/.espressif/v6.0.1/esp-idf/components/esp_hw_support/port/esp32s3/rtc_init.c:
41:     uint32_t g_dig_dbias_pvt_240m;
43:     uint32_t g_dig_dbias_pvt_non_240m;
42:     uint32_t g_rtc_dbias_pvt_240m;
44:     uint32_t g_rtc_dbias_pvt_non_240m;
--Type <RET> for more, q to quit, c to continue without paging--

File /home/divar/.espressif/v6.0.1/esp-idf/components/esp_hw_support/spi_bus_lock.c:
891:    const spi_bus_lock_dev_handle_t g_spi_lock_main_flash_dev;

File /home/divar/.espressif/v6.0.1/esp-idf/components/esp_mm/port/esp32s3/ext_mem_layout.c:
16:     const mmu_mem_region_t g_mmu_mem_regions[1];

File /home/divar/.espressif/v6.0.1/esp-idf/components/esp_system/esp_ipc.c:
49:     static volatile _Bool s_no_block_func_and_arg_are_ready[2];

File /home/divar/.espressif/v6.0.1/esp-idf/components/esp_system/panic.c:
72:     _Bool g_panic_abort;
73:     char *g_panic_abort_details;
77:     static uint32_t g_panic_entry_count[2];

File /home/divar/.espressif/v6.0.1/esp-idf/components/esp_system/port/panic_handler.c:
47:     void *g_exc_frames[2];

File /home/divar/.espressif/v6.0.1/esp-idf/components/esp_system/port/soc/esp32s3/cache_err_int.c:
57:     const register_bit_t ilg_int_st_bits[6];

--Type <RET> for more, q to quit, c to continue without paging--
File /home/divar/.espressif/v6.0.1/esp-idf/components/esp_system/startup.c:
48:     const sys_startup_fn_t g_startup_fn[2];
31:     uint64_t g_startup_time;

File /home/divar/.espressif/v6.0.1/esp-idf/components/esp_system/task_wdt/task_wdt.c:
58:     _Bool g_twdt_isr;

File /home/divar/.espressif/v6.0.1/esp-idf/components/log/src/log_level/log_level.c:
14:     esp_log_level_t esp_log_default_level;

File /home/divar/.espressif/v6.0.1/esp-idf/components/log/src/log_level/tag_log_level/cache/log_binary_heap.c:
67:     static cached_tag_entry_t s_log_cache[31];
69:     static uint32_t s_log_cache_entry_count;
68:     static uint32_t s_log_cache_max_generation;
72:     static uint32_t s_log_cache_misses;

File /home/divar/.espressif/v6.0.1/esp-idf/components/log/src/log_level/tag_log_level/linked_list/log_linked_list.c:
49:     static struct log_tags_head s_log_tags;

File /home/divar/.espressif/v6.0.1/esp-idf/components/log/src/os/log_lock.c:
21:     static SemaphoreHandle_t s_log_mutex;
--Type <RET> for more, q to quit, c to continue without paging--

File /home/divar/.espressif/v6.0.1/esp-idf/components/log/src/os/log_write.c:
17:     vprintf_like_t esp_log_vprint_func;

File /home/divar/.espressif/v6.0.1/esp-idf/components/log/src/os/util.c:
13:     static esp_log_cache_enabled_t esp_log_cache_enabled;

File /home/divar/.espressif/v6.0.1/esp-idf/components/mbedtls/mbedtls/tf-psa-crypto/drivers/builtin/src/threading.c:
317:    mbedtls_threading_mutex_t mbedtls_threading_psa_globaldata_mutex;
318:    mbedtls_threading_mutex_t mbedtls_threading_psa_rngdata_mutex;

File /home/divar/.espressif/v6.0.1/esp-idf/components/spi_flash/flash_ops.c:
84:     const spi_flash_guard_funcs_t g_flash_guard_default_ops;

File /home/divar/.espressif/v6.0.1/esp-idf/components/vfs/nullfs.c:
55:     static vfs_null_ctx_t g_fds;

File /home/divar/Desktop/Uni/1.ESP/HW3/student_starter/main/main.c:
39:     volatile uint32_t g_cli_command_count;
34:     volatile int32_t g_device_mode;
36:     volatile uint32_t g_event_counter;
--Type <RET> for more, q to quit, c to continue without paging--
37:     volatile uint32_t g_health_flags;
38:     volatile uint32_t g_heartbeat;
40:     volatile uint32_t g_last_health_alert_us;
41:     volatile uint64_t g_last_heartbeat_us;
33:     volatile uint32_t g_safety_cookie;
35:     volatile int32_t g_safety_level;
51:     volatile stress_state_t g_stress_state;
43:     static char g_device_id[13];
(gdb) 
(gdb) 
(gdb) p/x &g_safety_cookie
$1 = 0x3fc9100c
(gdb) p/x g_safety_cookie
$2 = 0xce40876a
(gdb) p g_device_mode
$3 = 0
(gdb) p g_device_mode 
$4 = 0
(gdb) p g_safety_level
$5 = 3
(gdb) x/32wx &g_safety_cookie
0x3fc9100c <g_safety_cookie>:   0xce40876a      0x40376a2c      0x00000000      0x40377588
0x3fc9101c:     0x3fc9443c      0x403762a0      0x00000000      0x4037b540
0x3fc9102c:     0x3fc94138      0x40377588      0x3fc94438      0x40376a2c
0x3fc9103c:     0x00000005      0x4037b540      0x3fc94138      0x40376a2c
0x3fc9104c:     0x00000007      0x40376a2c      0x00000008      0x40376a2c
0x3fc9105c:     0x00000009      0x4200b270      0x00000000      0x40376a2c
0x3fc9106c:     0x0000000b      0x40376a2c      0x0000000c      0x40376a2c
0x3fc9107c:     0x0000000d      0x40376a2c      0x0000000e      0x40376a2c
(gdb) 
(gdb) p g_cli_command_count
$6 = 40 
```

### مقایسه با UART

### دستور‌های اجرا شده:

```text
> status
device_id=1cdbd49955e8
mode=service
safety_level=1
events=5
health_flags=0x00000000
heartbeat=341
health=ok
> sample
OK sample_queued

SENSOR sample event=6
> sample
OK sample_queued
> 
SENSOR sample event=7
status
device_id=1cdbd49955e8
mode=service
safety_level=1
events=7
health_flags=0x00000000
heartbeat=347
health=ok
> setmode normal
OK mode_set normal
> status
device_id=1cdbd49955e8
mode=normal
safety_level=3
events=7
health_flags=0x00000000
heartbeat=362
health=ok
> log   
cookie=0xce40876a mode=0 safety=3 events=7 flags=0x00000000 heartbeat=513
stress buckets=[0,0,0,0] pending_mode=0 guard=0x5a17c0de

```

### جدول مقایسه‌ی متغیرها

| متغیر                 | در UART دیده می‌شود؟               | تغییر کرد؟             | توضیح کوتاه                           |
|-----------------------|------------------------------------|------------------------|---------------------------------------|
| `g_safety_cookie`     | بله (`log`)                        | ثابت                   | همیشه `0xce40876a`                    |
| `g_device_mode`       | بله (`status`, `log`)              | بله (با `setmode`)     | UART به‌صورت متن، GDB به‌صورت عدد خام |
| `g_safety_level`      | بله (`status`, `log`)              | بله (با `setmode`)     | (service) ↔ (normal)                  |
| `g_event_counter`     | بله (`status`, `log`)              | بله (با `sample`)      | ۵ → ۷                                 |
| `g_health_flags`      | بله (`status`, `log`)              | ثابت (صفر)             | چون خطایی رخ نداده                    |
| `g_heartbeat`         | بله (`status`, `log`, `heartbeat`) | بله، مستقل از فرمان‌ها | هر ثانیه زیاد می‌شود                  |
| `g_stress_state`      | بله (`log`)                        | ثابت                   | buckets/guard صفر ماندند              |
| `g_cli_command_count` | **هیچ‌جا**                         | بله (با هر فرمان)      | فقط با GDB دیدنی است                  |
| `g_last_heartbeat_us` | بله (`heartbeat`)                  | بله                    | در `status`/`log` نیست                |

### جمع‌بندی مرحله‌ی ۲

با مقایسه‌ی خروجی GDB و UART مشخص شد که فرمان `status` تنها زیرمجموعه‌ای محدود از حافظه‌ی واقعی دستگاه (`device_id`,
`mode`, `safety_level`, `events`, `health_flags`, `heartbeat`) را نشان می‌دهد، فرمان `log` کمی جزئیات بیشتری (شامل مقدار
خام `cookie`، `mode` عددی و اطلاعات `stress_state`) اضافه می‌کند، اما متغیرهایی مانند `g_cli_command_count` و
`g_last_health_alert_us` در هیچ‌کدام از خروجی‌های UART دیده نمی‌شوند و تنها با GDB قابل کشف‌اند. همچنین مشخص شد که برخی
مقادیر (مانند `g_heartbeat`) کاملاً مستقل از فرمان‌های ارسالی و صرفاً تابع زمان هستند، در حالی‌که برخی دیگر (مانند
`g_event_counter` و `g_device_mode`/`g_safety_level`) دقیقاً هم‌زمان با فرمان مربوطه تغییر می‌کنند. این تفاوت‌ها دقیقاً
نشان‌دهنده‌ی همان نکته‌ای است که هدف این مرحله بود: **خروجی UART فقط یک نمای محدود و پردازش‌شده از حافظه‌ی واقعی است، نه
بازتاب کامل آن.**


---

در ادامه متن کامل «مرحله‌ی ۳» رو براتون آماده کردم. جاهایی که نیاز به خروجی واقعی ترمینال شما دارد را با `[...]` مشخص کردم — چون من به برد شما دسترسی ندارم و نباید خروجی جعلی بسازم. شما فقط باید عملاً تست‌ها را اجرا کنید و خروجی واقعی را جای این براکت‌ها بگذارید.

---

## مرحله‌ی ۳: بازتولید تغییر ناخواسته

### ۳.۱ تحلیل اولیه‌ی کد

پیش از اجرای آزمایش، با بررسی کد `main.c` مشخص شد که مسیر پردازش فرمان `stress <n>` در تابع `sensor_task` مشکوک‌ترین بخش برای وقوع یک خطای حافظه است:

```c
volatile uint32_t *legacy_buckets = g_stress_state.buckets;
for (uint32_t i = 0; i < request.value; ++i) {
    uint32_t bucket_index = i / STRESS_BUCKET_WIDTH;
    if (bucket_index > STRESS_BUCKETS) {
        bucket_index = STRESS_BUCKETS;
    }
    legacy_buckets[bucket_index]++;
    ...
```

آرایه‌ی `buckets` (تعریف‌شده در `stress_state_t`) دقیقاً ۴ عضو دارد (`STRESS_BUCKETS = 4`، اندیس‌های معتبر ۰ تا ۳)، اما شرط کلمپ‌کردن اندیس با عملگر `>` نوشته شده، نه `>=`. در نتیجه وقتی `bucket_index` دقیقاً برابر با `STRESS_BUCKETS` (یعنی ۴) شود، این مقدار کلمپ نمی‌شود و کد به `legacy_buckets[4]` می‌نویسد که خارج از محدوده‌ی مجاز آرایه است. این حدس اولیه، هدف آزمایش‌های زیر است.

### ۳.۲ آزمایش پایه (طبق نمونه‌ی صورت تمرین)

دنباله‌ی فرمان‌های زیر از طریق UART ارسال شد:

```
reset
status
stress 10
status
stress 100
status
setmode service
status
```

**خروجی کامل ترمینال:**

```
> reset
OK reset
> status
device_id=1cdbd49955e8
mode=normal
safety_level=3
events=0
health_flags=0x00000000
heartbeat=9
health=ok
> stress 10
OK stress_queued count=10
> 
SENSOR stress_complete count=10 events=10
status
device_id=1cdbd49955e8
mode=normal
safety_level=3
events=10
health_flags=0x00000000
heartbeat=21
health=ok
> stress 100
OK stress_queued count=100

SENSOR stress_complete count=100 events=110
> 
> 
ALERT health flags=0x00000008
 
ALERT health flags=0x00000008

ALERT health flags=0x00000008

ALERT health flags=0x00000008

ALERT health flags=0x00000008

ALERT health flags=0x00000008
status
device_id=1cdbd49955e8
mode=service
safety_level=1
events=110
health_flags=0x00000008
heartbeat=37
health=alert
> 
ALERT health flags=0x00000008
setmod
ALERT health flags=0x00000008
e service
OK mode_set service
> 
ALERT health flags=0x00000008

ALERT health flags=0x00000008
status
device_id=1cdbd49955e8
mode=service
safety_level=1
events=110
health_flags=0x00000008
heartbeat=45
health=alert
> 
ALERT health flags=0x00000008

ALERT health flags=0x00000008

ALERT health flags=0x00000008
```

**مشاهده‌ی کلیدی:**
پس از `stress 10`، فرمان `status` نشان می‌دهد `mode=normal` و `safety_level=3` که با مقدار قبل از آن تفاوتی ندارد.

اما پس از `stress 100`، فرمان `status` بدون آنکه فرمان `setmode` ارسال شده باشد، مقادیر زیر را نشان می‌دهد:
```
mode=servicek
safety_level=1
health_flags=0x00000008
```

این تغییر **ناخواسته** است، چون هیچ فرمان `setmode` بین دو `status` ارسال نشده بود.

### ۳.۳ بررسی حافظه با GDB (قبل و بعد از `stress 100`)

**قبل از ارسال `stress 100`:**
```
(gdb) p g_stress_state
$7 = {buckets = {10, 0, 0, 0}, pending_mode = 0, guard = 1511506142}
(gdb) p g_device_mode 
$8 = 0
(gdb) p g_hea
g_health_flags  g_heartbeat     
(gdb) p g_health_flags 
$9 = 0
(gdb) continue &
Continuing.
```

**بعد از ارسال `stress 100`:**
```
(gdb) interrupt
(gdb) [esp32s3.cpu0] Target halted, PC=0x40378C4A, debug_reason=00000000
Set GDB target to 'esp32s3.cpu0'

Thread 1 "IDLE0" received signal SIGINT, Interrupt.
0x40378c4a in esp_cpu_wait_for_intr () at /home/divar/.espressif/v6.0.1/esp-idf/components/esp_hw_support/cpu.c:64
64      }

(gdb) p g_stress_state 
$10 = {buckets = {30, 20, 20, 20}, pending_mode = 0, guard = 1511506142}
(gdb) p g_device_mode 
$11 = 1
(gdb) p g_health_flags 
$12 = 8
```

**تحلیل نتایج:**

جمع افزایش مقادیر آرایه‌ی `buckets` برابر با ۲۰+۲۰+۲۰+۲۰ = ۸۰ واحد است، در حالی‌که ۱۰۰ رخداد استرس ارسال شد. این یعنی ۲۰ رخداد باقی‌مانده (مربوط به `i` از ۸۰ تا ۹۹، که طبق فرمول `bucket_index = i / 20` باید به اندیس ۴ بروند) در هیچ‌کدام از ۴ عضو قابل‌مشاهده‌ی آرایه ثبت نشده‌اند. علت این است که آرایه‌ی `buckets` فقط ۴ عضو (اندیس ۰ تا ۳) دارد، اما شرط کلمپ‌کردن اندیس در کد به‌صورت `if (bucket_index > STRESS_BUCKETS)` نوشته شده که وقتی `bucket_index` دقیقاً برابر ۴ باشد اجرا نمی‌شود. در نتیجه این ۲۰ رخداد مستقیماً در `legacy_buckets[4]` نوشته می‌شوند که خارج از محدوده‌ی مجاز حافظه‌ی این آرایه است.

مقدار `g_device_mode` از `0` به `1` (service) و `g_health_flags` از `0` به `8` تغییر کرد، بدون اینکه هیچ فرمان `setmode` ارسال شده باشد. مقدار ۸ در باینری معادل بیت سوم روشن است که در کد با نام `HEALTH_FLAG_UNEXPECTED_MODE` تعریف شده و فقط زمانی در `control_task` تنظیم می‌شود که `g_stress_state.pending_mode` غیرصفر باشد.

جالب توجه است که در این لحظه `pending_mode` دوباره برابر `0` مشاهده شد. دلیل این امر آن است که `control_task` هر ۱۰ میلی‌ثانیه یک‌بار مقدار `pending_mode` را بررسی و در صورت غیرصفر بودن، پس از اعمال تغییرات مربوطه، خودش این مقدار را مجدداً به صفر بازمی‌گرداند. این رفتار نشان می‌دهد نوشتن خارج از محدوده روی `legacy_buckets[4]` مقدار حافظه‌ی مجاور (که بر اساس چیدمان ساختار `stress_state_t` دقیقاً همان فیلد `pending_mode` است) را به یک مقدار گذرای غیرصفر تغییر داده، و این تغییر توسط `control_task` تشخیص داده شده و اثرات آن (تغییر mode و ست‌شدن پرچم) ثبت شده است، در حالی‌که خود مقدار میانی به‌دلیل زمان‌بندی رخداد بین دو مشاهده‌ی GDB قابل مشاهده نبود.

این شواهد قویاً فرضیه‌ی اولیه را تأیید می‌کنند: **خطا از یک نوشتن خارج از محدوده‌ی آرایه (buffer overflow) روی `g_stress_state.buckets` ناشی می‌شود که به فیلد مجاور `pending_mode` سرریز می‌کند.** تأیید قطعی این مکانیزم و یافتن دقیق خط کد مسئول، در مرحله‌ی ۴ با استفاده از `watch g_stress_state.pending_mode` انجام خواهد شد.

---

## مرحله‌ی ۴: پیدا کردن محل خطا با نقطه‌ی پایش

### نقطه‌ی پایش تنظیم‌شده

پیش از تنظیم نهایی watchpoint، به‌دلیل مشکلی که در بخش «چالش‌ها» توضیح داده شد (خطای عدم درج watchpoint سخت‌افزاری به‌دلیل ناهماهنگی هسته‌ها)، ابتدا پردازنده کاملاً ریست و متوقف شد و سپس watchpoint گذاشته شد:

```
(gdb) monitor reset halt
(gdb) delete
Delete all breakpoints, watchpoints, tracepoints, and catchpoints? (y or n) y
(gdb) i b
No breakpoints, watchpoints, tracepoints, or catchpoints.
(gdb) watch g_safety_level
Hardware watchpoint 1: g_safety_level
(gdb) i b 
Num     Type           Disp Enb Address    What
1       hw watchpoint  keep y              g_safety_level
(gdb) continue
Continuing.
```

با دستور `i b` نیز تأیید شد که فقط همین یک watchpoint فعال است و پیش از این هیچ breakpoint/watchpoint اضافه‌ای باقی نمانده:

```
(gdb) i b
Num     Type           Disp Enb Address    What
1       hw watchpoint  keep y              g_safety_level
        breakpoint already hit 1 time
```

### دنباله‌ی ورودی که watchpoint را فعال کرد

برخلاف مرحله‌ی ۳ که با `stress 100` خطا را بازتولید کرده بودیم، در این مرحله برای پیدا کردن **دقیق‌ترین آستانه‌ی وقوع خطا** سه ورودی متوالی زیر ارسال شد:

```
> stress 80
OK stress_queued count=80
SENSOR stress_complete count=80 events=80

> stress 79
OK stress_queued count=79
SENSOR stress_complete count=79 events=159

> stress 81
OK stress_queued count=81
SENSOR stress_complete count=81 events=240
--- Warning: Writing to serial is timing out...
```

**نکته‌ی کلیدی:** ورودی‌های `stress 80` و `stress 79` هیچ‌کدام watchpoint را فعال نکردند، اما بلافاصله پس از ارسال `stress 81`، watchpoint فعال شد و پردازنده متوقف شد (که همان لحظه ارسال `status` بعدی روی UART با پیام timeout مواجه شد، چون هسته‌ی CPU در GDB نگه‌داشته شده بود).

این رفتار دقیقاً با فرضیه‌ی مرحله‌ی ۳ (خطای off-by-one در کلمپ‌کردن `bucket_index`) سازگار است:

- در `sensor_task`، برای هر رخداد stress با اندازه‌ی `request.value`، حلقه از `i = 0` تا `i < request.value` اجرا می‌شود و `bucket_index = i / STRESS_BUCKET_WIDTH` (یعنی `i / 20`) محاسبه می‌شود.
- برای `stress 80`: بیشینه‌ی `i` برابر ۷۹ است → بیشینه‌ی `bucket_index = 79/20 = 3`. کاملاً داخل محدوده‌ی مجاز (۰ تا ۳).
- برای `stress 79`: بیشینه‌ی `i` برابر ۷۸ است → بیشینه‌ی `bucket_index = 78/20 = 3`. باز هم داخل محدوده.
- برای `stress 81`: بیشینه‌ی `i` برابر ۸۰ است → `bucket_index = 80/20 = 4`. چون شرط کلمپ کد به‌صورت `if (bucket_index > STRESS_BUCKETS)` نوشته شده (نه `>=`)، وقتی `bucket_index` دقیقاً برابر `STRESS_BUCKETS` (۴) شود، کلمپ نمی‌شود و کد مستقیماً به `legacy_buckets[4]` می‌نویسد که خارج از محدوده‌ی آرایه‌ی ۴‑عضوی `buckets` است.

این آستانه‌ی دقیق (فعال‌نشدن در ۸۰ و ۷۹، فعال‌شدن در ۸۱) خودش یک **مدرک قطعی و قابل بازتولید** برای اثبات خط دقیق خطا در کد است، نه فقط یک حدس مبتنی بر خواندن کد.

### لحظه‌ی فعال شدن watchpoint — بررسی ثبات‌ها، پشته و کد

پس از توقف پردازنده به‌دلیل تغییر `g_safety_level`، دستورهای زیر اجرا شد:

```
(gdb) info registers
...
a0             0x8200ec89          -2113868663
a1             0x3fcec2e0          1070514912
a2             0x0                 0
a3             0x0                 0
a4             0x3fc925e8          1070147048
a5             0x1                 1
...
```

```
(gdb) info locals
No locals.
```

```
(gdb) frame 0
#0  control_task (argument=0x0) at .../main/main.c:418
418                 g_health_flags |= HEALTH_FLAG_UNEXPECTED_MODE;
```

```
(gdb) list
413     
414         while (true) {
415             if (g_stress_state.pending_mode != 0) {
416                 g_device_mode = MODE_SERVICE;
417                 g_safety_level = 1;
418                 g_health_flags |= HEALTH_FLAG_UNEXPECTED_MODE;
419                 g_stress_state.pending_mode = 0;
420             }
421     
422             vTaskDelay(pdMS_TO_TICKS(10));
```

نکته‌ی مهم در تفسیر این خروجی: GDB بلافاصله **بعد از** نوشتن مقدار جدید در `g_safety_level` متوقف می‌شود و خط بعدی (۴۱۸) را به‌عنوان دستور بعدی نشان می‌دهد. یعنی خط ۴۱۷ (`g_safety_level = 1;`) همان دستوری است که مقدار حساس را تغییر داده و watchpoint را فعال کرده است. `info locals` نیز نشان می‌دهد `control_task` هیچ متغیر محلی‌ای در این نقطه ندارد (چون تابع تنها روی متغیرهای گلوبال کار می‌کند)، که با بدنه‌ی ساده‌ی این تابع در کد مطابقت دارد.

### پاسخ به پرسش‌های صورت تمرین

- **نقطه‌ی پایش را روی کدام متغیر گذاشتید؟**
  روی `g_safety_level` (طبق پیشنهاد صورت تمرین)، چون این متغیر دقیقاً همان مقداری بود که در مرحله‌ی ۳ بدون فرمان مستقیم `setmode` تغییر کرده بود.

- **با کدام ورودی فعال شد؟**
  دقیقاً با ارسال `stress 81`، در حالی‌که `stress 80` و `stress 79` (که بلافاصله پیش از آن ارسال شدند) هیچ‌کدام آن را فعال نکردند. این آستانه‌ی دقیق نشان می‌دهد فعال‌سازی watchpoint وابسته به مقدار عددی مشخصی است، نه رفتار تصادفی.

- **ردیابی پشته کدام تابع را نشان داد؟**
  تابع `control_task`، دقیقاً در خط `g_safety_level = 1;` (خط ۴۱۷ فایل `main.c`)، داخل بلوک `if (g_stress_state.pending_mode != 0)`.

- **تغییر مقدار مستقیم بود یا بر اثر نوشتن اشتباه در حافظه‌ی مجاور رخ داد؟**
  از نگاه *مستقیم کد*، خود `control_task` است که خط `g_safety_level = 1;` را اجرا می‌کند. اما این تابع فقط زمانی وارد این مسیر می‌شود که `g_stress_state.pending_mode` غیرصفر باشد. در برنامه‌ی سالم، هیچ‌کجا مستقیماً `pending_mode` را ست نمی‌کند (`reset_state` فقط آن را صفر می‌کند). بنابراین علت ریشه‌ای، **یک نوشتن خارج از محدوده‌ی آرایه (buffer overflow) در تابع `sensor_task`** است که هنگام پردازش `stress 81`، مقدار `legacy_buckets[4]` را می‌نویسد. و چون `buckets` تنها ۴ عضو دارد (اندیس ۰ تا ۳)، این نوشتن عملاً روی فیلد بعدی ساختار `stress_state_t` یعنی `pending_mode` می‌افتد و آن را از ۰ به یک مقدار غیرصفر تغییر می‌دهد. سپس `control_task` (که هر ۱۰ میلی‌ثانیه این فیلد را بررسی می‌کند) این مقدار غیرصفر را می‌بیند و به‌عنوان یک درخواست معتبر تغییر حالت تفسیر می‌کند، `g_device_mode` را به `MODE_SERVICE` و `g_safety_level` را به `1` تغییر می‌دهد.

- **شواهد شما برای این نتیجه‌گیری چیست؟**
    1. آستانه‌ی دقیق ریاضی: خطا فقط زمانی رخ می‌دهد که `count` طوری باشد که حداقل یک `i` در حلقه به مقدار `80` برسد (یعنی `count ≥ 81`). این دقیقاً با فرمول `bucket_index = i/20` و باگ کلمپ `if (bucket_index > STRESS_BUCKETS)` (به‌جای `>=`) هم‌خوانی دارد.
    2. Backtrace نشان داد تغییر مقدار در تابع `control_task` رخ داده، نه در مسیر پردازش فرمان UART. یعنی `g_safety_level` توسط یک منطق پس‌زمینه (نه مستقیماً توسط دستور کاربر) تغییر کرده است.
    3. در مرحله‌ی ۳ با بررسی مستقیم `g_stress_state` قبل و بعد از `stress 100` دیده شد که مجموع افزایش چهار عضو قابل‌مشاهده‌ی `buckets` کمتر از تعداد رخدادهای ارسالی بود. این نشان می‌داد بخشی از نوشتن‌ها به‌جایی خارج از این چهار عضو رفته‌اند — دقیقاً همان چیزی که اکنون با watchpoint تأیید شد.
    4. چیدمان ساختار `stress_state_t` (`buckets[4]` بلافاصله قبل از `pending_mode`) توضیح می‌دهد چرا این سرریز دقیقاً روی `pending_mode` اثر می‌گذارد، نه یک متغیر کاملاً نامرتبط دیگر.


---

## مرحله‌ی ۵: تحلیل پشته، وظیفه‌ی جاری و علت خطا

### ۵.۱ خطا در کدام وظیفه رخ داده است؟

با بررسی دقیق backtrace به‌دست‌آمده در مرحله‌ی قبل، مشخص شد که **محل توقف واقعی watchpoint** (یعنی جایی که مقدار `g_safety_level` واقعاً نوشته می‌شود) در تابع `control_task` است:

```
#0  control_task (argument=0x0) at .../main/main.c:418
418                 g_health_flags |= HEALTH_FLAG_UNEXPECTED_MODE;
```

اما این نکته نباید ما را به این نتیجه‌ی نادرست برساند که خطا از ابتدا در `control_task` ایجاد شده. باید بین **محل نوشتن نهایی مقدار حساس** و **محل ریشه‌ای وقوع خطای حافظه** تمایز قائل شد:

- تابع `control_task` صرفاً یک ناظر ساده است که هر ۱۰ میلی‌ثانیه یک پرچم را بررسی می‌کند:
```c
if (g_stress_state.pending_mode != 0) {
    g_device_mode = MODE_SERVICE;
    g_safety_level = 1;
    g_health_flags |= HEALTH_FLAG_UNEXPECTED_MODE;
    g_stress_state.pending_mode = 0;
}
```
این تابع **هیچ مسیر مجازی برای نوشتن مستقیم در `pending_mode` ندارد**. در واقع تنها کاری که با آن می‌کند خواندن و در پایان صفر کردن آن است.

- بنابراین ریشه‌ی واقعی خطا باید در جایی باشد که *کسی دیگر* مقدار `pending_mode` را (به‌صورت ناخواسته) غیرصفر کرده است. با ردیابی مسیر ورودی که watchpoint را فعال کرد (`stress 81`، پس از `stress 80` و `stress 79`)، مشخص شد این تغیر ناخواسته از تابع **`sensor_task`**، در مسیر پردازش فرمان `stress <n>`، ناشی می‌شود:

```c
volatile uint32_t *legacy_buckets = g_stress_state.buckets;
for (uint32_t i = 0; i < request.value; ++i) {
    uint32_t bucket_index = i / STRESS_BUCKET_WIDTH;
    if (bucket_index > STRESS_BUCKETS) {
        bucket_index = STRESS_BUCKETS;
    }
    legacy_buckets[bucket_index]++;
    ...
}
```

پس **مسیر کامل ایجاد خطا دو تابع را درگیر می‌کند**، نه یکی:
1. `sensor_task` → منشأ واقعی خطا (نوشتن خارج از محدوده‌ی آرایه)
2. `control_task` → قربانی این خطا (به اشتباه یک پرچم فاسدشده را به‌عنوان درخواست معتبر تفسیر می‌کند و مقدار حساس `g_safety_level` را تغییر می‌دهد)

این خطا **در مسیر دریافت و تجزیه‌ی فرمان UART رخ نداده** (یعنی خود `handle_command` و `parse_u32` کاملاً درست عمل می‌کنند و ورودی `stress <n>` را به‌درستی پارس و اعتبارسنجی می‌کنند). بلکه در مسیر **پردازش غیرهم‌زمان (asynchronous) این فرمان در یک وظیفه‌ی دیگر** (`sensor_task`) اتفاق می‌افتد.

### ۵.۲ نقش بافر، طول ورودی، اندیس آرایه و تبدیل نوع

در این باگ خاص، **بافر ورودی و طول رشته‌ی دریافتی از UART اصلاً نقشی در خطا ندارند** — بر خلاف باگ‌های کلاسیک سرریز بافر رشته‌ای، اینجا مقدار عددی وارد‌شده کاملاً معتبر و در محدوده‌ی مجاز `parse_u32` است:

```c
static bool parse_u32(const char *text, uint32_t *value)
{
    ...
    if (*end != '\0' || parsed > 200UL) {
        return false;
    }
    *value = (uint32_t)parsed;
    return true;
}
```

مقدار `count` تا ۲۰۰ پذیرفته می‌شود و هیچ خطای تبدیل نوع (type conversion) یا سرریز عددی در این تابع رخ نمی‌دهد.

مشکل اصلی، **اندیس آرایه** است. آرایه‌ی `buckets` در ساختار زیر تعریف شده:
```c
typedef struct {
    volatile uint32_t buckets[STRESS_BUCKETS]; // STRESS_BUCKETS = 4 → valid indexes: 0..3
    volatile int32_t pending_mode;
    volatile uint32_t guard;
} stress_state_t;
```

و شرط کلمپ‌کردن اندیس در `sensor_task`:
```c
if (bucket_index > STRESS_BUCKETS) {
    bucket_index = STRESS_BUCKETS;
}
```

این یک **خطای کلاسیک off-by-one** است: شرط باید `>=` باشد، نه `>`. وقتی `bucket_index` دقیقاً برابر `STRESS_BUCKETS` (یعنی ۴) شود، این مقدار کلمپ نمی‌شود و کد به `legacy_buckets[4]` می‌نویسد — یعنی یک اندیس خارج از محدوده‌ی آرایه‌ای که فقط ۴ عضو (اندیس ۰ تا ۳) دارد.

از آنجا که `buckets` اولین عضو `stress_state_t` است و بلافاصله بعد از آن `pending_mode` قرار دارد، این نوشتن خارج از محدوده دقیقاً حافظه‌ی متعلق به `pending_mode` را بازنویسی می‌کند. این دقیقاً یک نمونه‌ی ساده و کنترل‌شده از **حمله‌ی سرریز مبتنی بر ساختار پشت سر هم در حافظه (adjacent memory corruption)** است — نه سرریز پشته (stack overflow) و نه سرریز رشته‌ای، بلکه سرریز اندیس آرایه در یک ساختار سراسری (global struct overflow).

### ۵.۳ چرا چنین خطایی در یک سامانه‌ی نهفته مهم است؟

چند دلیل مهم برای این‌که این‌گونه خطاها در سیستم‌های نهفته حیاتی‌تر از سیستم‌های عمومی هستند:

1. **عدم وجود لایه‌ی محافظتی حافظه (MMU/MPU) در بسیاری از میکروکنترلرها:** برخلاف سیستم‌عامل‌های دسکتاپ که حافظه‌ی هر پروسه ایزوله است، در بسیاری از سیستم‌های نهفته تمام متغیرهای گلوبال و ساختارهای داده در یک فضای آدرس مشترک و بدون محافظت قرار دارند. بنابراین یک سرریز آرایه به‌راحتی می‌تواند مستقیماً روی متغیرهای حیاتی دیگر (مثل پرچم‌های ایمنی) اثر بگذارد، بدون آنکه سیستم‌عامل یا سخت‌افزار جلوی آن را بگیرد.

2. **رفتار نامرئی و بی‌صدا (silent corruption):** همان‌طور که در مرحله‌ی ۲ و ۳ دیدیم، فرمان `status` که رابط اصلی کاربر با دستگاه است، **هیچ نشانه‌ای از این خطای حافظه‌ای نشان نمی‌دهد** مگر پس از وقوع کامل آن (یعنی وقتی `mode` و `health_flags` قبلاً تغییر کرده‌اند). کاربر یا اپراتور معمولی هیچ‌گاه متوجه نمی‌شود که چرا و چگونه این تغییر رخ داده است. فقط با ابزار سطح پایینی مثل JTAG/GDB می‌توان علت را کشف کرد.

3. **ارتباط چندوظیفه‌ای (cross-task) باعث پیچیدگی ردیابی می‌شود:** خطا در یک وظیفه (`sensor_task`) ایجاد می‌شود اما اثر آن در وظیفه‌ی کاملاً دیگری (`control_task`) ظاهر می‌شود. در یک سیستم بی‌درنگ چندوظیفه‌ای، این جدایی زمانی و مکانی بین علت و معلول، دیباگ را بسیار دشوارتر از یک برنامه‌ی تک‌رشته‌ای می‌کند.

4. **تکرارپذیری وابسته به داده‌ی ورودی، نه یک شرط ثابت:** همان‌طور که در مرحله‌ی ۴ نشان داده شد، این خطا فقط با یک ورودی خاص (`count ≥ 81`) رخ می‌دهد. در تست‌های سطحی معمول (مثلاً تست با مقادیر کوچک `stress`)، این باگ اصلاً دیده نمی‌شود. این یعنی یک محصول می‌تواند مدت‌ها بدون مشکل ظاهری کار کند تا وقتی یک ورودی خاص (که شاید در محیط واقعی هم رخ دهد) آن را فعال کند.

### ۵.۴ پیامد در صورت استفاده از این وضعیت برای تصمیم ایمنی/امنیتی

اگر مقدار `g_safety_level` یا `g_device_mode` در این ثابت‌افزار برای یک **تصمیم واقعی ایمنی یا امنیتی** استفاده می‌شد (مثلاً برای فعال/غیرفعال کردن یک عملگر خطرناک، قطع برق یک موتور، یا تغییر سطح دسترسی یک کاربر)، پیامدهای این باگ می‌توانست بسیار جدی باشد:

- **تغییر ناخواسته‌ی حالت عملیاتی دستگاه بدون دستور مجاز:** دستگاه از `MODE_NORMAL` به `MODE_SERVICE` (که معمولاً برای دسترسی تعمیراتی و محدودتر طراحی می‌شود) سوئیچ می‌کند، بدون آنکه هیچ فرمان `setmode service` معتبری صادر شده باشد. اگر `MODE_SERVICE` واقعاً به معنای کاهش سطح محافظت‌های ایمنی یا باز شدن مسیرهای دسترسی حساس بود، این یعنی یک کاربر غیرمسئول (یا حتی یک ورودی کاملاً تصادفی/غیرعمدی) می‌تواند بدون احراز هویت، دستگاه را وارد یک حالت کم‌ایمن‌تر کند.

- **کاهش ناخواسته‌ی سطح ایمنی (`g_safety_level` از ۳ به ۱):** اگر این مقدار در منطق واقعی برای تصمیماتی مثل «آیا اجازه‌ی اجرای یک عملیات خطرناک وجود دارد یا نه» استفاده می‌شد، سرریز حافظه می‌توانست به‌طور بی‌سروصدا سطح محافظت را کاهش دهد و امکان اجرای عملیاتی را فراهم کند که در شرایط عادی مجاز نبود.

- **گمراه‌کننده بودن پرچم سلامت:** `HEALTH_FLAG_UNEXPECTED_MODE` ست می‌شود، اما این پرچم صرفاً *نتیجه‌ی جانبی* باگ اصلی است، نه علت آن. یک اپراتور که فقط پرچم‌های سلامت را می‌بیند ممکن است تصور کند مشکل از یک تغییر مجاز حالت است، در حالی‌که ریشه‌ی واقعی یک خطای حافظه‌ی کاملاً متفاوت (در یک زیرسیستم بی‌ربط به مدیریت حالت) است. این می‌تواند تیم پشتیبانی را به مسیر اشتباه تحلیل خطا هدایت کند.

- **قابلیت تکرار توسط یک مهاجم:** از آنجا که این باگ با یک ورودی کاملاً مشخص (`stress` با مقدار مشخص) و **قابل پیش‌بینی** رخ می‌دهد، یک عامل مخرب که به رابط UART دسترسی دارد (حتی بدون دسترسی به JTAG) می‌تواند عمداً این حالت را با ارسال `stress 81` (یا هر مقدار مشابه) القا کند و دستگاه را به حالت ناخواسته‌ای ببرد — این عملاً یک **حمله‌ی تزریق حالت (state injection) از طریق سرریز حافظه** است که از یک رابط ظاهراً بی‌خطر (فرمان `stress`) سوءاستفاده می‌کند.

---

 ##  مرحله‌ی ۶: اثر توقف پردازنده روی سامانه‌ی بی‌درنگ 

### آزمایش انجام‌شده

**رونوشت GDB / OpenOCD:**

```
(gdb) monitor reset halt
[esp32s3.cpu0] requesting target halt and executing a soft reset
[esp32s3.cpu0] Debug controller was reset.
[esp32s3.cpu0] Core was reset.
[esp32s3.cpu0] Target halted, PC=0x500000EF, debug_reason=00000000
[esp32s3.cpu0] Reset cause (3) - (Software core reset)
[esp32s3.cpu0] Target halted, PC=0x40000400, debug_reason=00000000
[esp32s3.cpu1] Target halted, PC=0x40000400, debug_reason=00000000
[esp32s3.cpu1] Reset cause (3) - (Software core reset)
[esp32s3.cpu0] Reset cause (3) - (Software core reset)
(gdb) continue
Continuing.
```
`WAITED 10 SECONDS`
```
^C[esp32s3.cpu0] Target halted, PC=0x4037848A, debug_reason=00000000
Set GDB target to 'esp32s3.cpu0'
[esp32s3.cpu1] Target halted, PC=0x40043A3B, debug_reason=00000000

Thread 8 "IDLE" received signal SIGINT, Interrupt.
0x4037848a in esp_cpu_wait_for_intr () at .../esp_hw_support/cpu.c:64
64      }
(gdb) continue
Continuing.
```

**خروجی UART متناظر:**

```
> status
device_id=1cdbd49955e8
mode=normal
safety_level=3
events=0
health_flags=0x00000000
heartbeat=3
health=ok
> status
device_id=1cdbd49955e8
mode=normal
safety_level=3
events=0
health_flags=0x00000004
heartbeat=6
health=alert
>
```

### تحلیل توالی زمانی آزمایش

1. بلافاصله پس از `monitor reset halt` و سپس `continue`، فرمان `status` اول ارسال شد و مقدار `heartbeat=3` و `health_flags=0x00000000` (یعنی `health=ok`) گزارش شد. این یعنی حدود ۳ ثانیه از اجرای واقعی برنامه (سه بار افزایش `heartbeat_task`) گذشته بود.
2. سپس طبق دستور کار، حدود ۱۰ ثانیه صبر شد و بعد با `Ctrl+C` (سیگنال `interrupt`) پردازنده متوقف شد. خروجی OpenOCD نشان می‌دهد در این لحظه `cpu0` داخل تابع `esp_cpu_wait_for_intr` (یعنی داخل تسک بی‌کار `IDLE`) بوده است — یعنی سیستم در آن لحظه هیچ کار فعالی در حال اجرا نداشت و کاملاً idle بود.
3. پردازنده برای مدتی در حالت متوقف نگه داشته شد (طبق دستورالعمل تمرین، ۵ تا ۱۰ ثانیه)، سپس با `continue` دوباره اجرا از سر گرفته شد.
4. فرمان `status` دوم نشان داد `heartbeat=6` (یعنی تنها ۳ افزایش دیگر رخ داده، نه بیشتر) اما `health_flags=0x00000004` و `health=alert`.

مقدار `0x00000004` دقیقاً برابر است با `HEALTH_FLAG_HEARTBEAT_STALE` که در کد این‌گونه تعریف شده:
```c
#define HEALTH_FLAG_HEARTBEAT_STALE  (1UL << 2)
```
و شرط تنظیم آن در `health_monitor_task` چنین است:
```c
if (now_us > g_last_heartbeat_us &&
    (now_us - g_last_heartbeat_us) > 3000000ULL) {
    current_flags |= HEALTH_FLAG_HEARTBEAT_STALE;
}
```

### توضیح مکانیزم دقیق این رفتار

نکته‌ی کلیدی این است که وقتی JTAG کل هسته‌ی `cpu0` را متوقف می‌کند، **اجرای کد برنامه (شامل زمان‌بند FreeRTOS و همه‌ی تسک‌ها مثل `heartbeat_task` و `health_monitor_task`) کاملاً متوقف می‌شود**، اما تایمر سخت‌افزاری‌ای که `esp_timer_get_time()` بر پایه‌ی آن کار می‌کند (که با ساعت مستقل از هسته کار می‌کند)، همچنان به شمارش زمان واقعی (wall-clock) ادامه می‌دهد.

در نتیجه:
- در طول مدتی که پردازنده متوقف بود، **`heartbeat_task` هیچ فرصتی برای اجرا و افزایش `g_heartbeat` یا به‌روزرسانی `g_last_heartbeat_us` نداشت**. به همین دلیل تنها ۳ واحد به `heartbeat` اضافه شد (نه بیشتر)، در حالی‌که زمان واقعی سپری‌شده بین دو `status` (شامل ۱۰ ثانیه انتظار + مدت توقف پردازنده) قطعاً بیشتر از ۳ ثانیه بود.
- به محض `continue` شدن پردازنده، `health_monitor_task` دوباره اجرا شد و مقدار `now_us` (که در طول توقف هم پیش رفته بود) را با `g_last_heartbeat_us` (که در طول توقف ثابت مانده بود) مقایسه کرد. چون این اختلاف از ۳,۰۰۰,۰۰۰ میکروثانیه (۳ ثانیه) بیشتر بود، پرچم `HEALTH_FLAG_HEARTBEAT_STALE` ست شد و `status` بعدی مقدار `health=alert` را نشان داد.

این دقیقاً یعنی **توقف پردازنده توسط ابزار عیب‌یابی، به‌طور مصنوعی وضعیتی مشابه با یک خطای واقعی (کندی یا گیرکردن تسک heartbeat) ایجاد کرده است**، بدون آنکه هیچ باگی در کد رخ داده باشد.

### پاسخ به پرسش‌های صورت تمرین

- **آیا مقدار event_counter، health_flags یا تپش سلامت پس از توقف تغییر کرد؟**
  مقدار `g_event_counter` بدون تغییر ماند (چون هیچ فرمان `sample` یا `stress`ای ارسال نشد)، اما `g_health_flags` از `0x00000000` به `0x00000004` تغییر کرد و `g_heartbeat` تنها به‌اندازه‌ی زمان واقعیِ *اجراشده* (نه زمان واقعیِ *سپری‌شده*) افزایش یافت. یعنی در طول توقف پردازنده، `heartbeat` منجمد شد اما ساعت مرجع (`esp_timer`) پیش رفت.

- **آیا توقف پردازنده باعث هشدار یا پایان‌مهلت شد؟**
  بله. همان‌طور که در کد `health_monitor_task` نیز دیده می‌شود، به‌محض تشخیص فاصله‌ی زمانی بیش از حد مجاز، پیام هشدار چاپ می‌شود: اگر این وضعیت برای تصمیم ایمنی یا امنیتی دستگاه استفاده می‌شد، پیامد آن چه بود؟ (اشاره به همین ریسک در مرحله‌ی ۵) — این‌جا هم دقیقاً یک نمونه از فعال‌شدن ناخواسته‌ی این نوع هشدار را با ابزار عیب‌یابی، نه با یک باگ واقعی، دیدیم. در کد `health_monitor_task` این‌گونه پیاده‌سازی شده:
```c
if (current_flags != 0) {
    g_last_health_alert_us = (uint32_t)now_us;
    printf("\nALERT health flags=0x%08lx\n", (unsigned long)current_flags);
}
```
که در آزمایش عملی، این پیام `ALERT` باید بلافاصله پس از `continue` شدن پردازنده (و رسیدن نوبت اجرای `health_monitor_task`) روی UART چاپ شده باشد.

- **چرا عیب‌یابی یک سامانه‌ی بی‌درنگ می‌تواند نتیجه‌ی آزمایش را تغییر دهد؟**
  چون توقف پردازنده توسط JTAG **تقارن زمانی بین اجرای کد و گذر زمان واقعی را می‌شکند**. تایمرهای سخت‌افزاری مستقل از هسته (مانند مبنای `esp_timer_get_time()`) در طول توقف پردازنده همچنان کار می‌کنند، اما اجرای تسک‌های FreeRTOS کاملاً متوقف می‌شود. در نتیجه هر منطقی که بر پایه‌ی «اختلاف زمان واقعی از آخرین رخداد» بنا شده باشد (مثل تشخیص heartbeat stale یا timeoutها)، پس از هر توقف با GDB به‌اشتباه فعال می‌شود — دقیقاً چیزی که در این آزمایش دیدیم. بدون هیچ تغییری در منطق برنامه، صرفاً با نگه‌داشتن CPU در حالت halt، پرچم هشدار سلامت فعال شد.

- **این نکته چه محدودیتی در تحلیل خطاهای پویا ایجاد می‌کند؟**
  این پدیده یک نمونه‌ی کلاسیک از **اثر ناظر (observer effect)** در دیباگ سیستم‌های بی‌درنگ است: همان ابزاری که برای مشاهده و تحلیل خطا استفاده می‌شود (JTAG/GDB)، خودش می‌تواند رفتار سیستم را تغییر دهد یا حتی نشانه‌های خطای جعلی تولید کند. این یعنی:
- هر بار که یک breakpoint یا watchpoint سبب توقف طولانی پردازنده شود، باید در تحلیل نهایی به این احتمال توجه شود که برخی پرچم‌های هشدار یا رفتارهای غیرمنتظره، **مصنوعی و ناشی از خودِ فرایند دیباگ** بوده‌اند، نه بازتابی از یک باگ واقعی در منطق برنامه.
- برای تمایز قائل‌شدن بین «خطای واقعی حافظه» (مثل چیزی که در مراحل ۳ و ۴ با `stress` دیدیم) و «اثر جانبی توقف دیباگر» (مثل چیزی که همین‌جا دیدیم)، باید همیشه به **علیت مشخص در کد** (مثلاً backtrace و مسیر اجرایی که مقدار را تغییر داده) رجوع کرد، نه صرفاً به ظاهرشدن یک پرچم هشدار در خروجی.
- این محدودیت به‌طور خاص در تحلیل مسائل زمان‌بندی (timing-sensitive bugs) یا هر منطقی که به heartbeat/timeout وابسته است، اهمیت بیشتری دارد. چرا که استفاده از دیباگر برای بررسی چنین منطقی می‌تواند خودش باعث بروز رفتار (false positive) شود که در حالت اجرای عادی (بدون دیباگر) هرگز رخ نمی‌داد.

------------

## مرحله‌ی ۷: اصلاح کد و آزمون پس از اصلاح

### ۷.۱ توضیح دقیق اصلاح انجام‌شده

با بررسی دقیق‌تر کد مشخص شد که باگ off-by-one که در مراحل ۳ و ۴ کشف شد، در واقع **دو خطای هم‌پوشان** دارد، نه یکی:

```c
uint32_t bucket_index = i / STRESS_BUCKET_WIDTH;
if (bucket_index > STRESS_BUCKETS) {
    bucket_index = STRESS_BUCKETS;
}
legacy_buckets[bucket_index]++;
```

آرایه‌ی `buckets` دقیقاً `STRESS_BUCKETS` (۴) عضو دارد، یعنی اندیس‌های معتبر آن `0` تا `STRESS_BUCKETS - 1` (یعنی ۰ تا ۳) است.

1. **خطای اول (شرط کلمپ):** شرط با `>` نوشته شده، نه `>=`. در نتیجه وقتی `bucket_index` دقیقاً برابر `STRESS_BUCKETS` (۴) شود، شرط برقرار نمی‌شود و کلمپ انجام نمی‌شود.
2. **خطای دوم (مقدار کلمپ):** حتی اگر فقط شرط را به `>=` تغییر می‌دادیم، مقدار کلمپ‌شده هنوز `STRESS_BUCKETS` (یعنی ۴) است که خودش یک اندیس نامعتبر است (چون آخرین اندیس معتبر ۳ است)! یعنی حتی با اصلاح نصفه، کد باز هم به `legacy_buckets[4]` می‌نوشت.

بنابراین اصلاح صحیح نیازمند تغییر **هر دو** بخش بود: هم شرط مقایسه و هم مقدار جایگزین در کلمپ.

### ۷.۲ اصلاح اصلی (رفع ریشه‌ای باگ)

```c
uint32_t bucket_index = i / STRESS_BUCKET_WIDTH;
if (bucket_index >= STRESS_BUCKETS) {
    bucket_index = STRESS_BUCKETS - 1U;
}
legacy_buckets[bucket_index]++;
```

با این تغییر، برای هر مقدار ممکن `i` (که بین ۰ تا ۱۹۹ محدود است، طبق اعتبارسنجی `parse_u32`)، `bucket_index` همیشه در بازه‌ی معتبر `[0, STRESS_BUCKETS-1]` باقی می‌ماند و هیچ نوشتنی خارج از محدوده‌ی آرایه‌ی `buckets` رخ نمی‌دهد.

### ۷.۳ لایه‌ی دفاعی اضافه (Defense in Depth)

علاوه بر رفع ریشه‌ای باگ، با بررسی ساختار `stress_state_t` یک نکته‌ی جالب دیده شد: فیلد `guard` (با مقدار جادویی `0x5A17C0DE`) از ابتدا در کد وجود داشت، اما **هیچ‌جا خوانده یا اعتبارسنجی نمی‌شد** — یعنی طراح کد این کنری (canary) را برای تشخیص فساد حافظه در نظر گرفته بود ولی هرگز از آن استفاده نکرده بود.

برای مقاوم‌سازی بیشتر در برابر باگ‌های مشابه در آینده (نه فقط همین یک باگ)، دو تغییر دفاعی اضافه شد:

1. یک فیلد کنری‌ی جدید (`bucket_guard`) دقیقاً بین آرایه‌ی `buckets` و فیلد حساس `pending_mode` اضافه شد. این فیلد به‌عنوان یک «منطقه‌ی قربانی‌شونده» (sacrificial buffer) عمل می‌کند: اگر باگ مشابهی در آینده دوباره باعث نوشتن یک یا دو خانه فراتر از انتهای `buckets` شود، ابتدا این فیلد آسیب می‌بیند، نه مستقیماً `pending_mode`.
2. `health_monitor_task` اکنون هم `bucket_guard` و هم `guard` را با مقدار مرجع‌شان مقایسه می‌کند و در صورت مغایرت، پرچم سلامت جدیدی (`HEALTH_FLAG_BUCKET_GUARD_BAD`) را فعال می‌کند. به این ترتیب، حتی اگر یک باگ حافظه‌ی مشابه دوباره در کد ظاهر شود، به‌جای تغییر بی‌صدای یک متغیر ایمنی حساس، ابتدا یک هشدار صریح در `status`/`log` دیده می‌شود.

این افزوده صرفاً یک لایه‌ی دفاعی اضافی است و جایگزین رفع ریشه‌ای باگ (بخش ۷.۲) نیست. بلکه طبق اصل «دفاع در عمق» در امنیت سامانه‌های نهفته، حتی اگر رفع ریشه‌ای در آینده به‌درستی نگه‌داری نشود، این لایه احتمال بروز یک فساد حافظه‌ی بی‌صدا و کشف‌نشده را کاهش می‌دهد.

### ۷.۴ روش آزمون پس از اصلاح

برای اثبات رفع باگ، دقیقاً همان دنباله‌ی ورودی‌هایی که در مراحل ۳ و ۴ باعث تغییر ناخواسته می‌شدند، دوباره تکرار شد:

**نقطه‌ی پایش GDB :**
```
(gdb) monitor reset halt
(gdb) delete
(gdb) watch g_safety_level
(gdb) watch g_stress_state.pending_mode
(gdb) continue
```

**دنباله‌ی UART:**
```
reset
status
stress 80
status
stress 79
status
stress 81
status
stress 100
status
```


**خروجی UART بعد از اصلاح:**
```
> reset
OK reset
> status
device_id=1cdbd49955e8
mode=normal
safety_level=3
events=0
health_flags=0x00000000
heartbeat=1
health=ok
> stress 80
OK stress_queued count=80
> 
SENSOR stress_complete count=80 events=80

> status
device_id=1cdbd49955e8
mode=normal
safety_level=3
events=80
health_flags=0x00000000
heartbeat=7
health=ok
> stress 79
OK stress_queued count=79
> 
SENSOR stress_complete count=79 events=159
 
> status
device_id=1cdbd49955e8
mode=normal
safety_level=3
events=159
health_flags=0x00000000
heartbeat=14
health=ok
> stress 81
OK stress_queued count=81
> 
SENSOR stress_complete count=81 events=240

> status
device_id=1cdbd49955e8
mode=normal
safety_level=3
events=240
health_flags=0x00000000
heartbeat=19
health=ok
> stress 100
OK stress_queued count=100
> 
SENSOR stress_complete count=100 events=340

> status
device_id=1cdbd49955e8
mode=normal
safety_level=3
events=340
health_flags=0x00000000
heartbeat=23
health=ok
> 

```

**خروجی GDB:**
```
(gdb) i b 
Num     Type           Disp Enb Address    What
1       hw watchpoint  keep y              g_safety_level
        breakpoint already hit 2 times
2       hw watchpoint  keep y              g_stress_state.pending_mode
        breakpoint already hit 2 times
(gdb) continue 
Continuing.


^C[esp32s3.cpu0] Target halted, PC=0x4037848A, debug_reason=00000000
Set GDB target to 'esp32s3.cpu0'
[esp32s3.cpu1] Target halted, PC=0x40043A40, debug_reason=00000000

Thread 3 "IDLE" received signal SIGINT, Interrupt.
0x4037848a in esp_cpu_wait_for_intr () at /home/divar/.espressif/v6.0.1/esp-idf/components/esp_hw_support/cpu.c:64
64      }
```

**بررسی مستقیم حافظه با GDB بعد از `stress 100`:**
```
(gdb)  p g_stress_state
$1 = {buckets = {80, 80, 80, 100}, bucket_guard = 2965694369, pending_mode = 0, guard = 1511506142}
(gdb) p g_device_mode
$2 = 0
(gdb) p g_safety_level
$3 = 3
(gdb) p g_health_flags
$4 = 0
```

### ۷.۵ مشاهدات

- مجموع افزایش چهار عضو `buckets`  **دقیقاً برابر** با تعداد رخداد ارسالی هستند (80+80+80+100=79+80+81+100).
- `g_stress_state.pending_mode`  همیشه `0` مانده است و هرگز به مقدار غیرصفر تبدیل نشده است.
- `g_device_mode` و `g_safety_level` به دلیل عدم ارسال صریح `setmode` **ثابت** مانده اند (`normal` و `3`).
- `g_health_flags` صفر است. 
- هیچ‌کدام از دو برک‌پوینت (`g_safety_level` و `g_stress_state.pending_mode`) در طول اجرای دنباله‌ی بالا فعال نشدند. یعنی `continue` بدون وقفه تا پایان آزمایش ادامه یافت. در انتها با control^C متوقف شد تا مقادیر را مشاهده و ثبت کنیم.
- فیلدهای کنری `guard` و `bucket_guard` مقدار اولیه‌ی خود را حفظ کردند.

همان‌طور که در خروجی‌های بالا دیده می‌شود، پس از اصلاح، وضعیت حساس دیگر به‌صورت ناخواسته تغییر نکرد و هیچ‌کدام از watchpointها فعال نشدند، که رفع کامل باگ off-by-one را تأیید می‌کند.

---

##   مرحله‌ی ۸: تحلیل امنیتی و مقاوم‌سازی

### چرا JTAG در زمان توسعه مفید است اما در محصول نهایی می‌تواند خطرناک باشد؟

در طول این تمرین، JTAG دقیقاً همان چیزی بود که امکان کشف باگ off-by-one در `sensor_task` را فراهم کرد: بدون آن، تنها می‌دیدیم که گاهی `mode` و `safety_level` بدون فرمان `setmode` تغییر می‌کنند، اما هرگز نمی‌فهمیدیم *چرا*. با `watch g_safety_level` و بررسی `bt`/`frame`/`list` در لحظه‌ی دقیق نوشتن مقدار، توانستیم مسیر اجرایی خطا را تا سطح یک خط کد (`legacy_buckets[bucket_index]++` در `sensor_task`) دنبال کنیم.

دقیقاً همین قابلیت‌ها — توقف دلخواه پردازنده، خواندن/نوشتن مستقیم هر آدرس حافظه، نصب نقطه‌ی توقف/پایش روی هر متغیر سراسری، و اجرای گام‌به‌گام کد — اگر در دست یک مهاجم با دسترسی فیزیکی به محصول نهایی قرار گیرد، ابزاری کامل برای:

- استخراج کامل محتوای RAM (شامل کلیدهای رمزنگاری، توکن‌های احراز هویت، یا داده‌های حساس کاربر که در حافظه‌ی زنده نگه داشته می‌شوند)
- دور زدن هر بررسی نرم‌افزاری (مثلاً با `set variable g_safety_level = 3` می‌توان مستقیماً هر بررسی ایمنی/مجوز را در حافظه بازنویسی کرد، بدون آنکه هیچ مسیر مجاز برنامه طی شود)
-  مهندسی معکوس کامل منطق ثابت‌افزار با دیدن اجرای زنده‌ی هر خط کد

فراهم می‌کند. تفاوت اصلی محیط توسعه و محصول نهایی این است که در توسعه، تهدید مهاجم مطرح نیست (خود ما داریم دیباگ می‌کنیم)، اما در محصول نهایی، هر کسی که به پورت USB/JTAG دستگاه دسترسی فیزیکی داشته باشد می‌تواند از همین قابلیت‌ها علیه دستگاه استفاده کند.

### تفاوت نسخه‌ی ساخت عیب‌یابی و نسخه‌ی ساخت انتشار از نظر اطلاعات قابل مشاهده چیست؟

در طول این تمرین، چند نمونه‌ی عینی از این تفاوت دیده شد:

- **نمادهای دیباگ (debug symbols):** GDB توانست دقیقاً نام تابع (`control_task`)، نام فایل و شماره‌ی خط (`main.c:417`) و حتی نام متغیرهای محلی و سراسری (`g_stress_state`, `g_safety_level`) را نشان دهد، چون فایل ELF ساخت شده با اطلاعات دیباگ کامل (`-g`) کامپایل شده بود. در یک نسخه‌ی انتشار واقعی، این جدول نمادها معمولاً حذف (strip) می‌شود. در نتیجه حتی اگر مهاجم بتواند JTAG را متصل کند، فقط آدرس‌های خام حافظه و دستورالعمل‌های اسمبلی را می‌بیند، نه نام تابع یا خط کد مبدأ.
- **لاگ‌های تفصیلی:** در این ثابت‌افزار، پیام‌هایی مثل `SENSOR stress_complete count=... events=...` یا `ALERT health flags=...` مستقیماً روی UART چاپ می‌شدند و اطلاعات داخلی دقیقی از وضعیت سیستم افشا می‌کردند. یک نسخه‌ی انتشار باید سطح لاگ را به حداقل (فقط خطاهای بحرانی، بدون جزئیات داخلی) کاهش دهد.
- **دسترسی به متغیرهای خام حافظه:** در مرحله‌ی ۲ دیدیم که فرمان `status` تنها زیرمجموعه‌ای از حافظه‌ی واقعی را نشان می‌دهد، اما GDB به تمام حافظه (شامل `g_cli_command_count`، جزئیات دقیق `stress_state_t`، و غیره) دسترسی کامل داشت. در نسخه‌ی انتشار، این «نمای کامل» باید اصلاً از طریق هیچ رابطی (حتی JTAG) در دسترس نباشد.

به‌طور خلاصه، نسخه‌ی ساخت عیب‌یابی *همه‌چیز* را برای راحتی توسعه‌دهنده افشا می‌کند، در حالی‌که نسخه‌ی انتشار باید طبق اصل حداقل افشا (least disclosure) طراحی شود: فقط همان اطلاعاتی که کاربر نهایی/اپراتور واقعاً به آن نیاز دارد (مثل `status` محدود موجود در این ثابت‌افزار)، نه جزئیات داخلی پیاده‌سازی.

### چرا رمزنگاری و احراز اصالت جایگزین ایمنی حافظه نمی‌شوند؟

رمزنگاری و احراز اصالت (authentication) مسائلی را حل می‌کنند که **کاملاً متفاوت** از باگی هستند که در این تمرین پیدا کردیم. باگ off-by-one در `sensor_task` هیچ ربطی به این نداشت که آیا داده رمزنگاری شده یا احراز هویت شده است. این باگ یک خطای منطقی محض در محاسبه‌ی اندیس آرایه بود که باعث شد برنامه‌ی *کاملاً معتبر و مجاز* به حافظه‌ی خودش (نه حافظه‌ی یک برنامه‌ی دیگر یا یک کانال ارتباطی) به‌شکل نادرست بنویسد.

به‌طور مشخص:

- **رمزنگاری** تضمین می‌کند که داده در حال انتقال یا ذخیره‌سازی برای یک ناظر بیرونی قابل خواندن نباشد. اما وقتی داده در RAM باز شده و برنامه در حال پردازش آن است، رمزنگاری هیچ کمکی به جلوگیری از نوشتن خارج از محدوده‌ی یک آرایه‌ی محلی نمی‌کند. چون در آن لحظه داده از حالت رمزشده خارج شده است.
- **احراز اصالت** تضمین می‌کند فرستنده‌ی یک پیام یا کد همان کسی است که ادعا می‌کند (مثلاً امضای دیجیتال فریمور). اما در این تمرین، فرمان `stress 81` کاملاً از یک منبع مجاز (کاربر UART) و با مقدار کاملاً معتبر (کمتر از سقف ۲۰۰ در `parse_u32`) ارسال شده بود. مشکل هرگز در اعتبار *فرستنده* نبود، بلکه در نحوه‌ی *پردازش داخلی* یک ورودی کاملاً مجاز بود.

به همین دلیل، ایمنی حافظه (memory safety) — یعنی تضمین اینکه هر دسترسی به حافظه دقیقاً در محدوده‌ی مجاز خودش انجام شود — یک لایه‌ی دفاعی کاملاً مستقل و مکمل است، نه جایگزین‌شدنی با رمزنگاری/احراز اصالت. حتی امن‌ترین سامانه‌ی رمزنگاری‌شده و احرازشده‌ی دنیا، اگر منطق داخلی‌اش یک باگ سرریز بافر ساده داشته باشد (مثل همینی که در `sensor_task` بود)، همچنان می‌تواند به همان شکلی که در مرحله‌ی ۵ نشان دادیم (تغییر بی‌صدای `pending_mode` و در نتیجه `safety_level`) دچار فساد وضعیت داخلی شود.

### اگر رمزگذاری حافظه‌ی فلش فعال باشد، آیا مشاهده‌ی RAM با ابزار عیب‌یابی هنوز مسئله است؟ چرا؟

بله، کاملاً مسئله است. **رمزگذاری فلش (Flash Encryption)** روی ESP32-S3 فقط محتوای *ذخیره‌شده در حافظه‌ی فلش خارجی/داخلی* را رمزنگاری می‌کند (کد برنامه و داده‌های ثابت که روی دیسک نگه داشته می‌شوند)، تا اگر کسی مستقیماً تراشه‌ی فلش را بخواند یا فایل باینری را دامپ کند، محتوای خام و قابل‌فهم به دست نیاورد.

اما در طول اجرای برنامه، تمام این داده در **RAM به‌صورت رمزگشایی‌شده و خام** قرار دارد — دقیقاً همان چیزی که ما در تمام این تمرین با GDB خواندیم (مثلاً `g_safety_cookie`، `g_stress_state`، یا حتی محتوای پشته و رجیسترها در `info registers`). واحد دیباگ سخت‌افزاری (JTAG debug unit) در سطح هسته‌ی پردازنده کار می‌کند، یعنی *بعد* از اینکه داده از فلش رمزگشایی و در RAM بارگذاری شده. بنابراین:

- رمزگذاری فلش، دسترسی آفلاین به تراشه‌ی حافظه (مثلاً با یک پروگرامر SPI خارجی) را بی‌فایده می‌کند.
- اما JTAG هنوز می‌تواند حین اجرا، مستقیماً حافظه‌ی زنده و رمزگشایی‌شده را بخواند — دقیقاً همان‌طور که ما `g_safety_cookie` (که مقدار جادویی `0xCE40876A` داشت) را مستقیماً از RAM با `p/x g_safety_cookie` خواندیم، بدون هیچ نیازی به شکستن رمزنگاری فلش.

پس رمزگذاری فلش و کنترل دسترسی JTAG **دو مسئله‌ی کاملاً مستقل** هستند. رمزگذاری فلش تهدید «سرقت فیزیکی چیپ حافظه» را می‌پوشاند، اما تهدید «دسترسی زنده به حافظه‌ی در حال اجرا از طریق JTAG» را نه.

### اگر راه‌اندازی امن فعال باشد، باز بودن رابط عیب‌یابی چه ریسکی ایجاد می‌کند؟

**راه‌اندازی امن (Secure Boot)** تضمین می‌کند که فقط کد امضاشده و تأییدشده توسط کلید مورد اعتماد سازنده روی دستگاه اجرا شود — یعنی جلوی جایگزینی فریمور با یک نسخه‌ی مخرب یا دستکاری‌شده در زمان بوت را می‌گیرد.

اما اگر رابط JTAG هم‌زمان باز باشد، Secure Boot عملاً بی‌اثر می‌شود، چون مهاجم دیگر نیازی به جایگزین‌کردن فریمور ندارد. او می‌تواند:

- همان فریمور امضاشده و مجاز را اجرا کند، اما با JTAG آن را **حین اجرا** دستکاری کند — دقیقاً مثل کاری که ما در این تمرین با `set variable`، تغییر مستقیم رجیسترها، یا نصب watchpoint روی متغیرهای حساس (`g_safety_level`) انجام دادیم.
- با نصب breakpoint روی هر نقطه از کد امضاشده، جریان اجرا را متوقف کند، رجیسترها یا حافظه را بازنویسی کند (مثلاً پرش از یک بررسی مجوز)، و سپس اجرا را ادامه دهد — بدون آنکه هرگز کد امضانشده‌ای وارد سیستم شود.
- کل حافظه‌ی RAM را بخواند و کلیدها یا داده‌های حساسی که فقط باید در حافظه‌ی داخلی و ایزوله باقی بمانند را استخراج کند.

به بیان دیگر، Secure Boot از یکپارچگی *کد در زمان بارگذاری* محافظت می‌کند، اما هیچ کنترلی روی یکپارچگی *اجرا در زمان دیباگ* ندارد. اگر JTAG باز بماند، این یعنی مهاجم یک «درب پشتی» کاملاً قانونی و رسمی (که خود سازنده برای دیباگ گذاشته) در اختیار دارد که کاملاً Secure Boot را دور می‌زند. دقیقاً مثل قفل‌کردن در جلوی خانه اما باز گذاشتن پنجره‌ی پشتی.

### برای محصول واقعی چه سیاستی برای JTAG، لاگ‌ها، نمادهای برنامه و نسخه‌های ساخت عیب‌یابی پیشنهاد می‌کنید؟

بر اساس تجربه‌ی این تمرین، سیاست پیشنهادی برای یک محصول واقعی مبتنی بر ESP32-S3 به شرح زیر است:

**۱. غیرفعال‌سازی دائمی JTAG در نسخه‌ی انتشار (نه در توسعه):**
- استفاده از eFuse مخصوص غیرفعال‌سازی JTAG (`DIS_USB_JTAG` / `HARD_DIS_JTAG` روی ESP32-S3) پیش از توزیع نهایی محصول، تا رابط دیباگ به‌طور فیزیکی و برگشت‌ناپذیر بسته شود.
- در صورت نیاز به دیباگ میدانی محدود (مثلاً برای پشتیبانی)، استفاده از یک مکانیزم احراز هویت جداگانه (مثل JTAG محافظت‌شده با رمز عبور که برخی تراشه‌ها پشتیبانی می‌کنند)، نه باز گذاشتن کامل و بدون قید JTAG.

**۲. جداسازی کامل نسخه‌ی ساخت دیباگ از نسخه‌ی انتشار:**
- کامپایل نسخه‌ی انتشار بدون فلگ `-g` و با `strip` کامل نمادها، تا حتی در صورت دسترسی فیزیکی، فایل باینری هیچ نام تابع/متغیر/خط کدی افشا نکند.
- غیرفعال‌سازی کامل تمام کدهای اختصاصی دیباگ (مثل توابع تست، فرمان‌های مخفی، یا نقاط ورودی تشخیصی) با `#ifdef DEBUG_BUILD` قبل از ساخت نسخه‌ی نهایی.

**۳. کاهش سطح لاگ‌ها به حداقل لازم:**
- در این ثابت‌افزار، `printf` مستقیماً جزئیات داخلی (مثل شمارنده‌ی دقیق `stress_state.buckets`) را افشا می‌کرد. در نسخه‌ی انتشار، سطح لاگ باید به رویدادهای بحرانی امنیتی/ایمنی محدود شود (مثلاً فقط `health=alert`، بدون جزئیات پرچم‌ها یا مقادیر خام داخلی).
- لاگ‌های حساس باید یا اصلاً چاپ نشوند یا فقط در حافظه‌ی داخلی رمزنگاری‌شده و قابل‌دسترسی از طریق یک کانال احرازشده ذخیره شوند.

**۴. فعال‌سازی مکمل Secure Boot + Flash Encryption:**
- همان‌طور که در پاسخ‌های بالا توضیح داده شد، این دو مکانیزم مکمل یکدیگرند، اما هیچ‌کدام جایگزین بستن JTAG نیستند. باید هر سه هم‌زمان (Secure Boot + Flash Encryption + JTAG disable) در محصول نهایی فعال باشند.

**۵. ایمنی حافظه به‌عنوان یک الزام مستقل در فرایند توسعه:**
- صرف‌نظر از رمزنگاری/احراز اصالت/وضعیت JTAG، تمام دسترسی‌های آرایه باید با ابزارهایی مثل static analysis (`cppcheck`, `clang-tidy`)، fuzz testing ورودی‌های عددی (شبیه آنچه در `stress <n>` دیدیم)، و بازبینی کد (code review) با تمرکز ویژه روی محاسبات اندیس، قبل از انتشار بررسی شوند — دقیقاً همان نوع باگی که در این تمرین با یک بازبینی دقیق‌تر کد (`bucket_index > STRESS_BUCKETS` به‌جای `>=`) قابل کشف بود، پیش از رسیدن به تولید.

---

## تست‌های پیشنهادی (نتایج نهایی)

| سناریو                               | خروجی مورد انتظار                           | نتیجه‌ی واقعی                                                                                                                                        |
|--------------------------------------|---------------------------------------------|------------------------------------------------------------------------------------------------------------------------------------------------------|
| اتصال OpenOCD و GDB                  | برنامه قابل توقف و ادامه‌دادن باشد          | ✅ در مرحله‌ی ۱ با `target remote :3333` و `monitor reset halt` اتصال برقرار شد و `continue`/توقف به‌درستی کار کرد.                                   |
| خواندن ثبات‌ها و ردیابی پشته         | خروجی واقعی در گزارش ثبت شود                | ✅ خروجی کامل `info registers` و چند نمونه `bt` (با سوییچ صحیح بین تردها) در مرحله‌ی ۱ ثبت شد.                                                        |
| اجرای status و مشاهده‌ی وضعیت با GDB | مقدارهای UART و RAM مقایسه شوند             | ✅ جدول مقایسه‌ی کامل در مرحله‌ی ۲؛ نشان داده شد `status` فقط زیرمجموعه‌ای از حافظه‌ی واقعی را نشان می‌دهد.                                           |
| اجرای ورودی‌های stress               | تغییر ناخواسته بازتولید شود                 | ✅ در مرحله‌ی ۳ با دنباله‌ی `stress 10 → stress 100`، تغییر ناخواسته‌ی `mode`/`safety_level`/`health_flags` بازتولید و مستند شد.                      |
| استفاده از نقطه‌ی پایش               | محل تغییر وضعیت مشخص شود                    | ✅ در مرحله‌ی ۴ با `watch g_safety_level` و آستانه‌ی دقیق (`stress 80/79` بی‌اثر، `stress 81` مؤثر)، محل دقیق در `control_task` خط ۴۱۷ شناسایی شد.    |
| تحلیل وظیفه و پشته                   | مسیر اجرایی خطا توضیح داده شود              | ✅ در مرحله‌ی ۵ تمایز بین محل نوشتن نهایی (`control_task`) و منشأ واقعی خطا (`sensor_task`) کامل توضیح داده شد.                                       |
| آزمون پس از اصلاح                    | تغییر ناخواسته دیگر رخ ندهد یا کنترل شود    | ✅ در مرحله‌ی ۷ دقیقاً همان دنباله‌ی خطاساز (۸۰،۷۹،۸۱،۱۰۰) دوباره اجرا شد و هیچ watchpointای فعال نشد؛ جمع buckets دقیقاً برابر با تعداد رخدادها بود. |
| توقف پردازنده با ابزار عیب‌یابی      | اثر روی تپش سلامت یا پایشگر سلامت تحلیل شود | ✅ در مرحله‌ی ۶ نشان داده شد توقف طولانی CPU باعث فعال‌شدن کاذب `HEALTH_FLAG_HEARTBEAT_STALE` می‌شود، بدون هیچ باگی در کد.                            |


---

## چالش‌ها:

### مشکل ترمینال UART

**۱. اتصال به پورت اشتباه**

در تلاش‌های اولیه، ترمینال UART به درستی کار نمی‌کرد. بعد از مقدار خوبی آزمون و خطا متوجه شدم که اتصال من به پورت
اشتباهی از دستگاه بوده است. با تعویض پورت به پورت صحیح (USB-Serial-JTAG)، این مشکل برطرف شد اما مشکل دیگری نمایان شد.

**۲. پردازش کاراکتر به کاراکتر ورودی**

به دلیل مدل بافرینگ پیاده‌سازی‌شدهٔ پیش‌فرض کنسول روی JTAG دستگاه، ورودی به‌صورت کاراکتر به کاراکتر خوانده و پردازش
می‌شد. یعنی هر بار که `fgets()` فراخوانی می‌شد، تنها یک کاراکتر از بافر خوانده می‌شد و به‌عنوان یک "خط" کامل در نظر
گرفته می‌شد. در نتیجه به ازای هر حرف از دستور وارد‌شده (مثلاً `h`، `e`، `l`، `p` به‌جای `help`)، پیغام
`ERR unknown_command` چاپ می‌شد.

علت این رفتار این بود که درایور پیش‌فرض کنسول (VFS پایه‌ی ROM) به‌صورت non-blocking عمل می‌کرد و به محض نبود داده‌ی
بیشتر در بافر، خواندن را با همان یک بایت موجود تمام می‌کرد.

این مشکل با نصب و فعال‌سازی صریح درایور `usb_serial_jtag` و استفاده از VFS مبتنی بر آن (به‌جای درایور پیش‌فرض
پولینگ‌محور) حل شد. به این ترتیب که با فراخوانی `usb_serial_jtag_driver_install()` و سپس
`usb_serial_jtag_vfs_use_driver()`، خواندن از ورودی به‌صورت بلاک‌شونده (blocking) و تا رسیدن کاراکتر پایان خط انجام
می‌گرفت.

**۳. عدم نمایش (اکو) کاراکترهای ورودی**

بعد از حل شدن این مشکل، متوجه شدم دستورهایی که تایپ می‌کنم روی ترمینال نمایش داده نمی‌شوند (شبیه به وارد کردن رمز عبور).
دلیل این موضوع این بود که VFS مبتنی بر درایور، بر‌خلاف کنسول پیش‌فرض، هیچ echo خودکاری برای کاراکترهای ورودی انجام
نمی‌دهد و صرفاً یک جریان خام از بایت‌ها را در اختیار برنامه قرار می‌دهد.

این مشکل با تغییر رویهٔ خواندن خط از ورودی حل شد. به‌جای استفاده مستقیم از `fgets()`، یک تابع سفارشی برای خواندن نوشتم
که هر کاراکتر را جداگانه با `getchar()` می‌خواند، بلافاصله با `putchar()` آن را روی خروجی نمایش می‌دهد (echo دستی)، و
همچنین کاراکتر backspace را به‌درستی مدیریت می‌کند تا امکان ویرایش خط قبل از ارسال دستور نیز فراهم باشد.

## چالش: خطای دسترسی USB در OpenOCD

**مشکل:**

هنگام اجرای دستور `openocd -f board/esp32s3-builtin.cfg`، با خطای زیر مواجه شدم:

```
Error: libusb_open() failed with LIBUSB_ERROR_ACCESS
Error: esp_usb_jtag: could not find or open device!
```

OpenOCD دستگاه ESP32-S3 را با VID و PID درست (`303a:1001`) شناسایی می‌کرد، اما به دلیل نداشتن مجوز دسترسی مستقیم کاربر
به دستگاه USB، نمی‌توانست آن را باز کند. این مشکل معمولاً در سیستم‌های لینوکسی رخ می‌دهد چون دسترسی به دستگاه‌های USB
به‌صورت پیش‌فرض محدود به کاربر `root` است.

**راه‌حل:**

این مشکل با ایجاد یک قانون udev برطرف شد که به دستگاه با VID/PID مشخص، مجوز دسترسی کامل می‌دهد.

دستورهایی که اجرا شد:

```bash
sudo nano /etc/udev/rules.d/99-esp32.rules
```

و محتوای زیر داخل فایل نوشته شد:

```
SUBSYSTEM=="usb", ATTR{idVendor}=="303a", ATTR{idProduct}=="1001", MODE="0666"
```

سپس برای اعمال قانون جدید:

```bash
sudo udevadm control --reload-rules
sudo udevadm trigger
```

در نهایت بورد قطع و دوباره وصل شد، و پس از آن دستور `openocd` بدون نیاز به `sudo` با موفقیت اجرا شد.



### چالش: خطای «Could not insert hardware watchpoint»

هنگام تلاش برای گذاشتن نقطه‌ی پایش (watchpoint) روی متغیر `g_safety_level` در مرحله‌ی ۴، با خطای زیر مواجه شدم:

```
(gdb) watch g_safety_level
Hardware watchpoint 5: g_safety_level
(gdb) continue
Continuing.
Warning:
Could not insert hardware watchpoint 5.
Could not insert hardware breakpoints:
You may have requested too many hardware breakpoints/watchpoints.

Command aborted.
```

**علت احتمالی:** واحد دیباگ سخت‌افزاری پردازنده (که watchpointها را با نوشتن در چند رجیستر ویژه‌ی آن پیاده‌سازی می‌کند) از سشن قبلی GDB/OpenOCD در وضعیت ناسازگار یا کش‌شده باقی مانده بود. علاوه بر آن، پردازنده هنگام تلاش برای درج watchpoint در حالت کاملاً پایدار و شناخته‌شده متوقف نبود.

**راه‌حل:** با اجرای دستور زیر قبل از گذاشتن مجدد watchpoint، مشکل برطرف شد:

```
(gdb) monitor reset halt
(gdb) watch g_safety_level
(gdb) continue
```

دستور `monitor reset halt` مستقیماً به OpenOCD ارسال می‌شود و باعث می‌شود تراشه کاملاً ریست شده (مانند فشردن دکمه‌ی ریست فیزیکی) و بلافاصله پیش از اجرای اولین دستورالعمل متوقف نگه داشته شود. این کار رجیسترهای دیباگ سخت‌افزاری را به حالت پیش‌فرض و پاک بازمی‌گرداند و هرگونه ناسازگاری بین وضعیت کش‌شده‌ی OpenOCD و وضعیت واقعی تراشه را برطرف می‌کند، به‌طوری‌که GDB می‌تواند دوباره از صفر watchpoint را با موفقیت ثبت کند.

این تجربه با نکته‌ای که در راهنمای خطاهای رایج صورت تمرین هم اشاره شده هم‌خوانی دارد: تعداد نقطە های پایش سخت افزاری محدود است. هم زمان نقطە های پایش زیاد نگذارید. به همین دلیل، پیش از تکرار آزمایش‌ها، همیشه ابتدا با `i b` مطمئن شدم breakpoint یا watchpoint اضافی از قبل فعال نمانده باشد.



## چالش: عدم توقف (Halt) هماهنگ هسته‌ها و شکست در ثبت Watchpoint

هنگام دیباگ برنامه روی ESP32-S3 (که دو هسته دارد: `cpu0` و `cpu1`)، با رفتار عجیبی مواجه شدم: با اینکه یکی از هسته‌ها
(`cpu1`) به‌درستی متوقف می‌شد، هسته‌ی دیگر (`cpu0`) همچنان در حال اجرا باقی می‌ماند و در نتیجه ثبت watchpoint با شکست
مواجه می‌شد:

```
Info : [esp32s3.cpu1] Target halted, PC=0x420093E2, debug_reason=00000002
Info : Set GDB target to 'esp32s3.cpu1'
Error: [esp32s3.cpu0] not halted
Error: [esp32s3.cpu0] not halted
Error: [esp32s3.cpu0] can't add write watchpoint at 0x3fc91000, target not halted
```

و در سمت GDB:

```
(gdb) watch g_stress_state.pending_mode
Hardware watchpoint 30: g_stress_state.pending_mode
(gdb) continue
Continuing.
[esp32s3.cpu0] not halted
[esp32s3.cpu0] not halted
Warning:
Could not insert hardware watchpoint 30.
Could not insert hardware breakpoints:
You may have requested too many hardware breakpoints/watchpoints.
```

**علت:** برخلاف تصور اولیه، مشکل به تعداد watchpointها ربطی نداشت. علت واقعی این بود که در حالت SMP (چند‌هسته‌ای)،
OpenOCD باید هر دو هسته را هم‌زمان متوقف کند تا بتواند رجیسترهای دیباگ سخت‌افزاری را که برای watchpoint/breakpoint
لازم است، بنویسد. اما در عمل، توقف یک هسته گاهی زودتر از هسته‌ی دیگر اتفاق می‌افتاد (یا اصلاً به آن broadcast نمی‌شد)،
و در نتیجه OpenOCD با دیدن این‌که `cpu0` هنوز در حال اجراست، از نوشتن رجیسترهای watchpoint امتناع می‌کرد.

این وضعیت با یک مشکل دیگر هم تشدید می‌شد: یکی از هسته‌ها اغلب داخل تابع `esp_cpu_wait_for_intr()` (که دستورالعمل
`WAITI` را اجرا می‌کند) بود:

```
0x40378c4a in esp_cpu_wait_for_intr () at .../esp_hw_support/cpu.c:64
```

پردازنده در این حالت کم‌مصرف (idle/wait-for-interrupt)، پاسخ‌دهی کندتر و غیرقابل‌پیش‌بینی‌تری به درخواست‌های توقف
JTAG دارد. همین موضوع باعث می‌شد دو هسته با تأخیر نسبت به هم متوقف شوند.

علاوه بر این، در بعضی مواقع خود لینک USB-JTAG داخلی تراشه به‌طور موقت قطع می‌شد:

```
libusb_bulk_write error: LIBUSB_ERROR_NO_DEVICE
esp_usb_jtag: device not found!
esp_usb_jtag: failed to revive USB device!
[esp32s3.cpu0] Failed to read debug stubs address location (-104)!
[esp32s3.cpu0] Polling failed, trying to reexamine
```

که باعث می‌شد OpenOCD مجبور به شناسایی دوباره‌ی تراشه شود و هرگونه هماهنگی زمانی بین دستورهای GDB و وضعیت واقعی
هسته‌ها از بین برود.

**راه‌حل:** برای رفع این مشکل، تصمیم گرفتم پردازنده را در حالت تک‌هسته‌ای (Unicore) اجرا کنم تا اساساً رقابت
(race) بین دو هسته برای توقف هم‌زمان از بین برود. این کار با فعال‌سازی گزینه‌ی زیر در `sdkconfig.defaults` انجام شد:

```
CONFIG_FREERTOS_UNICORE=y
```

سپس با اجرای دستورهای زیر، تنظیمات از نو اعمال شد:

```bash
rm sdkconfig
idf.py reconfigure
idf.py build flash
```

و برای اطمینان از اعمال شدن تنظیمات:

```bash
grep UNICORE sdkconfig
```

با این تغییر، تمام تسک‌های FreeRTOS و برنامه فقط روی `cpu0` اجرا می‌شوند و `cpu1` اصلاً راه‌اندازی نمی‌شود. در نتیجه
دیگر نیازی به هماهنگ‌سازی توقف بین دو هسته نبود، breakpoint و watchpointها همیشه با موفقیت ثبت می‌شدند، و خطاهای
`not halted` و `Could not insert hardware watchpoint` به‌طور کامل برطرف شدند.

نکته‌ی جانبی مهم: پیش از اعمال این تغییر، لازم بود کد پروژه از نظر استفاده از `xTaskCreatePinnedToCore(..., 1)` یا
هر نوع پین‌کردن صریح تسک به `cpu1` بررسی شود. چون در حالت Unicore، آن هسته اصلاً بوت نمی‌شود و هر تسکی که به‌طور
انحصاری به آن پین شده باشد، هرگز اجرا نخواهد شد.

چون اگر از esp32-c3 استفاده میکردم اصلا چنین مساله‌ای وجود نمی‌داشت(ذات تک-کور بودن این چیپ) این تصمیم گرفته شد.

