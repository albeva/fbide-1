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

#ifndef DOCUMENT_H_INCLUDED
#define DOCUMENT_H_INCLUDED

namespace fb
{

    /**
     * document types
     */
    enum DocumentType
    {
        DOCUMENT_MANAGED,   // is loaded into document tab area (text, rad, ...)
        DOCUMENT_CUSTOM     // any collection document (non tab area) projects, sessions, ...
    };


    /**
     * Standard document events
     */
    namespace DocEvent
    {
        const int DESTROY   = 1;
        const int RENAME    = 2;
        const int FOCUS     = 3;
    };


    /**
     * Base class for Documents
     *
     * In FBIde Dcoument is an abstract entity that in most
     * basic sense handles the file types accordingly.
     * For instance text/source files are handles in texteditor
     * while project or session files open other documents and so o
     */
    class DLLIMPORT CDocumentBase
    {
        int m_id;
        int m_type;
        wxString m_title;
        wxString m_file;
        static int NewId()
        {
            static int cnt = 0;
            return ++cnt;
        }

        public :
            CDocumentBase(DocumentType type) : m_id(NewId()), m_type(type) {}
            virtual ~CDocumentBase() { SendDocEvent(DocEvent::DESTROY); }

            /**
             * this function is used to lookup if document is able to perform
             * tasks specified by id (exID_SAVE - can it save or wxID_REDO - can it redo)
             */
            virtual bool CanDocDo (int evtId) { return false; }


            /**
             * Save document
             */
            virtual bool SaveDoc (const wxString & file) { return false; }


            /**
             * Load file in this document
             */
            virtual void LoadDoc (const wxString & file) { return; }


            /**
             * Set document file
             */
            virtual void SetDocFile (const wxString & file) { m_file = file; }


            /**
             * Get document file
             */
            const wxString & GetDocFile () const { return m_file; }


            /**
             * return wxWindow pointer if this
             * document has a window that can be
             * placed into tabbed document area.
             */
            virtual wxWindow * GetDocWindow () { return 0L; }


            /**
             * Title is used on the frame window and
             * other plaves where file have to be
             * identified by name.
             */
            virtual const wxString & GetDocTitle()
            {
                if (!m_title.Len()) SetDocTitle (_T(""));
                return m_title;
            }


            /**
             * Setname
             */
            virtual void SetDocTitle (const wxString & name);


            /**
             * return unique document id
             */
            inline int GetDocId () const { return m_id; }


            /**
             * return type of the document
             */
            inline int GetDocType () const { return m_type; }


            /**
             * Event map
             */
            typedef std::map<int, CMultiDelegate<void(CDocumentBase * doc, int evt)> > DocEvtMap;
            DocEvtMap evt;


            /**
             * send document event
             */
            void SendDocEvent (int event);
    };


    /**
     * Extender template for document types
     */
    template <class T, DocumentType TYPE = DOCUMENT_CUSTOM> class CDocument : public CDocumentBase, public T
    {
        public :
            /**
             * set document type as well
             */
            CDocument() : CDocumentBase(TYPE)
            {
                // check if this is a managable document
                if (TYPE == DOCUMENT_MANAGED)
                {
                    wxASSERT(dynamic_cast<wxWindow *>(this) != NULL);
                }
            }


            /**
             * return pointer to wxWindow object
             * of the document
             */
            virtual wxWindow * GetDocWindow () { return dynamic_cast<wxWindow *>(this); }
    };

}

#endif // DOCUMENT_H_INCLUDED
