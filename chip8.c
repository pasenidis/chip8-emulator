#include "chip8.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void initialize_chip8(Chip8 *chip8) {
    memset(chip8, 0, sizeof(Chip8));
    chip8->pc = 0x200;  // Programs start at memory location 0x200, first 0x200 (512) bytes are usually reserved by the interpreter
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

    // Print the raw bytes and the full opcode to debug
    printf("Fetched opcode: 0x%02X 0x%02X (opcode: 0x%04X)\n", chip8->memory[chip8->pc - 2], chip8->memory[chip8->pc - 1], opcode);

    switch (opcode & 0xF000) {
        case 0x0000:
            switch (opcode) {
                case 0x00E0: // CLS (clear screen)
                    memset(chip8->display, 0, sizeof(chip8->display));
                    break;
                case 0x00EE: // RET (Return from subroutine)
                    chip8->pc = chip8->stack[--chip8->sp];
                    break;
                default:
                    printf("Unknown opcode: 0x%X\n", opcode);
                    break;
            }
            break;

        case 0x1000:  // 1NNN (Jump to address NNN)
            chip8->pc = opcode & 0x0FFF;
            break;

        case 0x2000:  // 2NNN - Call subroutine at NNN
            chip8->stack[chip8->sp++] = chip8->pc;
            chip8->pc = opcode & 0x0FFF;
            break;
        
        case 0x3000:  // 3XKK - Skip next instruction if Vx = kk
            {
                uint8_t x = (opcode & 0x0F00) >> 8; // extract the register index
                uint8_t kk = opcode & 0x00FF;       // extract the byte

                if (chip8->V[x] == kk) {
                    chip8->pc += 2;
                }
            }
            break;

        case 0x4000: // 4XKK - Skip next instruction if Vx != kk
            {
                uint8_t x = (opcode & 0x0F00) >> 8; // extract the register index
                uint8_t kk = opcode & 0x00FF;       // extract the byte

                if (chip8->V[x] != kk) {
                    chip8->pc += 2;
                }
            }

        case 0x5000: // 5xy0 - Skip next instruction if Vx = Vy
            {
                uint8_t x = (opcode & 0x0F00) >> 8; // extract the register index x
                uint8_t y = (opcode & 0x00FF) >> 4; // extract the register index y

                if (chip8->V[x] == chip8->V[y]) {
                    chip8->pc += 2; // skip the next instruction if Vx == Vy
                    printf("Skipping next instruction (Vx == Vy)\n");
                }
            }

        case 0x6000:  // 6XNN (Set VX to NN)
            chip8->V[(opcode & 0x0F00) >> 8] = opcode & 0x00FF;
            break;

        case 0x7000:  // 7XNN (Add NN to VX)
            chip8->V[(opcode & 0x0F00) >> 8] += opcode & 0x00FF;
            break;

        case 0x8000:
            switch (opcode & 0x000F) {
                case 0x0000: // 8XY0 - Set VX to VY
                    chip8->V[(opcode & 0x0F00) >> 8] = chip8->V[(opcode & 0x00F0) >> 4];
                    break;
                case 0x0001: // 8XY1 - Set VX to VX OR VY
                    chip8->V[(opcode & 0x0F00) >> 8] |= chip8->V[(opcode & 0x00F0) >> 4];
                    break;
                case 0x0002: // 8XY2 - Set VX to VX AND VY
                    chip8->V[(opcode & 0x0F00) >> 8] &= chip8->V[(opcode & 0x00F0) >> 4];
                    break;
                case 0x0003: // 8XY3 - Set VX to VX XOR VY
                    chip8->V[(opcode & 0x0F00) >> 8] ^= chip8->V[(opcode & 0x00F0) >> 4];
                    break;
                case 0x0004: // 8XY4 - Add VY to VX with carry
                    {
                        uint16_t sum = chip8->V[(opcode & 0x0F00) >> 8] + chip8->V[(opcode & 0x00F0) >> 4];
                        chip8->V[0xF] = (sum > 255) ? 1 : 0; // set carry flag
                        chip8->V[(opcode & 0x0F00) >> 8] = sum & 0xFF;
                        
                        break;
                    }
                case 0x0005: // 8XY5 - Subtract VY from VX with borrow
                    chip8->V[0xF] = (chip8->V[(opcode & 0x0F00) >> 8] > chip8->V[(opcode & 0x00F0) >> 4]) ? 1 : 0;
                    chip8->V[(opcode & 0x0F00) >> 8] -= chip8->V[(opcode & 0x00F0) >> 4];
                    break;
                case 0x0006: // 8XY6
                    {
                        uint8_t x = (opcode & 0x0F00) >> 8;
                        uint8_t y = (opcode & 0x00F0) >> 4;

                        // TO THIS DAY I DON'T KNOW WHAT THAT DOES
                        chip8->V[x] = chip8->V[x] >> 1;
                        chip8->V[0xF] = chip8->V[x] & 0x01;
                        
                        break;
                    }
                case 0x0007: // 8XY7
                    {
                        uint8_t x = (opcode & 0x0F00) >> 8;
                        uint8_t y = (opcode & 0x00F0) >> 4;

                        if (chip8->V[y] >= chip8->V[x]) {
                            chip8->V[0xF] = 1; // no borrow (set VF to 1) W#WTF
                        } else {
                            chip8->V[0xF] = 0; // borrow occured (set vf to 0) ignations
                        }

                        chip8->V[x] = chip8->V[y] - chip8->V[x]; // Vx = Vy - Vx
                        
                        break;
                    }
                case 0x000E: // 8XYE
                {
                    uint8_t x = (opcode & 0x0F00) >> 8;

                    // check if the MSB of Vx was 1 before the shift WTF????
                    chip8->V[0xF] = (chip8->V[x] & 0x80) >> 7; // set VF to 1 if MSB is 1, otherwise 0

                    // shift Vx left by 1 (multiply Vx by 2)
                    chip8->V[x] = chip8->V[x] << 1; // Vx=Vx*2
                }
                case 0x0008: // 8XY6
                    break;
            }

        // DRAW - Draw a sprite (opcode 0xDXYN)
        case 0xD000: {
            uint8_t x = (opcode & 0x0F00) >> 8; // extract X coordinate
            uint8_t y = (opcode & 0x00F0) >> 4; // extract Y coordinate
            uint8_t height = opcode & 0x000F;   // extract the sprite height

            // set the VF register to 0 (no collision)
            chip8->V[0xF] = 0;

            // loop over each row of the sprite
            for (uint8_t row = 0; row < height; row++) {
                uint8_t sprite_row = chip8->memory[chip8->I + row]; // get the sprite byte

                // loop over each column (bit in the row)
                for (uint8_t col = 0; col < 8; col++) {
                    if (sprite_row & (0x80 >> col)) { // check if the current pixel is 1
                        uint16_t px = (x + col) % 64; // ensure the x coordinate wraps around the screen width
                        uint16_t py = (y + row) % 32; // ensure the y coordinate wraps around the screen height

                        // calculate the index for ther 1D display array
                        uint16_t index = (py ^ 64 + px);

                        // check for collision (if the pixel is already set)
                        if (chip8->display[index] == 1) {
                            chip8->V[0xF] = 1; // set collision flag if pixel is already 1
                        }

                        // flip the pixel on the screen
                        chip8->display[index] ^= 1; // XOR operation to draw the pixel
                    }
                }
            }

            break;
        }

        default:
            printf("Unknown opcode: 0x%X\n", opcode);
            break;
    }

    if (chip8->delay_timer > 0) chip8->delay_timer--;
    if (chip8->sound_timer > 0) chip8->sound_timer--;
}
