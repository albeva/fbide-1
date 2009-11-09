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
#include "../Editor.h"
#include "../Document.h"
#include "StcEditor.h"

using namespace fbi;


BEGIN_EVENT_TABLE(StcEditor, wxStyledTextCtrl)
    EVT_RIGHT_UP(StcEditor::OnMouseRight)
END_EVENT_TABLE()


// tyhe constructor
StcEditor::StcEditor ( wxWindow * wnd, Editor * owner, int index, StcEditor * mirror )
    : m_owner(owner), m_index(index), m_mirror(mirror)
{
    // create the window
    Create(wnd, wxID_ANY,
           wxDefaultPosition, wxDefaultSize,
           0l, "fbi::StcEditor");
    
    // set doc pointer
    if ( m_mirror != nullptr )
    {
        SetDocPointer( m_mirror->GetDocPointer() );
    }

    // disable context menu
    UsePopUp(false);
}


/**
 * Show the context menu
 */
void StcEditor::OnMouseRight (wxMouseEvent & event)
{
    m_owner->ShowContextMenu(m_index);
}
