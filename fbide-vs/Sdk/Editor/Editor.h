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
#pragma once
#include "../Document.h"

namespace fbi
{
    // splitter    
    class MultiSplitWindow;
    class StcEditor;

    /**
     * Editor panel
     */
    class SDK_DLL Editor : public Document
    {
        public :
            // default constructor
            Editor ();

            // Split / Unsplit vertically
            void OnSplitVertically(wxCommandEvent & event);

            // Split / Unsplit horizontally
            void OnSplitHorizontally(wxCommandEvent & event);

        private:

            // the panel that contains the controls
            wxPanel * m_panel;
            // main sizer
            wxSizer * m_sizer; 
            // splitter containint the editor(s)
            MultiSplitWindow * m_splitter;
            // main stc editor
            StcEditor * m_stc;
            // additional editors visible when split
            StcEditor * m_editors[4];

            // has an event table
            DECLARE_EVENT_TABLE()
    };

}