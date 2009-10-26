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
#include "IArtProvider.h"
#include "ClassicThemeProvider.h"

using namespace fbi;

namespace XPM {
    #include "xpm/xpm_tango/newsrc.xpm"
    #include "xpm/xpm_tango/opnproj.xpm"
    #include "xpm/xpm_tango/save.xpm"
    #include "xpm/xpm_tango/close.xpm"
    #include "xpm/xpm_tango/undo.xpm"
    #include "xpm/xpm_tango/redo.xpm"
    #include "xpm/xpm_tango/cut.xpm"
    #include "xpm/xpm_tango/copy.xpm"
    #include "xpm/xpm_tango/paste.xpm"
    #include "xpm/xpm_tango/search.xpm"
    #include "xpm/xpm_tango/srchrep.xpm"
    #include "xpm/xpm_tango/xpm_goto.xpm"
    #include "xpm/xpm_tango/screen.xpm"
    #include "xpm/xpm_tango/about.xpm"
    #include "xpm/xpm_tango/empty.xpm"
}


// construct
ClassicThemeProvider::ClassicThemeProvider ()
{
    // we use only xpm's so...
    m_bitmaps["new"]       = new wxBitmap (XPM::newsrc);
    m_bitmaps["open"]      = new wxBitmap (XPM::opnproj);
    m_bitmaps["save"]      = new wxBitmap (XPM::save);
    m_bitmaps["quit"]      = new wxBitmap (XPM::close);
    m_bitmaps["undo"]      = new wxBitmap (XPM::undo);
    m_bitmaps["redo"]      = new wxBitmap (XPM::redo);
    m_bitmaps["cut"]       = new wxBitmap (XPM::cut);
    m_bitmaps["copy"]      = new wxBitmap (XPM::copy);
    m_bitmaps["paste"]     = new wxBitmap (XPM::paste);
    m_bitmaps["find"]      = new wxBitmap (XPM::search);
    m_bitmaps["replace"]   = new wxBitmap (XPM::srchrep);
    m_bitmaps["goto"]      = new wxBitmap (XPM::xpm_goto);
    m_bitmaps["about"]     = new wxBitmap (XPM::about);
    m_bitmaps["log"]       = new wxBitmap (XPM::screen);
    m_bitmaps["icon.empty"]= new wxBitmap (XPM::empty);
    for (auto iter = m_bitmaps.begin(); iter != m_bitmaps.end(); iter++)
    {
        iter->second->SetMask(new wxMask(*iter->second, wxColour(191, 191, 191)));
    }
}


// destroy
ClassicThemeProvider::~ClassicThemeProvider()
{
    for (auto iter = m_bitmaps.begin(); iter != m_bitmaps.end(); iter++)
    {
        delete iter->second;
    }
}


/**
 * Get icon
 */
wxBitmap ClassicThemeProvider::GetIcon (const wxString & name, int size)
{
    auto iter = m_bitmaps.find(name);
    if (iter != m_bitmaps.end()) return *iter->second;
    return *m_bitmaps["icon.empty"];
}


/**
 * get icon size
 */
wxSize ClassicThemeProvider::GetIconSize (int type)
{
    return wxSize(16, 16);
}
