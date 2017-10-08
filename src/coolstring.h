/**********************************************\
*
*  Copyright (C) 2006 by Andrey A. Ugolnik
*  http://www.ugolnik.info
*  andrey@ugolnik.info
*
\**********************************************/

#pragma once

#include <vector>

class CCoolString final
{
public:
    CCoolString();
    ~CCoolString();

    void Add(const char* pchFormat, ...);
    void Draw();
    void Clear();
    int GetCount();

protected:
#define COOLSTRING_MAX_LEN  20
    struct _COOL_STRING
    {
        char* string;
        int seconds;
    };
    std::vector<_COOL_STRING> m_strings;
};
