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
#include "sdk/UiManager.h"
#include "webdoc.h"
#include <wx/stdpaths.h>
#include <wx/dir.h>

/**
 * contruct the wen view
 */
WebDoc::WebDoc(const wxString & file)
    : m_browser(NULL)
{
    // construct the editor
    wxPanel::Create (
        GET_UIMGR()->GetDocWindow(),
        wxID_ANY,
        wxDefaultPosition, wxDefaultSize,
        wxBORDER_SIMPLE
    );

    // for the example, disable cookies and popups
    wxWebPreferences webprefs = wxWebControl::GetPreferences();
    webprefs.SetIntPref(wxT("network.cookie.cookieBehavior"), 2 /* disable all cookies */);
    webprefs.SetIntPref(wxT("privacy.popups.policy"), 2 /* reject popups */);

    m_browser = new wxWebControl(this, -1);
    m_browser->OpenURI(_T("http://www.google.co.uk"));
    m_browser->Show();
    Show();


    if (!m_browser->IsOk())
    {
        wxMessageBox(_T("NOT OKAY"));
    }
}
