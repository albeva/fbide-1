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
    
    /**
     * Base class for all documents
     */
    class SDK_DLL Document : public wxEvtHandler
    {
        public :
            
            /**
             * Create new document
             */
            Document();


            /**
             * Destroy document
             */
            virtual ~Document();

  
            /**
             * Load file in this document
             */
            virtual bool LoadDocFile (const wxString & file) { return false; }


            /**
             * Set document file
             */
            virtual void SetDocFilename (const wxString & file) { m_filename = file; }


            /**
             * Get document file
             */
            virtual const wxString & GetDocFilename () const { return m_filename; }


            /**
             * return wxWindow pointer if this
             * document has a window that can be
             * placed into tabbed document area.
             */
            virtual wxWindow * GetDocWindow () const { return m_window; }


            /**
             * Set document window ptr
             */
            void SetDocWindow (wxWindow * wnd) { m_window = wnd; }


            /**
             * Title is used on the frame window and
             * other plaves where file have to be
             * identified by name.
             */
            virtual const wxString & GetDocTitle() const { return m_title; }


            /**
             * Setname
             */
            virtual void SetDocTitle (const wxString & name);


            /**
             * return unique document id
             */
            inline int GetDocId () const { return m_id; }


        protected:

            int         m_id;       // document unique ID
            wxString    m_title;    // document title
            wxString    m_filename; // filename associated with teh document
            wxWindow *  m_window;   // document window ptr
    };

}
