#include <stdio.h>

#include "esm/esm.h"

uint32_t esm_global_time;

int main(int argc, char *argv[])
{
	srand(time(NULL));
	esm_process();

	return 0;
}
