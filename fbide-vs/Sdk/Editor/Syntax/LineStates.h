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

#ifndef LINESTATE_H_INCLUDED
#define LINESTATE_H_INCLUDED

#include <list>
#include <vector>
#include <utility>

/**
 * this holds state info. Indent amount
 *
 * holds the amount to indent the code
 * and the state of the block. Also opening
 * and closing lines
 */
struct CStateInfo
{
    int lineOpen, lineClose;
    unsigned char indent, state;

    // simple constructor.
    CStateInfo (int open = 0, int close = 0, unsigned char _indent = 0, unsigned char _state = 0)
        : lineOpen(open), lineClose(close), indent(_indent), state(_state)
    {}
};

// list to hold the states
typedef std::list<CStateInfo> CStateInfoList;

// the state info range
typedef std::pair<CStateInfoList::iterator, CStateInfoList::iterator> CStateRange;

// the line - range pairs.
typedef std::pair<int, CStateRange> CLineStateInfo;
typedef std::vector<CLineStateInfo> CLineStateVector;



/**
 * hold all line states.
 * - code folding (can be used I'm SURE)
 * - coloring the blocks
 * - determining the previous state when closing the block
 */
class CLineStates
{
    public :

        // create new states
        CLineStates ();

        // Clear line state from this line
        void ClearFrom (int line);


        // adds the line if it doesn't yet exist
        void SetLine (int line, CStateRange ** range = NULL)
        {
            CLineStateVector::reverse_iterator riter = m_lineStates.rbegin();
            CLineStateVector::reverse_iterator rend = m_lineStates.rend();
            for ( ; riter != rend ; riter-- )
            {
                if (riter->first < line)
                {
                    m_lineStates.push_back(CLineStateInfo(line, riter->second));
                    m_lineMax = line;
                    if (range != NULL) *range = &m_lineStates.rbegin()->second;
                    return;
                }
                else if (riter->first == line)
                {
                    if (range != NULL) *range = &riter->second;
                    return;
                }
            }

            // m_lineStates[line] = CStateRange (m_allStates.begin(), m_allStates.begin());
            m_lineStates.push_back(CLineStateInfo(line, CStateRange (m_allStates.begin(), m_allStates.begin())));
            m_lineMax = line;
            if (range != NULL) *range = &m_lineStates.rbegin()->second;
        }


        // add new state to the line
        void PushState (int line, unsigned short indent, unsigned short state)
        {
            CStateRange & range = InitLineRange(line);
            range.second = m_allStates.insert(++range.second, CStateInfo(line, -1, indent, state));
        }


        /**
         * Set line state and return the range
         */
        CStateRange & InitLineRange (int line)
        {
            CStateRange * p = NULL;
            SetLine(line, &p);
            return *p;
        }


        // Retreave last state in the stack (curently active)
        CStateInfo GetActiveState (int line)
        {
            CLineStateVector::reverse_iterator riter = m_lineStates.rbegin();
            CLineStateVector::reverse_iterator rend = m_lineStates.rend();
            for ( ; riter != rend ; riter-- )
            {
                if (riter->first <= line) return *((riter->second).second);
            }
            return m_nullState;
        }


        // remove state from the line and return it
        CStateInfo PopState (int line)
        {
            CStateRange & range = InitLineRange(line);

            // no states to return...
            if (range.first == range.second) return m_nullState;

            // get result
            CStateInfo & result = *(range.second--);

            // mark the closing line
            result.lineClose = line;

            // ran out of states.
            if (range.first == range.second)
                range.first = range.second = m_allStates.insert(m_allStates.end(), m_nullState);

            return result;
        }


        /**
         * Get state range iterator
         */
        CStateRange GetStateRange (int line)
        {
            CLineStateVector::reverse_iterator riter = m_lineStates.rbegin();
            CLineStateVector::reverse_iterator rend = m_lineStates.rend();
            for ( ; riter != rend ; riter-- )
            {
                if (riter->first <= line) return riter->second;
            }
            return CStateRange(m_allStates.end(), m_allStates.end());
        }


    public :
        int             m_dialect;
        bool            m_drawIndentedBoxes;
        int             m_tabWidth;
        bool            m_optionEscape;

    private :
        CStateInfoList  m_allStates;
        CLineStateVector  m_lineStates;
        int             m_lineMax;
        CStateInfo      m_nullState;
};

#endif // LINESTATE_H_INCLUDED
