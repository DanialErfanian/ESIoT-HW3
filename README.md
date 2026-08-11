# Exercise 3 — Dynamic Analysis and Memory-Error Discovery with JTAG (CE-40876 / E3)

**Student:** Danial Erfanian  
**Student ID:** 402211745  
**Course:** Embedded Systems and IoT Security — Sharif University of Technology

---

## Execution Environment

| Item | Value |
|---|---|
| Board and chip | ESP32-S3 (QFN56), revision v0.2 |
| JTAG connection | Built-in USB |
| Operating system | Ubuntu 24.04 LTS |
| ESP-IDF version | ESP-IDF v6.0.1-dirty |
| GDB | GNU gdb (esp-gdb) 16.3_20250913 |
| OpenOCD | Open On-Chip Debugger v0.12.0-esp32-20260304 (2026-03-04-20:07) |
| Serial port | `/dev/ttyACM0` |

---

# Stage 1: Initial Setup and Evidence Collection

## Build and Flash

```bash
cd student_starter
idf.py set-target esp32s3
idf.py build
idf.py -p /dev/ttyACM0 flash monitor
```

## Help and Status Output After Startup

```text
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

## OpenOCD Connection

```text
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

## GDB Connection

During the first attempt, no execution activity was visible in GDB. The reason was that the following command had to be executed inside GDB:

```text
target remote :3333
```

After executing this command, OpenOCD detected the GDB connection, as shown in the log above:

```text
Info : accepting 'gdb' connection on tcp/3333
```

This enabled communication between OpenOCD and GDB and made it possible to read registers and inspect the live stack on the board.

## `info registers`

```text
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

## `bt` — Sample Stack Trace

```text
(gdb) bt
#0  0x40378c4a in esp_cpu_wait_for_intr () at /home/divar/.espressif/v6.0.1/esp-idf/components/esp_hw_support/cpu.c:64
#1  0x4200eefd in esp_vApplicationIdleHook () at /home/divar/.espressif/v6.0.1/esp-idf/components/esp_system/freertos_hooks.c:58
#2  0x4200ff0d in prvIdleTask (pvParameters=0x0) at /home/divar/.espressif/v6.0.1/esp-idf/components/freertos/FreeRTOS-Kernel/tasks.c:4350
#3  0x4200f479 in vPortTaskWrapper (pxCode=0x4200ff04 <prvIdleTask>, pvParameters=0x0) at /home/divar/.espressif/v6.0.1/esp-idf/components/freertos/FreeRTOS-Kernel/portable/xtensa/port.c:143
```

```text
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
```

```text
(gdb) thread 4
[Switching to thread 4 (Thread 1070520696)]
#0  health_monitor_task (argument=0x0) at /home/divar/Desktop/Uni/1.ESP/HW3/student_starter/main/main.c:444
444             uint32_t current_flags = g_health_flags & HEALTH_FLAG_UNEXPECTED_MODE;
```

```text
(gdb) bt
#0  health_monitor_task (argument=0x0) at /home/divar/Desktop/Uni/1.ESP/HW3/student_starter/main/main.c:444
#1  0x4200f479 in vPortTaskWrapper (pxCode=0x42009088 <health_monitor_task>, pvParameters=0x0) at /home/divar/.espressif/v6.0.1/esp-idf/components/freertos/FreeRTOS-Kernel/portable/xtensa/port.c:143
```

### Important Note About FreeRTOS Threads

As shown by the `info threads` output, several threads run concurrently in a FreeRTOS/ESP-IDF application. Some of them belong to the operating system and infrastructure, such as `IDLE0`, `IDLE1`, `ipc0`, and `ipc1`, and are unrelated to the application logic.

If `bt` is executed directly, GDB displays the stack trace of the current thread, which is often an `IDLE` thread. This usually provides no useful information about the location of an application-level fault.

The correct debugging procedure is:

1. Use `info threads` to list all threads.
2. Identify the application-related threads, such as `health_monitor_task`, `control_task`, and `heartbeat_task`.
3. Switch to the desired thread with `thread <number>`, for example:

   ```text
   (gdb) thread 4
   ```

4. Run `bt` to display the call stack for that specific thread.

This method focuses the analysis on the application tasks instead of unrelated operating-system threads.

---

# Stage 2: Observing Dynamic Program States

## GDB Commands

```text
(gdb) info variables g_
(gdb) p/x &g_safety_cookie
(gdb) p/x g_safety_cookie
(gdb) p g_device_mode
(gdb) p g_safety_level
(gdb) x/32wx &g_safety_cookie
```

## Output

```text
(gdb) p/x &g_safety_cookie
$1 = 0x3fc9100c

(gdb) p/x g_safety_cookie
$2 = 0xce40876a

(gdb) p g_device_mode
$3 = 0

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
```

```text
(gdb) p g_cli_command_count
$6 = 40
```

## UART Comparison

### Commands and Output

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

## Variable Comparison

| Variable | Visible through UART? | Changed? | Explanation |
|---|---:|---:|---|
| `g_safety_cookie` | Yes, through `log` | No | Always remained `0xce40876a`. |
| `g_device_mode` | Yes, through `status` and `log` | Yes, with `setmode` | UART displays text; GDB displays the raw numeric value. |
| `g_safety_level` | Yes, through `status` and `log` | Yes, with `setmode` | Changes between service and normal modes. |
| `g_event_counter` | Yes, through `status` and `log` | Yes, with `sample` | Changed from 5 to 7. |
| `g_health_flags` | Yes, through `status` and `log` | No | Remained zero because no error occurred. |
| `g_heartbeat` | Yes, through `status`, `log`, and `heartbeat` | Yes, independently of commands | Increased over time. |
| `g_stress_state` | Yes, through `log` | No | Buckets and guard remained unchanged. |
| `g_cli_command_count` | **No** | Yes, with every command | Visible only through GDB. |
| `g_last_heartbeat_us` | Yes, through `heartbeat` | Yes | Not shown by `status` or `log`. |

## Summary of Stage 2

Comparing GDB and UART output showed that the `status` command displays only a limited subset of the actual device memory: `device_id`, `mode`, `safety_level`, `events`, `health_flags`, and `heartbeat`.

The `log` command exposes somewhat more information, including the raw cookie, numeric mode, and `stress_state` details. However, variables such as `g_cli_command_count` and `g_last_health_alert_us` do not appear in either UART command and can only be discovered through GDB.

Some values, such as `g_heartbeat`, change independently of user commands and depend only on elapsed time. Other values, such as `g_event_counter`, `g_device_mode`, and `g_safety_level`, change directly in response to their corresponding commands.

This demonstrates the main point of this stage: **UART output is only a limited and processed view of the actual memory state; it is not a complete representation of the device’s memory.**

---

# Stage 3: Reproducing the Unexpected Change

## 3.1 Initial Code Analysis

Before running the experiment, inspection of `main.c` identified the processing path for `stress <n>` in `sensor_task` as the most suspicious location:

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

The `buckets` array contains exactly four elements (`STRESS_BUCKETS = 4`), so the valid indexes are 0 through 3. However, the clamping condition uses `>` instead of `>=`.

Therefore, when `bucket_index` becomes exactly equal to `STRESS_BUCKETS`—that is, 4—the condition is false and the index is not clamped. The program then writes to `legacy_buckets[4]`, which is outside the valid range of the array.

This hypothesis was tested in the following experiments.

## 3.2 Baseline Experiment

The following UART command sequence was executed:

```text
reset
status
stress 10
status
stress 100
status
setmode service
status
```

### Complete Terminal Output

```text
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
>
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

### Key Observation

After `stress 10`, the `status` command showed:

```text
mode=normal
safety_level=3
```

These values were unchanged from the initial state.

However, after `stress 100`, the next `status` command showed:

```text
mode=service
safety_level=1
health_flags=0x00000008
```

No `setmode` command had been sent between the two status queries. Therefore, this was an **unexpected state change**.

## 3.3 Memory Inspection with GDB

### Before Sending `stress 100`

```text
(gdb) p g_stress_state
$7 = {buckets = {10, 0, 0, 0}, pending_mode = 0, guard = 1511506142}

(gdb) p g_device_mode
$8 = 0

(gdb) p g_health_flags
$9 = 0

(gdb) continue &
Continuing.
```

### After Sending `stress 100`

```text
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

### Analysis

The visible bucket values account for:

```text
20 + 20 + 20 + 20 = 80
```

However, 100 stress events were requested. Thus, 20 events are missing from the four visible array elements. These correspond to `i = 80` through `i = 99`. For these values:

```text
bucket_index = i / 20 = 4
```

The array has only four elements, indexed from 0 to 3. Because the code checks:

```c
if (bucket_index > STRESS_BUCKETS)
```

rather than:

```c
if (bucket_index >= STRESS_BUCKETS)
```

the value 4 is not clamped. The code writes to:

```c
legacy_buckets[4]
```

which is outside the array bounds.

The value of `g_device_mode` changed from 0 to 1, and `g_health_flags` changed from 0 to 8, even though no `setmode` command was issued. The value 8 corresponds to the `HEALTH_FLAG_UNEXPECTED_MODE` bit, which is set by `control_task` when `g_stress_state.pending_mode` is nonzero.

At the moment of inspection, `pending_mode` was already zero. This is because `control_task` checks it every 10 milliseconds and resets it to zero after applying the corresponding state change. Thus, the corrupted value may have existed only briefly and may have been cleared before the GDB inspection.

The likely sequence is:

1. `sensor_task` writes to `legacy_buckets[4]`.
2. This overwrites the adjacent `pending_mode` field.
3. `control_task` observes a nonzero `pending_mode`.
4. It changes the device to service mode, sets `g_safety_level` to 1, and sets `HEALTH_FLAG_UNEXPECTED_MODE`.
5. It resets `pending_mode` to zero.

These observations strongly support the hypothesis that the root cause is an **out-of-bounds write caused by an off-by-one error in the bucket-index clamping logic**.

---

# Stage 4: Locating the Error with a Watchpoint

## Watchpoint Setup

Because of a hardware-watchpoint insertion problem, the target was first reset and halted:

```text
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

The `i b` command confirmed that only this watchpoint was active:

```text
(gdb) i b
Num     Type           Disp Enb Address    What
1       hw watchpoint  keep y              g_safety_level
        breakpoint already hit 1 time
```

## Input Sequence That Triggered the Watchpoint

Instead of using `stress 100`, the following sequence was used to identify the exact threshold:

```text
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

The `stress 80` and `stress 79` commands did not trigger the watchpoint. Immediately after `stress 81`, the watchpoint triggered and the processor stopped. The subsequent UART `status` command timed out because the CPU was halted inside GDB.

This behavior agrees precisely with the suspected off-by-one error:

- For `stress 80`, the largest loop index is 79:

  ```text
  79 / 20 = 3
  ```

  This is valid.

- For `stress 79`, the largest loop index is 78:

  ```text
  78 / 20 = 3
  ```

  This is also valid.

- For `stress 81`, the loop reaches `i = 80`:

  ```text
  80 / 20 = 4
  ```

  This produces an invalid index. Because the code uses `>` instead of `>=`, the value is not clamped.

Thus, the threshold of 81 provides a precise and reproducible demonstration of the bug.

## Register, Stack, and Source-Code Inspection

After the watchpoint triggered, the following register values were observed:

```text
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

```text
(gdb) info locals
No locals.
```

```text
(gdb) frame 0
#0  control_task (argument=0x0) at .../main/main.c:418
418                 g_health_flags |= HEALTH_FLAG_UNEXPECTED_MODE;
```

```text
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

GDB stops immediately **after** the instruction that writes the new value to the watched variable. Therefore, although line 418 is displayed as the next source line, line 417 is the instruction that changed `g_safety_level` and triggered the watchpoint.

The `info locals` output confirms that `control_task` has no local variables at this point and operates only on global state.

## Answers to the Exercise Questions

### Which variable was monitored?

The watchpoint was placed on:

```text
g_safety_level
```

This variable was selected because it changed unexpectedly in Stage 3 without a direct `setmode` command.

### Which input triggered it?

The watchpoint was triggered by:

```text
stress 81
```

The preceding values `stress 80` and `stress 79` did not trigger it.

### Which function appeared in the backtrace?

The backtrace identified:

```text
control_task
```

The relevant source line was:

```text
g_safety_level = 1;
```

inside the block that checks `g_stress_state.pending_mode`.

### Was the change direct or caused by an adjacent memory corruption?

From the immediate point of view, `g_safety_level` was directly written by `control_task`.

However, `control_task` enters this branch only when `pending_mode` is nonzero. In the normal program logic, `pending_mode` is not directly set to a nonzero value anywhere. Therefore, the root cause is the out-of-bounds write in `sensor_task`.

The write to:

```c
legacy_buckets[4]
```

overwrites the adjacent `pending_mode` field. `control_task` then interprets the corrupted value as a valid request and changes the device mode and safety level.

### Evidence

1. The exact mathematical threshold: the fault occurs only when `count >= 81`.
2. The backtrace shows that the final write to `g_safety_level` occurs in `control_task`, not in the UART command parser.
3. The visible bucket total is smaller than the number of requested stress events.
4. The layout of `stress_state_t` places `pending_mode` immediately after `buckets[4]`.
5. The source code contains the off-by-one condition:

   ```c
   if (bucket_index > STRESS_BUCKETS)
   ```

---

# Stage 5: Stack Analysis, Current Task, and Root Cause

## 5.1 Which Task Contains the Error?

The watchpoint stopped execution in `control_task`, at the point where `g_safety_level` was written:

```text
#0  control_task (argument=0x0) at .../main/main.c:418
418                 g_health_flags |= HEALTH_FLAG_UNEXPECTED_MODE;
```

However, this does not mean that `control_task` caused the original memory error. It is important to distinguish between:

- the location where the sensitive variable was finally written; and
- the location where the memory corruption originally occurred.

The relevant code in `control_task` is:

```c
if (g_stress_state.pending_mode != 0) {
    g_device_mode = MODE_SERVICE;
    g_safety_level = 1;
    g_health_flags |= HEALTH_FLAG_UNEXPECTED_MODE;
    g_stress_state.pending_mode = 0;
}
```

This task does not normally assign a nonzero value to `pending_mode`. It only reads the field and later clears it.

Therefore, the root cause is in `sensor_task`, in the processing path for `stress <n>`:

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

The complete causal chain is:

1. `sensor_task` performs an out-of-bounds write.
2. The write corrupts `pending_mode`.
3. `control_task` interprets the corrupted value as a valid mode-change request.
4. `control_task` changes `g_device_mode`, `g_safety_level`, and `g_health_flags`.

Thus, two tasks are involved:

1. **`sensor_task`** — the source of the memory corruption.
2. **`control_task`** — the victim that consumes the corrupted state and changes the sensitive variables.

The fault does not occur in UART parsing. `handle_command` and `parse_u32` correctly parse and validate the `stress <n>` input. The error occurs later, during asynchronous processing by `sensor_task`.

## 5.2 Role of the Buffer, Input Length, Array Index, and Type Conversion

The UART input buffer and string length are not responsible for this error. Unlike a traditional string-buffer overflow, the numeric input is valid and within the range accepted by `parse_u32`:

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

Values up to 200 are accepted. No integer overflow or invalid type conversion occurs in this function.

The actual problem is the array index.

The structure is:

```c
typedef struct {
    volatile uint32_t buckets[STRESS_BUCKETS];
    volatile int32_t pending_mode;
    volatile uint32_t guard;
} stress_state_t;
```

With:

```text
STRESS_BUCKETS = 4
```

the valid indexes are 0 through 3.

The faulty code is:

```c
if (bucket_index > STRESS_BUCKETS) {
    bucket_index = STRESS_BUCKETS;
}
```

This has two problems:

1. The comparison should use `>=`, not `>`.
2. The replacement value should be `STRESS_BUCKETS - 1`, not `STRESS_BUCKETS`.

When `bucket_index` equals 4, the code writes to an element beyond the end of the array. Since `pending_mode` is adjacent to `buckets`, this causes adjacent-memory corruption in a global structure.

This is not a stack overflow or a string overflow. It is an **out-of-bounds array-index error causing corruption of adjacent fields in a global structure**.

## 5.3 Why Is This Error Important in Embedded Systems?

### 1. Limited Memory Protection

Many microcontrollers do not provide the same process-level memory isolation found in desktop operating systems. Global variables and structures often share the same address space. Consequently, an array overflow can directly modify critical state without being blocked by the operating system or hardware.

### 2. Silent Corruption

The UART `status` command does not reveal the error before the corrupted state has already affected the system. The operator may only observe that the mode or safety level changed unexpectedly. A low-level tool such as JTAG/GDB is required to identify the actual cause.

### 3. Cross-Task Effects

The error occurs in one task, `sensor_task`, while its effect is observed in another task, `control_task`. In a real-time multi-tasking system, this temporal and spatial separation between cause and effect makes debugging significantly more difficult.

### 4. Input-Dependent Reproducibility

The fault occurs only for values of `count` greater than or equal to 81. Normal tests using small values may never expose the problem. A device may therefore operate apparently correctly for a long time until it receives a particular input.

## 5.4 Potential Safety and Security Consequences

If `g_safety_level` or `g_device_mode` were used for an actual safety or security decision—for example, enabling an actuator, disabling a protection mechanism, or changing access privileges—the consequences could be serious.

Possible effects include:

- An unauthorized transition from normal mode to service mode.
- An unexpected decrease in the safety level from 3 to 1.
- A false or misleading health status.
- Confusion during incident response because the health flag is only a consequence of the corruption.
- Intentional exploitation by an attacker who has access to the UART interface.

Because the input is predictable, an attacker could deliberately send:

```text
stress 81
```

or another sufficiently large value to corrupt `pending_mode` and cause an unintended state transition. This represents a form of **state injection through memory corruption**.

---

# Stage 6: Effect of Halting the Processor on the Real-Time System

## Experiment

### GDB/OpenOCD Transcript

```text
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

```text
WAITED 10 SECONDS
```

```text
^C[esp32s3.cpu0] Target halted, PC=0x4037848A, debug_reason=00000000
Set GDB target to 'esp32s3.cpu0'
[esp32s3.cpu1] Target halted, PC=0x40043A3B, debug_reason=00000000

Thread 8 "IDLE" received signal SIGINT, Interrupt.
0x4037848a in esp_cpu_wait_for_intr () at .../esp_hw_support/cpu.c:64
64      }

(gdb) continue
Continuing.
```

### Corresponding UART Output

```text
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

## Timing Analysis

1. After `monitor reset halt` and `continue`, the first `status` command reported `heartbeat=3` and `health_flags=0x00000000`.
2. The processor was then halted after approximately 10 seconds.
3. During the halt, the CPU was inside `esp_cpu_wait_for_intr`, which is part of the idle task.
4. After resuming execution, the second `status` command reported `heartbeat=6` rather than a value corresponding to the entire elapsed wall-clock time.
5. `health_flags` changed from `0x00000000` to `0x00000004`.

The value `0x00000004` corresponds to:

```c
#define HEALTH_FLAG_HEARTBEAT_STALE  (1UL << 2)
```

The health-monitor condition is:

```c
if (now_us > g_last_heartbeat_us &&
    (now_us - g_last_heartbeat_us) > 3000000ULL) {
    current_flags |= HEALTH_FLAG_HEARTBEAT_STALE;
}
```

## Mechanism

When JTAG halts the CPU, execution of the application, FreeRTOS scheduler, and tasks such as `heartbeat_task` and `health_monitor_task` stops. However, the hardware timer used by `esp_timer_get_time()` continues to advance.

Therefore:

- `heartbeat_task` does not run while the CPU is halted.
- `g_heartbeat` and `g_last_heartbeat_us` remain unchanged during the halt.
- The wall-clock timer continues advancing.
- After `continue`, `health_monitor_task` observes that the time since the last heartbeat exceeds three seconds.
- It sets `HEALTH_FLAG_HEARTBEAT_STALE`.

This means that JTAG debugging artificially creates a condition similar to a real heartbeat failure, even though no bug exists in the heartbeat logic.

## Answers to the Exercise Questions

### Did `event_counter`, `health_flags`, or the heartbeat change?

`g_event_counter` remained unchanged because no `sample` or `stress` command was issued.

`g_health_flags` changed from:

```text
0x00000000
```

to:

```text
0x00000004
```

The heartbeat increased only according to the time during which the program was actually running, not according to the total elapsed wall-clock time.

### Did halting cause an alert or timeout?

Yes. The health monitor detected a stale heartbeat and generated an alert:

```c
if (current_flags != 0) {
    g_last_health_alert_us = (uint32_t)now_us;
    printf("\nALERT health flags=0x%08lx\n",
           (unsigned long)current_flags);
}
```

### Why can debugging change the result of a real-time experiment?

Halting the processor breaks the relationship between program execution and real time. Hardware timers may continue running while the scheduler and application tasks are stopped. As a result, timeout and heartbeat logic may produce false positives.

### What limitation does this create for dynamic debugging?

This is an example of the **observer effect** in real-time debugging. The debugging tool itself can alter the behavior being measured.

Whenever a breakpoint or watchpoint causes a long halt, the analyst must consider that:

- health warnings may be artificial;
- timeout conditions may be debugger-induced;
- timing-sensitive behavior may not represent normal execution.

To distinguish a real memory error from a debugger side effect, the analysis must rely on causality in the code, backtraces, and the exact instruction that modified the state—not merely on the appearance of a health flag.

---

# Stage 7: Code Correction and Post-Fix Testing

## 7.1 Detailed Description of the Fix

The original code contained two overlapping errors:

```c
uint32_t bucket_index = i / STRESS_BUCKET_WIDTH;
if (bucket_index > STRESS_BUCKETS) {
    bucket_index = STRESS_BUCKETS;
}
legacy_buckets[bucket_index]++;
```

The array contains `STRESS_BUCKETS` elements, so its valid indexes are:

```text
0 ... STRESS_BUCKETS - 1
```

### Error 1: Incorrect Comparison

The condition uses `>` instead of `>=`. Consequently, an index exactly equal to `STRESS_BUCKETS` is not clamped.

### Error 2: Incorrect Clamping Value

Even if the condition were changed to `>=`, assigning:

```c
bucket_index = STRESS_BUCKETS;
```

would still produce an invalid index. The last valid index is:

```c
STRESS_BUCKETS - 1
```

Therefore, both parts must be corrected.

## 7.2 Root-Cause Fix

```c
uint32_t bucket_index = i / STRESS_BUCKET_WIDTH;
if (bucket_index >= STRESS_BUCKETS) {
    bucket_index = STRESS_BUCKETS - 1U;
}
legacy_buckets[bucket_index]++;
```

With this correction, every possible value of `i`—which is limited by `parse_u32` to a maximum of 199—produces an index in the valid range:

```text
[0, STRESS_BUCKETS - 1]
```

No write can therefore occur outside the array.

## 7.3 Additional Defense in Depth

The `stress_state_t` structure already contained a `guard` field with the magic value:

```text
0x5A17C0DE
```

However, the original code never checked it.

Two additional defensive changes were introduced:

1. A new `bucket_guard` field was placed between the `buckets` array and `pending_mode`.
2. `health_monitor_task` now validates both `bucket_guard` and `guard`. If either value differs from its expected value, it sets a new health flag:

   ```text
   HEALTH_FLAG_BUCKET_GUARD_BAD
   ```

The purpose of `bucket_guard` is to provide a sacrificial field. If a similar one- or two-word overflow occurs in the future, the guard should be corrupted before the sensitive `pending_mode` field.

This mechanism is an additional defense layer and does not replace the root-cause fix.

## 7.4 Post-Fix Test Procedure

### GDB Watchpoints

```text
(gdb) monitor reset halt
(gdb) delete
(gdb) watch g_safety_level
(gdb) watch g_stress_state.pending_mode
(gdb) continue
```

### UART Input Sequence

```text
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

### UART Output After the Fix

```text
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

### GDB Output

```text
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

### Direct Memory Inspection After `stress 100`

```text
(gdb) p g_stress_state
$1 = {buckets = {80, 80, 80, 100}, bucket_guard = 2965694369, pending_mode = 0, guard = 1511506142}

(gdb) p g_device_mode
$2 = 0

(gdb) p g_safety_level
$3 = 3

(gdb) p g_health_flags
$4 = 0
```

## 7.5 Observations

- The bucket values accounted for all generated stress events:

  ```text
  80 + 80 + 80 + 100 = 340
  ```

- `g_stress_state.pending_mode` remained zero.
- `g_device_mode` and `g_safety_level` remained unchanged at normal and 3.
- `g_health_flags` remained zero.
- The guard values remained intact.
- No unexpected mode transition occurred.
- The original test sequence, including `stress 81` and `stress 100`, no longer caused memory corruption.

The results confirm that the off-by-one error was successfully fixed.

---

# Stage 8: Security Analysis and Hardening

## Why Is JTAG Useful During Development but Dangerous in a Final Product?

JTAG was essential for discovering the off-by-one error in `sensor_task`. Without JTAG, the only visible symptom would have been that `mode` and `safety_level` sometimes changed without a corresponding `setmode` command.

Using GDB and watchpoints made it possible to:

- stop the processor;
- inspect memory and registers;
- monitor global variables;
- identify the exact instruction that modified `g_safety_level`;
- trace execution to `control_task`;
- infer that the corruption originated in `sensor_task`.

The same capabilities are dangerous in a deployed product. An attacker with physical access to the JTAG interface could:

- dump RAM, including cryptographic keys, authentication tokens, and sensitive user data;
- overwrite security- or safety-related variables;
- bypass software authorization checks;
- alter registers and control flow;
- reverse-engineer the firmware through live execution.

For example, an attacker could directly modify:

```text
g_safety_level
```

without using any legitimate command path.

## Difference Between Debug and Release Builds

### Debug Symbols

GDB was able to display:

- function names;
- source-file names;
- line numbers;
- global variables;
- local variables.

This was possible because the ELF file contained full debug information. A release build should normally remove or strip these symbols.

### Detailed Logs

The firmware printed detailed messages such as:

```text
SENSOR stress_complete count=...
ALERT health flags=...
```

A production firmware should reduce logging to the minimum necessary level and avoid exposing internal state.

### Raw Memory Access

UART exposed only a small subset of the system state. GDB exposed much more, including:

- `g_cli_command_count`;
- the complete `stress_state_t`;
- raw addresses;
- stack contents;
- register values.

A release product should not expose this information through an unauthenticated interface.

## Why Do Cryptography and Authentication Not Replace Memory Safety?

Cryptography and authentication address different security problems.

- Encryption protects data while it is stored or transmitted.
- Authentication verifies the identity or integrity of a sender, message, or firmware image.
- Memory safety ensures that code accesses only the memory belonging to the intended object.

In this experiment, `stress 81` was a valid and authenticated-looking input. The problem occurred because the internal implementation calculated an invalid array index.

Even if the input is encrypted and authenticated, the program can still perform an out-of-bounds write after decrypting and processing it in RAM.

Therefore, memory safety is an independent and complementary security requirement.

## Does Flash Encryption Prevent RAM Inspection Through JTAG?

No.

Flash encryption protects data stored in flash, including program code and constant data. During execution, however, the code and data are decrypted and loaded into RAM.

JTAG operates at the processor-debug level and can inspect live, decrypted RAM. Therefore, an attacker with JTAG access could still read:

- `g_safety_cookie`;
- `g_stress_state`;
- stack contents;
- registers;
- cryptographic keys held in memory.

Flash encryption protects against offline flash extraction. It does not protect against live JTAG access.

## What Risk Does an Open Debug Interface Create if Secure Boot Is Enabled?

Secure Boot ensures that only approved, signed firmware is loaded and executed. However, an open JTAG interface still allows an attacker to manipulate the execution of that approved firmware.

An attacker could:

- halt execution;
- modify RAM;
- change registers;
- bypass authorization checks;
- skip security-sensitive instructions;
- read secrets from memory;
- alter control flow without installing a new firmware image.

Thus, Secure Boot protects code integrity at boot time, but it does not protect runtime integrity when unrestricted JTAG access is available.

## Recommended Product Policy

### 1. Disable JTAG in Release Products

Use the ESP32-S3 eFuse options for permanently disabling JTAG, such as:

```text
DIS_USB_JTAG
HARD_DIS_JTAG
```

These should be programmed before final distribution when appropriate.

If field debugging is required, use a controlled and authenticated debugging mechanism rather than an unrestricted JTAG interface.

### 2. Separate Debug and Release Builds

- Build release firmware without debug symbols.
- Strip the ELF file.
- Remove debug-only commands and diagnostic paths.
- Use conditional compilation such as:

  ```c
  #ifdef DEBUG_BUILD
  ```

### 3. Minimize Logs

Production logs should expose only the information required by the operator. Detailed internal state, raw flags, counters, and memory-related values should not be printed over an unauthenticated UART connection.

Sensitive logs should either be disabled or stored in encrypted internal storage and exposed only through an authenticated channel.

### 4. Use Secure Boot and Flash Encryption Together

Secure Boot and Flash Encryption are complementary mechanisms. Neither replaces JTAG protection.

A production configuration should consider:

```text
Secure Boot + Flash Encryption + JTAG disable
```

### 5. Treat Memory Safety as an Independent Requirement

The development process should include:

- static analysis using tools such as `cppcheck` and `clang-tidy`;
- fuzz testing of numeric and command inputs;
- code review focused on array bounds and index calculations;
- compiler warnings;
- runtime assertions and canary checks;
- regression tests for boundary values such as 79, 80, 81, 199, and 200.

The bug found in this exercise could have been detected through a simple review of:

```c
bucket_index > STRESS_BUCKETS
```

instead of:

```c
bucket_index >= STRESS_BUCKETS
```

---

# Recommended Tests and Final Results

| Scenario | Expected result | Actual result |
|---|---|---|
| OpenOCD and GDB connection | The program can be halted and resumed. | ✅ The connection succeeded using `target remote :3333`; `continue` and halting worked correctly. |
| Register and stack tracing | Real register and stack output is recorded. | ✅ Complete `info registers` output and multiple `bt` examples were recorded. |
| Status and GDB state comparison | UART values can be compared with RAM values. | ✅ The comparison showed that `status` exposes only a subset of the actual memory state. |
| Stress-input testing | Unexpected state changes can be reproduced. | ✅ The `stress 10 → stress 100` sequence reproduced the unexpected mode, safety-level, and health-flag changes. |
| Watchpoint analysis | The location of the state change can be identified. | ✅ `watch g_safety_level` identified `control_task`, line 417, as the final write location. |
| Task and stack analysis | The complete execution path can be explained. | ✅ The distinction between the root cause in `sensor_task` and the final state change in `control_task` was established. |
| Post-fix testing | Unexpected changes no longer occur. | ✅ The same sequence—80, 79, 81, and 100—completed without triggering the watchpoints. |
| Processor halt experiment | The effect on heartbeat and health monitoring can be analyzed. | ✅ Halting the CPU caused the artificial `HEALTH_FLAG_HEARTBEAT_STALE` alert. |

---

# Challenges

## UART Terminal Issues

### 1. Incorrect Port

Initially, the UART terminal did not work correctly because it was connected to the wrong device port. After testing different ports, the correct USB Serial-JTAG port was selected.

### 2. Character-by-Character Input Processing

The default console buffering behavior caused input to be read one character at a time. For example, entering:

```text
help
```

was interpreted as four separate commands:

```text
h
e
l
p
```

This generated multiple:

```text
ERR unknown_command
```

messages.

The cause was that the default ROM-based VFS driver operated in a non-blocking manner. When only one byte was available, the read operation returned immediately instead of waiting for a complete line.

The problem was solved by explicitly installing and activating the `usb_serial_jtag` driver:

```c
usb_serial_jtag_driver_install();
usb_serial_jtag_vfs_use_driver();
```

This provided blocking line-oriented input behavior.

### 3. Input Echo

After fixing the buffering issue, typed characters were not displayed in the terminal. The driver did not provide automatic echoing.

A custom line-reading routine was implemented. It:

- reads characters individually using `getchar()`;
- echoes each character with `putchar()`;
- handles backspace;
- allows the user to edit a line before submitting it.

---

## OpenOCD USB Permission Error

### Problem

OpenOCD initially reported:

```text
Error: libusb_open() failed with LIBUSB_ERROR_ACCESS
Error: esp_usb_jtag: could not find or open device!
```

Although the device was detected with the correct VID/PID (`303a:1001`), the user did not have permission to access the USB device.

### Solution

The following udev rule was created:

```text
SUBSYSTEM=="usb", ATTR{idVendor}=="303a", ATTR{idProduct}=="1001", MODE="0666"
```

The rules were reloaded using:

```bash
sudo udevadm control --reload-rules
sudo udevadm trigger
```

After disconnecting and reconnecting the board, OpenOCD worked without `sudo`.

---

## “Could Not Insert Hardware Watchpoint” Error

### Problem

When setting a watchpoint on `g_safety_level`, GDB reported:

```text
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

### Cause

The hardware-debug unit may have remained in an inconsistent state from a previous OpenOCD/GDB session. In addition, the processor was not always halted in a stable and known state when the watchpoint was inserted.

### Solution

The following sequence solved the problem:

```text
(gdb) monitor reset halt
(gdb) watch g_safety_level
(gdb) continue
```

Before repeating experiments, existing breakpoints and watchpoints were also checked with:

```text
(gdb) i b
```

Since the number of hardware watchpoints is limited, old watchpoints must be deleted before creating new ones.

---

## SMP Halting and Watchpoint Registration

The ESP32-S3 has two cores, `cpu0` and `cpu1`. During debugging, one core sometimes halted while the other continued running:

```text
Info : [esp32s3.cpu1] Target halted, PC=0x420093E2, debug_reason=00000002
Info : Set GDB target to 'esp32s3.cpu1'
Error: [esp32s3.cpu0] not halted
Error: [esp32s3.cpu0] not halted
Error: [esp32s3.cpu0] can't add write watchpoint at 0x3fc91000, target not halted
```

GDB reported:

```text
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

### Cause

The problem was not necessarily the number of watchpoints. In SMP mode, OpenOCD must coordinate both cores before it can configure the hardware breakpoint and watchpoint registers.

One core sometimes remained running while the other was halted. The issue was made worse when one core was inside:

```text
esp_cpu_wait_for_intr()
```

which executes the `WAITI` instruction.

The USB-JTAG link also occasionally disconnected:

```text
libusb_bulk_write error: LIBUSB_ERROR_NO_DEVICE
esp_usb_jtag: device not found!
esp_usb_jtag: failed to revive USB device!
[esp32s3.cpu0] Failed to read debug stubs address location (-104)!
[esp_usb_jtag] Polling failed, trying to reexamine
```

This caused OpenOCD to re-detect the chip and disrupted synchronization between GDB and the two cores.

### Solution: Unicore Configuration

To eliminate synchronization races, the system was configured to run in unicore mode by adding the following to `sdkconfig.defaults`:

```text
CONFIG_FREERTOS_UNICORE=y
```

The project was then rebuilt:

```bash
rm sdkconfig
idf.py reconfigure
idf.py build flash
```

The configuration was verified using:

```bash
grep UNICORE sdkconfig
```

With unicore mode enabled, all FreeRTOS tasks ran on `cpu0`, and `cpu1` was not initialized. This eliminated the synchronization problem and made breakpoint and watchpoint insertion reliable.

Before enabling unicore mode, the code had to be checked for tasks explicitly pinned to `cpu1`, such as:

```c
xTaskCreatePinnedToCore(..., 1);
```

Any task exclusively pinned to `cpu1` would not run after enabling unicore mode.

Because an ESP32-C3 is inherently single-core, this synchronization problem would not occur on that chip. On the ESP32-S3, unicore mode was selected specifically to simplify and stabilize the JTAG debugging process.
