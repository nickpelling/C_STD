# C Implementation of Standard Container Library

This library implements a C-style version of the standard container library. The aim is not to duplicate C++'s templates, but rather to create familiar-feeling container helpers, that can easily and safely be used in production code.

# Using this container library

Creating and using a container is as simple as this:

	#include <stdio.h>

	#include "std/container.h"				// Include a single container header file

	int main(int argc, char * argv[])
	{
		int aiArray[10];
		size_t szNum;

		std_vector(int) v;					// Declare a container v containing ints
		std_construct(v);					// Construct (initialise) the container

		std_push_back(v, 1, 2, 3, 3, 5);	// Push 1 to the back of the container, then 2, 3, 4, 5

		std_for_each(v, it)
			printf("%d ", std_iterator_at(it)[0]);
		printf("\r\n");

		szNum = std_pop_back(v, aiArray, 10);	// Pop up to 10 items from the container into an array
		for (int i = 0; i < szNum; i++)
			printf("%d ", aiNum[i]);
		printf("\r\n");

		std_destruct(v);					// Destruct the container
	
		return 0;
	}

# Licence

For now, this code is still copyright Nick Pelling, but once it has been properly tested, it will have an open source licence.
