#include <iostream>

#include "ben/io.h"

int main()
{
	ben::str120 my_string;// = "obama %u";

	btl::print(my_string, 255);

	return 0;
}