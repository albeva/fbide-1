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
#include "DocManager.h"
#include "Document.h"

using namespace fbi;

// get new document id
static int NewId()
{
    static int cnt = 0;
    return ++cnt;
}


/**
 * Create new document
 */
Document::Document(wxWindow * wnd, const wxString & name)
    : m_id(NewId()), m_filename(""), m_window(wnd)
{
    SetDocTitle(name);
}


/**
 * Destroy the document
 */
Document::~Document ()
{
}


/**
 * Set document title
 */
void Document::SetDocTitle (const wxString & name)
{
    if (!name.Len())
    {
        m_title = GET_LANG().Get("document.unnamed", "id", wxString() << GetDocId());
    }
    else
    {
        m_title = name;
    }
}
