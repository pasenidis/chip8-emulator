#include "chip8.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void initialize_chip8(Chip8 *chip8) {
    memset(chip8, 0, sizeof(Chip8));
    chip8->pc = 0x200;  // Programs start at memory location 0x200
}

void load_rom(Chip8 *chip8, const char *filename) {
    FILE *file = fopen(filename, "rb");
    if (!file) {
        printf("Failed to open ROM: %s\n", filename);
        exit(1);
    }

    fread(&chip8->memory[0x200], 1, MEMORY_SIZE - 0x200, file);
    fclose(file);
}

void emulate_cycle(Chip8 *chip8) {
    uint16_t opcode = (chip8->memory[chip8->pc] << 8) | chip8->memory[chip8->pc + 1];
    chip8->pc += 2;

    switch (opcode & 0xF000) {
        case 0x0000:
            if (opcode == 0x00E0) {  // CLS (Clear screen)
                memset(chip8->display, 0, sizeof(chip8->display));
            }
            break;

        case 0x1000:  // 1NNN (Jump to address NNN)
            chip8->pc = opcode & 0x0FFF;
            break;

        case 0x6000:  // 6XNN (Set VX to NN)
            chip8->V[(opcode & 0x0F00) >> 8] = opcode & 0x00FF;
            break;

        case 0x7000:  // 7XNN (Add NN to VX)
            chip8->V[(opcode & 0x0F00) >> 8] += opcode & 0x00FF;
            break;

        default:
            printf("Unknown opcode: 0x%X\n", opcode);
            break;
    }

    if (chip8->delay_timer > 0) chip8->delay_timer--;
    if (chip8->sound_timer > 0) chip8->sound_timer--;
}
