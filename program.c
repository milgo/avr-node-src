#include "program.h"
#include "variables.h"

void loop()
{
	begin();
	setBOOL(DIGITALINPUT02, DI4());
	setBOOL(DIGITALINPUT01, DI3());
	DO1(getBOOL(DIGITALINPUT01));
	DO2(FLIP(0,TMR(0,5000,getBOOL(DIGITALINPUT02)),getBOOL(DIGITALINPUT02)));
	end();
}
