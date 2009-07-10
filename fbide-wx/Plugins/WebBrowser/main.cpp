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
#include "sdk/TypeManager.h"
#include "webdoc.h"
#include <wx/stdpaths.h>
#include <wx/dir.h>

using namespace fb;



struct WebBrowser : public CPluginBase
{
    // Attach plugin
    bool Attach ()
    {
        // init the XUL path
        wxString xulrunner_path = FindXulRunner(wxT("ide\\xr"));
        if (xulrunner_path.IsEmpty())
        {
            wxMessageBox(wxT("Could not find xulrunner directory"));
            return false;
        }

        // Finally, initialize the engine; Calling wxWebControl::InitEngine()
        // is very important and has to be made before using wxWebControl.
        // It instructs wxWebConnect where the xulrunner directory is.
        if (!wxWebControl::InitEngine(xulrunner_path))
        {
            wxMessageBox(_T("Failed to initalize the xulrunner"));
            return false;
        }

        // xulrunner is initalized!
        CTypeManager * tm = GET_TYPEMGR();
        tm->AddType(_T("text/html"), _T("HTML files"), new CTypeRegistrant<WebDoc>);
        tm->BindExtensions(_T("text/html"), _T("html;htm"));

        if (!tm->TypeExists(_T("default")))
            tm->BindAlias(_T("default"), _T("text/html"));

        return true;
    }

    // void
    bool Detach (bool force)
    {
        return true;
    }

    /**
     * find xul runner path
     *
     * @todo these should be in the plugin director
     * ide/plugins/WebBrowser/xr
     */
    wxString FindXulRunner(const wxString& xulrunner_dirname)
    {
        // get the location of this executable
        wxString exe_path = wxStandardPaths::Get().GetExecutablePath();
        wxString path_separator = wxFileName::GetPathSeparator();
        exe_path = exe_path.BeforeLast(path_separator[0]);
        exe_path += path_separator;

        wxString path;

        // first, check <exe_path>/<xulrunner_path>
        path = exe_path + xulrunner_dirname;
        if (wxDir::Exists(path))
            return path;

        // next, check <exe_path>/../<xulrunner_path>
        path = exe_path + wxT("..") + path_separator + xulrunner_dirname;
        if (wxDir::Exists(path))
            return path;

        // finally, check <exe_path>/../../<xulrunner_path>
        path = exe_path + wxT("..") + path_separator + wxT("..") + path_separator + xulrunner_dirname;
        if (wxDir::Exists(path))
            return path;

        return wxEmptyString;
    }

};

namespace
{
    CPluginProvider<WebBrowser, PLUGINTYPE_TOOL> plugin (_T("WebBrowser"));
}
