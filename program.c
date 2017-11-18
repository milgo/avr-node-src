#include "program.h"
#include "variables.h"

void loop()
{
	begin();
	setBOOL(DIGITALINPUT01, DI3());
	DO1(getBOOL(DIGITALINPUT01));
	end();
}
