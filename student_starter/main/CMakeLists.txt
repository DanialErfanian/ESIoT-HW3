idf_component_register(
    SRCS "main.c"
    INCLUDE_DIRS "."
)

target_compile_options(${COMPONENT_LIB} PRIVATE -Og -g3 -fno-omit-frame-pointer)
