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

#ifndef UIMANAGER_H_INCLUDED
#define UIMANAGER_H_INCLUDED

namespace fb
{

    /**
     * forward references
     */
    class IArtProvider;
    class CManager;
    class CDocumentBase;
    class CUiToolDescriptor;


    /**
     * User interface manager handles menus, toolbars
     * floatable / dockable panels and document views
     * in abstract way. It generates some UI interaction
     * specific logic and event handling
     *
     * Can be created only via CManager class.
     */
    class DLLIMPORT CUiManager
    {

        public :

            enum
            {
                PANE_TARGET_MANAGEMENT = 1,
                PANE_TARGET_UTILS
            };

            // Register new tool descriptor
            virtual void AddToolDescriptor (const wxString & name, CUiToolDescriptor * toolDesc) = 0;

            // get doucment "parent" window
            virtual wxWindow * GetDocWindow () = 0;

            // load layout specified by xml
            virtual void LoadLayout (const wxString & file) = 0;

            // Clean up ui before application close
            virtual void Unload () = 0;

            // Get menu with given name
            virtual wxMenu * GetMenu (const wxString & name) = 0;

            // Add new menu into system
            virtual void AddMenu (const wxString & name, wxMenu * menu, bool show = false) = 0;

            // Get toolbar by name
            virtual wxAuiToolBar * GetToolBar (const wxString & name) = 0;

            // add new toolbar
            virtual void AddToolBar (const wxString & name, wxAuiToolBar * toolbar, bool show = false) = 0;

            // Add document into Document view
            virtual void AddDocument (CDocumentBase * doc, bool show) = 0;

            // Get currently active document or return NULL if noen
            virtual CDocumentBase * GetActiveDocument () = 0;

            // Add pane
            virtual bool AddPane (const wxString & id, wxWindow * wnd, const wxString & group) = 0;

            // Get parent for the pane area
            virtual wxWindow * GetPaneWindow (const wxString & group) = 0;

            // Get pane by id
            virtual wxWindow * GetPane (const wxString & id) = 0;

            // remove pane
            virtual void RemovePane (const wxString & id) = 0;

            // Remove document
            virtual void RemoveDocument (CDocumentBase * doc) = 0;

            // Set art provider
            virtual void SetArtProvider (IArtProvider * provider) = 0;

            // Get art provider
            virtual IArtProvider * GetArtProvider () = 0;

            // Load the UI (finalize loading process)
            virtual void Load () = 0;

            // Registerable callback for ArtProviderChange event
            CMultiDelegate<void(IArtProvider *)> onArtProvderChange;

        private :

            friend class CManager;
            static CUiManager * Get ();
            static void Free ();

        protected :

            virtual ~CUiManager() {}
    };

}

#endif // UIMANAGER_H_INCLUDED
