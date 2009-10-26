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
#include "MenuHandler.h"
#include "ToolbarHandler.h"
#include "../Manager.h"
#include "../UiManager.h"
#include "IArtProvider.h"
using namespace fbi;


/**
 * Construct
 */
UiToolbarHandler::UiToolbarHandler() : m_aui(nullptr), m_parent(nullptr), m_menu(nullptr)
{
}


// Initalize
void UiToolbarHandler::Init (wxAuiManager * aui, wxMenu * menu)
{
    m_aui = aui;
    m_menu = menu;
    m_parent = m_aui->GetManagedWindow();
    // bind event handlers
    m_aui->Bind(wxEVT_AUI_PANE_CLOSE, &UiToolbarHandler::OnPaneClose, this, wxID_ANY);
    m_parent->Bind(wxEVT_COMMAND_MENU_SELECTED, &UiToolbarHandler::OnToolbarMenuClick, this, wxID_ANY);
}


// toolbars menu item clicked
void UiToolbarHandler::OnToolbarMenuClick(wxCommandEvent & event)
{
    // find the id
    auto iter = m_idbridge.find(event.GetId());
    if (iter == m_idbridge.end()) return;
    // iterate through the panes
    int tbId = iter->second;
    wxAuiPaneInfoArray & panes = m_aui->GetAllPanes();
    for (size_t iter = 0; iter < panes.Count(); iter++)
    {
        // if fount go and mark and return
        wxAuiPaneInfo & pane = panes[iter];
        if (pane.window->GetId() == tbId)
        {
            // mark and update aui
            pane.Show(event.IsChecked());
            m_aui->Update();
            return;
        }
    }
}


// Close toolbar?
void UiToolbarHandler::OnPaneClose(wxAuiManagerEvent & event)
{
    // not a toolbar pane ?
    if (!event.GetPane()->IsToolbar()) return;
    // find id mapping
    auto iter = m_idbridge.find(event.GetPane()->window->GetId());
    if (iter == m_idbridge.end()) return;
    // find menu item
    wxMenuItem * item = m_menu->FindItem(iter->second);
    if (item == nullptr) return;
    // mark
    item->Check(false);
}


// Uninitalize
// @todo remove all toolbar links from the menu
void UiToolbarHandler::UnInit ()
{
    // Release event handlers
    m_parent->Unbind(wxEVT_COMMAND_MENU_SELECTED, &UiToolbarHandler::OnToolbarMenuClick, this, wxID_ANY);
    m_aui->Unbind(wxEVT_AUI_PANE_CLOSE, &UiToolbarHandler::OnPaneClose, this, wxID_ANY);
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

    // vars
    Manager * mgr = GET_MGR();
    wxString label = mgr->GetLang()["toolbar." + name];

    // Add to aui
    m_aui->AddPane(
        toolbar,
        wxAuiPaneInfo().Name(name).Caption(label)
        .ToolbarPane().Top().Dockable(true).Show(show)
    );

    // Add menu item
    int menuId = ::wxNewId();
    wxMenuItem * item = m_menu->AppendCheckItem(menuId, label);
    item->Check(show);

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
    IdMap       & idmap = mgr->GetIdMap();

    auto id     = idmap[name];
    auto label  = lang[name];
    auto help   = lang[name + ".help"];
    auto style  = wxITEM_NORMAL;
    auto art    = mgr->GetUiManager()->GetArtProvider();

    wxBitmap bmp = art ? art->GetIcon(name) : wxNullBitmap;
    toolbar->AddTool(id, label, bmp, help, style);
    auto tool = toolbar->FindTool(id);
    tool->SetLongHelp(help);
}