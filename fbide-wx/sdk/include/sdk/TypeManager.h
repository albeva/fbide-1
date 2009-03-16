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

#ifndef TYPEMANAGER_H_INCLUDED
#define TYPEMANAGER_H_INCLUDED

namespace fb
{

    /**
     * Forward declarations...
     */
    class CDocumentBase;
    class CManager;
    class CTypeManager;


    /**
     * Base class for type registrants
     */
    class DLLIMPORT CTypeRegistrantBase
    {
        public:
            virtual CDocumentBase * Create (const wxString & file = _T("")) = 0;
    };



    /**
     * Template class for registering new document types
     * siply use :
     *     new CTypeRegistrant<classNme>;
     */
    template<class T> class CTypeRegistrant : public CTypeRegistrantBase
    {
        public : virtual CDocumentBase * Create (const wxString & file = _T("")) { return new T(file); }
    };


    /**
     * Manager document types. register new types by name
     * then associate file extensions and also generate
     * file filter list for open / save dialogs
     *
     * later this class could also handle registering
     * (optionally) said file types with the OS to be
     * loaded by FBIde
     */
    class DLLIMPORT CTypeManager
    {
        public :
            void AddType (const wxString & name, const wxString & desc, CTypeRegistrantBase * type);
            void BindExtensions (const wxString & type, const wxString & types);
            void BindAlias (const wxString & alias, const wxString & type);

            CDocumentBase * LoadFile (const wxString & file);
            CDocumentBase * NewDocument (const wxString & type);

            bool TypeExists (const wxString & type);

            wxString GetFileFilters (bool allFiles = false);

        private :
            friend class CManager;
            CTypeManager ();
            ~CTypeManager ();

            // private data implementation
            struct CData;
            std::auto_ptr<CData> m_data;
    };

}

#endif // TYPEMANAGER_H_INCLUDED
