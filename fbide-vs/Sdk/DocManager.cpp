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
#include "Manager.h"
#include "DocManager.h"
#include "EditorManager.h"
#include "TypeManager.h"
#include "Editor.h"
#include "UiManager.h"

using namespace fbi;


struct Test : Document, wxTextCtrl
{

    Test ()
    {
        Create (
            GET_UIMGR()->GetDocumentArea(), wxID_ANY, "", 
            wxDefaultPosition, wxDefaultSize,
            wxTE_MULTILINE | wxTE_BESTWRAP
        );
        SetDocWindow(this);
    }

};



/**
 * Manager class implementation
 */
struct TheDocManager : DocManager
{
    // create
    TheDocManager()
    {
        auto frame = GET_FRAME();
        frame->PushEventHandler(this);

        // register some types
        auto tm = GET_TYPEMGR();
        // freebasic
        tm->Register("freebasic", "FreeBASIC editor", DocumentCreator<Editor>);
        tm->BindExtensions("freebasic", "bas;bi;");
        
        // the default
        tm->BindAlias("default", "freebasic");
    }
    

    // new document
    void OnNew (wxCommandEvent & event)
    {
        // lock the ui to avoid any flicker
        wxWindowUpdateLocker uiLock(GET_FRAME());

        auto tm = GET_TYPEMGR();
        auto doc = tm->CreateFromType("default");
        if (doc)
        {
            GET_UIMGR()->AddDocument(doc);
        }
    }


    // open document
    void OnOpen (wxCommandEvent & event)
    {
        // vars
        auto tm = GET_TYPEMGR();
        auto & lang = GET_LANG();

        // show file load dialog
        wxString filters = tm->GetFileFilters(true);
        wxFileDialog dlg (
            GET_FRAME(),
            lang["open-file"],
            "", "",             // default dir, default file
            filters,            // wildcards
            wxFD_OPEN | wxFD_FILE_MUST_EXIST | wxFD_MULTIPLE
        );
        if (dlg.ShowModal() != wxID_OK) return;

        // get selected files
        wxArrayString paths;
        dlg.GetPaths(paths);

        // lock ui to avoid flickering
        wxWindowUpdateLocker uiLock(GET_FRAME());

        // Open the files
        for (int i = 0; i < paths.Count(); i++)
        {
            auto doc = tm->CreateFromFile(paths[i]);
            if (doc)
            {
                GET_UIMGR()->AddDocument(doc);
            }
        }
    }


    
    // handle events
    DECLARE_EVENT_TABLE()
};


BEGIN_EVENT_TABLE(TheDocManager, DocManager)
    EVT_MENU( wxID_NEW,     TheDocManager::OnNew )
    EVT_MENU( wxID_OPEN,    TheDocManager::OnOpen )
END_EVENT_TABLE()


// Implement Manager
IMPLEMENT_MANAGER( DocManager, TheDocManager )
