// File: Chip.cpp
// Author: Mark Connolly
// Description: Emulates the initialization and cyclye operation of the Chip-8 cpu
// by deciphering opcodes read from a binary rom image of the desired
// program. Cycles are emulated at rate of approximately 60Hz.

#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <string>
#include <Windows.h>
#include <ios>
#include "Chip.h"

//path of the rom image for the game
const char *filename = "games/PONG2";

//Chip 8 fontset for characters 0-F
unsigned char chip8_fontset[80] =
        {
                0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
                0x20, 0x60, 0x20, 0x20, 0x70, // 1
                0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
                0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
                0x90, 0x90, 0xF0, 0x10, 0x10, // 4
                0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
                0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
                0xF0, 0x10, 0x20, 0x40, 0x40, // 7
                0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
                0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
                0xF0, 0x90, 0xF0, 0x90, 0x90, // A
                0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
                0xF0, 0x80, 0x80, 0x80, 0xF0, // C
                0xE0, 0x90, 0x90, 0x90, 0xE0, // D
                0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
                0xF0, 0x80, 0xF0, 0x80, 0x80  // F
        };

//initialize
//Should be called when a chip object is first created to
//set each structure to its initial values
void Chip::initialize()
{
    //Initializes registers
    pc = 0x200;
    opcode = 0;
    I = 0;
    sp = 0;

    //Timers initialized
    sound_timer = 0;
    delay_timer = 0;

    //Clear Display
    for (unsigned char &i : gfx)
    {
    	i = 0;
    }

    //Clear stack
    for (unsigned short &i : stack)
    {
        i = 0;
    }

    //Clear registers V0-VF
    for (unsigned char &i : V)
    {
        i = 0;
    }

    //Clear memory
    for (unsigned char &i : memory)
    {
        i = 0;
    }
    //Writes the fontset into memory
    for (int i = 0; i < 80; i++)
    {
        memory[80 + i] = chip8_fontset[i];
    }
}

//keyPress
//Takes the value of a pressed key and sets the key bit for that key high
//Inputs: pressedKey: The value of the key being pressed
void Chip::keyPress(unsigned char pressedKey)
{
	key[pressedKey] = 1;
	std::cout << std::hex << static_cast<unsigned short>(pressedKey) << std::endl;
}

//keyReset
//Resets the value of key so that input is not registered twice
void Chip::keyReset()
{
	for (unsigned char &c : key)
	{
		c = 0;
	}
}

//emulateCycle
//Handles the emulation of each cycle dealing with a single opcode in memory
void Chip::emulateCycle()
{
	//While the sound_timer is high, emit a beep each cycle
    if (sound_timer > 0)
    {
    	Beep(1000, 500);
    }

    //Fetch Opcode
    opcode = memory[pc] << 8;
    opcode |= memory[pc + 1];

    //Decode and execute opcode
    opcodeHandler(opcode);

    //Increment the program timer
    pc += 2;

    //Update timers
    if (delay_timer != 0) delay_timer -= 1;
    if (sound_timer != 0) sound_timer -= 1;

    Delay(10);

    //Reset keys
    keyReset();
}

//loadGame
//Reads each byte from the ROM file and stores that value to the
//memory of the chip.
void Chip::loadGame()
{
	//counter used to determine which byte is ccurrently being stored
	//int counter = 0;
	std::ifstream gameFile(filename, std::ios::binary | std::ios::in);
	if (gameFile.is_open())
	{

		int size = gameFile.tellg();
		//Find the size of the file
		gameFile.seekg(0, std::ios::end);
		size = gameFile.tellg() - size;
		gameFile.seekg(0, std::ios::beg);

		char buffer[size];
		//Read each byte in the file into the buffer
		gameFile.read(buffer, size);

		//Iterate through the buffer to store each character into the memory
		for (char &c : buffer)
		{
			//counter += 1;
			//Commented while running games
			//Used to print out each of the lines of opcode in the ROM
			/*
			if (counter % 2 == 1)
			{
				std::cout << std::hex << pc << ": " << static_cast<unsigned short>(c) << " ";
			}
			else
			{
				std::cout << std::hex << static_cast<unsigned short>(c) << std::endl;
			}
			*/
			memory[pc] = c;
			pc += 1;
		}

		pc = 0x0200;
	}
	gameFile.close();
}


void Chip::opcodeHandler(unsigned short code)
{
    unsigned short x;
    unsigned short y;
    char keyPress = 0;
    std::cout << std::hex << opcode << std::endl;
    switch (opcode & 0xF000)
    {
        case 0x0000:
            switch (opcode & 0x00F0)
            {
                case 0x00E0:
                    switch (opcode & 0x000F)
                    {
                        case 0x0000:
                        	//00E0: Clear the screen
                        	for (unsigned char &pxl : gfx)
                        	{
                        		pxl = 0;
                        	}
                            V[15] = 0;
                            break;
                        case 0x000E:
                        	//00EE Return from a subroutine
                            pc = stack[sp - 1];
                            stack[sp - 1] = 0;
                            sp -= 1;
                            V[15] = 0;
                            break;
                    }
                    break;
                default:
                    break;
            }
            break;
        case 0x1000:
        	//1NNN: Jump to address NNN
            pc = opcode & 0x0FFF;
            pc -= 2;
            V[15] = 0;
            break;
        case 0x2000:
        	//2NNN: Call the subroutine at address NNN
            stack[sp] = pc;
            sp += 1;
            pc = opcode & 0x0FFF;
            pc -= 2;
            V[15] = 0;
            break;
        case 0x3000:
        	//3XNN: Skip the next if VX == NN
            x = (opcode >> 8) & 0x0F;
            if (V[x] == (opcode & 0x00FF)) pc += 2;
            V[15] = 0;
            break;
        case 0x4000:
        	//4XNN: Skip the next instruction if VX != NN
            x = (opcode >> 8) & 0x0F;
            if (V[x] != (opcode & 0x00FF)) pc += 2;
            V[15] = 0;
            break;
        case 0x5000:
        	//5XY0: Skip the next instruction if VX == VY
            x = (opcode >> 8) & 0x0F;
            y = (opcode >> 4) & 0x0F;
            if (V[x] == V[y]) pc += 2;
            V[15] = 0;
            break;
        case 0x6000:
        	//6XNN: Sets the value of VX to NN
            x = (opcode >> 8) & 0x0F;
            V[x] = (opcode & 0x00FF);
            V[15] = 0;
            break;
        case 0x7000:
        	//7XNN: increases the value of VX by NN (Carry not set)
            x = (opcode >> 8) & 0x0F;
            V[x] += (opcode & 0x00FF);
            V[15] = 0;
            break;
        case 0x8000:
            x = (opcode >> 8) & 0x0F;
            y = (opcode >> 4) & 0x0F;
            switch (opcode & 0x000F)
            {
                case 0x0000:
                	//8XY0: VX = VY
                    V[x] = V[y];
                    V[15] = 0;
                    break;
                case 0x0001:
                	//8XY1: VX |= VY
                    V[x] = V[x]|V[y];
                    V[15] = 0;
                    break;
                case 0x0002:
                	//8XY2: VX &= VY
                    V[x] = V[x] & V[y];
                    V[15] = 0;
                    break;
                case 0x0003:
                	//8XY3: VX ^= VY
                    V[x] = V[x] ^ V[y];
                    V[15] = 0;
                    break;
                case 0x0004:
                	//8XY4: VX += VY (Carry set)
                    if (V[x] > (0xFF - V[y])) V[15] = 1;
                    else V[15] = 0;
                    V[x] += V[y];
                    break;
                case 0x0005:
                	//8XY5: Vx -= VY (Carry set on no borrow)
                    if (V[x] > V[y]) V[15] = 1;
                    else V[15] = 0;
                    V[x] -= V[y];
                    break;
                case 0x0006:
                	//8XY6: VY = VY >> 1 and VX = LSB before shift
                    V[x] = V[y] & 0x01;
					V[y] = V[y] >> 1;
                    V[15] = 0;
                    break;
                case 0x0007:
                	//8XY7: VX = VY - VX (Carry set on no borrow)
                    V[x] = V[y] - V[x];
                    if (V[y] > V[x]) V[15] = 1;
                    else V[15] = 0;
                    break;
                case 0x000E:
                	//8XYE: VY = VY << 1 and VX = MSB before shift
                    V[x] = V[y] & 0x80;
                    V[y] = V[y] << 1;
                    V[15] = 0;
                    break;
            }
            break;
        case 0x9000:
        	//9XY0: Skips if VX != VY
            x = opcode >> 8 & 0x0F;
            y = opcode >> 4 & 0x0F;
            if ( V[x] != V[y]) pc += 2;
            V[15] = 0;
            break;
        case 0xA000:
        	//ANNN: I = NNN
            I = opcode & 0x0FFF;
            V[15] = 0;
            break;
        case 0xB000:
        	//BNNN: PC = NNN + V0
            pc = V[0] + (opcode & 0x0FFF) - 2;
            V[15] = 0;
            break;
        case 0xC000:
        	//CXNN: VX = rand() & NN
            x = opcode >> 8 & 0x0F;
            V[x] = (rand() % 256) & (opcode & 0x00FF);
            V[15] = 0;
            break;
        case 0xD000:
        	//DXYN: Draw a sprite stroed in location I
        	//with a height of N at the location VX, VY
        	//Xors the bit in location I with that in
        	//gfx (Carry set if xor removes bit)
            x = (opcode >> 8 & 0x000F);
            y = ((opcode >> 4) & 0x000F);
            x = V[x];
            y = V[y];
            //std::cout << "x: " << x << std::endl;
            //std::cout << "y: " << y << std::endl;
            V[15] = 0;
            for (unsigned short i = 0; i < ((opcode & 0x0F)); i++)
            {
            	for (int j = 7; j >= 0; j--)
            	{
            		if (((memory[I + i] >> j) & 0x01 == 1) && (gfx[(y*64) + x] == 1))
            		{
            			V[15] = 1;
            		}
            		gfx[(y*64) + x] ^= ((memory[I + i] >> j) & 0x01);
            		if (j == 0)
            		{
            			x -= 7;
            			y += 1;
            		}
            		else
            		{
            			x += 1;
            		}
            	}
            }
            break;
        case 0xE000:
            x = (opcode >> 8 & 0x0F);
            switch (opcode & 0x00FF)
            {
                case 0x009E:
                	//EX9E: Skip if VX == key pressed
                    if (key[V[x]] == 1)
                    {
                    	pc += 2;
                    }
                    V[15] = 0;
                    break;
                case 0x00A1:
                	//EXA1: Skip if VX != key pressed
                    if (key[V[x]] == 0)
                    {
                    	pc += 2;
                    }
                    V[15] = 0;
                    break;
            }
            break;
        case 0xF000:
            x = opcode >> 8 & 0x0F;
            switch (opcode & 0x00FF)
            {
                case 0x0007:
                	//FX07: VX = get delay
                    V[x] = delay_timer;
                    V[15] = 0;
                    break;
                case 0x000A:
                	//FX0A: poll for VX = key
                    while (!keyPress)
                    {
                    	for (unsigned char i = 0; i < 16; i++)
                    	{
                    		if (key[i] == 1)
                    		{
                    			keyPress = 1;
                    			V[x] = i;
                    		}
                    	}
                    }
                    V[15] = 0;
                    break;
                case 0x0015:
                	//FX15: delay_timer = VX
                    delay_timer = V[x];
                    V[15] = 0;
                    break;
                case 0x0018:
                	//FX18: sound_timer = VX
                    sound_timer = V[x];
                    V[15] = 0;
                    break;
                case 0x001E:
                	//FX1E: I += VX
                    I += V[x];
                    V[15] = 0;
                    break;
                case 0x0029:
                	//FX29: I = character in VX
                	I = (V[x] * 40) + 80;
                    V[15] = 0;
                    break;
                case 0x0033:
                	//FX33: Store BCD of VX in I, I+1 and I+2
                    memory[I] = (x / 100) % 10;
                    memory[I + 1] = (x / 10) % 10;
                    memory[I + 2] = x % 10;
                    V[15] = 0;
                    break;
                case 0x0055:
                	//FX55: Dump values of V0 through VX in memory at I,
                	//Incrementring I with each value  written
                    for (int i = 0; i <= x; i++)
                    {
                        memory[I] = V[i];
                        I += 1;
                    }
                    V[15] = 0;
                    break;
                case 0x0065:
                	//FX65: Read values in I into V0 through VX, incrementing
                	//I with each read value
                    for (int i = 0; i <= x; i++)
                    {
                        V[i] = memory[I];
                        I += 1;
                    }
                    V[15] = 0;
                    break;
            }
            break;
    }
}
