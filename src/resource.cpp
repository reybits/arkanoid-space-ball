/**********************************************\
*
*  Copyright (C) 2006 by Andrey A. Ugolnik
*  http://www.ugolnik.info
*  andrey@ugolnik.info
*
\**********************************************/

#include "resource.h"

#include <SDL_image.h>

#include <cstring>

const unsigned CResource::RES_SIGNATURE = 0xabcd;
const unsigned char CResource::RES_XOR = 0xaa;

const char* Images[] = {
    "energyhole.png",
    "monster_patrol.png",
    "monster_copter.png",
    "monster_eye.png",
    "monster_blackhole.png",
    "monster_fighter.png",
    "monster_hand.png",
    "monster_stone1.png",
    "monster_stone2.png",
    "monster_turbine.png",
    "monster_ufo.png",
    "monster_wreckage1.png",
    "monster_wreckage2.png",
    "balls.png",
    "vector.png",
    "fb.png",
    "bullets.png",
    "exploision.png",
    "bricks.png",
    "bricksmovbullets.png",
    "bonuses.png",
    "bonuses_small.png",
    "bonuses_aura.png",
    "gamebganims.png",
    "gamewall.png",
    "paddle.png",
    "mainmenubg.jpg",
    "mainmenubg2.jpg",
    "mainmenuicons.png",
    "cursor.png",
    "transp.png",
    "tutorial_dlg.png",
    "sinusstring.png",
    "options.png",
};

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

void CResource::preloadImages()
{
    for (size_t i = 0; i < sizeof(m_images) / sizeof(m_images[0]); i++)
    {
        m_images[i] = loadImage(Images[i]);
    }
}

void CResource::purgeImages()
{
    for (size_t i = 0; i < sizeof(m_images) / sizeof(m_images[0]); i++)
    {
        SDL_FreeSurface(m_images[i]);
        m_images[i] = nullptr;
    }
}

unsigned char* CResource::GetDataAllocMem(const char* name, unsigned& length) const
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

void CResource::FreeMem(unsigned char* data) const
{
    delete[] data;
}

void CResource::EncodeData(void* data, unsigned length) const
{
    unsigned char* p = static_cast<unsigned char*>(data);
    while (length--)
    {
        *p++ ^= RES_XOR;
    }
}

SDL_Surface* CResource::loadImage(const char* filename) const
{
    unsigned size;
    auto data = GetDataAllocMem(filename, size);
    if (data != nullptr)
    {
        auto rwops = SDL_RWFromConstMem(data, size);
        auto tmp = IMG_Load_RW(rwops, 1);

        FreeMem(data);
        if (tmp != nullptr)
        {
            auto optimizedImage = SDL_DisplayFormatAlpha(tmp);
            SDL_FreeSurface(tmp);
            printf(" done.\n");

            return optimizedImage;
        }
        else
        {
            printf(" %s\n", SDL_GetError());
        }
    }

    return nullptr;
}
