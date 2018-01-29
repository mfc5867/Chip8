//
// Created by mfc98 on 1/15/2018.
//

#ifndef CHIP8_CHIP_H
#define CHIP8_CHIP_H


class Chip
{
    unsigned short opcode;
    /*
     * 0x000-0x1FF - Chip 8 interpreter (contains font set in emu)
     * 0x050-0x0A0 - Used for the built in 4x5 pixel font set (0-F)
     * 0x200-0xFFF - Program ROM and work RAM
     */
    unsigned char memory[4096];
    //15 8-bit registers and one condition register for the carry bit
    unsigned char V[16];

    //16-bit register that points to a location in memory
    unsigned short I;

    //Program counter used to determine where to read next opcode
    unsigned short pc;

    //Timer that decreases by a value of one each turn while it is not equal to zero
    unsigned char delay_timer;

    //Timer that decreases by a value of one each turn while it is not equal to zero
    //A beep will be emiited each cycle that it is not zero
    unsigned char sound_timer;

    //Stack used to store location of pc before calling a subroutine
    unsigned short stack[16];

    //Pointer to current top of the stack
    unsigned short sp;

    //Storage of the current state of a hex keyboard representation
    unsigned char key[16];

    public:
    	void keyPress(unsigned char keyPressed);
    	void keyReset();
        void initialize();
        void loadGame();
        void emulateCycle();
        void opcodeHandler(unsigned short code);

        //Current state of the graphics
        //TODO: Make private and access through a function for main
        unsigned char gfx[64 * 32];
};

#endif //CHIP8_CHIP_H

/*
 * std::cout << "opcode: " << opcode << std::endl;
            	std::cout << "x: " << x << std::endl;
            	std::cout << "y: " << y << std::endl;
            	std::cout << "1: " << V[0] << std::endl;
            	std::cout << "2: " << V[1] << std::endl;
            	std::cout << "3: " << V[2] << std::endl;
            	std::cout << "4: " << V[3] << std::endl;
            	std::cout << "5: " << V[4] << std::endl;
            	std::cout << "6: " << V[5] << std::endl;
            	std::cout << "7: " << V[6] << std::endl;
            	std::cout << "8: " << V[7] << std::endl;
            	std::cout << "9: " << V[8] << std::endl;
            	std::cout << "a: " << V[9] << std::endl;
            	std::cout << "b: " << V[10] << std::endl;
            	std::cout << "c: " << V[11] << std::endl;
            	std::cout << "d: " << V[12] << std::endl;
            	std::cout << "e: " << V[13] << std::endl;
 */

