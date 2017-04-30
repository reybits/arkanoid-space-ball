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

enum class eImage
{
    EnergyHole,
    MonsterPatrol,
    MonsterCopter,
    MonsterEye,
    MonsterBlackhole,
    MonsterFighter,
    MonsterHand,
    MonsterStone1,
    MonsterStone2,
    MonsterTurbine,
    MonsterUfo,
    MonsterWreckage1,
    MonsterWreckage2,
    Balls,
    Vector,
    Fb,
    Bullets,
    Exploision,
    Bricks,
    BricksMovBullets,
    Bonuses,
    BonusesSmall,
    BonusesAura,
    GameBGanims,
    GameWall,
    Paddle,
    MainmenuBg,
    MainmenuBg2,
    MainMenuIcons,
    Cursor,
    Transp,
    TutorialDlg,
    SinusString,
    Options,

    Count
};

struct SDL_Surface;

class CResource final
{
public:
    CResource();
    ~CResource();

    bool Open(const char* filename);
    unsigned char* GetDataAllocMem(const char* name, unsigned& length) const;
    void FreeMem(unsigned char* data) const;

    void preloadImages();
    void purgeImages();

    SDL_Surface* getImage(eImage id) const
    {
        return m_images[(unsigned)id];
    }

public:
    void EncodeData(void* data, unsigned length) const;
    SDL_Surface* loadImage(const char* filename) const;
    void unloadImage(SDL_Surface* surface) const;

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

private:
    std::string m_filename;
    FileList m_listFiles;

    SDL_Surface* m_images[(unsigned)eImage::Count];
};
