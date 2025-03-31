# chip8-emulator

### Structure
- chip8.c -> core chip-8 CPU logic (fetch, decode, execute)
- chip8.h -> header file (declarations for `chip8.c`)
- main.c -> entry point, SDL2 graphics & input handling
- Makefile -> build script for easy compilation

### Roadmap
- [X] Memory & registers
- [] Implement standard Chip-8 instructions
- [] Implement 16-key keypad emulation
- [] Implement display emulation
- [] Timers & sound

### Material used
- [Cowgod's Chip-8 Technical Reference](http://devernay.free.fr/hacks/chip8/C8TECH10.HTM)