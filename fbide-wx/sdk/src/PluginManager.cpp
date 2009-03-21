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
#include "sdk/Manager.h"
#include "sdk/PluginManager.h"

using namespace fb;

/**
 * Create new plugin object
 */
CPluginBase::CPluginBase ()
    : m_provider(NULL), m_isAttached(false)
{
}


/**
 * Is plugin attached ?
 */
bool CPluginBase::IsAttached ()
{
    return m_isAttached;
}


/**
 * Set "attached" state
 */
void CPluginBase::SetAttached (bool attached)
{
    m_isAttached = attached;
}


/**
 * Get plugin type
 */
PluginType CPluginBase::GetType ()
{
    return m_provider->GetType();
}


/**
 * Get plugin id name
 */
wxString CPluginBase::GetName()
{
    return m_provider->GetName();
}


/**
 * Unload the plugin
 *
 * This has to be postponed till execution
 * leaved the plugin. Otherwise crash will happen.
 * utilize IDLE event?
 */
void CPluginBase::Unload()
{
    if (!IsAttached()) return;
    GET_PLUGINMGR()->Unload (this, true);
}


/**
 * Set the registrant
 */
void CPluginBase::SetProvider (CPluginProviderBase * provider)
{
    m_provider = provider;
}


/**
 * Get the registrant
 */
CPluginProviderBase * CPluginBase::GetProvider ()
{
    return m_provider;
}





















class CPluginManager::CData
{
    public:

        typedef std::map<wxString, CPluginProviderBase*> CPluginRegistry;
        CPluginRegistry m_providers;

};



/**
 * Create the instance
 */
CPluginManager::CPluginManager () : m_data(new CData())
{
}


/**
 * release the instance
 */
CPluginManager::~CPluginManager ()
{
    UnloadAll ();
}


/**
 * Unload all plugins
 */
void CPluginManager::UnloadAll ()
{
    typedef CData::CPluginRegistry::iterator Iterator;
    for (Iterator iter = m_data->m_providers.begin();
         iter != m_data->m_providers.end();
         iter = m_data->m_providers.begin())
    {
        CPluginProviderBase * provider = iter->second;
        Unload(provider->GetName(), true);
    }
}


/**
 * Notify all plugins about pending shutdown
 *
 * NB! This shoudl be called:
 *     - when fbide closes ( doen )
 *     - when plugin is about to be unloaded at runtime
 *       ensure that this is called only once!
 */
void CPluginManager::NotifyUnload ()
{
    CData::CPluginRegistry::iterator iter = m_data->m_providers.begin();
    for ( ; iter != m_data->m_providers.end(); iter++ )
    {
        iter->second->GetPlugin()->NotifyExit();
    }
}


/**
 * Register a new plugin
 */
void CPluginManager::AddProvider (const wxString & name, CPluginProviderBase * provider)
{
    m_data->m_providers[name] = provider;
}



/**
 * Load plugin file only. Will not instantiate
 */
bool CPluginManager::LoadPluginFile (const wxString & name)
{
    // vars
    CPluginProviderBase * provider;

    // check if already loaded ?
    if (m_data->m_providers[name]) return true;

    // normalize filename
    wxFileName file = wxDynamicLibrary::CanonicalizeName(GET_REG()[_T("dir.plugins")].AsString() + name, wxDL_MODULE);
    file.Normalize();

    // check file exists
    if (!file.FileExists())
    {
        wxLogError(_T("Plugin file '%s' not found"), file.GetFullPath().c_str());
        return false;
    }

    // load dll
    wxDynamicLibrary * dll = new wxDynamicLibrary(file.GetFullPath());
    if (!dll->IsLoaded())
    {
        wxLogError(_T("Could no load plugin '%s'"), name.c_str());
        delete dll;
        return false;
    }

    // check registrant
    provider = m_data->m_providers[name];
    if (!provider)
    {
        wxLogError(_T("No plugin provider for '%s' found"), name.c_str());
        delete dll;
        return false;
    }

    // register dll to registrant
    provider->SetDll(dll);

    // everything ok...
    return true;
}



/**
 * Get plugin instance by id
 */
CPluginBase * CPluginManager::GetPlugin (const wxString & name)
{
    CPluginProviderBase * provider = m_data->m_providers[name];
    if (!provider) return NULL;
    return provider->GetPlugin();
}


/**
 * Load plugin by name.
 */
CPluginBase * CPluginManager::LoadPlugin (const wxString & name)
{
    CPluginBase             * plugin;
    CPluginProviderBase   * provider;

    // check id file is loaded ?
    if (!LoadPluginFile(name)) return NULL;
    provider = m_data->m_providers[name];

    // Chech if plugin is already instantiated?
    if (provider->IsLoaded()) return provider->GetPlugin();

    // Get plugin instance
    plugin = provider->Create();
    if (!plugin)
    {
        // Some very wierd failure... ?
        wxLogError(_T("Failed to create plugin instance '%s'"), name.c_str());
        Unload(name, true);
        return NULL;
    }

    // set provider to the plugin
    plugin->SetProvider (provider);

    // Attach plugin
    if (!plugin->Attach())
    {
        Unload(name, true);
        return NULL;
    }

    // Final setup
    plugin->SetAttached(true);

    // and done
    return plugin;
}



/**
 * Unload the plugin
 */
bool CPluginManager::Unload (CPluginBase * plugin, bool force)
{
    return Unload(plugin->GetName(), force);
}


/**
 * Unload by plugin id
 */
bool CPluginManager::Unload (const wxString & name, bool force)
{
    CPluginProviderBase * provider = m_data->m_providers[name];
    if (!provider) return true;

    // release the plugin
    if (provider->IsLoaded())
    {
        CPluginBase * plugin = provider->GetPlugin();
        if (!plugin->Detach(force) && !force) return false;
        provider->Release();
    }

    // delete dll
    wxDynamicLibrary * dll = provider->GetDll();
    if (dll) delete dll;

    // remove provider
    m_data->m_providers.erase(m_data->m_providers.find(name));
    // plugin should use static object ??? delete provider;

    return true;
}

