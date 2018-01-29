//File: main.cpp
//Author: Mark Connolly
//Description: Emulates the behavior of the Chip-8
//architecture to play games from valid ROM image
//files for the system.
//Note: Currently will not run without SDL2.dll, unsure
//where it is being used. will remove once found if
//found before switching to SDL2.

#include <GL/glut.h>
#include <GL/freeglut.h>
#include <thread>
#include <iostream>
#include <Windows.h>
#include "Chip.h"

//Chip-8 object used to run a game
Chip chip8;

//Initializes the GLUT graphics to represent the 64 * 32
//pixel display
void startupScreen()
{
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0,640,0,320);
	glFlush();
}

//writePixel
//Writes each pixel in the gfx of the chip to the screen
void writePixel()
{
	glBegin(GL_QUADS);
	int y = 1;
	int x;
	float xtemp;
	float ytemp;
	for (int i = 0; i < (64 * 32); i++)
	{
		x = (i % 64) + 1;
		ytemp = y;
		glClear(GL_COLOR_BUFFER_BIT);
		switch (chip8.gfx[i])
		{
		case 1:
			glColor3f(1.0f, 1.0f, 1.0f);
			break;
		default:
			glColor3f(0.0f, 0.0f, 0.0f);
			break;
		}
		xtemp = (x - 32) * 0.031;
		ytemp = -(y - 16) * 0.0600;
		glVertex2f(xtemp - 0.031, ytemp - 0.0625);
		glVertex2f(xtemp - 0.031,ytemp);
		glVertex2f(xtemp,ytemp);
		glVertex2f(xtemp,ytemp - 0.0625);

		if (x == 64) y++;
	}
	glEnd();
	glFlush();

}

//keyDetection
//Converts keys to a hex keyboard representation
void keyDetection(unsigned char key, int x, int y)
{
	switch (key)
	{
	case '1':
		chip8.keyPress(10);
		break;
	case '2':
		chip8.keyPress(11);
		break;
	case '3':
		chip8.keyPress(12);
		break;
	case '4':
		chip8.keyPress(13);
		break;
	case 'q':
		chip8.keyPress(1);
		break;
	case 'w':
		chip8.keyPress(2);
		break;
	case 'e':
		chip8.keyPress(3);
		break;
	case 'r':
		chip8.keyPress(14);
		break;
	case 'a':
		chip8.keyPress(4);
		break;
	case 's':
		chip8.keyPress(5);
		break;
	case 'd':
		chip8.keyPress(6);
		break;
	case 'f':
		chip8.keyPress(15);
		break;
	case 'z':
		chip8.keyPress(7);
		break;
	case 'x':
		chip8.keyPress(8);
		break;
	case 'c':
		chip8.keyPress(9);
		break;
	case 'v':
		chip8.keyPress(0);
		break;
	}
}


//initDisplay
//Initializes the graphics window and functions
void initDisplay(int argc, char *argv[])
{
	glutInit(&argc, argv);
	glutInitWindowSize(640,320);
	glutInitWindowPosition(100,100);
	glutCreateWindow("Chip-8 Emulator");
	glutDisplayFunc(startupScreen);
	glutKeyboardFunc(keyDetection);
}

//main
//Emulates the behavior of the Chip-8 architecture
int main(int argc, char *argv[])
{
    chip8.initialize();
    chip8.loadGame();
    initDisplay(argc, argv);
    glutDisplayFunc(writePixel);
    glutIdleFunc(writePixel);
    for(;;)
    {
    	chip8.emulateCycle();
    	glutPostRedisplay();
    	glutMainLoopEvent();
    }

    return 0;
}
