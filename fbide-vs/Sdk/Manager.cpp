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
#include "UiManager.h"
#include "ScriptManager.h"
#include "DocManager.h"
#include "EditorManager.h"
#include "Registry.h"
#include "PluginManager.h"
#include "TypeManager.h"

using namespace fbi;


/**
 * Manager class implementation
 */
struct TheManager : Manager
{
    // create
    TheManager()
    {
        // Load common ids
        IdMap & idmap = GetIdMap();
        idmap.Register("new",       wxID_NEW);
        idmap.Register("open",      wxID_OPEN);
        idmap.Register("save",      wxID_SAVE);
        idmap.Register("quit",      wxID_EXIT);
        idmap.Register("undo",      wxID_UNDO);
        idmap.Register("redo",      wxID_REDO);
        idmap.Register("cut",       wxID_CUT);
        idmap.Register("copy",      wxID_COPY);
        idmap.Register("paste",     wxID_PASTE);
        idmap.Register("find",      wxID_FIND);
        idmap.Register("replace",   wxID_REPLACE);
        idmap.Register("goto",      wxNewId());
        idmap.Register("about",     wxID_ABOUT);
    };


    // destroy
    ~TheManager()
    {
        ReleaseManagers();
    }


    // get version information
    virtual const Version & GetVersion()
    {
        // the static version object
        static Version v =
        {
            SDK_VERSION_MAJOR,
            SDK_VERSION_MINOR,
            SDK_VERSION_RELEASE,
            SDK_VERSION_STRING,
            __TIMESTAMP__
        };
        // return const ref
        return v;
    }


    // get global registry object
    virtual Registry & GetRegistry() { return m_reg; }


    // get translations
    virtual Language & GetLang() { return m_lang; }


    // get id map
    virtual IdMap & GetIdMap() { return m_idMap; }


    Registry    m_reg;      // application registry ( configuration )
    Language    m_lang;     // language translations
    IdMap       m_idMap;    // the global id map
};


// Get Ui manager instance
UiManager * Manager::GetUiManager()
{
    return UiManager::GetInstance();
}


// Get Ui manager instance
ScriptManager * Manager::GetScriptManager()
{
    return ScriptManager::GetInstance();
}



// get document manager
DocManager * Manager::GetDocManager()
{
    return DocManager::GetInstance();
}


// Get editor manager
EditorManager * Manager::GetEditorManager()
{
    return EditorManager::GetInstance();
}


// get plugin manager
PluginManager * Manager::GetPluginManager()
{
    return PluginManager::GetInstance();
}


// type manager
TypeManager * Manager::GetTypeManager()
{
    return TypeManager::GetInstance();
}


/**
 * Release all managers. The order is important
 * And probably needs some adjustment later on
 *
 * Logically thinking plugin and scripting should go down
 * first, followed by various content managers and lastly
 * the ui
 */
void Manager::ReleaseManagers()
{
    PluginManager::Release();
    ScriptManager::Release();
    TypeManager::Release();
    EditorManager::Release();
    DocManager::Release();
    UiManager::Release();
}


// implement manager
IMPLEMENT_MANAGER( Manager, TheManager )
