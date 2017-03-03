/**********************************************\
*
*  Copyright (C) 2006 by Andrey A. Ugolnik
*  http://www.ugolnik.info
*  andrey@ugolnik.info
*
\**********************************************/

#pragma once

class CGame;
class CBall;
class CBonus;
class CBullet;
class CCoolString;
class CEnergyHole;
class CExploision;
class CMainMenu;
class CMonster;
class CMyString;
class CRandom;
class CResource;
class CSinusString;
class CTutorialDlg;

struct sHighscores;
struct sOptions;

namespace a
{

    void initialize();
    void deinitialize();

    const char* userProfile();

    sOptions& opt();

    CRandom& rnd();
    CResource* res();
    CGame* ark();
    CMainMenu* menu();
    CTutorialDlg* tutDlg();
    CMyString* fnt1();
    CMyString* fnt2();
    CMyString* fnt3();
    CMyString* fntTut();
    CSinusString* fntSin();
    CBonus* bonus();
    CBall* ball();
    CBullet* bullet();
    CMonster* monst();
    CEnergyHole* hole();
    CCoolString* coolstr();
    CExploision* expl();

    sHighscores& high();

}
