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
 */

#include "sdk/Editor.h"
#include "sdk/EditorEvent.h"
#include "EditorModMargin.h"

#include <algorithm>

using namespace fb;


// Set bit flags
static inline void SET_FLAG (unsigned & value, unsigned flag)
{ value |= flag; }


// unset bit flags
static inline void UNSET_FLAG (unsigned & value, unsigned flag)
{ value &= (~flag); }


// Number of elements allocaed per block.
enum {
    STATES_IN_CELL  = sizeof(unsigned) * 8
};


// help struct
struct LineStateInfo
{
    unsigned short counter;
    unsigned short savepos;
};



/**
 * Constructor
 */
CEditorModMargin::CEditorModMargin ( CEditor & editor )
    : m_editor ( editor )
{
}


/**
 * Push line state
 */
void CEditorModMargin::Push(unsigned line, unsigned marker)
{
    LOG_MSG_INT("Push ", line);
    LOG_INT(marker);

    // Ensure that vector is big enough
    if (line >= m_lines.size())
    {
        for (size_t s = m_lines.size(); s <= line; s++)
            m_lines.push_back(IntVector());
    }

    // get modification info
    IntVector & mod = m_lines[line];

    // get the cell or add a new entry
    if (mod.size() < (size_t)2)
    {
        mod.push_back(0);
        mod.push_back(0);
        // init save position to the end
        LineStateInfo & info = (LineStateInfo &)mod[0];
        info.savepos = -1;
    }

    // get cell number
    LineStateInfo & info = (LineStateInfo &)mod[0];
    int cell = ++info.counter;

    // set saved marker then set the save position
    // and set marker to EDITED
    if (marker == FLAG_SAVED)
    {
        info.savepos = cell;
        marker = FLAG_EDITED;
    }
    else
    {
        // if state is pushed "before" save point
        if (cell <= info.savepos) info.savepos = -1;
    }

    // get vector index and bit offset
    int offset = (cell % STATES_IN_CELL);
    int index  = (cell / STATES_IN_CELL) + 1;

    // expand array if needed
    if ((int)mod.size() <= index) mod.push_back(0);

    // mark the bit
    if ( marker == FLAG_EDITED )
        SET_FLAG(mod[index], 1 << offset);
    else
        UNSET_FLAG(mod[index], 1 << offset);
}



/**
 * Pop topmost and check next value
 */
unsigned CEditorModMargin::Pop (unsigned line)
{
    LOG_MSG_INT("Pop ", line);

    // no state here
    if ((int)m_lines.size() <= line) return FLAG_NONE;

    // get mod
    IntVector & mod = m_lines[line];

    // if it's too small?
    if (mod.size() < 2) return FLAG_NONE;

    // get line info
    LineStateInfo & info = (LineStateInfo &)mod[0];

    // no states ?
    if (info.counter == 0) return FLAG_NONE;

    // get cell
    int cell = --info.counter;

    // is it a save cell ?
    if ( cell == info.savepos ) return FLAG_SAVED;

    // get vector index and bit offset
    int offset = (cell % STATES_IN_CELL);
    int index  = (cell / STATES_IN_CELL) + 1;

    // return cell info
    return (mod[index] & (1 << offset)) ? FLAG_EDITED : FLAG_NONE;
}



/**
 * Redo the line
 */
unsigned CEditorModMargin::Redo ( unsigned line )
{
    LOG_MSG_INT("Redo ", line);

    // no state here
    if ((int)m_lines.size() <= line) return FLAG_NONE;

    // get mod
    IntVector & mod = m_lines[line];

    // if it's too small?
    if (mod.size() < 2) return FLAG_NONE;

    // get line info
    LineStateInfo & info = (LineStateInfo &)mod[0];

    // get cell
    int cell = ++info.counter;

    // is it a save cell ?
    if ( cell == info.savepos ) return FLAG_SAVED;

    // get vector index and bit offset
    int offset = (cell % STATES_IN_CELL);
    int index  = (cell / STATES_IN_CELL) + 1;

    // return cell info
    return (mod[index] & (1 << offset)) ? FLAG_EDITED : FLAG_NONE;

}



/**
 * Submit changes
 */
void CEditorModMargin::SubmitChanges ()
{
    // get the lines to update. exclude the
    // the ignored lines.
    IntSet lines;
    std::set_difference (
        m_pending.begin(), m_pending.end(),
        m_ignore.begin(), m_ignore.end(),
        inserter(lines, lines.begin())
    );

    // set state
    IntSet::iterator iter = lines.begin();
    for ( ; iter != lines.end(); iter++ )
    {
        int line = *iter;
        int flags = m_editor.MarkerGet(line);
        Push(line, flags & MARKER_FLAGS);
    }
}



/**
 * Modify line
 */
void CEditorModMargin::Modify ( unsigned startLine, int modified )
{
    // mark first line as modified
    SetMarker ( startLine, FLAG_EDITED );
    m_pending.insert ( startLine );

    // mark more lines ?
    if ( modified > 0 )
    {
        for ( ; modified; modified-- )
        {
            SetMarker ( startLine + modified, FLAG_EDITED );
            m_pending.insert ( startLine + modified );
        }
    }
    else if ( modified < 0 )
    {
        // lines removed ...
    }
}



/**
 * Add undo / redo line
 */
void CEditorModMargin::UndoRedo ( unsigned int startLine, int modified )
{
    m_pending.insert ( startLine );
    if ( modified > 0 )
        for ( ; modified; modified-- )
            m_pending.insert ( startLine + modified );
}



/**
 * Submit Undo action
 */
void CEditorModMargin::SubmitUndo ()
{
    IntSet::iterator iter = m_pending.begin();
    for ( ; iter != m_pending.end(); iter++ )
    {
        int line = *iter;
        int flags = Pop(line);
        SetMarker(line,flags);
    }
}



/**
 * Submit REDO action
 */
void CEditorModMargin::SubmitRedo ()
{
    IntSet::iterator iter = m_pending.begin();
    for ( ; iter != m_pending.end(); iter++ )
    {
        int line = *iter;
        int flags = Redo(line);
        SetMarker(line,flags);
    }
}



/**
 * Flush the line cache
 */
void CEditorModMargin::Flush ()
{
    m_pending.clear();
}



/**
 * Flush the ignore list
 */
void CEditorModMargin::FlushChangeIgnore ()
{
    m_ignore.clear();
}



/**
 * Copy pending lines to ignore list
 */
void CEditorModMargin::SetChangeIgnore ()
{
    std::copy (
        m_pending.begin(),
        m_pending.end(),
        inserter(m_ignore, m_ignore.begin())
    );
}



/**
 * Set line markers
 */
void CEditorModMargin::SetMarker ( unsigned line, unsigned marker )
{
    // update margin
    if (marker & FLAG_EDITED)
        m_editor.MarkerAdd(line, MARKER_EDITED);
    else
        m_editor.MarkerDelete(line, MARKER_EDITED);

    // set saved
    if (marker & FLAG_SAVED)
        m_editor.MarkerAdd(line, MARKER_SAVED);
    else
        m_editor.MarkerDelete(line, MARKER_SAVED);
}
