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

#ifndef DOCMANAGER_H_INCLUDED
#define DOCMANAGER_H_INCLUDED

namespace fb
{

    /**
     * Forward references
     */
    class CManager;
    class CDocumentBase;


    /**
     * This manager handles the open documens.
     * Keeps a list, forwards the save, open and etc
     * events. Does not handle document specific logic.
     *
     * Can be accessed only through CManager class
     */
    class DLLIMPORT CDocManager : public wxEvtHandler
    {
        public :
            bool CloseAll ();
            CDocumentBase * FindDocument (wxWindow * wnd);
            CDocumentBase * Open (const wxString & filename, bool show = true);
            void Open (const wxArrayString & filenames);


        private :
            friend class CManager;
            CDocManager ();
            ~CDocManager ();

            struct CData;
            CData * m_data;
    };

}

#endif // DOCMANAGER_H_INCLUDED
