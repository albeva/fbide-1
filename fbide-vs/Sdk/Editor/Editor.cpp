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
#include "sdk_pch.h"
#include "../Manager.h"
#include "../UiManager.h"
#include "../MultiSplitter.h"
#include "Editor.h"


using namespace fbi;

const int ID_SplitVertically = ::wxNewId();
const int ID_SplitHorizontally = ::wxNewId();

class fbi::StcEditor : public wxStyledTextCtrl
{
public:
    StcEditor(int index, Editor * owner, wxWindow * parent, int id)
    {
        Create(parent, id);
        UsePopUp(false);
    }

    // on right mouse button
    void OnMouseRight (wxMouseEvent & event)
    {
        wxMenu menu;
        menu.Append(ID_SplitVertically,     "Split Vertically");
        menu.Append(ID_SplitHorizontally,   "Split Horizontally");
        PopupMenu(&menu);
    }

   
    // destructor
    ~StcEditor() {}
    
    // receave events
    DECLARE_EVENT_TABLE()
};

BEGIN_EVENT_TABLE(StcEditor, wxStyledTextCtrl)
    EVT_RIGHT_UP(StcEditor::OnMouseRight)
END_EVENT_TABLE()



/**
 * Event table for documents
 */
BEGIN_EVENT_TABLE(Editor, Document)
    EVT_MENU(ID_SplitVertically, Editor::OnSplitVertically)
    EVT_MENU(ID_SplitHorizontally, Editor::OnSplitHorizontally)
END_EVENT_TABLE()

#define NEW_STC(index, parent) \
    m_editors[index] = new StcEditor(index, this, m_splitter, wxID_ANY); \
    if (parent != nullptr) \
        m_editors[index]->SetDocPointer(parent->GetDocPointer()); \
    m_splitter->ShowWindow(index, m_editors[index]);

/**
 * Create new editor
 */
Editor::Editor ()
{
    // Create the panel
    m_panel = new wxPanel(GET_UIMGR()->GetDocumentArea(), wxID_ANY);
    SetDocWindow(m_panel);
    m_panel->PushEventHandler(this);

    // teh main sizer
    m_sizer = new wxBoxSizer(wxVERTICAL);
    m_panel->SetSizer(m_sizer);

    // multisizer to contain the editors
    int style = wxSP_3DSASH | wxSP_LIVE_UPDATE | wxCLIP_CHILDREN;
    m_splitter = new MultiSplitWindow(m_panel, wxID_ANY, wxDefaultPosition, wxDefaultSize, style);
    m_sizer->Add(m_splitter, 1, wxGROW, 5);

    // clear
    memset(m_editors, 0, sizeof(m_editors));

    // create default editor
    NEW_STC(0, ((StcEditor*)nullptr))


}


// do a vertical split/unsplit
void Editor::OnSplitVertically(wxCommandEvent & event)
{
    NEW_STC(1, m_editors[0])
    // NEW_STC(2, m_editors[0])
    // NEW_STC(3, m_editors[0])
}

// do a horizontal split/unsplit
void Editor::OnSplitHorizontally(wxCommandEvent & event)
{
    NEW_STC(2, m_editors[0])
    NEW_STC(3, m_editors[0])
}