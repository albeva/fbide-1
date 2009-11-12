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
#include "../Editor.h"
#include "../Document.h"
#include "../EditorManager.h"
#include "StcEditor.h"
#include "StyleInfo.h"

#include "syntax/FreeBasic.h"

using namespace fbi;


//
// For the moment stick these here
// @temporary!
enum
{
    MarginLineNumbers   = 0,
    MarginChangeBar     = 1,
    MaginFoldBar        = 2
};


BEGIN_EVENT_TABLE(StcEditor, wxStyledTextCtrl)
    EVT_RIGHT_UP    (       StcEditor::OnMouseRight)
    EVT_STC_PAINTED (-1,    StcEditor::OnUpdateUi)
    EVT_STC_ZOOM    (-1,    StcEditor::OnZoom)
END_EVENT_TABLE()


// tyhe constructor
StcEditor::StcEditor ( wxWindow * wnd, Editor * owner, int index, StcEditor * mirror )
    : m_owner(owner), m_index(index), m_mirror(mirror)
{
    // create the window
    Create(wnd, wxID_ANY,
           wxDefaultPosition, wxDefaultSize,
           0l, "fbi::StcEditor");
    
    // set doc pointer
    if ( m_mirror != nullptr )
    {
        SetDocPointer( m_mirror->GetDocPointer() );
        SetSelection( m_mirror->GetSelectionStart(), m_mirror->GetSelectionEnd() );
        ScrollToLine( m_mirror->GetFirstVisibleLine() );
        EnsureCaretVisible();
    }

    // disable context menu
    UsePopUp(false);

    // load styles
    Setup(GET_EDITORMGR()->GetStyle());

    m_highlighter = new FreeBasicSyntax(this);



    wnd->Bind(wxEVT_STC_STYLENEEDED, [this](wxStyledTextEvent & event) {
        
    }, wxID_ANY);
}


// Show the context menu
void StcEditor::OnMouseRight (wxMouseEvent & event)
{
    m_owner->ShowContextMenu(m_index);
}


// Setup the editor. Load general config
void StcEditor::Setup (StyleParser * styles)
{
    auto & reg = GET_REG();

    // set default highlight
    SetStyle(wxSTC_STYLE_DEFAULT, styles->GetStyle(".default"));
    StyleClearAll();

    // Set Tab Width
    int tabWidth = reg["editor.tabWidth"].AsInt(4);
    SetTabWidth (tabWidth);

    // set to use tabs or spaces
    SetUseTabs(reg["editor.useTabs"].AsBool(false));

    // set if tab key indents or not?
    SetTabIndents(reg["editor.tabIndents"].AsBool(true));
    SetBackSpaceUnIndents (true); // ???

    // Set Indent size
    SetIndent(reg["editor.indentSize"].AsInt(tabWidth));

    // Set edge column size
    SetEdgeColumn(reg["editor.edgeColumnSize"].AsInt(80));

    // Set edge column mode
    wxString edgeModeType = reg["editor.edgeMode"].AsString("line");
    int edgeMode = wxSTC_EDGE_NONE;
    if (edgeModeType == "line") edgeMode = wxSTC_EDGE_LINE;
    else if (edgeModeType == "background") edgeMode = wxSTC_EDGE_BACKGROUND;
    SetEdgeMode (edgeMode);

    // Set EOL mode
    wxString eolModeType = reg["editor.eolMode"].AsString();
    if      (eolModeType == "CR") SetEOLMode(wxSTC_EOL_CR);
    else if (eolModeType == "LF") SetEOLMode(wxSTC_EOL_LF);
    else                          SetEOLMode(wxSTC_EOL_CRLF);

    // view EOL characters ?
    SetViewEOL(reg["editor.viewEOL"].AsBool());

    // show indent guides ?
    SetIndentationGuides(reg["editor.indentationGuides"].AsBool());

    // Show whitespaces ?
    SetViewWhiteSpace (
        reg["editor.viewWhiteSpace"].AsBool()
        ? wxSTC_WS_VISIBLEALWAYS : wxSTC_WS_INVISIBLE
    );

    // show line numbers ?
    if (reg["editor.showLineNumbers"].AsBool(true))
    {
        // line numbers
        if (styles && styles->PathExists(".line-margin"))
        {
            SetStyle(wxSTC_STYLE_LINENUMBER, styles->GetStyle(".line-margin"));
        }

        // calculate line widths
        bool dyn = m_dynamicLineNumberWidth = reg["editor.dynamicLineNumberWidth"].AsBool(true);
        CalcLineMarginWidth();
        // set margin width
        SetMarginWidth (MarginLineNumbers, 
                        dyn
                        ? m_dynLNWidths[0]
                        : m_dynLNWidths[sizeof(m_dynLNWidths) / sizeof(int) - 1]
        );

        SetMarginType(MarginLineNumbers, wxSTC_MARGIN_NUMBER);
        m_showLineNumbers = true;
    }
    else SetMarginWidth (MarginLineNumbers, 0);

    for (int i = 1; i < 10; i++)
        SetMarginWidth(i, 0);


    // caret
    if (styles && styles->PathExists(".caret"))
    {
        const StyleInfo & info = styles->GetStyle(".caret");
        SetCaretForeground(info.fg);
        SetCaretWidth(info.width ? info.width : 1);

        /**
         * @todo figure out why alpha doesn't work
         */
        if (reg["editor.highlightCurrentLine"].AsBool(true))
        {
            if (info.bg.IsOk())
            {
                SetCaretLineVisible(true);
                SetCaretLineBackground(info.bg);
                // SetCaretLineBackAlpha(200);
            }
        }
    }


    // Auto indent code
    /*
    if (m_data)
    {
        m_data->m_indent = reg["editor.autoIndent"].AsBool(false);
    }
    */


    // Code folding
    /*
    if (reg["editor.codeFolding"].AsBool(false))
    {

        // void SetFoldMarginColour(bool useSetting, const wxColour& back);
        // void SetFoldMarginHiColour(bool useSetting, const wxColour& fore);

        const StyleInfo & info = styles->GetStyle(".fold-margin");

        SetMarginWidth  (MaginFoldBar, info.width ? info.width : 12);
        SetMarginType   (MaginFoldBar, wxSTC_MARGIN_SYMBOL);
        SetMarginMask   (MaginFoldBar, wxSTC_MASK_FOLDERS);

        const wxColor & outline = info.fg;
        const wxColor & fore = info.bg;
        SetFoldFlags(wxSTC_FOLDFLAG_LINEAFTER_CONTRACTED);
        MarkerDefine(wxSTC_MARKNUM_FOLDER,          wxSTC_MARK_BOXPLUS,         fore, outline);
        MarkerDefine(wxSTC_MARKNUM_FOLDEREND,       wxSTC_MARK_BOXPLUSCONNECTED,fore, outline);
        MarkerDefine(wxSTC_MARKNUM_FOLDERMIDTAIL,   wxSTC_MARK_TCORNER,         fore, outline);
        MarkerDefine(wxSTC_MARKNUM_FOLDEROPEN,      wxSTC_MARK_BOXMINUS,        fore, outline);
        MarkerDefine(wxSTC_MARKNUM_FOLDEROPENMID,   wxSTC_MARK_BOXMINUSCONNECTED,fore, outline);
        MarkerDefine(wxSTC_MARKNUM_FOLDERSUB,       wxSTC_MARK_VLINE,           fore, outline);
        MarkerDefine(wxSTC_MARKNUM_FOLDERTAIL,      wxSTC_MARK_LCORNER,         fore, outline);

        SetMarginSensitive (MaginFoldBar, 1);
        SetProperty ("fold",                "1");
        SetProperty ("fold.comment",        "1");
        SetProperty ("fold.compact",        "1");
        SetProperty ("fold.preprocessor",   "1");
    }
    */

    /*
    // Show modification margin
    if (reg["editor.showChangeMargin"].AsBool(true))
    {
        const StyleInfo & info = styles->GetStyle(_T(".edit-margin"));
        m_data->m_changeMargin = true;

        SetMarginWidth(MARGIN_CHANGE, 4);
        SetMarginType(MARGIN_CHANGE,  wxSTC_MARGIN_SYMBOL);
        SetMarginWidth(MARGIN_CHANGE, 4);
        SetMarginMask(MARGIN_CHANGE, (1 << CEditorModMargin::MARKER_SAVED) | (1 << CEditorModMargin::MARKER_EDITED) );

        // edited marker
        MarkerDefine(CEditorModMargin::MARKER_EDITED, wxSTC_MARK_FULLRECT);
        MarkerSetBackground(CEditorModMargin::MARKER_EDITED, info.bg);

        // saved marker
        MarkerDefine(CEditorModMargin::MARKER_SAVED, wxSTC_MARK_FULLRECT);
        MarkerSetBackground(CEditorModMargin::MARKER_SAVED, info.fg);
    }
    */

    // process TAB key manually to allow
    // adding switching between tabs ????
    // CmdKeyClear (wxSTC_KEY_TAB, 0);

}


// Set style
void StcEditor::SetStyle (int nr, const StyleInfo & style)
{
    // wxMessageBox(style.AsString());
    StyleSetFaceName    (nr, style.font);
    StyleSetSize        (nr, style.size);
    StyleSetItalic      (nr, style.italic);
    StyleSetBold        (nr, style.bold);
    StyleSetUnderline   (nr, style.underlined);
    StyleSetForeground  (nr, style.fg);
    StyleSetBackground  (nr, style.bg);
}


// Calculate margin widths for dynamic line margin
void StcEditor::CalcLineMarginWidth ()
{
    int w = TextWidth (wxSTC_STYLE_LINENUMBER, "0");
    for (int i = 0; i < sizeof(m_dynLNWidths) / sizeof(int); i++)
        m_dynLNWidths[i] = w * (i + 3);
}


// Update the editor UI
void StcEditor::OnUpdateUi (wxStyledTextEvent & event)
{
    // int line = m_parent->GetCurrentLine();
    if (m_showLineNumbers && m_dynamicLineNumberWidth)
    {
        // get last visible line
        int lastLine = GetFirstVisibleLine() + LinesOnScreen();

        // detect width
        int w = lastLine < 99 ?
            m_dynLNWidths[0] : lastLine < 999 ?
                m_dynLNWidths[1] : lastLine < 9999 ?
                    m_dynLNWidths[2] : lastLine < 99999 ?
                        m_dynLNWidths[3] : m_dynLNWidths[4];
        // set width
        SetMarginWidth (MarginLineNumbers, w);
    }
}


// Zoom in/out
void StcEditor::OnZoom (wxStyledTextEvent & event)
{
    CalcLineMarginWidth();
    if (m_showLineNumbers && !m_dynamicLineNumberWidth)
    {
        SetMarginWidth (MarginLineNumbers, m_dynLNWidths[sizeof(m_dynLNWidths) / sizeof(int) - 1]);
    }
}