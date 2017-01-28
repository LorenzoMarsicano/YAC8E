#ifndef CHIP8_H
#define CHIP8_H

unsigned char MEM[4096];
unsigned char V[16];
unsigned short i;
unsigned short pc;
unsigned char sp;
unsigned short STACK[16];
unsigned char SCREEN[64 * 32];
unsigned char delay_timer;
unsigned char sound_timer;
unsigned short opcode;
unsigned char key[16];

void init();

void emulateCycle();

#endif
