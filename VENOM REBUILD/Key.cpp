#include "plugin.h"

unsigned char	currStates[256] = {};
unsigned char	prevStates[256] = {};
unsigned int	timeDelayPressed[256] = {};

void Dark::Key::Update() 
{
	memcpy(prevStates, currStates, 256);
	GetKeyboardState(currStates);
}

bool Dark::Key::Check(unsigned int key) 
{
	return key < 256 && (currStates[key] & 0x80);
}

bool Dark::Key::CheckJustDown(unsigned int key) 
{
	return key < 256 && (currStates[key] & 0x80) && !(prevStates[key] & 0x80);
}

bool Dark::Key::CheckJustUp(unsigned int key) 
{
	return key < 256 && !(currStates[key] & 0x80) && (prevStates[key] & 0x80);
}