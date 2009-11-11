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
#include "EditorManager.h"
#include "TypeManager.h"
#include "Editor.h"
#include "Editor/StyleInfo.h"

using namespace fbi;


/**
 * Manager class implementation
 */
struct TheEditorManager : EditorManager
{

    // create
    TheEditorManager ()
    {
        m_styleParser.LoadFile(GET_REG()["path.ide.data"].AsString() + "/default.css");
    }


    // destroy
    ~TheEditorManager () {}
    

    // get new editor object
    virtual Editor * CreateEditor()
    {
        return nullptr;
        // return new Editor();
    }


    // get style
    virtual StyleParser * GetStyle ()
    {
        return &m_styleParser;
    }


    StyleParser m_styleParser;
};


// Implement Manager
IMPLEMENT_MANAGER(EditorManager, TheEditorManager)
