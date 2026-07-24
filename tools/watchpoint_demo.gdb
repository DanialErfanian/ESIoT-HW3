set pagination off
target remote :3333
monitor reset halt
thbreak app_main
continue

info variables g_
p/x &g_safety_cookie
p/x g_safety_cookie
p g_device_mode
p g_safety_level
p/x g_health_flags
x/32wx &g_safety_cookie

watch g_safety_level
continue
