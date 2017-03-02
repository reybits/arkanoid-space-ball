/**********************************************\
*
*  Copyright (C) 2006 by Andrey A. Ugolnik
*  http://www.ugolnik.info
*  andrey@ugolnik.info
*
\**********************************************/

#pragma once

#include <vector>

struct sOptions;

class CTutorialDlg final
{
public:
    CTutorialDlg(sOptions& options);
    ~CTutorialDlg();

    void AddDialog(int nX, int nY, int nCategory, int nMsgId);
    bool ShowDialog();
    void Reset();

private:
    sOptions& m_options;
    bool m_abShownDialogs[3][50];
    bool m_tutorialMode;
    struct sTutorial
    {
        int nX, nY;
        int nCategory, nMsgId;
    };
    std::vector<sTutorial>m_vecTutorialDlg;
};
