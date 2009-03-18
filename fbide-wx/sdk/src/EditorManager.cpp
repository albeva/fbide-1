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

#include "sdk/Manager.h"
#include "sdk/Editor.h"
#include "sdk/EditorEvent.h"
#include "sdk/EditorManager.h"
#include "sdk/Singleton.h"

using namespace fb;

struct EdMgr : public CEditorManager, public CSingleton<EdMgr>
{
    // Alloc accesses ...
    friend class CSingleton<EdMgr>;
    friend class CEditorManager;


    // Register new editor
    void RegisterEditor ( CEditor * editor )
    {
        GetEventMap(EVT_OPEN)(EVT_OPEN, *editor);
    }

};

// Get and Free
CEditorManager * CEditorManager::Get () { return EdMgr::GetInstance(); }
void CEditorManager::Free () { EdMgr::Release (); }
