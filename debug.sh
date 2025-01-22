#!/bin/sh
set -e
cmake --preset Debug
cd build/Debug
ninja
openocd -f interface/stlink.cfg -f target/stm32g4x.cfg -c "program ECU-25.elf" &
OCDPID=$!
unset -e
gdb --symbols=ECU-25.elf --eval-command="target extended-remote :3333"
kill $OCDPID
