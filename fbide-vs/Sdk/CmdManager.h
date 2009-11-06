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

    // check event
    wxDECLARE_EXPORTED_EVENT(SDK_DLL, fbiCMD_CHECK,     wxCommandEvent);
    wxDECLARE_EXPORTED_EVENT(SDK_DLL, fbiCMD_ENABLE,    wxCommandEvent);
    
    /**
     * Command manager is used for mapping between textual name and ID value
     * mainly used by xml loader in UiManager.
     * It alos provides a way to bind UI elements (check item, menus, dropdowns, ...) to
     * the IDs so that UI loader knows how to handle such special cases.
     */
    class SDK_DLL CmdManager : public wxEvtHandler, public NonCopyable
    {
        public :
        
        // data structure to hold information
        struct Entry {
            int         id;         // numeric ID
            int         type;       // type
            bool        checked;    // For Entry_Check
            bool        enabled;    // is control enabled ?
            wxObject *  object;     // the control pointer
        };

        // Entry types
        enum Type {
            Type_Normal,
            Type_Check,
            Type_Menu
        };

        // Retreave registered id
        // if doesn't exist will return new id value and log a warning
        virtual int GetId (const wxString & name) = 0;

        // Get entry for the given name. If doesn't exist log a warning and create
        // a default entyr
        virtual Entry & GetEntry (const wxString & name) = 0;

        // retreave the entry for the given name. nullptr if doesn't exist
        virtual Entry * FindEntry (const wxString & name) = 0;

        // retreave entry by registered numeric ID
        virtual Entry * FindEntry (int id) = 0;

        // register id
        virtual void Register (const wxString & name, int id, 
                               CmdManager::Type type = CmdManager::Type_Normal,
                               wxObject * object = nullptr, bool checked = false, bool enabled = true) = 0;

        // check if id is registred
        virtual bool IsRegistered (const wxString & name) = 0;

        // unregister the id
        virtual void UnRegister (const wxString & name) = 0;

        // toggle the checkbox
        virtual void Check (const wxString & name, bool state) = 0;

        // toggle checkbox identified by numeric ID
        virtual void Check (int id, bool state) = 0;

        // enable / disable signal by name
        virtual void Enable (const wxString & name, bool state) = 0;

        // enable / disable signal by numeric id
        virtual void Enable (int id, bool state) = 0;

        // declare the manager
        DECLARE_MANAGER(CmdManager)
    };

}
