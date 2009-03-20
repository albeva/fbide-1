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

// show logs
#define SHOW_LOG

// wrapper macro
#ifdef SHOW_LOG
#   define LOG(_m) _m
#else
#   define LOG(_m)
#endif


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
    #ifdef SHOW_LOG
        if ( marker == FLAG_SAVED ) { LOG_MSG("Push ( FLAG_SAVED ) {"); }
        else if ( marker == FLAG_EDITED ) { LOG_MSG("Push ( FLAG_EDITED ) {"); }
        else { LOG_MSG("Push ( FLAG_NONE ) {"); }
        LOG_MSG_INT("    ", line);
        LOG_MSG_INT("    ", marker);
    #endif

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
    int cell = info.counter++;
    LOG(LOG_MSG_INT("    ", cell));

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

    LOG(LOG_MSG("}"));
}



/**
 * Set value of the current position
 */
void CEditorModMargin::Set(unsigned line, unsigned marker)
{
    #ifdef SHOW_LOG
        if ( marker == FLAG_SAVED ) { LOG_MSG("Set ( FLAG_SAVED ) {"); }
        else if ( marker == FLAG_EDITED ) { LOG_MSG("Set ( FLAG_EDITED ) {"); }
        else { LOG_MSG("Set ( FLAG_NONE ) {"); }
        LOG_MSG_INT("    ", line);
        LOG_MSG_INT("    ", marker);
    #endif


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
    int cell = info.counter;
    LOG(LOG_MSG_INT("    ", cell));

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

    LOG(LOG_MSG("}"));
}



/**
 * Pop topmost and check next value
 */
unsigned CEditorModMargin::Undo (unsigned line)
{
    #ifdef SHOW_LOG
        LOG_MSG("Undo {");
        LOG_MSG_INT("    ", line);
    #endif

    // no state here
    if ((int)m_lines.size() <= line)
    {
        LOG(LOG_MSG("} = FLAG_NONE (m_lines.size() <= line)"));
        return FLAG_NONE;
    }

    // get mod
    IntVector & mod = m_lines[line];

    // if it's too small?
    if (mod.size() < 2)
    {
        LOG(LOG_MSG("} = FLAG_NONE (mod.size() < 2)"));
        return FLAG_NONE;
    }

    // get line info
    LineStateInfo & info = (LineStateInfo &)mod[0];

    // no states ?
    if (info.counter == 0)
    {
        LOG(LOG_MSG("} = FLAG_NONE (info.counter)"));
        return FLAG_NONE;
    }

    // get cell
    int cell = --info.counter;
    LOG(LOG_MSG_INT("    ", cell));

    // is it a save cell ?
    if ( cell == info.savepos )
    {
        LOG(LOG_MSG("} FLAG_SAVED = (cell == info.savepos)"));
        return FLAG_SAVED;
    }

    // get vector index and bit offset
    int offset = (cell % STATES_IN_CELL);
    int index  = (cell / STATES_IN_CELL) + 1;

    // edited ?
    #ifdef SHOW_LOG
        if (mod[index] & (1 << offset))
        {
            LOG_MSG("} = FLAG_EDITED");
            return FLAG_EDITED;
        }
        LOG_MSG("} = FLAG_NONE");
        return FLAG_NONE;
    #else
        return mod[index] & (1 << offset) ? FLAG_EDITED : FLAG_NONE;
    #endif
}



/**
 * Redo the line
 */
unsigned CEditorModMargin::Redo ( unsigned line )
{
    #ifdef SHOW_LOG
        LOG_MSG("Redo {");
        LOG_MSG_INT("    ", line);
    #endif

    // no state here
    if ((int)m_lines.size() <= line)
    {
        LOG(LOG_MSG("} = FLAG_NONE (m_lines.size() <= line)"));
        return FLAG_NONE;
    }

    // get mod
    IntVector & mod = m_lines[line];

    // if it's too small?
    if (mod.size() < 2)
    {
        LOG(LOG_MSG("} = FLAG_NONE (mod.size() < 2)"));
        return FLAG_NONE;
    }

    // get line info
    LineStateInfo & info = (LineStateInfo &)mod[0];

    // get cell
    int cell = ++info.counter;
    LOG(LOG_MSG_INT("    ", cell));

    // is it a save cell ?
    if ( cell == info.savepos )
    {
        LOG(LOG_MSG("} = FLAG_SAVED (info.savepos)"));
        return FLAG_SAVED;
    }

    // get vector index and bit offset
    int offset = (cell % STATES_IN_CELL);
    int index  = (cell / STATES_IN_CELL) + 1;

    // edited ?
    #ifdef SHOW_LOG
        if (mod[index] & (1 << offset))
        {
            LOG_MSG("} = FLAG_EDITED");
            return FLAG_EDITED;
        }
        LOG_MSG("} = FLAG_NONE");
        return FLAG_NONE;
    #else
        return mod[index] & (1 << offset) ? FLAG_EDITED : FLAG_NONE;
    #endif
}



/**
 * Modify line
 */
void CEditorModMargin::Modify ( unsigned int startLine, int modified )
{
    int endLine = startLine + ( modified > 0 ? modified : 0 );
    for (int line = startLine; line <= endLine; line++ )
    {
        if ( m_pending.insert(line).second )
        {
            LOG(LOG_MSG_INT("-- Modify:", line));
            Push( line, m_editor.MarkerGet( line ) & MARKER_FLAGS );
            SetMarker( line, FLAG_EDITED );
            Set ( line, FLAG_EDITED );
        }
    }

    if ( modified < 0 )
    {
        modified = -modified;
        for ( int line = startLine + 1; line <= startLine + modified; line++ )
        {
            if ( m_undo.insert(line).second )
            {
                LOG(LOG_MSG_INT("-- Modify ( delete ):", line));
                // Push( line, m_editor.MarkerGet( line ) & MARKER_FLAGS );
                int nextMarker = m_editor.MarkerGet( line + modified );
                Push( line, nextMarker & MARKER_FLAGS );
                Set ( line, m_editor.MarkerGet( line ) & MARKER_FLAGS );

                /*
                Push( line, m_editor.MarkerGet( line ) & MARKER_FLAGS );

                int m = m_editor.MarkerGet( line + modified );
                // SetMarker( line + modified, m );
                Set ( line, m );
                */
            }
        }
    }
}



/**
 * Undo the lines
 */
void CEditorModMargin::Undo ( unsigned int startLine, int modified )
{
    int endLine = startLine + ( modified > 0 ? modified : 0 );
    for (int line = startLine; line <= endLine; line++ )
    {
        if ( m_pending.insert(line).second )
        {
            LOG(LOG_MSG_INT("-- Undo:", line));
            Set ( line, m_editor.MarkerGet( line ) & MARKER_FLAGS );
            SetMarker( line, Undo( line ) );
        }
    }

    if ( modified < 0 )
    {
        modified = -modified;
        for ( int line = startLine + 1; line <= startLine + modified; line++ )
        {
            if ( m_undo.insert(line).second )
            {
                LOG(LOG_MSG_INT("-- Undo ( delete ):", line));
                //Set ( line, m_editor.MarkerGet( line ) & MARKER_FLAGS );
                Undo( line );
            }
        }
    }
}



/**
 * Redo the lines
 */
void CEditorModMargin::Redo ( unsigned int startLine, int modified )
{
    int endLine = startLine + ( modified > 0 ? modified : 0 );
    for (int line = startLine; line <= endLine; line++ )
    {
        if ( m_pending.insert(line).second )
        {
            LOG(LOG_MSG_INT("-- Redo:", line));
            //Set ( line, m_editor.MarkerGet( line ) & MARKER_FLAGS );
            SetMarker( line, Redo( line ) );
        }
    }

    if ( modified < 0 )
    {
        modified = -modified;
        for ( int line = startLine + 1; line <= startLine + modified; line++ )
        {
            if ( m_undo.insert(line).second )
            {
                LOG(LOG_MSG_INT("-- Redo ( delete ):", line));
                //Set ( line, m_editor.MarkerGet( line ) & MARKER_FLAGS );
                Redo( line );
            }
        }
    }
}



/**
 * Add undo / redo line
 */
/*
void CEditorModMargin::UndoRedo ( unsigned int startLine, int modified )
{
    // add unique lines
    m_pending.insert ( startLine );
    if ( modified > 0 )
    {
        for ( ; modified; modified-- )
            m_pending.insert( startLine + modified );
    }
    else if ( modified < 0 )
    {
        modified = -modified;
        for ( ; modified; modified-- )
            m_undo.insert( startLine + modified );
    }
}
*/



/**
 * Submit Undo action
 */
/*
void CEditorModMargin::SubmitUndo ()
{
    IntSet::iterator iter = m_pending.begin();
    for ( ; iter != m_pending.end(); iter++ )
    {
        int line = *iter;
        LOG(LOG_MSG_INT("-- SubmitUndo:", line));
        Set ( line, m_editor.MarkerGet( line ) & MARKER_FLAGS );
        SetMarker( line, Undo( line ) );
    }

    iter = m_undo.begin();
    for ( ; iter != m_undo.end(); iter++ )
    {
        int line = *iter;
        LOG(LOG_MSG_INT("-- SubmitUndo:", line));
        Undo( line );
    }
}
*/



/**
 * Submit REDO action
 */
/*
void CEditorModMargin::SubmitRedo ()
{
    IntSet::iterator iter = m_pending.begin();
    for ( ; iter != m_pending.end(); iter++ )
    {
        int line = *iter;
        LOG(LOG_MSG_INT("-- SubmitRedo:", line));
        SetMarker( line, Redo( line ) );
    }
}
*/


/**
 * Flush the line cache
 */
void CEditorModMargin::Flush ()
{
    LOG(LOG_MSG("-- Flush"));
    m_pending.clear();
    m_undo.clear();
}



/**
 * Get count of pending lines
 */
unsigned CEditorModMargin::GetPendingCount ()
{
    return m_pending.size();
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
