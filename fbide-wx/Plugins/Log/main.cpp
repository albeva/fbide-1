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

#include <wx/thread.h>
#include <queue>

/**
 * Run the logging inside the thread
 * so that large amount of log wouldn't get
 * in teh way of responsivness of the main UI
 *
 * works on Win32. how about linux ?
 */
struct LogThread
    : public wxThread,
      public wxLog
{
    LogThread ( wxTextCtrl * tctrl, wxButton * logStopBtn )
        :   m_tctrl( tctrl ), m_logStopBtn( logStopBtn )
    {
    }

    // ...
    virtual void DoLog(wxLogLevel level, const wxChar *szString, time_t t)
    {
        wxMutexLocker lock(m_inputLock);
        m_text.push(szString);
        if (NULL != m_logStopBtn)
            m_logStopBtn->Enable(true);
    }


    /**
     * the thread function
     */
    virtual ExitCode Entry()
    {
        // hold the message
        wxString msg;

        // run the thread
        while ( !TestDestroy() )
        {
            // check if there are any messages ?
            {
                wxMutexLocker lock1(m_inputLock);
                if ( m_text.empty() )
                {
                    if (NULL != m_logStopBtn)
                        m_logStopBtn->Disable();
                    continue;
                }
                msg = m_text.front();
                m_text.pop();
            }

            // log the message
            {
                wxMutexLocker lock(m_tctrlLock);
                if (NULL != m_tctrl)
                {
                    m_tctrl->AppendText ( msg );
                    m_tctrl->AppendText ( _T("\n") );
                }
            }
        }

        return NULL;
    }

    // clear the log window
    void Clear ()
    {
        wxMutexLocker lock(m_tctrlLock);
        if (NULL != m_tctrl) m_tctrl->Clear();
    }

    // Stop the log input
    void Stop ()
    {
        wxMutexLocker lock(m_inputLock);
        while ( !m_text.empty() ) m_text.pop();
        if (NULL != m_logStopBtn) m_logStopBtn->Disable();
    }

    // NotifyExit
    void NotifyExit ()
    {
        LogThread::Stop();
        wxMutexLocker lock1(m_inputLock);
        m_logStopBtn = NULL;
    }

    wxTextCtrl * m_tctrl;
    wxButton   * m_logStopBtn;
    wxMutex m_inputLock, m_tctrlLock;
    std::queue<wxString> m_text;
};



struct LogPlugin
    :   public CPluginBase,
        public wxEvtHandler
{
    // Attach plugin
    bool Attach ()
    {

        CManager &mgr = *GET_MGR();
        CUiManager &ui= *mgr.GetUiManager();

        // get parent pointer
        wxWindow * p = ui.GetPaneWindow(_T("logs"));
        wxASSERT( p != NULL );
        if ( p == NULL ) return false;

        // panel
        wxPanel * panel = new wxPanel(p);

        // sizer
        wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);
        panel->SetSizer(sizer);

        ID_LOG_CLEAR = wxNewId();
        ID_LOG_STOP = wxNewId();

        // the toolbar
        wxToolBar* toolbar = new wxToolBar( panel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTB_FLAT|wxTB_HORIZONTAL );
        // log button
        wxButton* logClearBtn = new wxButton( toolbar, ID_LOG_CLEAR, _("Clear"), wxDefaultPosition, wxDefaultSize, 0 );
        logClearBtn->SetName(_T("Clear"));
        toolbar->AddControl(logClearBtn);
        // stop button
        wxButton* logStopBtn = new wxButton( toolbar, ID_LOG_STOP, _("Stop"), wxDefaultPosition, wxDefaultSize, 0 );
        logStopBtn->SetName(_T("Clear"));
        toolbar->AddControl(logStopBtn);

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
        panel->wxEvtHandler::SetNextHandler(dynamic_cast<wxEvtHandler *>(this));
        Connect(
            ID_LOG_CLEAR,
            wxEVT_COMMAND_BUTTON_CLICKED,
            wxCommandEventHandler(LogPlugin::OnCommand)
        );
        Connect(
            ID_LOG_STOP,
            wxEVT_COMMAND_BUTTON_CLICKED,
            wxCommandEventHandler(LogPlugin::OnCommand)
        );

        // the thread
        m_logThread = new LogThread (tctrl, logStopBtn);
        m_logThread->Create();
        m_logThread->Run();

        // Set log target
        wxLog::SetLogLevel(wxLOG_Info);
        wxLog::SetActiveTarget(m_logThread);

        // success
        return true;
    }


    // show the log window
    void OnCommand (wxCommandEvent & e)
    {
        int id = e.GetId();

        if ( id == ID_LOG_CLEAR ) m_logThread->Clear();
        else if ( id == ID_LOG_STOP ) m_logThread->Stop();
    }

    // void
    bool Detach (bool force)
    {
        wxLog::SetActiveTarget(NULL);
        m_logThread->Delete();
    }

    // about to close ?
    void NotifyExit ()
    {
        wxLog::SetActiveTarget(NULL);
        m_logThread->NotifyExit();
    }

    // the text control to send the log to
    wxTextCtrl  * tctrl;
    LogThread   * m_logThread;

    // IDs
    int ID_LOG_CLEAR;
    int ID_LOG_STOP;
};

namespace
{
    CPluginProvider<LogPlugin, PLUGINTYPE_TOOL> plugin (_T("LogPlugin"));
}
