#include "program.h"
#include "variables.h"

void loop()
{
	begin();
	DO1(1,AND(0,DI4(2),DI3(3)));
	end();
}
