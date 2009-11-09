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
     * Simple hashmap containing identifier and
     * meaning for translations, etc...
     */
    class SDK_DLL Language : public NonCopyable
    {
        public :

            /// get translation by key. If doesn't exist return the
            /// key.
            wxString & operator [] ( const wxString & key );

            /// get the translation
            wxString Get(const wxString & key) { return (*this)[key]; }

            /// get translation by key. If doesn't exist return the
            /// key. Replace tags
            wxString Get(const wxString & key, const StringHashMap & map);

            /// get translation. use std::pair for tag substitution
            wxString Get(const wxString & key, const std::pair<wxString, wxString> & tag);

            /// get translation and replace tag
            wxString Get(const wxString & key, const wxString & tag, const wxString & value);

            /// Load the language file 
            void Load (const wxString & file);

        private :
            StringHashMap m_map;
    };

}
