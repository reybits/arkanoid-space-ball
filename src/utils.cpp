/**********************************************\
*
*  Copyright (C) 2006 by Andrey A. Ugolnik
*  http://www.ugolnik.info
*  andrey@ugolnik.info
*
\**********************************************/

#include "utils.h"

void EncodeDecode(void* data, uint32_t size)
{
    uint8_t* p = (uint8_t*)data;
    uint8_t by = 0xaa;
    while (size--)
    {
        *p ^= by;
        *p += 128;
        by += 3;
        p++;
    }
}
