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
#include "Frame.h"

#include "sdk/Manager.h"
#include "sdk/UiManager.h"
#include "sdk/TypeManager.h"

using namespace fb;

/**
 * Main application event table
 */
BEGIN_EVENT_TABLE(CFrame, wxFrame)
    EVT_MENU    (wxID_EXIT, CFrame::OnExit)
    EVT_CLOSE   (CFrame::OnClose)
END_EVENT_TABLE()


/**
 * Construct the main FBIde frame window
 */
CFrame::CFrame(wxFrame *frame, const wxString& title)
    : wxFrame(
        frame, -1, title, wxDefaultPosition,
        wxSize(640, 480)
    )
{

}


/**
 * Do cleanup here
 */
CFrame::~CFrame()
{
}


/**
 * Close window
 */
void CFrame::OnClose(wxCloseEvent &event)
{
    /*
    int answer = wxMessageBox (_T("Quit FBIde?"), _T("Exit FBIde"), wxYES_NO | wxICON_QUESTION);
    if (answer != wxYES) return;
    */
    CManager::Get()->GetUiManager()->Unload();
    Destroy();
}


/**
 *
 */
void CFrame::OnExit (wxCommandEvent & event)
{
    Close(true);
}
