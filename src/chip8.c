#include <stdio.h>
#include <stdlib.h>
#include <time.h>
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
    chip8_drawFlag = 0;
    int j;
    // Clear display
    for(j = 0; j < 64 * 32; j++)
           SCREEN[j] = 0;
    // Clear stack
    for(j = 0; j < 16; j++)
           STACK[j] = 0;
    // Clear registers V0-VF and the keys
    for(j = 0; j < 16; j++)
          KEY[j] = V[j] = 0;
    // Clear memory
    for(j = 0; j < 4096; j++)
        MEM[j] = 0;
    // Load the fontset
    for(j = 0; j < 80; j++)
        MEM[j] = chip8_fontset[j];

    srand(time(NULL));
}

int CHIP8_LoadGame(const char filename[]){
    CHIP8_Init();

    // Open the ROM
    FILE *romFile = fopen(filename, "r");
    if(romFile == NULL){
        fputs("File error", stderr);
        return 0;
    }

    //Count the size of the ROM and go back at beginning
    fseek(romFile, 0, SEEK_END);
    long romSize = ftell(romFile);
    rewind(romFile);

    //Allocate enough memory to store the ROM
    char *buffer = (char*) malloc(sizeof(char) * romSize);
    if(buffer == NULL){
        fputs("Memory Error", stderr);
        return 0;
    }

    //Copy the ROM inside the buffer, bit by bit
    size_t result = fread(buffer, 1, romSize, romFile);
    if(result != romSize){
        fputs("Reading error", stderr);
        return 0;
    }

    //Copy the buffer inside the memory
    if((4096 - 512) > romSize)
        for(int j = 0; j < romSize; j++)
            MEM[j + 512] = buffer[j];
    else {
        fputs("Rom too big", stderr);
        return 0;
    }
    fclose(romFile);
    free(buffer);
    return 1;

}

void CHIP8_EmulateCycle(){
    // FETCH
    if(pc < 4095){
        opcode = MEM[pc] << 8 | MEM[pc + 1];
        //printf("The actual opcode is: 0x%04x\n", opcode);
        // DECODE
        switch(opcode & 0xF000){
            case 0x0000:
                switch(opcode & 0x000F){
                    case 0x0000: // 0x0000 -> Clear the screen
                        for(int i = 0; i < 64*32; i++)
                            SCREEN[i] = 0x0;
                        printf("REDRAW!\n");
                        chip8_drawFlag = 1;
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

            case 0x7000: //0x7XKK -> Set V[X] = V[X] + KK
                V[(opcode & 0x0F00) >> 8] += (opcode & 0x00FF);
                pc += 2;
            break;

            case 0x8000:
                switch(opcode & 0x000F){
                    case 0x0000: // 0x8XY0 -> Set V[X] = V[Y]
                        V[(opcode & 0x0F00) >> 8] = V[(opcode & 0x00F0) >> 4];
                        pc += 2;
                    break;

                    case 0x0001: // 0x8XY1 -> Set V[X] = V[X] OR V[Y]
                        V[(opcode & 0x0F00) >> 8] |= V[(opcode & 0x00F0) >> 4];
                        pc += 2;
                    break;

                    case 0x0002: // 0x8XY2 -> Set V[X] = V[X] AND V[Y]
                        V[(opcode & 0x0F00) >> 8] &= V[(opcode & 0x00F0) >> 4];
                        pc += 2;
                    break;

                    case 0x0003: //0x8XY3 -> Set V[X] = V[X] XOR V[Y]
                        V[(opcode & 0x0F00) >> 8] ^= V[(opcode & 0x00F0) >> 4];
                        pc += 2;
                    break;

                    case 0x0004: //0x8XY4 -> Set V[X] = V[X] + V[Y] and VF = carry
                        V[(opcode & 0x0F00) >> 8] += V[(opcode & 0x00F0) >> 4];
                        if(V[(opcode & 0xF00) >> 8] > 0xFF) V[0xF] = 1;
                        else V[0xF] = 0;
                        pc += 2;
                    break;

                    case 0x0005: //0x8XY5 -> Set V[F] = 1 if V[X] > V[Y], then V[X] -= V[Y]
                        if(V[(opcode & 0x0F00) >> 8] > V[(opcode & 0x00F0) >> 4]) V[0xF] = 1;
                        else V[0xF] = 0;
                        V[(opcode & 0x0F00) >> 8] -= V[(opcode & 0x00F0) >> 4];
                        pc += 2;
                    break;

                    case 0x0006: //0x8XY6 -> V[X] = V[X] SHR 1, V[F] = least significant bit of V[X]
                        V[0xF] = V[(opcode & 0x0F00) >> 8] & 0x1;
                        V[(opcode & 0x0F00) >> 8] >>= 1;
                        pc += 2;
                    break;

                    case 0x0007: //0x8XY7 -> Set V[F] = 1 if V[Y] > V[X], then V[Y] -= V[X]
                        if(V[(opcode & 0x00F0) >> 4] > V[(opcode & 0x0F00) >> 8]) V[0xF] = 1;
                        else V[0xF] = 0;
                        V[(opcode & 0x00F0) >> 4] -= V[(opcode & 0x0F00) >> 8];
                        pc += 2;
                    break;

                    case 0x000E: //0x8XYE -> V[X] = V[X] SHL 1, V[F] = most significant bit of V[X]
                        V[0xF] = V[(opcode & 0x0F00) >> 8] >> 7;
                        V[(opcode & 0x0F00) >> 8] <<= 1;
                        pc += 2;
                    break;
                }
            break;

            case 0x9000: // 0x9XY0 -> Skip next instruction if V[X] != V[Y]
                if(V[(opcode & 0x0F00) >> 8] != V[(opcode & 0x00F0) >> 4]) pc += 4;
                else pc += 2;
            break;

            case 0xA000: // 0xANNN -> Set I = NNN
                i = (opcode & 0x0FFF);
                pc += 2;
            break;

            case 0xB000: //0xBNNN -> Jump to location NNN + V[0]
                pc = V[0x0] + (opcode & 0x0FFF);
            break;

            case 0xC000: //0xCXKK -> Set V[X] = Random byte AND kk
                V[(opcode & 0x0F00) >> 8] = (rand() % 0xFF) & (opcode & 0x00FF);
                pc += 2;
            break;

            case 0xD000: // 0xDXYN -> Display n-byte sprite starting at memory location I at (Vx, Vy), VF = collision
                {
                    unsigned short height = V[(opcode & 0x000F)];
                    unsigned short x = V[(opcode & 0x0F00) >> 8];
                    unsigned short y = V[(opcode & 0x00F0) >> 4];
                    unsigned short pixel;
                    V[0xF] = 0;
                    for(int yline = 0; yline < height; yline++){
                        pixel = MEM[i + yline];
                        for(int xline = 0; xline < 8; xline++){
                            if(pixel & (0x80 >> xline) != 0){
                                if(SCREEN[(x + xline + ((y + yline) * 64)) % (64*32)] == 1){
                                    V[0xF] = 1;
                                }
                                SCREEN[x + xline + ((y + yline) * 64)] ^= 1;
                            }
                        }
                    }
                    chip8_drawFlag = 1;
                    pc += 2;
                }
            break;

            case 0xE000:
                switch(opcode & 0x00FF){
                    case 0x009E: //0xEX9E -> Skip next instruction if key with the value of Vx is pressed
                        if(KEY[V[(opcode & 0x0F00) >> 8]] == 1) pc += 4;
                        else pc += 2;
                    break;

                    case 0x00A1: //0xEXA1 -> Skip next instruction if key with the value of Vx is not pressed
                        if(KEY[V[(opcode & 0x0F00) >> 8]] != 1) pc += 4;
                        else pc += 2;
                    break;
                }
            break;

            case 0xF000:
                switch(opcode & 0x00FF){
                    case 0x0007: // 0xFX07 -> Set V[X] = delay_timer
                        V[(opcode & 0x0F00) >> 8] = delay_timer;
                        pc += 2;
                    break;

                    case 0x000A: // 0xFX0A -> Wait for a key press, store the value of the key in V[X]
                        {
                            int keyPressed = 0;
                            for(int j = 0; j < 16; j++){
                                if(KEY[j] != 0){
                                    V[(opcode & 0x0F00) >> 8] = j;
                                    keyPressed = 1;
                                }
                            }
                            if(keyPressed == 0) return;
                            else printf("KeyPressed:%d\n", keyPressed);
                            pc += 2;
                        }
                    break;

                    case 0x0015: // 0xFX15 -> Set delay_timer = V[X]
                        delay_timer = V[(opcode & 0x0F00) >> 8];
                        pc += 2;
                    break;

                    case 0x0018: // 0xFX18 -> set sound_timer = V[X]
                        delay_timer = V[(opcode & 0x0F00) >> 8];
                        pc += 2;
                    break;

                    case 0x001E: // 0xFX1E -> Set I += V[X]
                        i += V[(opcode & 0x0F00) >> 8];
                        pc += 2;
                    break;

                    case 0x0029: // 0xFX29 -> Set i = location of sprite for digit V[X] ???
                        i = V[(opcode & 0x0F00) >> 8] * 0x5;
                        pc += 2;
                    break;

                    case 0x0033: // 0xFX33 -> Store BCD representation of Vx in memory locations I, I+1, and I+2.
                        MEM[i] = V[(opcode & 0x0F00) >> 8] / 100;
                        MEM[i + 1] = (V[(opcode & 0x0F00) >> 8] / 10) % 10;
                        MEM[i + 2] = (V[(opcode & 0x0F00) >> 8] % 100) % 10;
                        pc += 2;
                    break;

                    case 0x0055: // 0xFX55 -> Store registers V0 through Vx in memory starting at location I.
                        for(int j = 0; j <= ((opcode & 0x0F00) >> 8); j++)
                            MEM[i + j] = V[j];
                        i += ((opcode & 0x0F00) >> 8) + 1;
                        pc += 2;
                    break;

                    case 0x0065: // 0xFX65 -> Read registers V0 through Vx from memory starting at location I.
                        for(int j = 0; j <= ((opcode & 0x0F00) >> 8); j++)
                            V[j] = MEM[i + j];
                        i += ((opcode & 0x0F00) >> 8) + 1;
                        pc += 2;
                    break;
                }
            break;
        }
    }
    else{
        fputs("Program counter is outside memory", stderr);
        return;
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
