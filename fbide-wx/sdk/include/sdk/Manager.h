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

#ifndef MANAGER_H_INCLUDED
#define MANAGER_H_INCLUDED

#define GET_MGR()       fb::CManager::Get()
#define GET_UIMGR()     fb::CManager::Get()->GetUiManager()
#define GET_TYPEMGR()   fb::CManager::Get()->GetTypeManager()
#define GET_DOCMGR()    fb::CManager::Get()->GetDocManager()
#define GET_PLUGINMGR() fb::CManager::Get()->GetPluginManager()
#define GET_LANG()      fb::CManager::Get()->GetLang()
#define GET_REG()       fb::CManager::Get()->GetReg()
#define GET_EDITORMGR() fb::CManager::Get()->GetEditorManager()

#include "sdk/Registry.h"

namespace fb
{

    /**
     * Forward declarations
     */
    class CUiManager;
    class CTypeManager;
    class CDocManager;
    class CPluginManager;
    class CStyleParser;
    class CEditorManager;


    /**
     * Manager is entry to FBIde SDK
     */
    class DLLIMPORT CManager
    {
        public :
            // Get and release the instance
            static CManager * Get ();

            // Release (and clean up) managers
            static void Free ();

            // language and registry are managed by
            // CManager as well.
            CRegistry & GetReg ();

            // Language
            const wxString & GetLang (const wxString & name);

            // ID - name relation
            void RegisterId (const wxString & name, int id);
            int GetId (const wxString & name);

            // wxFrame handling
            void SetFrame (wxFrame * frame);
            wxFrame * GetFrame ();

            // managers
            CUiManager      * GetUiManager();
            CTypeManager    * GetTypeManager ();
            CDocManager     * GetDocManager ();
            CPluginManager  * GetPluginManager ();
            CStyleParser    * GetStyleParser (const wxString & name);
            CEditorManager  * GetEditorManager ();

            void SetStyleParser (const wxString & name, CStyleParser * parser);

            // config and lang related
            void LoadConfig (const wxString & file);
            void SaveConfig (const wxString & file);
            void LoadLang (const wxString & file);

            void SetupPalette (const wxString & filename);

            bool IsShuttingDown () { return m_shutdown; }

        private :
            CManager ();
            ~CManager ();

            // disable copying
            void operator = (CManager &) {};

            // private data implementation "pimpl"
            struct CData;
            std::auto_ptr<CData> m_data;
            bool m_shutdown;
    };

}

#endif // MANAGER_H_INCLUDED
