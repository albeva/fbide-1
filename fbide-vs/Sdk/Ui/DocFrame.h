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
#pragma once

#include "ToolbarHandler.h"

namespace fbi
{

    // forward reference
    class Document;

    /**
     * Frame into what documents are decoupled
     */
    class SDK_DLL DocFrame : public wxFrame
    {
        public:

            // Create new docframe from a Document object
            DocFrame(Document * doc);
        
        private:

            // document managed
            Document * m_document;

            // aui manager
            wxAuiManager m_aui;

            // manage toolbars
            UiToolbarHandler m_tbarHandler;

            // handle events
            DECLARE_EVENT_TABLE()
    };

}
