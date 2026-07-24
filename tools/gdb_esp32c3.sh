#!/usr/bin/env bash
set -e
ELF=${1:-student_starter/build/CE40876_E3_JTAG_Runtime_Forensics.elf}
riscv32-esp-elf-gdb "$ELF"
