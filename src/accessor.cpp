/**********************************************\
*
*  Andrey A. Ugolnik
*  http://www.ugolnik.info
*  andrey@ugolnik.info
*
\**********************************************/

#include "accessor.h"
#include "ball.h"
#include "bonus.h"
#include "bullet.h"
#include "coolstring.h"
#include "energyhole.h"
#include "exploision.h"
#include "game.h"
#include "mainmenu.h"
#include "monster.h"
#include "mystring.h"
#include "random.h"
#include "resource.h"
#include "sinusstring.h"
#include "tutorialdlg.h"
#include "utils.h"

#include <memory>
#include <sys/stat.h>

namespace a
{

    namespace
    {
        char UserProfile[MAX_PATH] = { 0 };

        CRandom Rnd;
        sOptions Options;
        sHighscores Highscores;

        std::unique_ptr<CResource> Resource;
        std::unique_ptr<CGame> Game;
        std::unique_ptr<CMainMenu> MainMenu;
        std::unique_ptr<CTutorialDlg> TutorialDlg;
        std::unique_ptr<CMyString> Font;
        std::unique_ptr<CMyString> Font2;
        std::unique_ptr<CMyString> Font3;
        std::unique_ptr<CMyString> FontTutorial;
        std::unique_ptr<CSinusString> SinusString;
        std::unique_ptr<CBall> Ball;
        std::unique_ptr<CBullet> Bullet;
        std::unique_ptr<CBonus> Bonus;
        std::unique_ptr<CMonster> Monster;
        std::unique_ptr<CEnergyHole> EnergyHole;
        std::unique_ptr<CCoolString> CoolString;
        std::unique_ptr<CExploision> Exploision;

        void readConfig()
        {
            char buffer[PATH_MAX];

            snprintf(buffer, sizeof(UserProfile), "%sconfig", UserProfile);
            FILE* file = fopen(buffer, "rb");
            if (file)
            {
                fread(&Options, sizeof(Options), 1, file);
                Options.bppIdx %= sizeof(g_bppList) / sizeof(g_bppList[0]);
                fclose(file);
            }

            snprintf(buffer, sizeof(UserProfile), "%sscores", UserProfile);
            file = fopen(buffer, "rb");
            if (file)
            {
                fread(&Highscores, sizeof(Highscores), 1, file);
                fclose(file);
                EncodeDecode(&Highscores, sizeof(Highscores));
            }
        }

        void writeConfig()
        {
            // try to create directory if not exist
#ifdef _WIN32
            CreateDirectory(UserProfile, 0);
#else   //#elseif __linux__ // linux and mac os x
            mkdir(UserProfile, 0700);
#endif

            char buffer[PATH_MAX];

            snprintf(buffer, sizeof(UserProfile), "%sconfig", UserProfile);
            FILE* file = fopen(buffer, "wb");
            if (file)
            {
                fwrite(&Options, sizeof(Options), 1, file);
                fclose(file);
            }

            snprintf(buffer, sizeof(UserProfile), "%sscores", UserProfile);
            file = fopen(buffer, "wb");
            if (file)
            {
                EncodeDecode(&Highscores, sizeof(Highscores));
                fwrite(&Highscores, sizeof(Highscores), 1, file);
                fclose(file);
            }
        }

        void FillUserProfilePath()
        {
#if defined(__linux__)

            const auto xdgPath = getenv("XDG_CONFIG_HOME");
            if (xdgPath != nullptr)
            {
                snprintf(UserProfile, sizeof(UserProfile), "%s/arkanoidsb/", xdgPath);
            }
            else
            {
                snprintf(UserProfile, sizeof(UserProfile), "%s/.config/arkanoidsb/", getenv("HOME"));
            }

#elif defined(_WIN32)

            char buffer[PATH_MAX];
            //GetUserProfileDirectory(0, achTemp, sizeof(buffer));
            ExpandEnvironmentStrings("%USERPROFILE%", buffer, sizeof(buffer));
            snprintf(UserProfile, sizeof(UserProfile), "%s\\.arkanoidsb\\", buffer);

#else

            snprintf(UserProfile, sizeof(UserProfile), "%s/Library/Application Support/arkanoidsb/", getenv("HOME"));

#endif
        }

    }

    void initialize()
    {
        FillUserProfilePath();
        printf("Users config dir: %s\n", UserProfile);
        readConfig();

        Resource.reset(new CResource());
        Game.reset(new CGame(Options));
        MainMenu.reset(new CMainMenu(Options, Highscores));
        TutorialDlg.reset(new CTutorialDlg(Options));
        Font.reset(new CMyString());
        Font2.reset(new CMyString());
        Font3.reset(new CMyString());
        FontTutorial.reset(new CMyString());
        SinusString.reset(new CSinusString());
        Ball.reset(new CBall());
        Bullet.reset(new CBullet());
        Bonus.reset(new CBonus());
        Monster.reset(new CMonster());
        EnergyHole.reset(new CEnergyHole());
        CoolString.reset(new CCoolString());
        Exploision.reset(new CExploision());
    }

    void deinitialize()
    {
        Exploision.reset();
        CoolString.reset();
        EnergyHole.reset();
        Monster.reset();
        Bonus.reset();
        Bullet.reset();
        Ball.reset();
        SinusString.reset();
        FontTutorial.reset();
        Font3.reset();
        Font2.reset();
        Font.reset();
        TutorialDlg.reset();
        MainMenu.reset();
        Game.reset();
        Resource.reset();

        writeConfig();
    }

    const char* userProfile()
    {
        return UserProfile;
    }

    CRandom& rnd()
    {
        return Rnd;
    }

    CResource* res()
    {
        return Resource.get();
    }

    CGame* ark()
    {
        return Game.get();
    }

    CMainMenu* menu()
    {
        return MainMenu.get();
    }

    CTutorialDlg* tutDlg()
    {
        return TutorialDlg.get();
    }

    CMyString* fnt1()
    {
        return Font.get();
    }

    CMyString* fnt2()
    {
        return Font2.get();
    }

    CMyString* fnt3()
    {
        return Font3.get();
    }

    CMyString* fntTut()
    {
        return FontTutorial.get();
    }

    CSinusString* fntSin()
    {
        return SinusString.get();
    }

    CBall* ball()
    {
        return Ball.get();
    }

    CBullet* bullet()
    {
        return Bullet.get();
    }

    CBonus* bonus()
    {
        return Bonus.get();
    }

    CMonster* monst()
    {
        return Monster.get();
    }

    CEnergyHole* hole()
    {
        return EnergyHole.get();
    }

    CCoolString* coolstr()
    {
        return CoolString.get();
    }

    CExploision* expl()
    {
        return Exploision.get();
    }

    sHighscores& high()
    {
        return Highscores;
    }

}
