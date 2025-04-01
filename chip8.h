#ifndef CHIP8_H
#define CHIP8_H

#include <stdint.h>

#define MEMORY_SIZE 4096
#define DISPLAY_WIDTH 64
#define DISPLAY_HEIGHT 32

typedef struct {
    uint8_t memory[MEMORY_SIZE];
    uint8_t V[16];  // Registers V0 - VF
    uint16_t I;  // Index register, 16-bit size, usually stores memory addresses, only the lowest 12 bits are usually used
    uint16_t pc; // Program counter
    uint16_t stack[16];
    uint8_t sp;  // Stack pointer
    uint8_t display[DISPLAY_WIDTH * DISPLAY_HEIGHT]; // 64x32 monochrome display represented as a 1D array
    uint8_t keypad[16];  // Keypad state
    uint8_t delay_timer;
    uint8_t sound_timer;
} Chip8;

void initialize_chip8(Chip8 *chip8);
void load_rom(Chip8 *chip8, const char *filename);
void emulate_cycle(Chip8 *chip8);

#endif
