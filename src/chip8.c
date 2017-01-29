#include <stdio.h>
#include <stdlib.h>
#include "chip8.h"

unsigned char chip8_fontset[80] = {
    0xF0, 0x90, 0x90, 0x90, 0xF0, //0
    0x20, 0x60, 0x20, 0x20, 0x70, //1
    0xF0, 0x10, 0xF0, 0x80, 0xF0, //2
    0xF0, 0x10, 0xF0, 0x10, 0xF0, //3
    0x90, 0x90, 0xF0, 0x10, 0x10, //4
    0xF0, 0x80, 0xF0, 0x10, 0xF0, //5
    0xF0, 0x80, 0xF0, 0x90, 0xF0, //6
    0xF0, 0x10, 0x20, 0x40, 0x40, //7
    0xF0, 0x90, 0xF0, 0x90, 0xF0, //8
    0xF0, 0x90, 0xF0, 0x10, 0xF0, //9
    0xF0, 0x90, 0xF0, 0x90, 0x90, //A
    0xE0, 0x90, 0xE0, 0x90, 0xE0, //B
    0xF0, 0x80, 0x80, 0x80, 0xF0, //C
    0xE0, 0x90, 0x90, 0x90, 0xE0, //D
    0xF0, 0x80, 0xF0, 0x80, 0xF0, //E
    0xF0, 0x80, 0xF0, 0x80, 0x80  //F
};

void CHIP8_Init(){
    pc = 0x200;
    opcode = 0;
    i = 0;
    sp = 0;
    drawFlag = 0;
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

void CHIP8_LoadGame(unsigned char buffer[], int size){
    for(int i = 0; i < size; i++){
        MEM[i + 512] = buffer[i];
    }
}

void CHIP8_EmulateCycle(){
    // FETCH
    opcode = MEM[pc] << 8 | MEM[pc + 1];

    // DECODE
    switch(opcode & 0xF000){
        case 0x0000:
            switch(opcode & 0x000F){
                case 0x0000: // 0x0000 -> Clear the screen
                    for(int i = 0; i < 64*32; i++)
                        SCREEN[i] = 0x0;
                    drawFlag = 1;
                    pc += 2;
                break;

                case 0x000E: // 0x000E -> return from a subroutine
                    pc = STACK[sp];
                    sp--;
                    pc += 2;
                break;
            }
        break;

        case 0x1000: //0x1NNN -> Jump to NNN
            pc = opcode & 0x0FFF;
        break;

        case 0x2000: //0x2NNN -> Call subroutine at NNN
            sp++;
            STACK[sp] = pc;
            pc = opcode & 0x0FFF;
        break;

        case 0x3000: //0x3XKK -> Skip next instruction if V[x] == kk
            if(V[(opcode & 0x0F00) >> 8] == (opcode & 0x00FF)) pc += 4;
            else pc += 2;
        break;

        case 0x4000: //0x4XKK -> Skip next instruction if V[x] != kk
            if(V[(opcode & 0x0F00) >> 8] != (opcode & 0x00FF)) pc += 4;
            else pc += 2;
        break;

        case 0x5000: //0x5XY0 -> Skip next instruction if V[X] == V[Y]
            if(V[(opcode & 0x0F00) >> 8] == V[(opcode & 0x00F0) >> 4]) pc += 4;
            else pc += 2;
        break;

        case 0x6000: //0x6XKK -> Set V[X] = KK
            V[(opcode & 0x0F00) >> 8] = (opcode & 0x00FF);
            pc += 2;
        break;

    }

    // Timers
    if(delay_timer > 0)
        --delay_timer;
    if(sound_timer > 0){
        if(sound_timer == 1)
            printf("BEEP\a\n");
        --sound_timer;
    }
}
