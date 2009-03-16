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
    #include "xpm/newsrc.xpm"
    #include "xpm/opnproj.xpm"
    #include "xpm/save.xpm"
    #include "xpm/close.xpm"
    #include "xpm/undo.xpm"
    #include "xpm/redo.xpm"
    #include "xpm/cut.xpm"
    #include "xpm/copy.xpm"
    #include "xpm/paste.xpm"
    #include "xpm/search.xpm"
    #include "xpm/srchrep.xpm"
    #include "xpm/xpm_goto.xpm"
    #include "xpm/about.xpm"
    #include "xpm/empty.xpm"
}

using namespace fb;


/**
 * Gfx provider
 */
class ClassicThemeProvider : public IArtProvider
{
    public :

        // Create plugin instance
        ClassicThemeProvider ()
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
            m_bitmaps[_T("icon.empty")]= new wxBitmap (XPM::empty);
            for (CBitmapMap::iterator iter = m_bitmaps.begin(); iter != m_bitmaps.end(); iter++)
            {
                iter->second->SetMask(new wxMask(*iter->second, wxColour(191, 191, 191)));
            }
        }

        // clean up
        virtual ~ClassicThemeProvider ()
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

// Register plugin
CPluginArtProvider<ClassicThemeProvider> plugin(_T("ClassicTheme"));
