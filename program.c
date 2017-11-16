#include "program.h"
#include "variables.h"

void loop()
{
	begin();
	FLP(2,FLP(0,FLP(1,AND(I3(),I4()),getINT(NEWINT)),I1()),I5());
	end();
}
