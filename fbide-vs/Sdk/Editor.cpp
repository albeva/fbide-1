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
#include "MultiSplitter.h"
#include "Editor.h"
#include "Editor/StcEditor.h"

using namespace fbi;


// IDs for split views
const int ID_SplitViewHide  = ::wxNewId();
const int ID_SplitView1     = ::wxNewId();
const int ID_SplitView2     = ::wxNewId();
const int ID_SplitView3     = ::wxNewId();


// Route events
BEGIN_EVENT_TABLE(Editor, MultiSplitWindow)
    EVT_MENU(ID_SplitView1,     Editor::OnSplitView)
    EVT_MENU(ID_SplitView2,     Editor::OnSplitView)
    EVT_MENU(ID_SplitView3,     Editor::OnSplitView)
    EVT_MENU(ID_SplitViewHide,  Editor::OnSplitView)
END_EVENT_TABLE()


/**
 * Create new editor
 */
Editor::Editor () : m_activeIndex(0)
{
    wxLogMessage("Creating an editor");

    // the multisplitter
    Create(GET_UIMGR()->GetDocumentArea(), wxID_ANY,
           wxDefaultPosition, wxDefaultSize,
           wxSP_3DSASH | wxSP_LIVE_UPDATE | wxCLIP_CHILDREN );
    
    // set min size
    SetMinSize(40, 40, 40, 40);

    // null the m_editors
    memset(m_editors, 0, sizeof(m_editors));

    // create first editor. Others are created with view is actally split
    auto stc = new StcEditor(this, this, 0, nullptr);
    
    // show this in multisplitter
    ShowTopLeft(stc);
}


/**
 * Load file
 */
bool Editor::LoadDocFile (const wxString & file)
{
    if (!m_editors[0]->LoadFile( file )) return false;
    m_editors[0]->SetSelection(0,0);
    return true;
}


/**
 * Show context menu
 */
void Editor::ShowContextMenu ( int index )
{
    wxMenu menu;
    wxMenu * split_menu = new wxMenu();
    menu.AppendSubMenu(split_menu, "Split view", "Split the editor into multiple views");

    if (index != 0)
    {
        auto item0 = split_menu->Append(ID_SplitViewHide, "Hide this", "Hide current view");
    }
    // top-right
    split_menu->AppendCheckItem(ID_SplitView1, "View 1", "Show top-right view")
        ->Check( m_editors[1] != nullptr && IsWindowVisible(1) );
    // bottom left
    auto item2 = split_menu->AppendCheckItem(ID_SplitView2, "View 2", "Show or hide bottom-left view");
    item2->Check( m_editors[2] != nullptr && IsWindowVisible(2) );

    // bottom right
    auto item3 = split_menu->AppendCheckItem(ID_SplitView3, "View 3", "Show or hide bottom-right view");
    item3->Check( m_editors[3] != nullptr && IsWindowVisible(3) );

    // show the menu
    m_activeIndex = index;
    PopupMenu(&menu);
}


/**
 * Hide ot show a split view
 */
void Editor::OnSplitView (wxCommandEvent & event)
{
    // vars
    int index = 0;
    bool show = event.IsChecked();

    // hide ui updates
    wxWindowUpdateLocker uiLock(GET_FRAME());

    // what to do ...
    if      (event.GetId() == ID_SplitView1) index = 1;
    else if (event.GetId() == ID_SplitView2) index = 2;
    else if (event.GetId() == ID_SplitView3) index = 3;
    else if (event.GetId() == ID_SplitViewHide)
    {
        if (m_activeIndex != 0)
        {
            HideWindow(m_activeIndex);
        }
    }
    else return;

    // create if needed and hide / show the editor
    if (index != 0)
    {
        // create new editor ( lazy loading )
        if (m_editors[index] == nullptr)
        {
            m_editors[index] = new StcEditor(this, this, index, m_editors[m_activeIndex]);
            ShowWindow(index, m_editors[index], show);
            if (show) m_editors[index]->SetFocus();
            return;
        }

        // hide / show existing one
        ShowWindow(index, nullptr, event.IsChecked());
    }
    else index = m_activeIndex; // so code below would work.

    // set focus to next logical editor
    if (show) m_editors[index]->SetFocus();
    else
    {
        if (index == 3)
        {
            if (IsWindowVisible(2)) m_editors[2]->SetFocus();
            else if (IsWindowVisible(1)) m_editors[1]->SetFocus();
            else m_editors[0]->SetFocus();
        }
        else if (index == 2)
        {
            if (IsWindowVisible(3)) m_editors[3]->SetFocus();
            else m_editors[0]->SetFocus();
        }
        else
        {
            m_editors[0]->SetFocus();
        }
    }
}
