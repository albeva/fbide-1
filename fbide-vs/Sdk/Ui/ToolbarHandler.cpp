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
#include "../Manager.h"
#include "../CmdManager.h"
#include "../UiManager.h"
#include "IArtProvider.h"
#include "MenuHandler.h"
#include "ToolbarHandler.h"

using namespace fbi;

// show / hide toolbars menu constant
const int ID_ToolbarToggle = ::wxNewId();

/**
 * Construct
 */
UiToolbarHandler::UiToolbarHandler() : 
    m_aui(nullptr), m_parent(nullptr), 
    m_menu(nullptr), m_showTbars(false),
    m_visibleCnt(0)
{
}


// Initalize
void UiToolbarHandler::Init (wxAuiManager * aui, bool useMenu)
{
    // vars
    m_aui = aui;
    m_parent = m_aui->GetManagedWindow();
    auto cmdMgr = GET_CMDMGR();
    
    // bind event handlers
    m_aui->Bind(wxEVT_AUI_PANE_CLOSE, &UiToolbarHandler::OnPaneClose, this, wxID_ANY);
    m_parent->Bind(wxEVT_COMMAND_MENU_SELECTED, &UiToolbarHandler::OnCommandEvent, this, wxID_ANY);
    
    // register toolbars menu
    if (!cmdMgr->IsRegistered("menu.toolbars"))
    {
        m_menu = new wxMenu();
        cmdMgr->Register("menu.toolbars", ::wxNewId(), CmdManager::Type_Menu, m_menu);
    }
    
    // toggle toolbars
    if (!cmdMgr->IsRegistered("toolbars.toggle"))
    {
        /// @todo - read from config ?
        m_showTbars = true;
        cmdMgr->Register("toolbars.toggle", ID_ToolbarToggle, CmdManager::Type_Check, nullptr, true, m_showTbars);
    }
    else
    {
        m_showTbars = cmdMgr->GetEntry("toolbars.toggle").checked;
    }

    // don't use menus
    if (!useMenu) m_menu = nullptr;

    // toggle
    cmdMgr->Bind(fbiCMD_CHECK,  &UiToolbarHandler::OnCmdMgrEvent, this, wxID_ANY);
    cmdMgr->Bind(fbiCMD_ENABLE, &UiToolbarHandler::OnCmdMgrEvent, this, wxID_ANY);
}


// toggle the checkboxes
void UiToolbarHandler::OnCmdMgrEvent(wxCommandEvent & event)
{
    // allow others to catch the event
    event.Skip();

    // update the toolbars
    wxAuiPaneInfoArray & panes = m_aui->GetAllPanes();
    for (size_t iter = 0; iter < panes.Count(); iter++)
    {
        wxAuiPaneInfo & pane = panes[iter];
        if (!pane.IsToolbar()) continue;
        auto tbar = dynamic_cast<wxAuiToolBar*>(pane.window);
        if (tbar == nullptr) continue;

        if (event.GetEventType() == fbiCMD_CHECK)
        {
            tbar->ToggleTool(event.GetId(), event.IsChecked());
        }
        else if (event.GetEventType() == fbiCMD_ENABLE)
        {
            tbar->EnableTool(event.GetId(), event.IsChecked());
        }
    }
    m_aui->Update();
}


// toolbars menu item clicked
void UiToolbarHandler::OnCommandEvent(wxCommandEvent & event)
{
    // allow other event handlers to catch this event
    event.Skip();

    // vars
    int id = event.GetId();
    bool show = event.IsChecked();

    // show / hide toolbars ?
    if (id == ID_ToolbarToggle)
    {
        ToggleToolbars(event.IsChecked());
        return;
    }

    // find the id. Skip otherwise
    auto iter = m_idbridge.find(id);
    if (iter == m_idbridge.end()) return;
    int tbId = iter->second;

    // show toolbars if they are hidden
    if (!m_showTbars && show)
    {
        m_visibleMap[tbId] = true;
        ToggleToolbars(true);
        GET_CMDMGR()->Check(ID_ToolbarToggle, true);
        return;
    }

    // iterate through the panes
    wxAuiPaneInfoArray & panes = m_aui->GetAllPanes();
    for (size_t iter = 0; iter < panes.Count(); iter++)
    {
        // if fount go and mark and return
        wxAuiPaneInfo & pane = panes[iter];
        if (pane.window->GetId() == tbId)
        {
            // mark and update aui
            pane.Show(show);
            if (show) m_visibleCnt ++;
            else m_visibleCnt--;
            m_visibleMap[tbId] = show;
            m_aui->Update();
            break;
        }
    }
    
    // if show the toolbar and toolbars globally are hidden then update
    // the "show toolbars" toggle
    GET_CMDMGR()->Enable(ID_ToolbarToggle, m_visibleCnt);
}


// Close toolbar?
void UiToolbarHandler::OnPaneClose(wxAuiManagerEvent & event)
{
    //event.
    // allow other event handlers to catch this event by defalt
    event.Skip();

    // not a toolbar pane ?
    if (!event.GetPane()->IsToolbar() || m_menu == nullptr) return;

    // find id mapping
    int tbId = event.GetPane()->window->GetId();
    auto iter = m_idbridge.find(tbId);
    if (iter == m_idbridge.end()) return;
    // find menu item
    wxMenuItem * item = m_menu->FindItem(iter->second);
    if (item == nullptr) return;
    
    // mark
    event.Skip(false);
    item->Check(false);
    m_visibleCnt--;
    m_visibleMap[tbId] = false;

    // disable
    if (m_visibleCnt == 0) GET_CMDMGR()->Enable(ID_ToolbarToggle, false);
}


// Show or hide the toolbars
void UiToolbarHandler::ToggleToolbars(bool show)
{
    if (m_showTbars == show) return;

    // m_showTbars = show;
    for (auto iter = m_map.begin(); iter != m_map.end(); iter++)
    {
        // toggle the toolbar
        wxAuiToolBar * tbar = iter->second;
        if (m_visibleMap[tbar->GetId()])
        {
            m_aui->GetPane(tbar).Show(show);
            m_visibleCnt += show ? 1 : -1;
            // toggle associated menu command
            // find ID
            if (m_menu == nullptr) continue;

            auto idIter = m_idbridge.find(tbar->GetId());
            if (idIter == m_idbridge.end()) continue;
            // find menu entry and toggle if exists
            auto item = m_menu->FindItem(idIter->second);
            if (item != nullptr) item->Check(show);
        }
    }
    m_aui->Update();
    m_showTbars = m_visibleCnt > 0;
}


// Uninitalize
// @todo remove all toolbar links from the menu
void UiToolbarHandler::UnInit ()
{
    // Release event handlers
    m_parent->Unbind(wxEVT_COMMAND_MENU_SELECTED, &UiToolbarHandler::OnCommandEvent, this, wxID_ANY);
    m_aui->Unbind(wxEVT_AUI_PANE_CLOSE, &UiToolbarHandler::OnPaneClose, this, wxID_ANY);
    GET_CMDMGR()->Unbind(fbiCMD_CHECK, &UiToolbarHandler::OnCmdMgrEvent, this, wxID_ANY);
    GET_CMDMGR()->Unbind(fbiCMD_ENABLE, &UiToolbarHandler::OnCmdMgrEvent, this, wxID_ANY);

    // clear up
    m_menu = nullptr;
    m_aui = nullptr;
    m_parent = nullptr;
    m_map.clear();
}


/**
 * Load Menu description from xml
 */
void UiToolbarHandler::Load (wxXmlNode * xml)
{
    // vars
    wxString id;
    Manager * mgr = GET_MGR();
    Language & lang = mgr->GetLang();
    auto art = mgr->GetUiManager()->GetArtProvider();

    // styles
    int style = wxAUI_TB_GRIPPER
              | wxAUI_TB_HORZ_LAYOUT
              | wxAUI_TB_OVERFLOW
              ;
    if (art == nullptr) style |= wxAUI_TB_TEXT;
    
    // iterate
    for ( ; xml; xml = xml->GetNext())
    {
        if (xml->GetType() == wxXML_COMMENT_NODE) continue;

        // check that this is a menu entry
        if (xml->GetName() != "toolbar")
        {
            wxLogWarning("Only <toolbar> entries allowed in <toolbars> section. <%s> found", xml->GetName());
            continue;
        }

        // empty toolbar
        if (xml->GetChildren() == nullptr) continue;
        
        // get id and check
        if((id = xml->GetAttribute("id")) == "")
        {
            wxLogError("Id attribute missing in <toolbar> entry");
            continue;
        }

        // add this toolbar
        bool addTb = false;

        // show this toolbar 
        bool show = xml->GetAttribute("show", "yes") == "yes";

        // get tbar
        wxAuiToolBar * tbar = GetToolBar(id);
        if (tbar == nullptr)
        {
            tbar = new wxAuiToolBar (
                m_parent, ::wxNewId(),
                wxDefaultPosition, wxDefaultSize,
                style
            );
            if (art) tbar->SetToolBitmapSize(art->GetIconSize());
            addTb = true;
        }

        // iterate through children (if any) and add them to the toolbar
        for (auto child = xml->GetChildren(); child; child = child->GetNext())
        {
            if (child->GetType() == wxXML_COMMENT_NODE) continue;
            wxString tag = child->GetName();

            // seperator
            if (tag == "separator")
            {
                tbar->AddSeparator ();
            }
            else if (tag == "item")
            {
                wxString item_id = child->GetAttribute("id");
                if (item_id == "")
                {
                    wxLogError("Id attribute missing in <item> entry");
                    continue;
                }
                AddToolBarItem (item_id, tbar);
            }
            else
            {
                wxLogWarning("Unknown tag in <toolbar> section named '<%s>'", tag);
            }
        }

        // refresh the toolbar
        tbar->Realize ();
        if (addTb) AddToolBar (id, tbar, show);
    }

    m_aui->Update();
}


// get toolbar by id
wxAuiToolBar * UiToolbarHandler::GetToolBar (const wxString & id)
{
    auto iter = m_map.find(id);
    if (iter == m_map.end()) return nullptr;
    return iter->second;
}


// Add toolbar
void UiToolbarHandler::AddToolBar (const wxString & name, wxAuiToolBar * toolbar, bool show)
{
    if (GetToolBar(name))
    {
        wxLogWarning("Toolbar with id '%s' already exists", name);
        return;
    }
    m_map[name] = toolbar;

    // is toolbar really visible?
    bool isVisible = show && m_showTbars;

    // vars
    Manager * mgr = GET_MGR();
    wxString label = mgr->GetLang()["toolbar." + name];

    // Add to aui
    m_aui->AddPane(
        toolbar,
        wxAuiPaneInfo().Name(name).Caption(label)
        .ToolbarPane().Top().Dockable(true).Show(isVisible)
    );
    if (isVisible) m_visibleCnt += 1;
    m_visibleMap[toolbar->GetId()] = show;

    // no menu... don't bother
    if (m_menu == nullptr) return;

    // Add menu item
    int menuId = ::wxNewId();
    wxMenuItem * item = m_menu->AppendCheckItem(menuId, label);
    item->Check(isVisible);

    // id bridge. Is this OK? each ID should be unique ... so it should be okay
    m_idbridge[toolbar->GetId()] = menuId;
    m_idbridge[menuId] = toolbar->GetId();
}


/**
 * Add an item to the toolbar
 */
void UiToolbarHandler::AddToolBarItem (const wxString & name, wxAuiToolBar * toolbar)
{
    assert(toolbar != nullptr);
    assert(name != "");

    Manager     * mgr   = GET_MGR();
    Language    & lang  = mgr->GetLang();
    CmdManager  * cmdMgr = mgr->GetCmdManager();

    auto &entry = cmdMgr->GetEntry(name);
    auto label  = lang[name];
    auto help   = lang[name + ".help"];
    auto art    = mgr->GetUiManager()->GetArtProvider();
    auto id     = entry.id;

    if (entry.type == CmdManager::Type_Normal)
    {
        wxBitmap bmp = art ? art->GetIcon(name) : wxNullBitmap;
        auto tool = toolbar->AddTool(id, label, bmp, help, wxITEM_NORMAL);
        tool->SetLongHelp(help);
    }
    else if (entry.type == CmdManager::Type_Check)
    {
        wxBitmap bmp = art ? art->GetIcon(name) : wxNullBitmap;
        auto tool = toolbar->AddTool(id, label, bmp, help, wxITEM_CHECK);
        tool->SetLongHelp(help);
        toolbar->ToggleTool(id, entry.checked);
    }
}
