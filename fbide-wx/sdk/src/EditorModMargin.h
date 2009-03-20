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

#ifndef EDITOR_MOD_MARGIN_H_INCLUDED
#define EDITOR_MOD_MARGIN_H_INCLUDED

#include <set>

namespace fb
{

    class CEditor;

    /**
     * Editor modification margin indicator
     */
    struct CEditorModMargin
    {

        // markers
        enum
        {
            MARKER_SAVED    = 0,
            MARKER_EDITED   = 1,
            FLAG_NONE       = 0,
            FLAG_SAVED      = 1 << MARKER_SAVED,
            FLAG_EDITED     = 1 << MARKER_EDITED,
            MARKER_FLAGS    = FLAG_SAVED | FLAG_EDITED,
            FULL_EDITED_FLAG= 0xAAAAAAAA,

            BITS_PER_STATE  = 2,
            CELL_SIZE       = ( sizeof(unsigned) * 8 ),
            STATES_IN_CELL  = CELL_SIZE / BITS_PER_STATE,
        };

        // construct
        CEditorModMargin ( CEditor & editor );

        // Modify line
        void Modify ( unsigned startLine, int lines, int markers );

        // submit modified
        void SubmitModify ();

        // Undo action
        void Undo ( unsigned int startLine, int lines );

        // Redo action
        void Redo ( unsigned int startLine, int lines );

        // Apply Undo / Redo
        void ApplyMarkers ();

        // Push this state to all modified lines
        void SetSavePos ( );

        // Flush the line cache
        void Flush ();


        private :

            // Set line markers
            void SetMarker ( unsigned line, unsigned markers );

            // Push value to stack
            void Push(unsigned line, unsigned flags);

            // Set current position to a value
            void Set(unsigned line, unsigned flags);

            // get current value
            unsigned Top ( unsigned line );

            // Pop value from stack
            unsigned Undo ( unsigned line );

            // Redo the line
            unsigned Redo ( unsigned line );

            // Set to hold the modified lines
            typedef std::set<int> IntSet;

            // vector of int's
            typedef std::vector<unsigned>       IntVector;

            // the lines vector
            typedef std::vector<IntVector>      LinesVector;

            // The line marker
            typedef std::pair<unsigned, unsigned>   LineMarker;

            // line markers
            typedef std::vector< LineMarker >   MarkerVector;

            CEditor       & m_editor;
            IntSet          m_pending, m_undoredo;
            LinesVector     m_lines;
            MarkerVector    m_lineMarkers;
    };

}

#endif // EDITOR_MOD_MARGIN_H_INCLUDED
