/**********************************************\
*
*  Copyright (C) 2006 by Andrey A. Ugolnik
*  http://www.ugolnik.info
*  andrey@ugolnik.info
*
\**********************************************/

#include "generatelevel.h"
#include "accessor.h"
#include "game.h"
#include "random.h"
#include "resource.h"

CGenerateLevel::CGenerateLevel()
{
    for (int i = 0; i < 10; i++)
    {
        m_anPrevPatterns[i] = -1;
    }
    m_pbyLevelPatterns  = 0;
    m_nPatterns         = 0;
}

CGenerateLevel::~CGenerateLevel()
{
    a::res()->FreeMem((unsigned char*)m_pbyLevelPatterns);
}

void CGenerateLevel::Generate(int nLevel, bool bUseCustom)
{
    // clear level
    a::ark()->m_vecLevelBricks.clear();
    memset(m_abyLevel, 0, sizeof(m_abyLevel));

    int nCustomLevelsCount  = 0;
    if (bUseCustom == true)
    {
        printf("Use custom levels mode.\n");
        char buffer[MAX_PATH];
        snprintf(buffer, sizeof(buffer), "%scustomlevels", a::userProfile());
        FILE* file = fopen(buffer, "rb");
        if (file != nullptr)
        {
            fseek(file, 0, SEEK_END);
            nCustomLevelsCount  = ftell(file) / (BRICKS_WIDTH * BRICKS_HEIGHT);
            fseek(file, (nLevel % nCustomLevelsCount) * (BRICKS_WIDTH * BRICKS_HEIGHT), SEEK_SET);
            fread(m_abyLevel, (BRICKS_WIDTH * BRICKS_HEIGHT), 1, file);
            fclose(file);
        }
        else
        {
            printf("  No custom levels found, using internal levels set.\n");
            memcpy(m_abyLevel, &m_pbyLevelPatterns[nLevel % m_nPatterns], BRICKS_WIDTH * BRICKS_HEIGHT);
        }
    }
    else
    {
        /*for(int x = 0; x < BRICKS_WIDTH; x++) {
            Uint8   byType  = 1 + a::rnd().Get(BOX_END - 1);
            for(int y = 0; y < BRICKS_HEIGHT; y++) {
                SetBrick(x, y, byType);
            }
        }*/

        EnableCursor(true);
        if (nLevel < 10)
        {
            for (int x = 0; x < BRICKS_WIDTH; x++)
            {
                for (int y = 0; y < BRICKS_HEIGHT; y++)
                {
                    SetBrick(x, y, *(m_pbyLevelPatterns + nLevel * BRICKS_WIDTH * BRICKS_HEIGHT + BRICKS_HEIGHT * x + y));
                }
            }
        }
        else
        {
            int nMaxIterations  = 100;
            //do {
            for (int i = 0; i < 4; i++)
            {
                _FillLevel(1, i);
            }
            /*if(_CalculateBricksCount() > 100) {// && nLevel < 15) {
                int nDx = (1 + a::rnd().Get(6));
                int nDy = (1 + a::rnd().Get(5));

                for(int x = BRICKS_WIDTH / 2 - nDx; x < BRICKS_WIDTH / 2 + nDx; x++) {
                    for(int y = BRICKS_HEIGHT / 2 - nDy; y < BRICKS_HEIGHT / 2 + nDy; y++) {
                        SetBrick(x, y, BOX_NONE);
                    }
                }
            }
            */
            _RollLevel();   // Roll level

            m_bClonedByVertical     = false;
            m_bClonedByHorizontal   = false;
            switch (a::rnd().Get(4))
            {
            case 0:
                _CloneLevelBy(CLONE_HORIZONTAL);
                _CloneLevelBy(CLONE_VERTICAL);
                break;
            case 1:
                _CloneLevelBy(CLONE_VERTICAL);
                _CloneLevelBy(CLONE_HORIZONTAL);
                break;
            case 2:
                _CloneLevelBy(CLONE_VERTICAL);
                break;
            case 3:
                _CloneLevelBy(CLONE_HORIZONTAL);
                break;
            }
            printf("Bricks count %d, iteration %d\n", _CalculateBricksCount(), nMaxIterations);
            //} while(_CalculateBricksCount() < 80 && --nMaxIterations);
        }
    }

    // fill actual level
    //printf("(II) Generated level data below:\n");
    _BRICK  brick;
    int nCountToShoot   = 0;
    int nBricksCount    = 0;
    for (int y = 0; y < BRICKS_HEIGHT; y++)
    {
        for (int x = 0; x < BRICKS_WIDTH; x++)
        {
            Uint8   byType  = GetBrick(x, y);
            //printf("%s", !byType ? " " : "O");
            //printf("%2d", byType);
            if (byType > BOX_NONE && byType < BOX_END)
            {
                nBricksCount++;
                brick.fX                = BRICK_X + x * BRICK_W;
                brick.fY                = BRICK_Y + y * BRICK_H;
                brick.byType            = byType;
                brick.nCountToDie       = ((byType >= BOX_IM_0 && byType < BOX_BOMB_0) ? 15 : -1);
                brick.nFrame            = 0;
                brick.nAnimateType      = -1;
                brick.bDir              = byType == BOX_MOV_0 ? (a::rnd().Get(2) == 0 ? false : true) : (byType == BOX_MOV_1 ? true : false);
                brick.nDirChangeCount   = 0;
                brick.nCountToShoot     = 5 + nCountToShoot;
                a::ark()->m_vecLevelBricks.push_back(brick);
                if (byType >= BOX_SHOOT_0 && byType < BOX_BOMB_0)
                {
                    nCountToShoot += 5;
                }
            }
        }
        //printf("\n");
    }

    // if ustom level empty, we use internal levels
    if (bUseCustom == true && nBricksCount == 0)
    {
        if (nCustomLevelsCount < nLevel)
        {
            printf("Custom level empty, switch to internal levels\n");
            return Generate(nLevel, false);
        }
        else
        {
            printf("Now bricks on custom level %d, skip to next\n", nLevel++);
            return Generate(nLevel, true);
        }
    }

    printf("Total bricks on level %u\n", (uint32_t)a::ark()->m_vecLevelBricks.size());
}

void CGenerateLevel::_FillLevel(int nLevel, int nPos)
{
    int nPattern;
    do
    {
        nPattern    = a::rnd().Get(m_nPatterns);
    }
    while (_IsPatternExist(nPattern) == true);

    for (int x = nPos * 3; x < nPos * 3 + 3; x++)
    {
        for (int y = 0; y < BRICKS_HEIGHT; y++)
        {
            Uint8   byType  = *(m_pbyLevelPatterns + nPattern * BRICKS_WIDTH * BRICKS_HEIGHT + BRICKS_HEIGHT * x + y);
            //byType    = min(byType, a::rnd().Get(BOX_END));

            /*if(nLevel < 5) {
                if(byType >= BOX_DBL_0 && byType < BOX_TRD_0) {
                    SetBrick(x, y, BOX_NONE);
                }
            }
            else if(nLevel < 25) {
                if(byType >= BOX_TRD_0 && byType < BOX_IM_0) {
                    SetBrick(x, y, BOX_DBL_0 + (BOX_TRD_0 - BOX_DBL_0));
                }
                if(byType >= BOX_IM_0 && byType < BOX_BOMB_0) {
                    SetBrick(x, y, BOX_TRD_0 + (BOX_IM_0 - BOX_TRD_0));
                }
            }*/

            //byType    = BOX_10;
            SetBrick(x, y, byType);
        }
    }
}

void CGenerateLevel::_CloneLevelBy(int nType)
{
    if (nType == CLONE_VERTICAL)
    {
        m_bClonedByVertical = true;
        for (int y = 0; y < BRICKS_HEIGHT; y++)
        {
            for (int x = 0; x < BRICKS_WIDTH / 2; x++)
            {
                SetBrick(BRICKS_WIDTH - 1 - x, y, GetBrick(x, y));
            }
        }
    }
    else if (nType == CLONE_HORIZONTAL)
    {
        m_bClonedByHorizontal   = true;
        for (int x = 0; x < BRICKS_WIDTH; x++)
        {
            for (int y = 0; y < BRICKS_HEIGHT / 2; y++)
            {
                SetBrick(x, BRICKS_HEIGHT - 1 - y, GetBrick(x, y));
            }
        }
    }
}

bool CGenerateLevel::_IsPatternExist(int nPattern)
{
    int i;

    for (i = 0; i < 8; i++)
    {
        if (m_anPrevPatterns[i] == nPattern)
        {
            printf("pattern %d already used\n", nPattern);
            return true;
        }
    }
    for (i = 0; i < 7; i++)
    {
        m_anPrevPatterns[i] = m_anPrevPatterns[i + 1];
    }

    m_anPrevPatterns[7] = nPattern;

    return  false;
}

void CGenerateLevel::SetBrick(int nX, int nY, int nType)
{
    m_abyLevel[nX][nY]  = nType;
}

int CGenerateLevel::GetBrick(int nX, int nY)
{
    return  m_abyLevel[nX][nY];
}

void CGenerateLevel::_RollLevel()
{
    if (a::rnd().Get(2) == 0)
    {
        int nRollCount  = a::rnd().Get(BRICKS_WIDTH - 1);
        for (int i = 0; i < nRollCount; i++)
        {
            _RollHorizontal();
        }
    }
    else
    {
        int nRollCount  = a::rnd().Get(BRICKS_HEIGHT - 1);
        for (int i = 0; i < nRollCount; i++)
        {
            _RollVertical();
        }
    }
}

int CGenerateLevel::_CalculateBricksCount()
{
    int     nCount  = 0;

    for (int x = 0; x < BRICKS_WIDTH; x++)
    {
        for (int y = 0; y < BRICKS_HEIGHT; y++)
        {
            Uint8   byType  = GetBrick(x, y);
            if (byType > BOX_NONE) // && (byType < BOX_IM_0 || byType >= BOX_BOMB_0)) {
            {
                nCount++;
            }
        }
    }

    return  nCount;
}

void CGenerateLevel::_RollHorizontal()
{
    if (m_bClonedByVertical == true)
    {
        return;
    }

    Uint8   abyLine[BRICKS_HEIGHT];
    int i, x, y;
    for (i = 0; i < BRICKS_HEIGHT; i++)
    {
        abyLine[i]  = GetBrick(0, i);
    }

    for (x = 1; x < BRICKS_WIDTH; x++)
    {
        for (y = 0; y < BRICKS_HEIGHT; y++)
        {
            SetBrick(x - 1, y, GetBrick(x, y));
        }
    }

    for (i = 0; i < BRICKS_HEIGHT; i++)
    {
        SetBrick(BRICKS_WIDTH - 1, i, abyLine[i]);
    }
}

void CGenerateLevel::_RollVertical()
{
    if (m_bClonedByHorizontal == true)
    {
        return;
    }

    Uint8   abyLine[BRICKS_WIDTH];
    int i, x, y;
    for (i = 0; i < BRICKS_WIDTH; i++)
    {
        abyLine[i]  = GetBrick(i, 0);
    }

    for (y = 1; y < BRICKS_HEIGHT; y++)
    {
        for (x = 0; x < BRICKS_WIDTH; x++)
        {
            SetBrick(x, y - 1, GetBrick(x, y));
        }
    }

    for (i = 0; i < BRICKS_WIDTH; i++)
    {
        SetBrick(i, BRICKS_HEIGHT - 1, abyLine[i]);
    }
}

void CGenerateLevel::LoadPatterns()
{
    unsigned int    nDataLen;
    m_pbyLevelPatterns  = (Uint8*)a::res()->GetDataAllocMem("patterns.bin", nDataLen);
    if (m_pbyLevelPatterns)
    {
        m_nPatterns = nDataLen / (BRICKS_WIDTH * BRICKS_HEIGHT);
    }
    printf("loaded %d patterns\n", m_nPatterns);
}
