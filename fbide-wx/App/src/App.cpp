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

#include "../wx_pch.h"
#include <wx/clipbrd.h>
#include <wx/dir.h>
//#include <wx/windowlist.h>

#include "App.h"
#include "Frame.h"

#include "sdk/Manager.h"
#include "sdk/UiManager.h"
#include "sdk/DocManager.h"
#include "sdk/TypeManager.h"
#include "sdk/ArtProvider.h"
#include "sdk/PluginManager.h"
#include "sdk/StyleInfo.h"

#include "sdk/Document.h"

using namespace fb;

/**
 * Register Application class with WX
 * and trigger loading
 */
DECLARE_APP(CApp);
IMPLEMENT_APP(CApp);

#include "sdk/Registry.h"

/**
 * This is application entry point
 */
bool CApp::OnInit()
{

    FBIDE_TRY
    {
        // create application window
        wxFrame * frame = new CFrame(0L, _T("FBIde"));

        // Load the base manager and set frame
        CManager        & mgr = *CManager::Get();
        mgr.SetFrame (frame);

        // load sub managers
        CUiManager      & ui  = *mgr.GetUiManager();

        // registry
        CRegistry         & reg = mgr.GetReg();


        // Set base configuration
        reg["dir.base"]     = ::wxPathOnly(argv[0]) + _T("/");
        reg["dir.data"]     = reg["dir.base"].AsString() + _T("ide/");
        reg["dir.plugins"]  = reg["dir.data"].AsString() + _T("plugins/");
        reg["file.config"]  = reg["dir.data"].AsString() + _T("config.ini");

        // Load config file
        mgr.LoadConfig(reg["file.config"].AsString());

        // Load colour pallettes
        mgr.SetupPalette(_T(""));

        // Load language file
        mgr.LoadLang(reg["dir.data"].AsString() + reg["file.lang"].AsString());

        // Each component will have to register it's
        // own ID's to global id map and give each
        // unique name. ID's are used also for mapping
        // images
        mgr.RegisterId(_T("new"),       wxID_NEW);
        mgr.RegisterId(_T("open"),      wxID_OPEN);
        mgr.RegisterId(_T("save"),      wxID_SAVE);
        mgr.RegisterId(_T("quit"),      wxID_EXIT);
        mgr.RegisterId(_T("undo"),      wxID_UNDO);
        mgr.RegisterId(_T("redo"),      wxID_REDO);
        mgr.RegisterId(_T("cut"),       wxID_CUT);
        mgr.RegisterId(_T("copy"),      wxID_COPY);
        mgr.RegisterId(_T("paste"),     wxID_PASTE);
        mgr.RegisterId(_T("find"),      wxID_FIND);
        mgr.RegisterId(_T("replace"),   wxID_REPLACE);
        mgr.RegisterId(_T("goto"),      wxNewId());
        mgr.RegisterId(_T("about"),     wxID_ABOUT);
        // mgr.RegisterId(_T("log"),       wxNewId());


        // Initalize docManager. Must be called after wxFrame has been assigned to uiManager
        GET_DOCMGR();

        // Load plugin manager
        CPluginManager * pm = GET_PLUGINMGR();

        // experimental plugins
        pm->LoadPlugin(_T("FBIdePlugin"));

        // Load LogPlugin
        pm->LoadPlugin(_T("LogPlugin"));

        // Load Skin plugin
        pm->LoadPlugin(reg["ui.plugin.theme"].AsString("TangoTheme"));

        // Load styles
        mgr.SetStyleParser(
            _T("default"),
            new CStyleParser(reg["dir.data"].AsString() + reg["file.ui.style"].AsString())
        );

        // Load UI
        // ui.SetArtProvider(new CArtProvider());
        ui.LoadLayout(reg["dir.data"].AsString() + reg["file.ui.layout"].AsString());

        // Load up projects
        //pm->LoadPlugin(_T("Projects"));

        // startup is OK, show window and return
        ui.Load();

        // open files in sample/ directory
        /*
        wxDir dir (reg["dir.base"].AsString() + _T("samples/"));
        if (dir.IsOpened() && dir.HasFiles(_T("*.bas")))
        {
            wxArrayString files;
            dir.GetAllFiles(dir.GetName(), &files, _T("*.bas"));
            GET_DOCMGR()->Open(files);
        }
        */

        return true;
    }
    FBIDE_CATCH();
    return false;
}


/**
 * Make Log window visible
 */
/*
void CApp::ShowLog (wxCommandEvent &)
{
    if (wxLogWindow * log = dynamic_cast<wxLogWindow *>(wxLog::GetActiveTarget()))
    {
        log->Show();
    }
}
*/


/**
 * Close down the program and do final cleanup
 */
int CApp::OnExit ()
{
    FBIDE_TRY
    {



        // Save the config
        CManager & mgr = *CManager::Get();
        CRegistry  & reg = mgr.GetReg();
        wxString file = reg[_T("file.config")].AsString();
        reg.erase(_T("dir.base"));
        reg.erase(_T("dir.data"));
        reg.erase(_T("dir.plugins"));
        reg.erase(_T("file.config"));
        mgr.SaveConfig(file);

        // Release the manager. It will
        // internally release all attached
        // sub managers
        CManager::Free();

        // don't let the clipboard data be emptied!
        wxTheClipboard->Flush();

        // return native OnExit...
        return wxApp::OnExit();
    }
    FBIDE_CATCH();
    return EXIT_FAILURE;
}



/**
 * Run the system and catch any thrown exceptions from the application
 * during it's lifetime
 */
int CApp::OnRun ()
{
    FBIDE_TRY
    {
        return wxApp::OnRun();
    }
    FBIDE_CATCH();
    return EXIT_FAILURE;
}
