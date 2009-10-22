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

#include "variant.h"
#include <unordered_map>

namespace fb
{
    /**
     * Simple hashmap containing identifier and
     * meaning for translations, etc...
     */
    class Language
    {
        public :

            /// get translation by key. If doesn't exist return the
            /// key.
            /// @note Should it log a warning ?
            wxString & operator [] ( const wxString & key ) {
                auto iter = m_map.find(key);
                if (iter != m_map.end()) return iter->second;
                return m_map[key] = key;
            }

        private :
            StringMap m_map;
    };

}
