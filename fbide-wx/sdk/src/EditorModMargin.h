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
        };

        // construct
        CEditorModMargin ( CEditor & editor );

        // Modify line
        void Modify ( unsigned startLine, int lines );

        // push undo / redo line
        void Undo ( unsigned int startLine, int lines );

        // Redo
        void Redo ( unsigned int startLine, int lines );

        // Submit Undo action
        void SubmitUndo ();

        // Submit Redo action
        void SubmitRedo ();

        // Flush the line cache
        void Flush ();

        // Get number of lines
        unsigned GetPendingCount ();


        private :

            // Set line markers
            void SetMarker ( unsigned line, unsigned markers );

            // Push value to stack
            void Push(unsigned line, unsigned flags);

            // Set current position to a value
            void Set(unsigned line, unsigned flags);

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

            CEditor       & m_editor;
            IntSet          m_pending, m_undo;
            LinesVector     m_lines;
    };

}

#endif // EDITOR_MOD_MARGIN_H_INCLUDED
