#!/bin/sh
set -e
cmake --preset Debug
cd build/Debug
ninja
exitcode="${?}"

if [ "$1" != "build" ] && [ "$1" != "b" ]
then
	openocd -f interface/stlink.cfg -f target/stm32g4x.cfg -c "program ECU-25.elf" &

	OCDPID=$!
	sleep 1
	ps -p $OCDPID > /dev/null 2>&1	# Exits program on failure to find PID

	arm-none-eabi-gdb --symbols=ECU-25.elf --eval-command="target extended-remote :3333"
	kill $OCDPID
else
	echo "Ninja exited with status ${exitcode}"
fi
