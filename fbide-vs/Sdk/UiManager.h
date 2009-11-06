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
#pragma once

namespace fbi
{

    class Manager;
    class IArtProvider;

    /**
     * Main manager class. This is aproxy class that holds
     * the instances and other bookkeeping of the SDK
     * and should be used to access the SDK API
     *
     * This class is a singleton
     */
    class SDK_DLL UiManager : private NonCopyable
    {
        public:

        // Load ui. return false if failed.
        virtual bool Load () = 0;

        // Load the layout defination
        virtual void LoadLayout (const wxString & file) = 0;

        // Get application title
        virtual wxString GetTitle () = 0;

        // get the main frame
        virtual wxFrame * GetFrame () = 0;

        // get document area location
        virtual wxWindow * GetDocumentArea () = 0;

        // Add document
        virtual void AddDocument(wxWindow * wnd, const wxString & title) = 0;

        // Set art provider
        virtual void SetArtProvider (IArtProvider * provider) = 0;

        // get art provider
        virtual IArtProvider * GetArtProvider () = 0;

        // declare this class as a manager
        DECLARE_MANAGER(UiManager)
    };

}
