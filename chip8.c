#include "chip8.h"

void init(){
    pc = 0x200;
    opcode = 0;
    i = 0;
    sp = 0;

    int j;
    // Clear display
    for(j = 0; j < 64 * 32; j++)
           SCREEN[j] = 0;
    // Clear stack
    for(j = 0; j < 16; j++)
           STACK[j] = 0;
    // Clear registers V0-VF
    for(j = 0; j < 16; j++)
           V[j] = 0;
    // Clear memory
    for(j = 0; j < 4096; j++)
        MEM[j] = 0;
    // Load the fontset
    for(j = 0; j < 80; j++)
        MEM[j] = chip8_fontset[j];
}
