/**********************************************\
*
*  Copyright (C) 2006 by Andrey A. Ugolnik
*  http://www.ugolnik.info
*  andrey@ugolnik.info
*
\**********************************************/

#include "assets.h"

#import <Foundation/Foundation.h>
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
        CFBundleRef mainBundle = CFBundleGetMainBundle();
        if (mainBundle)
        {
            CFURLRef executable = CFBundleCopyExecutableURL(mainBundle);
            CFStringRef string = CFURLCopyFileSystemPath(executable, kCFURLPOSIXPathStyle);

            CFStringGetCString(string, m_assetsPath, sizeof(m_assetsPath), kCFStringEncodingUTF8);

            CFRelease(string);
            CFRelease(executable);
        }

        char* p = ::strrchr(m_assetsPath, '/');
        if (p != nullptr)
        {
            p[0] = 0;
            p = ::strrchr(m_assetsPath, '/');
            if (p != nullptr)
            {
                p[1] = 0;
                ::strcat(m_assetsPath, "Resources/assets/");
            }
        }

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
