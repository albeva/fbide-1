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

#include <wx/wxprec.h>
#include <wx/stdpaths.h>
#include <wx/apptrait.h>
#include <wx/regex.h>
#include <wx/filename.h>
#include <wx/xml/xml.h>
#include <wx/aui/aui.h>
#include <wx/gdicmn.h>
#include <wx/fileconf.h>
#include <wx/wfstream.h>
#include <wx/dynlib.h>
#include <wx/log.h>
#include <wx/stc/stc.h>

#include <unordered_map>
#include <memory>

#include "Sdk.h"
#include "Variant.h"
#include "Utilities.h"
#include "Version.h"
#include "Registry.h"
#include "Language.h"
#include "IdMap.h"
