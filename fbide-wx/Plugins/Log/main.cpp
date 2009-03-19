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
#include "sdk/PluginManager.h"
#include "sdk/UiManager.h"


using namespace fb;



struct LogPlugin
    :   public CPluginBase,
        public wxEvtHandler,
        public wxLogPassThrough
{
    // Attach plugin
    bool Attach ()
    {
        CManager &mgr = *GET_MGR();
        CUiManager &ui= *mgr.GetUiManager();

        // IDs
        int ID_LOG_CLEAR = wxNewId();

        // get parent pointer
        wxWindow * p = ui.GetPaneWindow(_T("logs"));
        wxASSERT( p != NULL );
        if ( p == NULL ) return false;

        // panel
        wxPanel * panel = new wxPanel(p);

        // sizer
        wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);
        panel->SetSizer(sizer);

        // the toolbar
        wxToolBar* toolbar = new wxToolBar( panel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTB_FLAT|wxTB_HORIZONTAL );
        wxButton* logClearBtn = new wxButton( toolbar, ID_LOG_CLEAR, _("Clear"), wxDefaultPosition, wxDefaultSize, 0 );
        logClearBtn->SetName(_T("Clear"));
        toolbar->AddControl(logClearBtn);
        toolbar->Realize();
        sizer->Add(toolbar, 0, wxGROW|wxLEFT|wxRIGHT|wxTOP, 5);

        // create control
        tctrl = new wxTextCtrl (
            panel, wxID_ANY,
            _T(""),
            wxDefaultPosition, wxDefaultSize,
            wxTE_MULTILINE | wxTE_READONLY | wxTE_BESTWRAP
        );
        tctrl->SetFont(wxFont(
            12,
            wxFONTFAMILY_MODERN,
            wxFONTSTYLE_NORMAL,
            wxFONTWEIGHT_NORMAL,
            false,
            _T("Courier New")
        ));
        sizer->Add(tctrl, 1, wxGROW|wxLEFT|wxRIGHT|wxBOTTOM, 5);

        // Add to the UI
        ui.AddPane(_T("Log"), panel, _T("logs"));

        // connect clear event
        panel->SetNextHandler(this);
        Connect(
            ID_LOG_CLEAR,
            wxEVT_COMMAND_BUTTON_CLICKED,
            wxCommandEventHandler(LogPlugin::OnLogClear)
        );

        // Set log target
        wxLog::SetLogLevel(wxLOG_Info);
        wxLog::SetActiveTarget(this);

        // success
        return true;
    }


    // show the log window
    void OnLogClear (wxCommandEvent & e)
    {
        tctrl->Clear();
    }


    // ...
    virtual void DoLog(wxLogLevel level, const wxChar *szString, time_t t)
    {
        tctrl->AppendText ( szString );
        tctrl->AppendText ( _T("\n") );
    }


    // Detach plugin
    bool Detach (bool force)
    {
        return true;
    }

    // the text control to send the log to
    wxTextCtrl * tctrl;
};

namespace
{
    CPluginProvider<LogPlugin, PLUGINTYPE_TOOL> plugin (_T("LogPlugin"));
}
