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
     * Command manager is used for mapping between textual name and ID value
     * mainly used by xml loader in UiManager.
     * It alos provides a way to bind UI elements (check item, menus, dropdowns, ...) to
     * the IDs so that UI loader knows how to handle such special cases.
     */
    class SDK_DLL CmdManager : public NonCopyable
    {
        public :

            // Retreave registered id
            // if doesn't exist will return new id value and log a warning
            virtual int GetId (const wxString & name) = 0;

            // register id
            virtual void Register (const wxString & name, int id, bool overwrite = false) = 0;

            // check if id is registred
            virtual bool IsRegistered (const wxString & name) = 0;

            // unregister the id
            virtual void UnRegister (const wxString & name) = 0;

        private :

            DECLARE_MANAGER(CmdManager)
    };

}
