/**********************************************\
*
*  Copyright (C) 2006 by Andrey A. Ugolnik
*  http://www.ugolnik.info
*  andrey@ugolnik.info
*
\**********************************************/

#include "resource.h"

#include <cstring>

const unsigned CResource::RES_SIGNATURE = 0xabcd;
const unsigned char CResource::RES_XOR = 0xaa;

CResource::CResource()
{
}

CResource::~CResource()
{
}

bool CResource::Open(const char* filename)
{
    FILE* f = fopen(filename, "rb");
    if (f)
    {
        unsigned signature;
        // read signature
        fread(&signature, 1, sizeof(signature), f);
        if (signature == RES_SIGNATURE)
        {
            // read files count
            unsigned count;
            fread(&count, 1, sizeof(count), f);
            EncodeData(&count, sizeof(count));

            m_listFiles.reserve(count);

            sItemHeader header;
            for (unsigned i = 0; i < count; i++)
            {
                // read and store files list
                if (sizeof(header) == fread(&header, 1, sizeof(header), f))
                {
                    EncodeData(&header, sizeof(header));
                    //printf("header '%s' %d bytes\n", header.name, header.length);
                    m_listFiles.push_back(header);
                }
                else
                {
                    printf("Incorrect resource file size\n");
                    fclose(f);
                    return false;
                }
            }
            m_filename = filename;
            printf("Resource file '%s' contain %d files\n", filename, count);
            return true;
        }
        printf("Invalid resource file\n");
        fclose(f);
    }

    printf("File '%s' not found.\n", filename);

    return false;
}

unsigned char* CResource::GetDataAllocMem(const char* name, unsigned& length)
{
    if (m_filename.empty() == false)
    {
        printf("Opening '%s' resource...", name);
        for (size_t i = 0, size = m_listFiles.size(); i < size; i++)
        {
            if (strcmp(m_listFiles[i].name, name) == 0)
            {
                length = m_listFiles[i].length;
                unsigned char* data = new unsigned char[length];

                FILE* f = fopen(m_filename.c_str(), "rb");
                if (f)
                {
                    fseek(f, m_listFiles[i].position, SEEK_SET);
                    fread(data, 1, length, f);
                    fclose(f);
                    EncodeData(data, length);
                    return data;
                }
            }
        }
        printf("\n  file '%s' not found\n", name);
    }
    else
    {
        printf("Resource file not loaded\n");
    }

    return 0;
}

void CResource::FreeMem(unsigned char* data)
{
    delete[] data;
}

void CResource::EncodeData(void* data, unsigned length)
{
    unsigned char* p = static_cast<unsigned char*>(data);
    while (length--)
    {
        *p++ ^= RES_XOR;
    }
}
