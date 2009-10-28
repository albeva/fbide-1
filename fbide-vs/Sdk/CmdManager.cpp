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


#include <functional>


using namespace fbi;

/**
 * Manager class implementation
 */
struct TheCmdManager : CmdManager
{

    // Add new entry. Internal call only
    inline Entry & AddEntry (const wxString & name, int id, CmdManager::Type type, wxObject * object, bool checked)
    {
        Entry & entry = m_map[name].entry;
        entry.id = id;
        entry.type = type;
        entry.object = object;
        entry.checked = checked;
        return entry;
    }


    // Retreave registered id
    // if doesn't exist will return new id value and log a warning
    virtual int GetId (const wxString & name)
    {
        auto iter = m_map.find(name);
        if (iter == m_map.end())
        {
            wxLogWarning("Id '%s' is not registered with CmdManager", name);
            Entry & entry = AddEntry(name, ::wxNewId(), CmdManager::Type_Normal, nullptr, false);
            return entry.id;
        }
        return iter->second.entry.id;
    }


    // Retreave registered control kind
    virtual Entry * FindEntry (const wxString & name)
    {
        auto iter = m_map.find(name);
        if (iter == m_map.end())
        {
            return nullptr;
        }
        return &iter->second.entry;
    }


    // get entry or create a default one
    virtual Entry & GetEntry (const wxString & name)
    {
        auto iter = m_map.find(name);
        if (iter == m_map.end())
        {
            wxLogWarning("Id '%s' is not registered with CmdManager", name);
            return AddEntry(name, ::wxNewId(), CmdManager::Type_Normal, nullptr, false);
        }
        return iter->second.entry;
    }


    // register id
    virtual  void Register (const wxString & name, int id, CmdManager::Type type, wxObject * object, bool checked)
    {
        // Already registered?
        if (m_map.find(name) != m_map.end())
        {
            wxLogWarning("Attempting to overwrite existing id '%s' in CmdManager", name);
            return;
        }
        // check for integrity
        if (type == CmdManager::Type_Menu && !dynamic_cast<wxMenu*>(object))
        {
            wxLogError("Object passed must be an instance of wxMenu");
            return;
        }
        // insert
        AddEntry(name, id, type, object, checked);
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

    
    // Check a check item. Basically notify any listeber bound
    virtual void Check (const wxString & name, bool state)
    {
        auto iter = m_map.find(name);
        if (iter == m_map.end())
        {
            wxLogWarning("Id '%s' is not registered with CmdManager", name);
            return;
        }
        // iter->second.m_checkDlg(state);
    }

    /*
    // Bind listener
    virtual void BindCeckListener (const wxString & name, Delegate<void(bool)> dg)
    {
        auto iter = m_map.find(name);
        if (iter == m_map.end())
        {
            wxLogWarning("Id '%s' is not registered with CmdManager", name);
            return;
        }
        iter->second.m_checkDlg += dg;
    }


    // UnBind the listener
    virtual void UnBindCeckListener (const wxString & name, Delegate<void(bool)> dg)
    {
        auto iter = m_map.find(name);
        if (iter == m_map.end())
        {
            wxLogWarning("Id '%s' is not registered with CmdManager", name);
            return;
        }
        iter->second.m_checkDlg -= dg;
    }
    */

    // Internal data structure for entries
    struct InternalEntry
    {
        Entry                       entry;      // the public data entry
        // MultiDelegate<void(bool)>   m_checkDlg; // check callback delegate
    };

    // data
    HashMap<InternalEntry> m_map;    // hold name id pairs
};


// Implement Manager
IMPLEMENT_MANAGER(CmdManager, TheCmdManager)