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
#include "../UiManager.h"
#include "../Document.h"
#include "DocFrame.h"

using namespace fbi;

// events
BEGIN_EVENT_TABLE(DocFrame, wxFrame)
END_EVENT_TABLE()


// create new document
DocFrame::DocFrame(Document * document) : m_document(document)
{
    // sanity
    wxASSERT(m_document != nullptr);
    wxASSERT(m_document->GetDocWindow() != nullptr);

    // create the frame
    wxFrame::Create(
        GET_FRAME(), wxID_ANY, m_document->GetDocTitle(),
        wxDefaultPosition, wxDefaultSize,
        wxDEFAULT_FRAME_STYLE
    );

    // use aui to manage the layout
    m_aui.SetFlags(wxAUI_MGR_LIVE_RESIZE | wxAUI_MGR_DEFAULT);
    m_aui.SetManagedWindow(this);

    // bring the document over
    wxWindow * docWnd = m_document->GetDocWindow();
    docWnd->Reparent(this);

    // Add it
    m_aui.AddPane (
        docWnd,
        wxAuiPaneInfo()
        .Name("fbDocMgrDocument")
        .CenterPane()
        .PaneBorder(false)
    );
    
    // initalize the toolbar
    m_tbarHandler.Init(&m_aui, false);

    // load layout
    auto & reg = GET_REG();
    wxString file = reg["path.ide.data"].AsString() + "docframe.xml";
    
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
        {
            wxMessageBox("menus not implemented");
            // m_menuHandler.Load(child->GetChildren());
        }
        else if (child->GetName() == "toolbars")
        {
            m_tbarHandler.Load(child->GetChildren());
        }
        else
        {
            wxLogWarning("Invalid entry <%s> in '%s'", child->GetName(), layoutFile.c_str());
        }
    }

    // Refresh ui
    m_aui.Update();
}


/**
 * Dock back
 */
void DocFrame::OnDockBack (wxCommandEvent & event)
{
}
