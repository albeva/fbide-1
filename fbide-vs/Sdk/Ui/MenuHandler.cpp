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
#include "../Manager.h"
#include "../UiManager.h"
#include "IArtProvider.h"
using namespace fbi;


/**
 * Construct
 */
UiMenuHandler::UiMenuHandler() : m_mbar(nullptr)
{
}


// Initalize
void UiMenuHandler::Init (wxMenuBar * menubar)
{
    m_mbar = menubar;
}


// Uninitalize
void UiMenuHandler::UnInit ()
{
    m_mbar = nullptr;
    m_map.clear();
}


/**
 * Load Menu description from xml
 */
void UiMenuHandler::Load (wxXmlNode * xml, wxMenu * parent)
{
    wxString id;
    Manager * mgr = GET_MGR();
    Language & lang = mgr->GetLang();

    for ( ; xml; xml = xml->GetNext())
    {
        if (xml->GetType() == wxXML_COMMENT_NODE) continue;

        // check that this is a menu entry
        if (xml->GetName() == "menu")
        {
            // get id and check
            if((id = xml->GetAttribute("id")) == "")
            {
                wxLogError("Id attribute missing in <menu> entry");
                continue;
            }

            // create new menu ?
            wxMenu * menu = GetMenu(id);
            if (menu == nullptr) 
            {
                menu = new wxMenu();
                m_map[id] = menu;
            }

            // Need to show
            if (menu->GetParent() == nullptr && menu->GetMenuBar() == nullptr)
            {
                if (parent == nullptr) m_mbar->Append(menu, lang["group." + id]);
                else parent->AppendSubMenu(menu, lang["group." + id]);
            }
            
            // the children
            if (xml->GetChildren() != nullptr) Load(xml->GetChildren(), menu);

            // next item
            continue;
        }

        // integrity check. Follwing can only appear in <menu> block
        if (parent == nullptr)
        {
            wxLogWarning("Only <menu> entries allowed in <menus> section. <%s> found", xml->GetName());
            continue;
        }

        // the menu item
        if (xml->GetName() == "item")
        {
            // get id and check
            if((id = xml->GetAttribute("id")) == "")
            {
                wxLogError("Id attribute missing in <item> entry");
                continue;
            }
            // add an item to the menu
            AddMenuItem (id, parent);
        }
        else if (xml->GetName() == "separator")
        {
            parent->AppendSeparator();
        }
        else
        {
            wxLogWarning("Unknown tag in <menu> section named '<%s>'", xml->GetName());
        }
    }
}


// Get menu by ID
wxMenu * UiMenuHandler::GetMenu (const wxString & id)
{
    auto iter = m_map.find(id);
    if (iter == m_map.end()) return nullptr;
    return iter->second;
}


// Add new menu
void UiMenuHandler::AddMenu (const wxString & id, wxMenu * menu, bool show)
{
    assert(id != "");
    assert(menu != nullptr);

    if (GetMenu(id) != nullptr)
    {
        wxLogWarning ("Menu with id '%s' already registered", id);
        return;
    }
    m_map[id] = menu;
    if (show) m_mbar->Append(menu, GET_LANG()["group." + id]);
}


// Add a new item to the menu
void UiMenuHandler::AddMenuItem (const wxString & name, wxMenu * parent)
{
    assert(parent != nullptr);
    assert(name != "");

    Manager     * mgr   = GET_MGR();
    Language    & lang  = mgr->GetLang();
    IdMap       & idmap = mgr->GetIdMap();

    auto id     = idmap[name];
    auto label  = lang[name];
    auto help   = lang[name + ".help"];
    auto style  = wxITEM_NORMAL;
    auto art    = mgr->GetUiManager()->GetArtProvider();

    auto item = new wxMenuItem (parent, id, label, help, style);
    if (art != NULL) item->SetBitmap(art->GetIcon(name));
    parent->Append(item);
}
