set mem inaccessible-by-default off
set confirm off
set target-async on

target extended-remote :3333
mon reset halt
mon arm semihosting enable

file build/isimud.elf
load
