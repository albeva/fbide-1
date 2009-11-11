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

        // Load colours
        LoadColours();
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
        // check doc pointer
        wxASSERT(doc != nullptr);
        if (doc == nullptr) return;
        
        // get window
        wxWindow * wnd = doc->GetDocWindow();
        wxASSERT(wnd != nullptr);
        if (wnd == nullptr) return;

        // add document
        m_docMap[wnd->GetId()] = doc;
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
        // ??? shoudl events be sent only to active window/document?
        /*
        for (auto iter = m_docMap.begin(); iter != m_docMap.end(); iter++)
        {
            event.StopPropagation();
            auto childFrame = dynamic_cast<wxFrame *>(iter->second->GetDocWindow()->GetParent());
            if (childFrame)
            {
                childFrame->GetEventHandler()->ProcessEvent(event);
            }
        }
        */

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


    // Load some colours by name
    void LoadColours ()
    {
    	wxTheColourDatabase->AddColour("AliceBlue",				wxColour("#F0F8FF"));
    	wxTheColourDatabase->AddColour("AntiqueWhite",			wxColour("#FAEBD7"));
    	wxTheColourDatabase->AddColour("Aqua",					wxColour("#00FFFF"));
    	wxTheColourDatabase->AddColour("Aquamarine",			wxColour("#7FFFD4"));
    	wxTheColourDatabase->AddColour("Azure",					wxColour("#F0FFFF"));
    	wxTheColourDatabase->AddColour("Beige",					wxColour("#F5F5DC"));
    	wxTheColourDatabase->AddColour("Bisque",				wxColour("#FFE4C4"));
    	wxTheColourDatabase->AddColour("Black",					wxColour("#000000"));
    	wxTheColourDatabase->AddColour("BlanchedAlmond",		wxColour("#FFEBCD"));
    	wxTheColourDatabase->AddColour("Blue",					wxColour("#0000FF"));
    	wxTheColourDatabase->AddColour("BlueViolet",			wxColour("#8A2BE2"));
    	wxTheColourDatabase->AddColour("Brown",					wxColour("#A52A2A"));
    	wxTheColourDatabase->AddColour("BurlyWood",				wxColour("#DEB887"));
    	wxTheColourDatabase->AddColour("CadetBlue",				wxColour("#5F9EA0"));
    	wxTheColourDatabase->AddColour("Chartreuse",			wxColour("#7FFF00"));
    	wxTheColourDatabase->AddColour("Chocolate",				wxColour("#D2691E"));
    	wxTheColourDatabase->AddColour("Coral",					wxColour("#FF7F50"));
    	wxTheColourDatabase->AddColour("CornflowerBlue",		wxColour("#6495ED"));
    	wxTheColourDatabase->AddColour("Cornsilk",				wxColour("#FFF8DC"));
    	wxTheColourDatabase->AddColour("Crimson",				wxColour("#DC143C"));
    	wxTheColourDatabase->AddColour("Cyan",					wxColour("#00FFFF"));
    	wxTheColourDatabase->AddColour("DarkBlue",				wxColour("#00008B"));
    	wxTheColourDatabase->AddColour("DarkCyan",				wxColour("#008B8B"));
    	wxTheColourDatabase->AddColour("DarkGoldenRod",			wxColour("#B8860B"));
    	wxTheColourDatabase->AddColour("DarkGray",				wxColour("#A9A9A9"));
    	wxTheColourDatabase->AddColour("DarkGreen",				wxColour("#006400"));
    	wxTheColourDatabase->AddColour("DarkKhaki",				wxColour("#BDB76B"));
    	wxTheColourDatabase->AddColour("DarkMagenta",			wxColour("#8B008B"));
    	wxTheColourDatabase->AddColour("DarkOliveGreen",		wxColour("#556B2F"));
    	wxTheColourDatabase->AddColour("Darkorange",			wxColour("#FF8C00"));
    	wxTheColourDatabase->AddColour("DarkOrchid",			wxColour("#9932CC"));
    	wxTheColourDatabase->AddColour("DarkRed",				wxColour("#8B0000"));
    	wxTheColourDatabase->AddColour("DarkSalmon",			wxColour("#E9967A"));
    	wxTheColourDatabase->AddColour("DarkSeaGreen",			wxColour("#8FBC8F"));
    	wxTheColourDatabase->AddColour("DarkSlateBlue",			wxColour("#483D8B"));
    	wxTheColourDatabase->AddColour("DarkSlateGray",			wxColour("#2F4F4F"));
    	wxTheColourDatabase->AddColour("DarkTurquoise",			wxColour("#00CED1"));
    	wxTheColourDatabase->AddColour("DarkViolet",			wxColour("#9400D3"));
    	wxTheColourDatabase->AddColour("DeepPink",				wxColour("#FF1493"));
    	wxTheColourDatabase->AddColour("DeepSkyBlue",			wxColour("#00BFFF"));
    	wxTheColourDatabase->AddColour("DimGray",				wxColour("#696969"));
    	wxTheColourDatabase->AddColour("DodgerBlue",			wxColour("#1E90FF"));
    	wxTheColourDatabase->AddColour("FireBrick",				wxColour("#B22222"));
    	wxTheColourDatabase->AddColour("FloralWhite",			wxColour("#FFFAF0"));
    	wxTheColourDatabase->AddColour("ForestGreen",			wxColour("#228B22"));
    	wxTheColourDatabase->AddColour("Fuchsia",				wxColour("#FF00FF"));
    	wxTheColourDatabase->AddColour("Gainsboro",				wxColour("#DCDCDC"));
    	wxTheColourDatabase->AddColour("GhostWhite",			wxColour("#F8F8FF"));
    	wxTheColourDatabase->AddColour("Gold",					wxColour("#FFD700"));
    	wxTheColourDatabase->AddColour("GoldenRod",				wxColour("#DAA520"));
    	wxTheColourDatabase->AddColour("Gray",					wxColour("#808080"));
    	wxTheColourDatabase->AddColour("Green",					wxColour("#008000"));
    	wxTheColourDatabase->AddColour("GreenYellow",			wxColour("#ADFF2F"));
    	wxTheColourDatabase->AddColour("HoneyDew",				wxColour("#F0FFF0"));
    	wxTheColourDatabase->AddColour("HotPink",				wxColour("#FF69B4"));
    	wxTheColourDatabase->AddColour("IndianRed",				wxColour("#CD5C5C"));
    	wxTheColourDatabase->AddColour("Indigo",				wxColour("#4B0082"));
    	wxTheColourDatabase->AddColour("Ivory",					wxColour("#FFFFF0"));
    	wxTheColourDatabase->AddColour("Khaki",					wxColour("#F0E68C"));
    	wxTheColourDatabase->AddColour("Lavender",				wxColour("#E6E6FA"));
    	wxTheColourDatabase->AddColour("LavenderBlush",			wxColour("#FFF0F5"));
    	wxTheColourDatabase->AddColour("LawnGreen",				wxColour("#7CFC00"));
    	wxTheColourDatabase->AddColour("LemonChiffon",			wxColour("#FFFACD"));
    	wxTheColourDatabase->AddColour("LightBlue",				wxColour("#ADD8E6"));
    	wxTheColourDatabase->AddColour("LightCoral",			wxColour("#F08080"));
    	wxTheColourDatabase->AddColour("LightCyan",				wxColour("#E0FFFF"));
    	wxTheColourDatabase->AddColour("LightGoldenRodYellow",	wxColour("#FAFAD2"));
    	wxTheColourDatabase->AddColour("LightGrey",				wxColour("#D3D3D3"));
    	wxTheColourDatabase->AddColour("LightGreen",			wxColour("#90EE90"));
    	wxTheColourDatabase->AddColour("LightPink",				wxColour("#FFB6C1"));
    	wxTheColourDatabase->AddColour("LightSalmon",			wxColour("#FFA07A"));
    	wxTheColourDatabase->AddColour("LightSeaGreen",			wxColour("#20B2AA"));
    	wxTheColourDatabase->AddColour("LightSkyBlue",			wxColour("#87CEFA"));
    	wxTheColourDatabase->AddColour("LightSlateGray",		wxColour("#778899"));
    	wxTheColourDatabase->AddColour("LightSteelBlue",		wxColour("#B0C4DE"));
    	wxTheColourDatabase->AddColour("LightYellow",			wxColour("#FFFFE0"));
    	wxTheColourDatabase->AddColour("Lime",					wxColour("#00FF00"));
    	wxTheColourDatabase->AddColour("LimeGreen",				wxColour("#32CD32"));
    	wxTheColourDatabase->AddColour("Linen",					wxColour("#FAF0E6"));
    	wxTheColourDatabase->AddColour("Magenta",				wxColour("#FF00FF"));
    	wxTheColourDatabase->AddColour("Maroon",				wxColour("#800000"));
    	wxTheColourDatabase->AddColour("MediumAquaMarine",		wxColour("#66CDAA"));
    	wxTheColourDatabase->AddColour("MediumBlue",			wxColour("#0000CD"));
    	wxTheColourDatabase->AddColour("MediumOrchid", 			wxColour("#BA55D3"));
    	wxTheColourDatabase->AddColour("MediumPurple",			wxColour("#9370D8"));
    	wxTheColourDatabase->AddColour("MediumSeaGreen",		wxColour("#3CB371"));
    	wxTheColourDatabase->AddColour("MediumSlateBlue",		wxColour("#7B68EE"));
    	wxTheColourDatabase->AddColour("MediumSpringGreen",		wxColour("#00FA9A"));
    	wxTheColourDatabase->AddColour("MediumTurquoise",		wxColour("#48D1CC"));
    	wxTheColourDatabase->AddColour("MediumVioletRed",		wxColour("#C71585"));
    	wxTheColourDatabase->AddColour("MidnightBlue",			wxColour("#191970"));
    	wxTheColourDatabase->AddColour("MintCream",				wxColour("#F5FFFA"));
    	wxTheColourDatabase->AddColour("MistyRose",				wxColour("#FFE4E1"));
    	wxTheColourDatabase->AddColour("Moccasin",				wxColour("#FFE4B5"));
    	wxTheColourDatabase->AddColour("NavajoWhite",			wxColour("#FFDEAD"));
    	wxTheColourDatabase->AddColour("Navy",					wxColour("#000080"));
    	wxTheColourDatabase->AddColour("OldLace",				wxColour("#FDF5E6"));
    	wxTheColourDatabase->AddColour("Olive",					wxColour("#808000"));
    	wxTheColourDatabase->AddColour("OliveDrab",				wxColour("#6B8E23"));
    	wxTheColourDatabase->AddColour("Orange",				wxColour("#FFA500"));
    	wxTheColourDatabase->AddColour("OrangeRed",				wxColour("#FF4500"));
    	wxTheColourDatabase->AddColour("Orchid",				wxColour("#DA70D6"));
    	wxTheColourDatabase->AddColour("PaleGoldenRod",			wxColour("#EEE8AA"));
    	wxTheColourDatabase->AddColour("PaleGreen",				wxColour("#98FB98"));
    	wxTheColourDatabase->AddColour("PaleTurquoise",			wxColour("#AFEEEE"));
    	wxTheColourDatabase->AddColour("PaleVioletRed",			wxColour("#D87093"));
    	wxTheColourDatabase->AddColour("PapayaWhip",			wxColour("#FFEFD5"));
    	wxTheColourDatabase->AddColour("PeachPuff",				wxColour("#FFDAB9"));
    	wxTheColourDatabase->AddColour("Peru",					wxColour("#CD853F"));
    	wxTheColourDatabase->AddColour("Pink",					wxColour("#FFC0CB"));
    	wxTheColourDatabase->AddColour("Plum",					wxColour("#DDA0DD"));
    	wxTheColourDatabase->AddColour("PowderBlue",			wxColour("#B0E0E6"));
    	wxTheColourDatabase->AddColour("Purple",				wxColour("#800080"));
    	wxTheColourDatabase->AddColour("Red",					wxColour("#FF0000"));
    	wxTheColourDatabase->AddColour("RosyBrown",				wxColour("#BC8F8F"));
    	wxTheColourDatabase->AddColour("RoyalBlue",				wxColour("#4169E1"));
    	wxTheColourDatabase->AddColour("SaddleBrown",			wxColour("#8B4513"));
    	wxTheColourDatabase->AddColour("Salmon",				wxColour("#FA8072"));
    	wxTheColourDatabase->AddColour("SandyBrown",			wxColour("#F4A460"));
    	wxTheColourDatabase->AddColour("SeaGreen",				wxColour("#2E8B57"));
    	wxTheColourDatabase->AddColour("SeaShell",				wxColour("#FFF5EE"));
    	wxTheColourDatabase->AddColour("Sienna",				wxColour("#A0522D"));
    	wxTheColourDatabase->AddColour("Silver",				wxColour("#C0C0C0"));
    	wxTheColourDatabase->AddColour("SkyBlue",				wxColour("#87CEEB"));
    	wxTheColourDatabase->AddColour("SlateBlue",				wxColour("#6A5ACD"));
    	wxTheColourDatabase->AddColour("SlateGray",				wxColour("#708090"));
    	wxTheColourDatabase->AddColour("Snow",					wxColour("#FFFAFA"));
    	wxTheColourDatabase->AddColour("SpringGreen",			wxColour("#00FF7F"));
    	wxTheColourDatabase->AddColour("SteelBlue",				wxColour("#4682B4"));
    	wxTheColourDatabase->AddColour("Tan",					wxColour("#D2B48C"));
    	wxTheColourDatabase->AddColour("Teal",					wxColour("#008080"));
    	wxTheColourDatabase->AddColour("Thistle",				wxColour("#D8BFD8"));
    	wxTheColourDatabase->AddColour("Tomato",				wxColour("#FF6347"));
    	wxTheColourDatabase->AddColour("Turquoise",				wxColour("#40E0D0"));
    	wxTheColourDatabase->AddColour("Violet",				wxColour("#EE82EE"));
    	wxTheColourDatabase->AddColour("Wheat",					wxColour("#F5DEB3"));
    	wxTheColourDatabase->AddColour("White",					wxColour("#FFFFFF"));
    	wxTheColourDatabase->AddColour("WhiteSmoke",			wxColour("#F5F5F5"));
    	wxTheColourDatabase->AddColour("Yellow",				wxColour("#FFFF00"));
    	wxTheColourDatabase->AddColour("YellowGreen",			wxColour("#9ACD32"));
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
