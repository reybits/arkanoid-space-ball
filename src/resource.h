/**********************************************\
*
*  Copyright (C) 2006 by Andrey A. Ugolnik
*  http://www.ugolnik.info
*  andrey@ugolnik.info
*
\**********************************************/

#pragma once

#include <vector>
#include <string>

class CResource final
{
public:
    CResource();
    ~CResource();

    bool Open(const char* filename);
    unsigned char* GetDataAllocMem(const char* name, unsigned& length);
    void FreeMem(unsigned char* data);

public:
    void EncodeData(void* data, unsigned length);

public:
    static const unsigned RES_SIGNATURE;
    static const unsigned char RES_XOR;
    static const unsigned RES_MAX_NAME = 99;

    struct sItemHeader
    {
        char name[RES_MAX_NAME + 1];
        unsigned position; // position in resource file (seek)
        unsigned length; // length of data
    };
    typedef std::vector<sItemHeader> FileList;

protected:
    std::string m_filename;
    FileList m_listFiles;
};
