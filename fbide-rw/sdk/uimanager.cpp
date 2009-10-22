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
#include "manager.h"
#include "uimanager.h"

using namespace fb;


/**
 * Manager class implementation
 */
struct TheUiManager : UiManager, wxEvtHandler
{
    // create
    TheUiManager () : m_frame(NULL) {}


    // destroy
    ~TheUiManager ()
    {}


    // Load user interface. Return false if failed.
    virtual bool Load ()
    {
        m_frame = new wxFrame(
            NULL, wxID_ANY, GetTitle() ,
            wxDefaultPosition, wxSize(300,200)
        );
        wxTheApp->SetTopWindow(m_frame);
        m_frame->PushEventHandler(this);
        m_frame->Show();

        return true;
    }


    // Get application title
    // - maybe use later a title template
    //   to allow customing?
    virtual wxString GetTitle ()
    {
        return wxString("FBIde ") + GET_MGR()->GetVersion().string;
    }


    // get the main frame
    virtual wxFrame * GetFrame ()
    {
        return m_frame;
    }


    virtual wxWindow * GetDocumentArea ()
    {
        return NULL;
    }


    // Handle close event
    void OnClose (wxCloseEvent & event)
    {
        m_frame->RemoveEventHandler(this);
        m_frame->Destroy();
    }

    // main frame
    wxFrame * m_frame;

    // route events
    DECLARE_EVENT_TABLE();
};


// event dispatching
BEGIN_EVENT_TABLE(TheUiManager, wxEvtHandler)
    EVT_CLOSE(TheUiManager::OnClose)
END_EVENT_TABLE()


// Implement Manager
IMPLEMENT_MANAGER(UiManager, TheUiManager)
