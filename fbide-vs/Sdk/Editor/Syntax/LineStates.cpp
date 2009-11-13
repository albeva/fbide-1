/*
 * This file is part of FBIde project
 *
 * FBIde is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * FBIde is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with FBIde.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Author: Albert Varaksin <albeva@me.com>
 * Copyright (C) The FBIde development team
 *
 * This file is based on VB lexer for Scintilla by
 * Neil Hodgson <neilh@scintilla.org>
 */
#include "sdk_pch.h"
#include "LineStates.h"


// construct new object
CLineStates::CLineStates () :
    m_nullState(-1, -1, 0, 0),
    m_lineMax(0)
{
    // push back so maps could have a valid iterators
    m_allStates.push_back(m_nullState);
}


// clear states from the given line
void CLineStates::ClearFrom (int line)
{
    // Remove block states
    if (m_allStates.size() > 1) // skip first one
    {
        CStateInfoList::iterator iter = ++m_allStates.begin();
        for ( ; iter != m_allStates.end(); )
        {
            CStateInfoList::iterator tmp = iter++;
            if (tmp->lineOpen >= line)
            {
                m_allStates.erase (tmp);
                break;
            }
            else if (iter->lineClose < line)
            {
                tmp->lineClose = -1;
            }
        }
    }

    // remove lines.
    if (line <= m_lineMax)
    {
        // remove lines
        CLineStateVector::iterator iter = m_lineStates.begin();
        m_lineMax = 0;
        for ( ; iter != m_lineStates.end(); iter++)
        {
            if (iter->first >= line)
            {
                m_lineStates.erase(iter, m_lineStates.end());
                return;
            }
            m_lineMax = iter->first;
        }
    }
}
