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
#include "CmdManager.h"
#include "Ui/MenuHandler.h"
#include "Ui/ToolbarHandler.h"
#include "Ui/IArtProvider.h"
#include "Ui/ClassicThemeProvider.h"
#include "Ui/DocFrame.h"
#include "Document.h"

using namespace fbi;

// decouple the tab
const int ID_DecoupleTab = ::wxNewId();

/**
 * Manager class implementation
 */
struct TheUiManager : UiManager, wxEvtHandler
{
    // create
    TheUiManager () : m_frame(nullptr), m_artProvider(nullptr)
    {
        // register UI related IDs
        auto cmdMgr = GET_CMDMGR();
        cmdMgr->Register("fullscreen", ::wxNewId(), CmdManager::Type_Check, nullptr);
    }


    // destroy
    ~TheUiManager ()
    {}


    // Load user interface. Return false if failed.
    virtual bool Load ()
    {
        // Create the frame and set events
        m_frame = new wxFrame(
            NULL, wxID_ANY, GetTitle() ,
            wxDefaultPosition, wxSize(400, 300)
        );
        wxTheApp->SetTopWindow(m_frame);
        m_frame->PushEventHandler(this);

        // Load default art provider
        SetArtProvider(new ClassicThemeProvider());

        // Initalize the menubar
        auto menuBar = new wxMenuBar();
        m_frame->SetMenuBar(menuBar);
        m_menuHandler.Init(menuBar);

        // Initalize AUI
        m_aui.SetFlags(wxAUI_MGR_LIVE_RESIZE | wxAUI_MGR_DEFAULT);
        m_aui.SetManagedWindow(m_frame);
        m_aui.Update();

        // doc styles
        int tabStyles = wxAUI_NB_TAB_MOVE | wxAUI_NB_SCROLL_BUTTONS |
            wxAUI_NB_WINDOWLIST_BUTTON | wxAUI_NB_TOP | wxAUI_NB_TAB_SPLIT;

        // Create document area
        m_docArea = new wxAuiNotebook (
            m_frame,
            wxID_ANY,
            wxDefaultPosition, wxDefaultSize,
            tabStyles | wxAUI_NB_CLOSE_BUTTON | wxAUI_NB_CLOSE_ON_ACTIVE_TAB
        );
        m_aui.AddPane (
            m_docArea,
            wxAuiPaneInfo()
            .Name("fbUiMgrDocArea")
            .CenterPane()
            .PaneBorder(false)
        );

        // Initalize toolbar
        m_tbarHandler.Init(&m_aui);

        // Success
        return true;
    }
    

    // Load layout from xml
    virtual void LoadLayout (const wxString & file)
    {
        // the xml object
        wxXmlDocument xml;

        // Normalize the file path
        wxFileName f(file);
        f.Normalize();
        wxString layoutFile = f.GetFullPath();
        
        // Check for errors ...
        if (!f.FileExists() || !xml.Load(layoutFile))
        {
            wxLogError(_T("Could not load layout file \"%s\""), layoutFile.c_str());
            return;
        }
        if (!xml.IsOk() || xml.GetRoot()->GetName() != _T("fbide"))
        {
            wxLogError(_T("Malformed layout file \"%s\""), layoutFile.c_str());
            return;
        }
        if (xml.GetRoot()->GetAttribute("type") != "ui-layout")
        {
            wxLogError(_T("Type is not 'ui-layou' in \"%s\""), layoutFile.c_str());
            return;
        }

        // load layout
        wxXmlNode * child = xml.GetRoot()->GetChildren();
        for ( ; child; child = child->GetNext())
        {
            if (child->GetType() == wxXML_COMMENT_NODE) continue;

            if (child->GetName() == "menus")
                m_menuHandler.Load(child->GetChildren());
            else if (child->GetName() == "toolbars")
                m_tbarHandler.Load(child->GetChildren());
            else
            {
                wxLogWarning("Invalid entry <%s> in '%s'", child->GetName(), layoutFile.c_str());
            }
        }

        // Refresh ui
        m_aui.Update();
    }


    // Get application title
    // - maybe use later a title template
    //   to allow customing?
    virtual wxString GetTitle ()
    {
        return wxString("FBIde ") + GET_MGR()->GetVersion().string;
    }


    // get the main frame
    virtual wxFrame * GetFrame () { return m_frame; }


    // Get document area base window
    virtual wxWindow * GetDocumentArea () { return m_docArea; }


    // Set art provider
    virtual void SetArtProvider (IArtProvider * provider) { m_artProvider = provider; }


    // get art provider
    virtual IArtProvider * GetArtProvider () { return m_artProvider; }


    // add document
    virtual void AddDocument(Document * doc)
    {
        m_docMap[doc->GetDocWindow()->GetId()] = doc;
        m_docArea->AddPage(doc->GetDocWindow(), doc->GetDocTitle(), true);
    }



    // Handle close event
    void OnClose (wxCloseEvent & event)
    {
        // release art provider
        if (m_artProvider != nullptr)
        {
            delete m_artProvider;
            m_artProvider = nullptr;
        }
        // release toolbar handler
        m_tbarHandler.UnInit();
        // release menu handler
        m_menuHandler.UnInit();
        // release aui
        m_aui.UnInit();
        // get rid of the frame
        m_frame->RemoveEventHandler(this);
        m_frame->Destroy();
    }


    // handle command events
    void OnCommandEvent (wxCommandEvent & event)
    {
        // sllow others to catch
        event.Skip();

        // pass events to documents
        for (auto iter = m_docMap.begin(); iter != m_docMap.end(); iter++)
        {
            event.StopPropagation();
            auto childFrame = dynamic_cast<wxFrame *>(iter->second->GetDocWindow()->GetParent());
            if (childFrame)
            {
                childFrame->GetEventHandler()->ProcessEvent(event);
            }
        }

        // handle toggle events for menus, toolbars, ...
        auto cmdMgr = GET_CMDMGR();
        auto info = cmdMgr->FindEntry(event.GetId());
        if (info != nullptr && info->type == CmdManager::Type_Check)
        {
            cmdMgr->Check(event.GetId(), event.IsChecked());
        }
    }


    // On tab context menu
    void OnTabContextMenu (wxAuiNotebookEvent & event)
    {
        // ensure tab is visible
        m_docArea->SetSelection(event.GetSelection());
        wxMenu * menu = new wxMenu();
        menu->Append(ID_DecoupleTab, "Decouple", "Decouple this tab");
        m_frame->PopupMenu(menu);
    }


    // on decouple
    void OnDecouple (wxCommandEvent & event)
    {
        // decouple the tab
        int index = m_docArea->GetSelection();
        wxWindow * wnd = m_docArea->GetPage(index);
        Document * doc = m_docMap[wnd->GetId()];
        m_docArea->RemovePage(index);

        // create new window
        DocFrame * docFrame = new DocFrame(doc);
        docFrame->Show();
    }


    // vars
    wxFrame *           m_frame;                    // the main application frame
    wxAuiManager        m_aui;                      // AUI manager instance
    UiMenuHandler       m_menuHandler;              // app menu handler / manager
    UiToolbarHandler    m_tbarHandler;              // app toolbar handler
    IArtProvider *      m_artProvider;              // The art provider object
    wxAuiNotebook *     m_docArea;                  // Document area
    std::unordered_map<int, Document *> m_docMap;   // document map

    // route events
    DECLARE_EVENT_TABLE();
};


// event dispatching
BEGIN_EVENT_TABLE(TheUiManager, wxEvtHandler)
    EVT_CLOSE   (                           TheUiManager::OnClose)
    EVT_MENU    (wxID_ANY,                  TheUiManager::OnCommandEvent)
    EVT_MENU    (ID_DecoupleTab,            TheUiManager::OnDecouple)
    EVT_AUINOTEBOOK_TAB_RIGHT_UP( wxID_ANY, TheUiManager::OnTabContextMenu)
END_EVENT_TABLE()


// Implement Manager
IMPLEMENT_MANAGER(UiManager, TheUiManager)
