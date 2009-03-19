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

#include "wx_pch.h"
#include "sdk/Manager.h"
#include "sdk/PluginManager.h"
#include "sdk/PluginArtProvider.h"
#include "sdk/UiManager.h"
#include "sdk/ArtProvider.h"

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

using namespace fb;

/**
 * Gfx provider
 */
class TangoThemeProvider : public IArtProvider
{
    public :
        // Create plugin instance
        TangoThemeProvider ()
        {
            // we use only xpm's so...
            m_bitmaps[_T("new")]       = new wxBitmap (XPM::newsrc);
            m_bitmaps[_T("open")]      = new wxBitmap (XPM::opnproj);
            m_bitmaps[_T("save")]      = new wxBitmap (XPM::save);
            m_bitmaps[_T("quit")]      = new wxBitmap (XPM::close);
            m_bitmaps[_T("undo")]      = new wxBitmap (XPM::undo);
            m_bitmaps[_T("redo")]      = new wxBitmap (XPM::redo);
            m_bitmaps[_T("cut")]       = new wxBitmap (XPM::cut);
            m_bitmaps[_T("copy")]      = new wxBitmap (XPM::copy);
            m_bitmaps[_T("paste")]     = new wxBitmap (XPM::paste);
            m_bitmaps[_T("find")]      = new wxBitmap (XPM::search);
            m_bitmaps[_T("replace")]   = new wxBitmap (XPM::srchrep);
            m_bitmaps[_T("goto")]      = new wxBitmap (XPM::xpm_goto);
            m_bitmaps[_T("about")]     = new wxBitmap (XPM::about);
            m_bitmaps[_T("log")]       = new wxBitmap (XPM::screen);
            m_bitmaps[_T("icon.empty")]= new wxBitmap (XPM::empty);
            for (CBitmapMap::iterator iter = m_bitmaps.begin(); iter != m_bitmaps.end(); iter++)
            {
                iter->second->SetMask(new wxMask(*iter->second, wxColour(191, 191, 191)));
            }
        }

        // clean up
        virtual ~TangoThemeProvider ()
        {
            for (CBitmapMap::iterator iter = m_bitmaps.begin(); iter != m_bitmaps.end(); iter++)
            {
                delete iter->second;
            }
        }


        virtual wxBitmap GetIcon (const wxString & name, int size = IArtProvider::SMALL)
        {
            CBitmapMap::iterator iter = m_bitmaps.find(name);
            if (iter != m_bitmaps.end())
            {
                return *iter->second;
            }
            return *m_bitmaps[_T("icon.empty")];
        }


        virtual wxSize GetIconSize (int type)
        {
            return wxSize(16, 16);
        }


    private :
        WX_DECLARE_STRING_HASH_MAP(wxBitmap *, CBitmapMap);
        CBitmapMap m_bitmaps;
};

// Register the plugin
CPluginArtProvider<TangoThemeProvider> plugin(_T("TangoTheme"));
