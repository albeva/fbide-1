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
#include "Manager.h"
#include "UiManager.h"
#include "Editor.h"

#include "wxFourWaySplitter.h"

using namespace fbi;


/**
 * Event table for documents
 */
BEGIN_EVENT_TABLE(Editor, Document)
    // EVT_SPLITTER_SASH_POS_CHANGING  (-1, Editor::OnSplitChanging)
    // EVT_SPLITTER_UNSPLIT            (-1, Editor::OnUnSplit)
END_EVENT_TABLE()


/**
 * Changing the split position
 */
void Editor::OnSplitChanging(wxSplitterEvent & event)
{
    /*
    if (event.GetId() == splitID1)
    {
        if (splitWin2->IsSplit())
            splitWin2->SetSashPosition(event.GetSashPosition());
    }
    else if (event.GetId() == splitID2)
    {
        if (splitWin1->IsSplit())
            splitWin1->SetSashPosition(event.GetSashPosition());
    }
    */
}


/**
 * About to unsplit the splitters
 */
void Editor::OnUnSplit(wxSplitterEvent & event)
{
    /*
    if (event.GetId() == splitID1)
    {
        splitWin2->Unsplit();
        splitWin2->UpdateSize();
    }
    else if (event.GetId() == splitID2)
    {
        splitWin1->Unsplit();
        splitWin1->UpdateSize();
    }
    */
}



/**
 * Create new editor
 */
Editor::Editor ()
{
    m_panel = new wxPanel(GET_UIMGR()->GetDocumentArea(), wxID_ANY);
    SetDocWindow(m_panel);
    m_panel->PushEventHandler(this);

    auto vbox = new wxBoxSizer(wxVERTICAL);
    m_panel->SetSizer(vbox);


    int style = wxSP_3DBORDER|wxSP_3DSASH|wxSP_LIVE_UPDATE|wxNO_BORDER|wxCLIP_CHILDREN;
    auto splitter = new wxFourWaySplitter(m_panel, wxID_ANY, wxDefaultPosition, wxDefaultSize, style);

    auto newstc = [&] (wxStyledTextCtrl * parent) -> wxStyledTextCtrl * {
        auto stc = new wxStyledTextCtrl( splitter );
        if (parent != nullptr)
        {
            stc->ReleaseDocument(stc->GetDocPointer());
            stc->SetDocPointer(parent->GetDocPointer());
        }
        return stc;
    };

    auto e1 = newstc(nullptr);
    auto e2 = newstc(e1);
    auto e3 = newstc(e1);
    auto e4 = newstc(e1);

    splitter->SetWindow(0, e1);
    splitter->SetWindow(1, e2);
    splitter->SetWindow(2, e3);
    splitter->SetWindow(3, e4);


    vbox->Add(splitter, 1, wxGROW, 5);




    /*
    int splitterStyles = wxSP_3DBORDER|wxSP_3DSASH|wxSP_LIVE_UPDATE|wxNO_BORDER|wxCLIP_CHILDREN;
    auto splitH = new wxSplitterWindow(
        m_panel, wxID_ANY, 
        wxDefaultPosition, wxDefaultSize,
        splitterStyles
    );
    

    splitID1 = ::wxNewId();
    splitWin1 = new wxSplitterWindow(
        splitH, splitID1, 
        wxDefaultPosition, wxDefaultSize,
        splitterStyles
    );
    auto editor1 = new wxStyledTextCtrl( splitWin1 );
    auto editor2 = new wxStyledTextCtrl( splitWin1 );
    editor2->ReleaseDocument(editor2->GetDocPointer());
    editor2->SetDocPointer(editor1->GetDocPointer());
    splitWin1->SplitHorizontally(editor1, editor2);

    splitID2 = ::wxNewId();
    splitWin2 = new wxSplitterWindow(
        splitH, splitID2, 
        wxDefaultPosition, wxDefaultSize,
        splitterStyles
    );
    auto editor3 = new wxStyledTextCtrl( splitWin2 );
    editor3->ReleaseDocument(editor3->GetDocPointer());
    editor3->SetDocPointer(editor1->GetDocPointer());
    auto editor4 = new wxStyledTextCtrl( splitWin2 );
    editor4->ReleaseDocument(editor4->GetDocPointer());
    editor4->SetDocPointer(editor1->GetDocPointer());
    splitWin2->SplitHorizontally(editor3, editor4);


    splitH->SplitVertically(splitWin1, splitWin2);
    vbox->Add(splitH, 1, wxGROW, 5);
    /*
    auto editor1 = new wxStyledTextCtrl( splitH );
    auto editor2 = new wxStyledTextCtrl( splitH );
    editor2->ReleaseDocument(editor2->GetDocPointer());
    editor2->SetDocPointer(editor1->GetDocPointer());

    splitH->SplitHorizontally(editor1, editor2);
    */
}