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

#ifndef SDK_PCH_H_INCLUDED
#define SDK_PCH_H_INCLUDED

// wxWudgets headers
#include <wx/wxprec.h>
#include <wx/hashmap.h>
#include <wx/filename.h>
#include <wx/button.h>
#include <wx/aui/aui.h>
#include <wx/xml/xml.h>
#include <wx/fileconf.h>
#include <wx/wfstream.h>
#include <wx/artprov.h>
#include <wx/aui/auibook.h>
#include <wx/variant.h>
#include <wx/dynlib.h>
#include <wx/log.h>
#include <wx/wxscintilla.h>

// Include STL
#include <memory>
#include <map>
#include <vector>

// FBIde specific global headers
#include "sdk/sdk.h"
#include "sdk/Exception.h"
#include "sdk/MultiDelegate.h"

namespace fb
{
    // Global declarations
    WX_DECLARE_HASH_MAP(wxString, wxString, wxStringHash, wxStringEqual, CStringStringHashMap);
    WX_DECLARE_STRING_HASH_MAP(int,         wxStringIntMap);

};

#endif // WX_PCH_H_INCLUDED
