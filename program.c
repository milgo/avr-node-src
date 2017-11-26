#include "program.h"
#include "variables.h"

void loop()
{
	begin();
	setBOOL(INPUT1, DI3(2));
	DO1(3,FLIP(0,TMR(1,1000,getBOOL(INPUT1)),getBOOL(INPUT1)));
	end();
}
