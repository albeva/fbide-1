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
#include "manager.h"
#include "uimanager.h"
#include "scriptmanager.h"
#include "registry.h"

using namespace fb;


/**
 * Manager class implementation
 */
struct TheManager : Manager
{
    // create
    TheManager () {}


    // destroy
    ~TheManager () { ReleaseManagers(); }


    // get version information
    const Version & GetVersion ()
    {
        // the static version object
        static Version v = {
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
    Registry & GetRegistry () { return m_reg; }


    // application registry ( configuration )
    Registry m_reg;
};


// Get Ui manager instance
UiManager * Manager::GetUiManager ()
{
    return UiManager::GetInstance();
}


// Get Ui manager instance
ScriptManager * Manager::GetScriptManager ()
{
    return ScriptManager::GetInstance();
}


// Release all managers
void Manager::ReleaseManagers ()
{
    ScriptManager::Release();
    UiManager::Release();
}


// implement manager
IMPLEMENT_MANAGER(Manager, TheManager)
