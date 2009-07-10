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
#include "sdk/DocManager.h"
#include "sdk/UiManager.h"
#include "sdk/TypeManager.h"
#include "sdk/Document.h"

using namespace fb;

extern const int ID_DOCUMENT_NOTEBOOK;



struct CDocManager::CData : wxEvtHandler
{
    // event handlers
    void OnNew (wxCommandEvent & event);
    void OnOpen (wxCommandEvent & event);
    void OnDocumentClosed (CDocumentBase * doc, int evt);


    /**
     * Any command event that shoudl be routed to the
     * document
     */
    void OnMenuEvent (wxCommandEvent & event)
    {
        // Find active document
        CDocumentBase * doc = GET_UIMGR()->GetActiveDocument();
        if (doc == NULL) return;

        wxWindow * wnd = dynamic_cast<wxWindow *>(doc);
        wxASSERT(wnd != NULL);

        wnd->wxEvtHandler::ProcessEvent(event);
    }


    /**
     * Save document
     */
    void OnSave (wxCommandEvent & event)
    {
        CDocumentBase * doc = GET_UIMGR()->GetActiveDocument();
        if (doc == NULL) return;

        wxString file = doc->GetDocFile();

        // if doc has a file then ASSUME that this file BELONGS
        // to the doc and just save the file.
        if (file.Len())
        {
            doc->SaveDoc(file);
            return;
        }

        // managers
        CUiManager      & ui = *GET_UIMGR();
        CTypeManager    & tm = *GET_TYPEMGR();
        CManager        & mgr = *GET_MGR();

        //
        wxFileDialog dlg (
            0L,                             // parent window
            mgr.GetLang(_T("save-file")),   // title
            _T(""), _T(""),                 // default dir, default file
            tm.GetFileFilters(true),        // wildcards
            wxFD_SAVE |wxFD_OVERWRITE_PROMPT
        );

        if (dlg.ShowModal() != wxID_OK) return;

        // get file. Check if we are over writing it?
        file = dlg.GetPath();
        doc->SaveDoc(file);
        doc->SetDocFile(file);
    }


    // members
    typedef std::vector<CDocumentBase *> docList;
    docList m_docList;

    CDocManager * m_parent;

    // we process events here
    DECLARE_EVENT_TABLE();
};


/**
 * Event table
 */
BEGIN_EVENT_TABLE(CDocManager::CData, wxEvtHandler)
    EVT_MENU(wxID_NEW,  CDocManager::CData::OnNew)
    EVT_MENU(wxID_OPEN, CDocManager::CData::OnOpen)

    EVT_MENU(wxID_UNDO, CDocManager::CData::OnMenuEvent)
    EVT_MENU(wxID_REDO, CDocManager::CData::OnMenuEvent)
    EVT_MENU(wxID_CUT,  CDocManager::CData::OnMenuEvent)
    EVT_MENU(wxID_COPY, CDocManager::CData::OnMenuEvent)
    EVT_MENU(wxID_PASTE,CDocManager::CData::OnMenuEvent)

    EVT_MENU(wxID_SAVE, CDocManager::CData::OnSave)
END_EVENT_TABLE()



/**
 * create new default document
 */
void CDocManager::CData::OnNew (wxCommandEvent & event)
{
    CUiManager      & ui = *GET_UIMGR();
    CTypeManager    & tm = *GET_TYPEMGR();
    CManager        & mgr = *GET_MGR();

    // Freeze stops updating the window.
    // so stop updating until all is properly loaded
    mgr.GetFrame()->Freeze();

    wxString type = _T("default");

    CDocumentBase * doc = tm.NewDocument(type);
    if (doc == 0L)
    {
        mgr.GetFrame()->Thaw();
        return; //throw EXCEPTION(_T("Document type '") + type + _T("' is not registered"));
    }
    m_docList.push_back(doc);

    // destruction event (for cleanup)
    doc->evt[DocEvent::DESTROY] += MakeDelegate(this, &CDocManager::CData::OnDocumentClosed);

    // if it's a content document add to the document area
    if (doc->GetDocType() == DOCUMENT_MANAGED)
    {
        if (doc) ui.AddDocument(doc, true);// doc->GetDocWindow(), doc->GetDocTitle(), true);
    }

    mgr.GetFrame()->Thaw();
}



/**
 * show open dialog
 */
void CDocManager::CData::OnOpen (wxCommandEvent & event)
{
    CTypeManager    & tm = *GET_TYPEMGR();
    CManager        & mgr = *GET_MGR();

    wxFileDialog dlg (
        0L,                             // parent window
        mgr.GetLang(_T("load-file")),   // title
        _T(""), _T(""),                 // default dir, default file
        tm.GetFileFilters(true),        // wildcards
        wxFD_OPEN | wxFD_FILE_MUST_EXIST | wxFD_MULTIPLE
    );
    if (dlg.ShowModal() == wxID_OK)
    {
        wxArrayString paths;
        dlg.GetPaths(paths);
        m_parent->Open(paths);
    }
}


/**
 *
 */
void CDocManager::CData::OnDocumentClosed (CDocumentBase * doc, int evt)
{
    docList::iterator iter = m_docList.begin();
    while (iter != m_docList.end())
    {
        if (*iter == doc)
        {
            m_docList.erase(iter);
            return;
        }
        iter++;
    }
}



/**
 * Create instance
 */
CDocManager::CDocManager () : m_data(new CDocManager::CData)
{
    m_data->m_parent = this;

    wxFrame * frame = GET_MGR()->GetFrame();
    if (frame == 0L) throw EXCEPTION(_T("UiManager must be initalized and have a frame before creating DocManager!"));

    // add this manager to frame event handlers list
    frame->PushEventHandler(m_data);
}



/**
 * Destroy and do cleanup
 */
CDocManager::~CDocManager()
{
    // GET_MGR()->GetFrame()->RemoveEventHandler(m_data);
    delete m_data;
}


/**
 * Find document by a window pointer
 */
CDocumentBase * CDocManager::FindDocument (wxWindow * wnd)
{
    //if (wnd == NULL) return NULL;
    wxASSERT(wnd != NULL);

    CData::docList::iterator iter = m_data->m_docList.begin();
    while (iter != m_data->m_docList.end())
    {
        CDocumentBase * doc = *iter;
        if (doc->GetDocWindow() == wnd) return doc;
        iter++;
    }

    // recursive
    return FindDocument(wnd->GetParent());
}


/**
 * Open a single file
 */
CDocumentBase * CDocManager::Open (const wxString & filename, bool show)
{
    CUiManager      & ui = *GET_UIMGR();
    CTypeManager    & tm = *GET_TYPEMGR();
    // CManager        & mgr = *GET_MGR();

    CDocumentBase * doc = tm.LoadFile(filename);
    if (doc == 0L) return NULL;

    m_data->m_docList.push_back(doc);

    // destruction event (for cleanup)
    doc->evt[DocEvent::DESTROY] += MakeDelegate(m_data, &CDocManager::CData::OnDocumentClosed);

    // if it's a content document add to the document area
    if (doc->GetDocType() == DOCUMENT_MANAGED)
    {
        if (doc) ui.AddDocument(doc, show);
    }

    return doc;
}


/**
 * Open several files
 */
void CDocManager::Open (const wxArrayString & filenames)
{
    //CUiManager      & ui = *GET_UIMGR();
    //CTypeManager    & tm = *GET_TYPEMGR();
    CManager        & mgr = *GET_MGR();

    mgr.GetFrame()->Freeze();

    for (size_t i = 0; i < filenames.Count(); i++)
    {
        Open(filenames[i], i == 0);
    }

    mgr.GetFrame()->Thaw();
}


/**
 * Close all opent documents.
 * Each doc might ask SAVE
 *
 * @return success
 */
bool CDocManager::CloseAll ()
{
    CUiManager * ui = CManager::Get()->GetUiManager();

    CData::docList::iterator iter = m_data->m_docList.begin();
    while (iter != m_data->m_docList.end())
    {
        CDocumentBase * doc = *iter;
        iter = m_data->m_docList.erase(iter);

        ui->RemoveDocument(doc);
        delete doc;
    }
    return true;
}
