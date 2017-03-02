/**********************************************\
*
*  Andrey A. Ugolnik
*  http://www.ugolnik.info
*  andrey@ugolnik.info
*
\**********************************************/

#include "accessor.h"
#include "arkanoidsbgame.h"
#include "ball.h"
#include "bonus.h"
#include "bullet.h"
#include "coolstring.h"
#include "energyhole.h"
#include "exploision.h"
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

        std::unique_ptr<CResource> g_Resource;
        std::unique_ptr<CArkanoidSBGame> g_Arkanoid;
        std::unique_ptr<CMainMenu> g_MainMenu;
        std::unique_ptr<CTutorialDlg> g_TutorialDlg;
        std::unique_ptr<CMyString> g_Font;
        std::unique_ptr<CMyString> g_Font2;
        std::unique_ptr<CMyString> g_Font3;
        std::unique_ptr<CMyString> g_FontTutorial;
        std::unique_ptr<CSinusString> g_SinusString;
        std::unique_ptr<CBall> g_Ball;
        std::unique_ptr<CBullet> g_Bullet;
        std::unique_ptr<CBonus> g_Bonus;
        std::unique_ptr<CMonster> g_Monster;
        std::unique_ptr<CEnergyHole> g_EnergyHole;
        std::unique_ptr<CCoolString> g_CoolString;
        std::unique_ptr<CExploision> g_Exploision;

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

        g_Resource.reset(new CResource());
        g_Arkanoid.reset(new CArkanoidSBGame(Options));
        g_MainMenu.reset(new CMainMenu(Options, Highscores));
        g_TutorialDlg.reset(new CTutorialDlg(Options));
        g_Font.reset(new CMyString());
        g_Font2.reset(new CMyString());
        g_Font3.reset(new CMyString());
        g_FontTutorial.reset(new CMyString());
        g_SinusString.reset(new CSinusString());
        g_Ball.reset(new CBall());
        g_Bullet.reset(new CBullet());
        g_Bonus.reset(new CBonus());
        g_Monster.reset(new CMonster());
        g_EnergyHole.reset(new CEnergyHole());
        g_CoolString.reset(new CCoolString());
        g_Exploision.reset(new CExploision());
    }

    void deinitialize()
    {
        g_Exploision.reset();
        g_CoolString.reset();
        g_EnergyHole.reset();
        g_Monster.reset();
        g_Bonus.reset();
        g_Bullet.reset();
        g_Ball.reset();
        g_SinusString.reset();
        g_FontTutorial.reset();
        g_Font3.reset();
        g_Font2.reset();
        g_Font.reset();
        g_TutorialDlg.reset();
        g_MainMenu.reset();
        g_Arkanoid.reset();
        g_Resource.reset();

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
        return g_Resource.get();
    }

    CArkanoidSBGame* ark()
    {
        return g_Arkanoid.get();
    }

    CMainMenu* menu()
    {
        return g_MainMenu.get();
    }

    CTutorialDlg* tutDlg()
    {
        return g_TutorialDlg.get();
    }

    CMyString* fnt1()
    {
        return g_Font.get();
    }

    CMyString* fnt2()
    {
        return g_Font2.get();
    }

    CMyString* fnt3()
    {
        return g_Font3.get();
    }

    CMyString* fntTut()
    {
        return g_FontTutorial.get();
    }

    CSinusString* fntSin()
    {
        return g_SinusString.get();
    }

    CBall* ball()
    {
        return g_Ball.get();
    }

    CBullet* bullet()
    {
        return g_Bullet.get();
    }

    CBonus* bonus()
    {
        return g_Bonus.get();
    }

    CMonster* monst()
    {
        return g_Monster.get();
    }

    CEnergyHole* hole()
    {
        return g_EnergyHole.get();
    }

    CCoolString* coolstr()
    {
        return g_CoolString.get();
    }

    CExploision* expl()
    {
        return g_Exploision.get();
    }

    sHighscores& high()
    {
        return Highscores;
    }

}
