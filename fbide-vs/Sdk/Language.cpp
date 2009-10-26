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
#include "sdk_pch.h"
using namespace fbi;


/**
 * Get translation for ID or return ID itself
 */
wxString & Language::operator [] ( const wxString & key ) {
    auto iter = m_map.find(key);
    if (iter != m_map.end()) return iter->second;
    return m_map[key] = key;
}


/**
 * Get translation and replace tags
 */
wxString Language::Get (const wxString & key, const StringHashMap & map)
{
    wxString str = (*this)[key]; // create a copy
    for (auto iter = map.begin(); iter != map.end(); iter++) {
        str.Replace("{" + iter->first + "}", iter->second, true);
    }
    return str;
}


/**
 * Load language
 */
void Language::Load (const wxString & file)
{
    if (!wxFileExists(file)) {
        wxLogError("Language file '%s' not found", file);
        return;
    }
    wxFileInputStream is (file);
    wxFileConfig ini(is);

    long index = 0;
    wxString entry;
    if (!ini.GetFirstEntry (entry, index)) return;

    do
    {
        (*this)[entry] = ini.Read(entry, "");
    } while (ini.GetNextEntry(entry, index));
}