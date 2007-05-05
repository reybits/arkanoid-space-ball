/***************************************************************************
 *   Copyright (C) 2006 by Andrey A. Ugolnik                               *
 *   andrey@wegroup.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
#ifndef TUTORIALDLG_H
#define TUTORIALDLG_H

/**
	@author Andrey A. Ugolnik <andrey@wegroup.org>
*/
class CTutorialDlg{
public:
    CTutorialDlg();

    ~CTutorialDlg();
	 void AddDialog(int nX, int nY, int nCategory, int nMsgId);
    bool ShowDialog();
    void Reset();

protected:
	bool m_abShownDialogs[3][50];
	bool m_bTutorialMode;
	struct _TUTORIAL {
		int	nX, nY;
		int	nCategory, nMsgId;
	};
	std::vector<_TUTORIAL>m_vecTutorialDlg;
};

#endif
