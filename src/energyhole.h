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

#ifndef ENERGYHOLE_H
#define ENERGYHOLE_H

class CEnergyHole
{
public:
    CEnergyHole();

    ~CEnergyHole();
    void AddEnergyHole();
    void RemoveAll();
    void Draw();
    void Move();

protected:
    struct sEnergyHole
    {
        float x;
        float y;
        Uint32 time;
        int frame;
        Uint32 move_time;
        int angle;
        bool is_over;
    };
    std::vector<sEnergyHole> m_vecEnergyHole;
};

#endif

