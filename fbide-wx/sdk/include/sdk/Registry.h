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

#ifndef REGISTRY_H_INCLUDED
#define REGISTRY_H_INCLUDED

#include "sdk/Variant.h"

namespace fb
{

    /**
     * Simple registry class
     */
    class CRegistry : public std::map<wxString, CVariant>
    {
        public :
            CVariant & operator [] (const char * c_ch)
            {
                return std::map<wxString, CVariant>::operator[](wxString(c_ch, wxConvUTF8));
            }
            CVariant & operator [] (const wxChar * c_wch)
            {
                return std::map<wxString, CVariant>::operator[](c_wch);
            }
    };

}

#endif // REGISTRY_H_INCLUDED
