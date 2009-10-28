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

namespace fbi
{

    /**
     * Handle menus
     */
    struct UiToolbarHandler
    {
        // constructor
        UiToolbarHandler();

        // Initalize
        void Init (wxAuiManager * aui);

        // Uninitalize
        void UnInit ();

        // Load from XML
        void Load (wxXmlNode * node);

        // get toolbar by id
        wxAuiToolBar * GetToolBar (const wxString & id);

        // Add toolbar
        void AddToolBar (const wxString & name, wxAuiToolBar * toolbar, bool show = true);

        // add item to toolbar
        void AddToolBarItem (const wxString & name, wxAuiToolBar * toolbar);

        // Catch pane close events (for toolbars)
        void OnPaneClose(wxAuiManagerEvent & event);

        // catch tbar menu clicks
        void OnToolbarMenuClick(wxCommandEvent & event);

        // Show / Hide the toolbars
        void OnToggleToolbars(wxCommandEvent & event);


        private :
            wxAuiManager *                  m_aui;      // toolbar owner
            wxWindow *                      m_parent;   // parent window
            wxMenu *                        m_menu;     // menu where to put toolbar entries
            HashMap<wxAuiToolBar *>         m_map;      // hold id-toolbar associations
            std::unordered_map<int, int>    m_idbridge; // bridge toolbar id and a menu commands
            bool                            m_showTbars;// Show toolbars
    };

}
