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
#ifndef COOLSTRING_H
#define COOLSTRING_H
/**
	@author Andrey A. Ugolnik <andrey@wegroup.org>
*/
class CCoolString{
public:
    CCoolString();

    ~CCoolString();
    void Add(const char *pchFormat, ...);
    void Draw();
    void Clear();
    int GetCount();

protected:
#define COOLSTRING_MAX_LEN	20
	struct _COOL_STRING {
/*		float	fX, fY;
		float	fDelta;
		int	nPos;
		int	nLen;
		Uint8	byPoint[8][COOLSTRING_MAX_LEN * 8];*/
		char	*pchString;
		int	nSeconds;
	};
	std::vector<_COOL_STRING>m_vecCoolString;
};

#endif
