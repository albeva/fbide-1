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
#include "CmdManager.h"

using namespace fbi;

/**
 * Manager class implementation
 */
struct TheCmdManager : CmdManager
{

    // Retreave registered id
    // if doesn't exist will return new id value and log a warning
    virtual int GetId (const wxString & name)
    {
        auto iter = m_map.find(name);
        if (iter == m_map.end())
        {
            wxLogWarning("Id '%s' is not registered in IdMap", name);
            int newid = ::wxNewId();
            m_map[name] = newid;
            return newid;
        }
        return iter->second;
    }


    // register id
    virtual  void Register (const wxString & name, int id, bool overwrite)
    {
        auto iter = m_map.find(name);
        if (iter != m_map.end() && !overwrite)
        {
            wxLogWarning("Attempting to overwrite existing id '%s' in idmap", name);
            return;
        }
        m_map[name] = id;
    }


    // check if id is registred
    virtual bool IsRegistered (const wxString & name)
    {
        return m_map.find(name) == m_map.end();
    }


    // unregister the id
    virtual void UnRegister (const wxString & name)
    {
        auto iter = m_map.find(name);
        if (iter == m_map.end()) return;
        m_map.erase(iter);
    }

    // data
    HashMap<int> m_map;    // hold name id pairs
};


// Implement Manager
IMPLEMENT_MANAGER(CmdManager, TheCmdManager)