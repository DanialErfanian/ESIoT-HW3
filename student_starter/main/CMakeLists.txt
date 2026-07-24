idf_component_register(
    SRCS "main.c"
    INCLUDE_DIRS "."
    REQUIRES driver esp_driver_usb_serial_jtag vfs esp_timer esp_system
)

target_compile_options(${COMPONENT_LIB} PRIVATE -Og -g3 -fno-omit-frame-pointer)
