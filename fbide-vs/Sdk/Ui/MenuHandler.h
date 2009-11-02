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
    struct UiMenuHandler
    {
        // constructor
        UiMenuHandler();

        // Initalize
        void Init (wxMenuBar *);

        // Uninitalize
        void UnInit ();

        // Load from XML
        void Load (wxXmlNode * node, wxMenu * parent = nullptr);

        // Get menu by ID
        wxMenu * GetMenu (const wxString & id);

        // Add new menu
        void AddMenu (const wxString & id, wxMenu * menu, bool show = false);

        // Add a new item to the menu
        void AddMenuItem (const wxString & id, wxMenu * parent);

        // Flag check items
        // void CheckItem(const wxString & name, CmdManager::Entry & entry);
        
        private :
            HashMap<wxMenu *>   m_map;          // hold id-menu associations
            wxMenuBar *         m_mbar;         // the menu bar
    };

}
