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


// get cell offset
#define GET_OFFSET(_cell)   (((_cell) % STATES_IN_CELL) * BITS_PER_STATE)
// get cell index
#define GET_INDEX(_cell)    (((_cell) / STATES_IN_CELL) + 1)
// get the state
#define GET_STATE(_val, _offset) (((_val) >> (_offset)) & MARKER_FLAGS)
// Calculate the new state
#define MAKE_STATE(_val, _offset, _marker) \
    (((_val) & (~(MARKER_FLAGS << (_offset)))) | ((_marker) & MARKER_FLAGS) << (_offset))

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
    {/*
    Push(1, FLAG_NONE);
    Push(1, FLAG_EDITED);
    Push(1, FLAG_EDITED);
    Push(1, FLAG_EDITED);
    Push(1, FLAG_SAVED);
    Push(1, FLAG_EDITED);
    Push(1, FLAG_EDITED);
    Push(1, FLAG_SAVED);
    Push(1, FLAG_EDITED);

    while ( m_lines[1][0] != -1 )
    {
        int p = Undo( 1 );
        if ( p == FLAG_NONE )
        {
            LOG_MSG("FLAG_NONE");
        }
        else if ( p == FLAG_SAVED )
        {
            LOG_MSG("FLAG_SAVED");
        }
        else if ( p == FLAG_EDITED )
        {
            LOG_MSG("FLAG_EDITED");
        }
        else
        {
            LOG_MSG_INT("Wrong!!!", p);
        }
    }
    */}
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
        mod.push_back(-1);
        mod.push_back(0);
    }

    // get cell number
    int cell = ++mod[0];
    LOG(LOG_MSG_INT("    ", cell));
    LOG(LOG_MSG_INT("    ", mod[0]));

    // get vector index and bit offset
    int offset = GET_OFFSET(cell);
    int index  = GET_INDEX(cell);
    LOG(LOG_MSG_INT("    ", offset));
    LOG(LOG_MSG_INT("    ", index));

    // expand array if needed
    if ((int)mod.size() <= index) mod.push_back(0);

    // mark the cell
    mod[index] = MAKE_STATE(mod[index], offset, marker);

    LOG(LOG_MSG("}"));
}



/**
 * Get top value ( current )
 */
unsigned CEditorModMargin::Top ( unsigned line )
{
    #ifdef SHOW_LOG
        LOG_MSG("Top {");
        LOG_MSG_INT("    ", line);
    #endif

    // no state here
    if (m_lines.size() <= line)
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

    // no states ?
    if (mod[0] == (unsigned)-1)
    {
        LOG(LOG_MSG("} = FLAG_NONE (mod[0] == -1)"));
        return FLAG_NONE;
    }

    // get cell
    int cell = mod[0];
    LOG(LOG_MSG_INT("    ", cell));
    LOG(LOG_MSG_INT("    ", mod[0]));

    // get vector index and bit offset
    int offset = GET_OFFSET(cell);
    int index  = GET_INDEX(cell);
    LOG(LOG_MSG_INT("    ", offset));
    LOG(LOG_MSG_INT("    ", index));

    // Get cell state
    #ifdef SHOW_LOG
        unsigned f = GET_STATE(mod[index], offset);
        if (f == FLAG_NONE) { LOG_MSG("} = FLAG_NONE"); }
        else if ( f == FLAG_EDITED) { LOG_MSG("} = FLAG_EDITED"); }
        else if ( f == FLAG_SAVED) { LOG_MSG("} = FLAG_SAVED"); }
        return f;
    #else
        return GET_STATE(mod[index], offset);
    #endif
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
    if (m_lines.size() <= line)
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

    // no states ?
    if (mod[0] == (unsigned)-1)
    {
        LOG(LOG_MSG("} = FLAG_NONE ( (int)mod[0] == -1 )"));
        return FLAG_NONE;
    }
    else if ( mod[0] == 0 )
    {
        mod[0]--;
        LOG(LOG_MSG("} = FLAG_NONE ( (int)mod[0] == 0 )"));
        return FLAG_NONE;
    }

    // get cell
    int cell = --mod[0];
    LOG(LOG_MSG_INT("    ", cell));
    LOG(LOG_MSG_INT("    ", mod[0]));

    // get vector index and bit offset
    int offset = GET_OFFSET(cell);
    int index  = GET_INDEX(cell);
    LOG(LOG_MSG_INT("    ", offset));
    LOG(LOG_MSG_INT("    ", index));

    // Get cell state
    #ifdef SHOW_LOG
        unsigned f = GET_STATE(mod[index], offset);
        if (f == FLAG_NONE) { LOG_MSG("} = FLAG_NONE"); }
        else if ( f == FLAG_EDITED) { LOG_MSG("} = FLAG_EDITED"); }
        else if ( f == FLAG_SAVED) { LOG_MSG("} = FLAG_SAVED"); }
        return f;
    #else
        return GET_STATE(mod[index], offset);
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
    if (m_lines.size() <= line)
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

    // get cell
    int cell = ++mod[0];
    LOG(LOG_MSG_INT("    ", cell));

    // get vector index and bit offset
    int offset = GET_OFFSET(cell);
    int index  = GET_INDEX(cell);
    LOG(LOG_MSG_INT("    ", offset));
    LOG(LOG_MSG_INT("    ", index));

    // Get cell state
    #ifdef SHOW_LOG
        unsigned f = GET_STATE(mod[index], offset);
        if (f == FLAG_NONE) { LOG_MSG("} = FLAG_NONE"); }
        else if ( f == FLAG_EDITED) { LOG_MSG("} = FLAG_EDITED"); }
        else if ( f == FLAG_SAVED) { LOG_MSG("} = FLAG_SAVED"); }
        return f;
    #else
        return GET_STATE(mod[index], offset);
    #endif
}



/**
 * Modify line
 */
void CEditorModMargin::Modify ( unsigned int startLine, int modified, int markers )
{
    // lines added
    if ( modified > 0 )
    {
        int lastLine = m_lines.size();
        if ( lastLine > 1 )
        {
            lastLine--;
            for (unsigned line = lastLine; line > startLine; line-- )
            {
                Push( line + modified, Top( line ) );
            }
        }
    }
    // lines removed
    else if ( modified < 0 )
    {
        unsigned deleted = -modified;
        int lastLine = m_lines.size();
        if ( lastLine >  deleted )
        {
            lastLine -= deleted;
            for (unsigned line = startLine + 1; line < lastLine; line ++ )
            {
                Push( line, Top( line + deleted ) );
            }
        }
    }
    // current lines
    int endLine = startLine + ( modified > 0 ? modified : 0 );
    for (int line = startLine; line <= endLine; line++ )
    {
        if ( m_pending.insert(line).second )
        {
            // LOG(LOG_MSG_INT("-- Modify:", line));
            Push( line, markers );
            SetMarker( line, markers );
        }
    }
}



/**
 * Set all modified lines
 */
void CEditorModMargin::SetSavePos (  )
{
    for ( unsigned line = 0; line < m_lines.size(); line++ )
    {
        IntVector & mod = m_lines[line];
        if ( mod.size() < 2 ) continue;

        // only redo
        if ( mod[0] == -1 )
        {
            for ( unsigned i = 1; i < mod.size(); i++ )
                mod[i] = FULL_EDITED_FLAG;
            continue;
        }

        // get cell nr
        int cell = mod[0];

        // index and offset of the cell
        int offset = GET_OFFSET(cell);
        int index  = GET_INDEX(cell);
        LOG_INT(index);
        LOG_INT(offset);
        LOG_BIN(mod[index]);

        // mark all REDO as edited
        for ( unsigned i = index + 1; i < mod.size(); i++  )
            mod[i] = FULL_EDITED_FLAG;
        if ( offset < ( CELL_SIZE - BITS_PER_STATE ) )
        {
            // OK
            mod[index] &= ((unsigned)-1) >> ((CELL_SIZE - offset)-2);
            mod[index] |= ( FULL_EDITED_FLAG << (offset+2) );
        }

        // find any previous SAVE position
        for ( unsigned i = 0; i < index; i++ )
        {
            for ( unsigned x = 0; x < CELL_SIZE; x += BITS_PER_STATE )
            {
                if ( GET_STATE(mod[i], x) == FLAG_SAVED )
                {
                    mod[i] = MAKE_STATE(mod[i], x, FLAG_EDITED);
                }
            }
        }
        for ( unsigned x = 0; x < (offset); x += BITS_PER_STATE )
        {
            if ( GET_STATE(mod[index], x) == FLAG_SAVED )
            {
                mod[index] = MAKE_STATE(mod[index], x, FLAG_EDITED);
            }
        }



        // mark current state as edited
        if ( GET_STATE(mod[index], offset) != FLAG_NONE )
        {
            mod[index] = MAKE_STATE(mod[index], offset, FLAG_EDITED);
            Push ( line, FLAG_SAVED );
            SetMarker ( line, FLAG_SAVED );
        }
        else
        {
            // Push ( line, FLAG_NONE );
        }
        LOG_BIN(mod[index]);
    }
}



/**
 * Undo the lines
 */
void CEditorModMargin::Undo ( unsigned startLine, int modified )
{
    // lines added or removed ?
    if ( modified != 0 )
    {
        int deleted = modified > 0 ? modified : -modified;
        int lastLine = m_lines.size();
        if ( lastLine > deleted )
        {
            lastLine -= deleted;
            for (unsigned line = startLine + 1; line < lastLine; line++ )
            {
                Undo( line + deleted );
            }
        }

        if ( modified < 0 )
        {
            for ( int line = 1; line <= deleted; line++ )
            {
                Undo( line + startLine );
            }
        }
    }
    // current lines
    int endLine = startLine + ( modified > 0 ? modified : 0 );
    for (int line = startLine; line <= endLine; line++ )
    {
        if ( m_pending.insert(line).second )
        {
            m_lineMarkers.push_back( LineMarker( line, Undo( line ) ) );
        }
    }
}



/**
 * Redo the lines
 */
void CEditorModMargin::Redo ( unsigned startLine, int modified )
{
    // lines added or removed ?
    if ( modified != 0 )
    {
        int deleted = modified > 0 ? modified : -modified;
        int lastLine = m_lines.size();
        if ( lastLine > deleted )
        {
            lastLine -= deleted;
            for (unsigned line = startLine + 1; line < lastLine; line++ )
            {
                Redo( line + deleted );
            }
        }

        if ( modified < 0 )
        {
            for ( int line = 1; line <= deleted; line++ )
            {
                Redo( line + startLine );
            }
        }
    }
    // current lines
    int endLine = startLine + ( modified > 0 ? modified : 0 );
    for (int line = startLine; line <= endLine; line++ )
    {
        if ( m_pending.insert(line).second )
        {
            m_lineMarkers.push_back( LineMarker( line, Redo( line ) ) );
        }
    }
}



/**
 * Apply markers in the linemarkers array
 */
void CEditorModMargin::ApplyMarkers ()
{
    MarkerVector::iterator iter = m_lineMarkers.begin();
    for ( ; iter != m_lineMarkers.end(); iter++ )
    {
        SetMarker( iter->first, iter->second );
    }
}



/**
 * Flush the line cache
 */
void CEditorModMargin::Flush ()
{
    LOG(LOG_MSG("-- Flush"));
    m_pending.clear();
    m_undoredo.clear();
    m_lineMarkers.clear();
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
