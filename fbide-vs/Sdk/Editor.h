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

#include "Document.h"
#include "MultiSplitter.h"

namespace fbi
{
    // forward declarations
    class MultiSplitWindow;
    class StcEditor;


    /**
     * Editor panel
     */
    class SDK_DLL Editor : public MultiSplitWindow, public Document
    {
        public :

            // default constructor
            Editor ();

            // Load file
            virtual bool LoadDocFile (const wxString & file);

            // Show context menu for the editor
            void ShowContextMenu ( int index );

            // On split view
            void OnSplitView (wxCommandEvent & event);

        private:
            
            // the 4 editors. index 0 is the main
            // others are additional views
            wxTextCtrl * m_editors[4];

            // selected editor index
            int m_activeIndex;

            // has an event table
            DECLARE_EVENT_TABLE()
    };

}