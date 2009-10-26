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
     * Id map. Map numeric ID's to strings so xml
     * based UI can properly load and bind ID values
     * Is ID grouping needed? Probably not...
     */
    class SDK_DLL IdMap : public NonCopyable
    {
        public :

            // Retreave registered id
            // if doesn't exist will return new id value and log a warning
            int operator [] (const wxString & name);

            // register id
            void Register (const wxString & name, int id, bool overwrite = false);

            // check if id is registred
            bool IsRegistered (const wxString & name);

            // unregister the id
            void UnRegister (const wxString & name);

        private :

            HashMap<int> m_map;
    };

}
