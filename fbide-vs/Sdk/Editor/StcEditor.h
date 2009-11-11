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

namespace fbi
{

    // forward declarations
    class Editor;
    class StyleParser;
    class StyleInfo;
    
    /**
     * The editor
     */
    class SDK_DLL StcEditor : public wxStyledTextCtrl
    {
    public:
        
        // constructor
        StcEditor ( wxWindow * wnd, Editor * owner, int index, StcEditor * mirror );

        // on mouse right up ( show context menu for the editor )
        void OnMouseRight (wxMouseEvent & event);

        // Setup the editor. Load general config
        void Setup (StyleParser * styles);

        // Set style
        void SetStyle (int nr, const StyleInfo & styleInfo);

    private:
        
        // precalculate the line-number margin width
        void CalcLineMarginWidth();
        // precalculated number margin widths
        int m_dynLNWidths[5];
        // show line numbers
        bool m_showLineNumbers;
        // dynamically size the line-number margin
        int m_dynamicLineNumberWidth;
        
        // the owner of this editor
        Editor *    m_owner;
        // editor index ( in MultiSplitWindow )
        int         m_index;
        // mirror the editor (for main null)
        StcEditor * m_mirror;


        DECLARE_EVENT_TABLE()
    };

}
