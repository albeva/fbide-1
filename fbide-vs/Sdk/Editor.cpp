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
    // the multisplitter
    Create(GET_UIMGR()->GetDocumentArea(), wxID_ANY,
           wxDefaultPosition, wxDefaultSize,
           wxSP_3DSASH | wxSP_LIVE_UPDATE | wxCLIP_CHILDREN );

    // null the m_editors
    memset(m_editors, 0, sizeof(m_editors));

    // create first editor. Others are created with view is actally split
    m_editors[0] = new StcEditor(this, this, 0, nullptr);
    
    // show this in multisplitter
    ShowTopLeft(m_editors[0]);

}


/**
 * Load file
 */
bool Editor::LoadDocFile (const wxString & file)
{
    return m_editors[0]->LoadFile( file );
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
        split_menu->Append(ID_SplitViewHide, "Hide this", "Hide current view");
    }
    // top-right
    split_menu->AppendCheckItem(ID_SplitView1, "Top left", "Show top-right view")
        ->Check( m_editors[1] != nullptr && IsVisible(1) );
    // bottom left
    split_menu->AppendCheckItem(ID_SplitView2, "Bottom left", "Show or hide bottom-left view")
        ->Check( m_editors[2] != nullptr && IsVisible(2) );
    // bottom right
    split_menu->AppendCheckItem(ID_SplitView3, "Bottom right", "Show or hide bottom-right view")
        ->Check( m_editors[3] != nullptr && IsVisible(3) );

    // show the menu
    m_activeIndex = index;
    PopupMenu(&menu);
}


/**
 * Hide ot show a split view
 */
void Editor::OnSplitView (wxCommandEvent & event)
{
    int index = 0;
    if      (event.GetId() == ID_SplitView1) index = 1;
    else if (event.GetId() == ID_SplitView2) index = 2;
    else if (event.GetId() == ID_SplitView3) index = 3;
    else if (event.GetId() == ID_SplitViewHide)
    {
        if (m_activeIndex != 0)
        {
            HideWindow(m_activeIndex);
        }
        return;
    }

    wxWindowUpdateLocker uiLock(GET_FRAME());
    if (m_editors[index] == nullptr)
    {
        m_editors[index] = new StcEditor(this, this, index, m_editors[0]);
        ShowWindow(index, m_editors[index], event.IsChecked());
        return;
    }
    ShowWindow(index, nullptr, event.IsChecked());
}
