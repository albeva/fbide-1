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
#include "Editor.h"
#include "UiManager.h"

using namespace fbi;


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
    }
    

    /**
     * new
     */
    void OnNew (wxCommandEvent & event)
    {
        auto editor = GET_EDITORMGR()->CreateEditor();
        GET_UIMGR()->AddDocument(editor);
    }

    
    // handle events
    DECLARE_EVENT_TABLE()
};


BEGIN_EVENT_TABLE(TheDocManager, DocManager)
    EVT_MENU( wxID_NEW, TheDocManager::OnNew)
END_EVENT_TABLE()


// Implement Manager
IMPLEMENT_MANAGER( DocManager, TheDocManager )
