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

    // Forward reference
    class Document;

    // Typedef for document creator
    typedef Document * (*DocCreatorFn)( );

    // Create TypeCreator method
    // DocumentCreator<MyDocument>
    template<class T> Document * DocumentCreator( )
    {
        return new T();
    }

    /**
     * Manage file types and associated loaders
     * and construct load / save dialogs
     */
    class SDK_DLL TypeManager : private NonCopyable
    {
    public:

        // Register new type with fbide.
        virtual void Register (const wxString & type, 
                               const wxString & desc,
                               DocCreatorFn creator ) = 0;
        
        // Bind file extensions to the type
        virtual void BindExtensions (const wxString & type,
                                     const wxString & extensions) = 0;

        // Bind type alias to an existing target type or alias
        // optionally allow overwriting if alias already exists
        virtual void BindAlias (const wxString & alias,
                                const wxString & target,
                                bool overwrite = false ) = 0;
        
        // Create new document. Deduct type from extension
        // if failed show a dialog with an option to open the file
        // with any other registered loaders or use OS to open it
        // return nullptr if not opened in the editor
        virtual Document * CreateFromFile (const wxString & file) = 0;


        // Create new document using type name. If not registred
        // return nullptr and log a warning.
        virtual Document * CreateFromType (const wxString & type) = 0;

        // Create new document by registered extenions.
        // if multiple creators are registered show a dialog allowing
        // to select the proper type.
        // return nullptr and log a warning if no extension is registered
        // with the type
        virtual Document * CreateFromExtension (const wxString & ext) = 0;

        // check if type is registered. Will resolve aliases to the 
        // actual type
        virtual bool IsRegistered ( const wxString & type ) = 0;

        // Get file filters to be used with file load / save dialogs
        virtual wxString GetFileFilters ( bool incAllFiles ) = 0;

        // declare this class as a manager
        DECLARE_MANAGER(TypeManager)
    };

}
