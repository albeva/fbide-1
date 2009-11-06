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

// check event
wxDEFINE_EVENT(fbi::fbiCMD_CHECK,   wxCommandEvent);
wxDEFINE_EVENT(fbi::fbiCMD_ENABLE,  wxCommandEvent);


/**
 * Manager class implementation
 */
struct TheCmdManager : CmdManager
{

    // Add new entry. Internal call only
    inline Entry & AddEntry (const wxString & name, int id, CmdManager::Type type, wxObject * object, bool checked, bool enabled)
    {
        auto sp = std::make_shared<InternalEntry>();
        m_map[name] = sp;
        m_idMap[id] = sp;
        Entry & entry = sp->m_entry;
        entry.id = id;
        entry.type = type;
        entry.object = object;
        entry.checked = checked;
        entry.enabled = enabled;
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
            Entry & entry = AddEntry(name, ::wxNewId(), CmdManager::Type_Normal, nullptr, false, true);
            return entry.id;
        }
        return iter->second->m_entry.id;
    }


    // Retreave registered control kind
    virtual Entry * FindEntry (const wxString & name)
    {
        auto iter = m_map.find(name);
        if (iter == m_map.end())
        {
            return nullptr;
        }
        return &iter->second->m_entry;
    }


    // retreave entry by registered numeric ID
    virtual Entry * FindEntry (int id)
    {
        auto iter = m_idMap.find(id);
        if (iter == m_idMap.end())
        {
            return nullptr;
        }
        return &iter->second->m_entry;
    }


    // get entry or create a default one
    virtual Entry & GetEntry (const wxString & name)
    {
        auto iter = m_map.find(name);
        if (iter == m_map.end())
        {
            wxLogWarning("Id '%s' is not registered with CmdManager", name);
            return AddEntry(name, ::wxNewId(), CmdManager::Type_Normal, nullptr, false, true);
        }
        return iter->second->m_entry;
    }


    // register id
    virtual  void Register (const wxString & name, int id, CmdManager::Type type, wxObject * object, bool checked, bool enabled)
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
        AddEntry(name, id, type, object, checked, enabled);
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
        m_idMap.erase(iter->second->m_entry.id);
        m_map.erase(iter);
    }


    // toggle the checkbox
    virtual void Check (const wxString & name, bool state)
    {
        _check(FindEntry(name), state);
    }


    // toggle checkbox identified by numeric ID
    virtual void Check (int id, bool state)
    {
        _check(FindEntry(id), state);
    }


    // toggle the item
    void _check(Entry * entry, bool state)
    {
        if (entry == nullptr) return;
        if (entry->checked == state || entry->type != Type_Check) return;
        entry->checked =  state;
        wxCommandEvent event(fbiCMD_CHECK, entry->id);
        event.SetInt( state );
        ProcessEvent(event);
    }


    // enable / disable item
    virtual void Enable (const wxString & name, bool state)
    {
        _enable(FindEntry(name), state);
    }


    // toggle checkbox identified by numeric ID
    virtual void Enable (int id, bool state)
    {
        _enable(FindEntry(id), state);
    }


    // toggle the item
    void _enable(Entry * entry, bool state)
    {
        if (entry == nullptr) return;
        if (entry->enabled == state) return;
        entry->enabled =  state;
        wxCommandEvent event(fbiCMD_ENABLE, entry->id);
        event.SetInt( state );
        ProcessEvent(event);
    }


    // Internal data structure for entries
    struct InternalEntry
    {
        Entry       m_entry;    // the public data entry
    };


    // data
    typedef std::shared_ptr<InternalEntry> EntryPtr;
    HashMap<EntryPtr> m_map;                    // hold name id pairs
    std::unordered_map<int, EntryPtr> m_idMap;  // hold association by numeric id
};


// Implement Manager
IMPLEMENT_MANAGER(CmdManager, TheCmdManager)