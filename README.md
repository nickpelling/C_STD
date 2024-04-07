# C Implementation of Standard Container Library

This library is a C version of the standard container library. The aim is not to duplicate C++'s templates, but rather to create familiar-feeling container helpers, that can easily and safely be used in production code.

# Using this container library

Creating and using a container is as simple as this:

    #include <stdio.h>
	
    // Include a single container header file
    #include "std/container.h"

    int main(int argc, char * argv[])
    {
        int aiArray[10];
        size_t szNum;

        // Declare a container v containing ints
        std_vector(int) v;					

        // Construct (initialise) the container
        std_construct(v);

        // Push 1 to the back of the container, then 2, 3, 4, 5
        std_push_back(v, 1, 2, 3, 4, 5);

        for (std_each(v, it))
        {
            printf("%d ", std_iterator_at(it)[0]);
        }
        printf("\r\n");

        // Pop up to 10 items from the container into an array
        // We only pushed 5 items to it, so szNum will end up set to 5
        szNum = std_pop_back(v, aiArray, 10);
        for (size_t i = 0; i < szNum; i++)
            printf("%d ", aiArray[i]);
        printf("\r\n");

        // Destruct (destroy) the container
        std_destruct(v);
	
        return 0;
    }

Currently, the library implements vector/list/deque/queue/stack, but more containers will appear in future releases.

# Published under the MIT License

Copyright (c) 2024 Nick Pelling

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
