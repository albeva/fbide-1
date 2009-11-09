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
#include "Editor/Editor.h"
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
        tm->BindExtensions("freebasic", "bas;bi;txt");

        // generic
        tm->Register("generic", "Generic Editor", DocumentCreator<Test>);
        tm->BindExtensions("generic", "txt;ini;xml");
        
        // the default
        tm->BindAlias("default", "freebasic");
    }
    

    /**
     * new
     */
    void OnNew (wxCommandEvent & event)
    {
        auto tm = GET_TYPEMGR();
        auto doc = tm->CreateFromFile("somefile.sdf");
        if (doc)
        {
            GET_UIMGR()->AddDocument(doc);
        }
    }

    
    // handle events
    DECLARE_EVENT_TABLE()
};


BEGIN_EVENT_TABLE(TheDocManager, DocManager)
    EVT_MENU( wxID_NEW, TheDocManager::OnNew)
END_EVENT_TABLE()


// Implement Manager
IMPLEMENT_MANAGER( DocManager, TheDocManager )
