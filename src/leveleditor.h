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
#ifndef LEVELEDITOR_H
#define LEVELEDITOR_H

/**
	@author Andrey A. Ugolnik <andrey@wegroup.org>
*/
class CLevelEditor{
public:
    CLevelEditor();

    ~CLevelEditor();
    void Draw();
    void Load();
    void Save();

protected:
	Uint8	*m_pbyLevels;
	int m_nCurrentLevel;
	int m_nLevelsCount;
	bool m_bIsSelectBrickMode;
    int m_nBrickType;
	Uint8 m_abyLevel[BRICKS_WIDTH][BRICKS_HEIGHT];
};

#endif
