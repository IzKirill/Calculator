#include "Clear_Buffer.h"
#include <stdio.h>

int ClearStatusBuffer()
{
    int ch = 0, i = 0;

    while((ch = getchar()) != '\n')
    {
        if (ch != ' ' && ch != '\n')
            i = 1;
    }

    if (i == 1)
    {
        return 1;
    }

    return 0;
}
