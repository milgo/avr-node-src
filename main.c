#include "program.h"
#include "setup.h"

int main(void)
{
	setup();
    while (1) 
    {
        loop();
    }
}