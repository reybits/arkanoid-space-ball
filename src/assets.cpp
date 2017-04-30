/**********************************************\
*
*  Copyright (C) 2006 by Andrey A. Ugolnik
*  http://www.ugolnik.info
*  andrey@ugolnik.info
*
\**********************************************/

#include "assets.h"

#include <cstring>

namespace assets
{
    namespace
    {
        char m_assetsPath[1024] = { 0 };
        unsigned m_assetsPathLength = 0;
    }

    void initialize()
    {
        ::strncpy(m_assetsPath, "assets/", sizeof(m_assetsPath));
        m_assetsPathLength = ::strlen(m_assetsPath);
    }

    void deinitialize()
    {
    }

    const char* makePath(const char* path)
    {
        ::strncpy(m_assetsPath + m_assetsPathLength, path, sizeof(m_assetsPath) - m_assetsPathLength);
        return m_assetsPath;
    }
}
