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

#ifndef PLUGINMANAGER_H_INCLUDED
#define PLUGINMANAGER_H_INCLUDED

namespace fb
{

    // fwd ref
    class CPluginProviderBase;
    class CPluginManager;


    /**
     * To ease differentation between plugins...
     */
    enum PluginType
    {
        PLUGINTYPE_ARTPROVIDER,
        PLUGINTYPE_EDITOR,
        PLUGINTYPE_TOOL,
        PLUGINTYPE_WIZARD
    };

    /**
     * Base class for plugins
     */
    class DLLIMPORT CPluginBase
    {

        // allow PluginManager to access private bits
        friend class CPluginManager;

        // Get provider
        CPluginProviderBase * GetProvider ();

        // Set provider
        void SetProvider (CPluginProviderBase *);

        // Set this plugin to be "attached"
        void SetAttached (bool attached);


        public :

            // Create new plugin instance (should be a template?)
            CPluginBase ();

            // destroy
            virtual ~CPluginBase() {}

            // Get type
            PluginType GetType ();

            // Get plugin name
            wxString GetName ();

            // Is attached
            bool IsAttached ();

            // Unload this plugin (should be used with care)
            void Unload ();

            // Call on attach
            virtual bool Attach () = 0;

            // And when detached
            virtual bool Detach (bool force) { return true; };

        private :

            CPluginProviderBase   * m_provider;
            bool                    m_isAttached;

    };


    /**
     * Base class for plugin registrants
     */
    class DLLIMPORT CPluginProviderBase
    {

        public :

            // Get plugin instance (if loaded)
            CPluginBase * GetPlugin () { return m_plugin; }

            // get plugin type
            PluginType GetType () { return m_type; }

            // get sdk version
            virtual int GetSDKVersion() = 0;

            // Get plugin name (registration name)
            wxString GetName () { return m_name; }

            // Is loaded ?
            bool IsLoaded () { return m_plugin != NULL; };


        protected :

            // Allow plugin manager to access private bits
            friend class CPluginManager;

            PluginType          m_type;
            wxString            m_name;
            CPluginBase         * m_plugin;
            wxDynamicLibrary    * m_dll;

            // Create plugin registrant object
            CPluginProviderBase (PluginType type, const wxString & name)
                : m_type(type), m_name(name), m_plugin(NULL), m_dll(NULL)
            {}

            // clean up...
            virtual ~CPluginProviderBase () {}

            // Get plugin instance (if loaded)
            virtual CPluginBase * Create () = 0;

            // Destroy plugin
            void Release ()
            {
                if (m_plugin == NULL) return;
                delete m_plugin;
                m_plugin = NULL;
            }

            // Set the DLL handle
            void SetDll (wxDynamicLibrary * dll)
            {
                m_dll = dll;
            }

            // Get Dll
            wxDynamicLibrary * GetDll ()
            {
                return m_dll;
            }

    };



    /**
     * This class manages the plugins
     *
     * Can be accessed via CManager
     */
    class DLLIMPORT CPluginManager
    {
        public :

            // Register new plugin provider
            void AddProvider (const wxString & name, CPluginProviderBase * registrant);

            // Load plugin by ID. (fbide/ide/plugins is default folder)
            CPluginBase * LoadPlugin (const wxString & name);

            // Will only load dll but not instantiate the plugin
            bool LoadPluginFile (const wxString & file);

            // Get plugin instance (if loaded)
            CPluginBase * GetPlugin (const wxString & name);

            // Unload the plugin.
            bool Unload (CPluginBase * plugin, bool force);
            bool Unload (const wxString & name, bool force);

        private :
            friend class CManager;
            CPluginManager ();
            ~CPluginManager ();

            struct CData ;
            std::auto_ptr<CData> m_data;
    };



    /**
     * Template used for registaring ny plugins easily and hasslefree.
     * namespace
     * {
     *     CPluginProvider<PluginClass, PLUGINTYPE> plugin(_T("PluginName"));
     * }
     */
    template<class T, PluginType type> class CPluginProvider : public CPluginProviderBase
    {
        public:
            CPluginProvider(const wxString& name) : CPluginProviderBase(type, name)
            {
                CManager::Get()->GetPluginManager()->AddProvider(name, this);
            }

            /**
             * Destroy
             */
            virtual ~CPluginProvider() {}

            /**
             * Get plugin instance
             */
            virtual CPluginBase * Create ()
            {
                if (m_plugin == NULL) m_plugin = new T ();
                return m_plugin;
            }


            /**
             * Get version
             */
            virtual int GetSDKVersion()
            {
                return (SDK_VERSION_MAJOR * 1000) + (SDK_VERSION_MINOR * 100) + SDK_VERSION_RELEASE;
            }

    };

}

#endif // PLUGINMANAGER_H_INCLUDED
