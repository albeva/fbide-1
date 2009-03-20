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

#include "wx_pch.h"
#include "sdk/Manager.h"
#include "sdk/Editor.h"
#include "sdk/StyleInfo.h"
#include "sdk/EditorEvent.h"
#include "sdk/EditorManager.h"

#include "EditorModMargin.h"

#include <stdlib.h>

// #include "sdk/LexFreeBasic.hxx"

using namespace fb;

/**
 * Constants
 */
enum
{
    MARGIN_LINENUMBERS  = 0,
    MARGIN_CHANGE       = 1,
    MARGIN_FOLDING      = 2
};


/**
 * Private data implementation for the CEditor
 */
struct CEditor::CData : public wxEvtHandler
{

    // action states for modification margin
    enum
    {
        MOD_MARGIN_MODIFY = 1,
        MOD_MARGIN_UNDO_REDO
    };


    // simple constructor
    CData (CEditor * parent)
        : m_parent(parent),
          m_dynamicLineNumberWidth(false),
          m_changeMargin(false),
          m_width3(0), m_width4(0), m_width5(0),
          m_width6(0), m_width7(0),
          m_modMarginState(0),
          m_modMargin(0)
    {
        m_modMargin = new CEditorModMargin(*m_parent);
    }


    ~CData ()
    {
        delete m_modMargin;
    }


    // On key down
    void OnKeyDown ( wxKeyEvent &event )
    {
        event.Skip();
    }


    // Code was modified
    void OnModified (wxScintillaEvent & event)
    {
        if (!m_ready) return;

        // edit flags
        enum
        {
            editFlags      = wxSCI_MOD_INSERTTEXT | wxSCI_MOD_DELETETEXT,
            beforeEditFlags= wxSCI_MOD_BEFOREDELETE | wxSCI_MOD_BEFOREINSERT,
            undoRedoFlags  = wxSCI_PERFORMED_UNDO | wxSCI_PERFORMED_REDO
        };

        int mod     = event.GetModificationType();
        int line    = m_parent->LineFromPosition(event.GetPosition());
        int lines   = event.GetLinesAdded();

        // insert or edit text
        if ( mod & editFlags )
        {
            // performed by user
            if ( mod & wxSCI_PERFORMED_USER )
            {
                // this is a beginning of the new action
                // save the previous one.
                if ( mod & wxSTI_STARTACTION )
                {
                    // m_modMargin->SubmitChanges();
                    // m_modMargin->SubmitModify ();
                    m_modMargin->Flush();
                }

                // mark line as modified
                m_modMargin->Modify( line, lines, CEditorModMargin::FLAG_EDITED );
                m_modMarginState = MOD_MARGIN_MODIFY;
            }
            // performed undo or redo action
            else if ( mod & undoRedoFlags )
            {
                // First step undo / redo
                if ( m_modMarginState != MOD_MARGIN_UNDO_REDO )
                {
                    // flush any existing lines
                    m_modMargin->Flush();

                    // set state
                    m_modMarginState = MOD_MARGIN_UNDO_REDO;
                }

                // perform undo or redo
                if ( mod & wxSCI_PERFORMED_UNDO )
                    m_modMargin->Undo ( line, lines );
                else
                    m_modMargin->Redo ( line, lines );

                // the last step
                if ( mod & wxSCI_LASTSTEPINUNDOREDO )
                {
                    m_modMargin->ApplyMarkers();
                    m_modMarginState = 0;
                }
            }
        }


        // event object
        bool show = false;

        // show = true;
        if ( show )
        {
            bool add = false;
            #define EVT_INFO(_id) if (mod & _id) { LOG_MSG(#_id); add = true; }
            EVT_INFO(wxSCI_MOD_INSERTTEXT)
            EVT_INFO(wxSCI_MOD_DELETETEXT)
            EVT_INFO(wxSCI_PERFORMED_UNDO)
            EVT_INFO(wxSCI_PERFORMED_REDO)
            EVT_INFO(wxSCI_MULTISTEPUNDOREDO)
            EVT_INFO(wxSCI_LASTSTEPINUNDOREDO)
            EVT_INFO(wxSCI_MOD_BEFOREINSERT)
            EVT_INFO(wxSCI_MOD_BEFOREDELETE)
            EVT_INFO(wxSCI_MULTILINEUNDOREDO)
            EVT_INFO(wxSTI_STARTACTION)
            // EVT_INFO(wxSCI_MOD_CHANGEMARKER)
            // EVT_INFO(wxSCI_MOD_CHANGESTYLE)
            // EVT_INFO(wxSCI_MOD_CHANGEFOLD)
            // EVT_INFO(wxSTI_MOD_CHANGEINDICATOR)
            // EVT_INFO(wxSTI_MOD_CHANGELINESTATE)
            // EVT_INFO(wxSCI_MODEVENTMASKALL)
            if (add)
            {
                EVT_INFO(wxSCI_PERFORMED_USER)
                LOG_INT(line);
                LOG_INT(lines);
                LOG_MSG("--------------------");
            }
            #undef EVT_INFO
        }

    }


    // Update UI...
    void OnUpdateUi (wxScintillaEvent & event)
    {
        // int line = m_parent->GetCurrentLine();
        if (m_showLineNumbers && m_dynamicLineNumberWidth)
        {
            // get last visible line
            int lastLine = m_parent->GetFirstVisibleLine() + m_parent->LinesOnScreen();

            // detect width
            int w = lastLine < 99 ?
                m_width3 : lastLine < 999 ?
                    m_width4 : lastLine < 9999 ?
                        m_width5 : lastLine < 99999 ?
                            m_width6 : m_width7;

            // set width
            m_parent->SetMarginWidth (0, w);
        }
    }


    // Margin was clicked. Move this to CEditor ???
    void OnMarginClick  ( wxScintillaEvent &event )
    {
        if (event.GetMargin() == MARGIN_FOLDING)
        {
            int lineClick = m_parent->LineFromPosition (event.GetPosition());
            int levelClick = m_parent->GetFoldLevel (lineClick);
            if ((levelClick & wxSCI_FOLDLEVELHEADERFLAG) > 0) {
                m_parent->ToggleFold (lineClick);
            }
        }
    }


    // Character added
    void OnCharAdded (wxScintillaEvent & event)
    {
        /*
         * Indent caret to a position if \n
         */
        if (m_indent && event.GetKey() == '\n')
        {
            wxScintilla * editor = m_parent;
            int cLine = editor->GetCurrentLine();
            int lineInd = editor->GetLineIndentation(cLine - 1);
            editor->SetLineIndentation (cLine, lineInd);
            editor->GotoPos(editor->PositionFromLine (cLine) + lineInd);
        }
    }


    // Undo
    void OnEditorEvent (wxCommandEvent & event)
    {
        switch (event.GetId())
        {
            case wxID_UNDO :
                m_parent->Undo();
                break;
            case wxID_REDO :
                m_parent->Redo();
                break;
            case wxID_CUT :
                m_parent->Cut();
                break;
            case wxID_COPY :
                m_parent->Copy();
                break;
            case wxID_PASTE :
                m_parent->Paste();
                break;
            default :
                wxLogMessage(_T("Unhandled EditorEvent"));
                return;
        }

        // don't loose the caret
        m_parent->EnsureCaretVisible();
    }


    // Zoom in/out
    void OnZoom (wxScintillaEvent & event)
    {
        CalcLineMarginWidth();
        if (m_showLineNumbers && !m_dynamicLineNumberWidth)
        {
            m_parent->SetMarginWidth (MARGIN_LINENUMBERS, m_width6);
        }
    }



    // Calculate margin widths for dynamic line margin
    void CalcLineMarginWidth ()
    {
        int w = m_parent->TextWidth (wxSCI_STYLE_LINENUMBER, _T("0"));
        m_width3 = w * 3;
        m_width4 = w * 4;
        m_width5 = w * 5;
        m_width6 = w * 6;
        m_width7 = w * 7;
    }


    CEditor     * m_parent; // parent class
    bool        m_indent;   // can indent code ?
    bool        m_ready;
    bool        m_showLineNumbers;
    bool        m_dynamicLineNumberWidth;
    bool        m_changeMargin;

    int         m_width3;
    int         m_width4;
    int         m_width5;
    int         m_width6;
    int         m_width7;
    int         m_modMarginState;

    // CChangeHistory m_changes;
    CEditorModMargin * m_modMargin;

    DECLARE_EVENT_TABLE();
};


/**
 * Event routing
 */
BEGIN_EVENT_TABLE (CEditor::CData, wxEvtHandler)
    // EVT_KEY_DOWN (CEditor::CData::OnKeyDown)
    EVT_SCI_MODIFIED    (-1, CEditor::CData::OnModified)
    EVT_SCI_MARGINCLICK (-1, CEditor::CData::OnMarginClick)
    EVT_SCI_CHARADDED   (-1, CEditor::CData::OnCharAdded)
    EVT_SCI_PAINTED     (-1, CEditor::CData::OnUpdateUi)
    EVT_SCI_ZOOM        (-1, CEditor::CData::OnZoom)
    EVT_MENU            (wxID_UNDO, CEditor::CData::OnEditorEvent)
    EVT_MENU            (wxID_REDO, CEditor::CData::OnEditorEvent)
    EVT_MENU            (wxID_CUT,  CEditor::CData::OnEditorEvent)
    EVT_MENU            (wxID_COPY, CEditor::CData::OnEditorEvent)
    EVT_MENU            (wxID_PASTE,CEditor::CData::OnEditorEvent)
END_EVENT_TABLE()


/**
 * Construct new CEditor widget
 */
CEditor::CEditor (
    wxWindow *parent, wxWindowID id,
    const wxPoint& pos, const wxSize& size,
    long style, const wxString& name
)
{
    CEditor::Create (parent, id, pos, size, style, name);
}



/**
 * Clean up.
 */
CEditor::~CEditor ()
{
    RemoveEventHandler(m_data);
    if (m_data) delete m_data;
}


/**
 * Create the editor widget
 */
bool CEditor::Create(
    wxWindow *parent, wxWindowID id,
    const wxPoint& pos, const wxSize& size,
    long style, const wxString& name
)
{
    // call parent Create method
    wxScintilla::Create (parent, id, pos, size, style, name);

    // private data
    m_data = new CEditor::CData (this);

    // Set event handler
    SetNextHandler(m_data);
}


/**
 * Mark editor as ready and fully loaded
 */
void CEditor::MarkReady (bool state)
{
    GET_EDITORMGR()->RegisterEditor(this);
    if (m_data) m_data->m_ready = state;
}


/**
 * Get is ready state?
 */
bool CEditor::GetReady ()
{
    return m_data && m_data->m_ready;
}


/**
 * Setup general editor configuration
 * Should be called after editor is fully styled
 * otherwise some sizes won't fit properly
 *
 * @todo have all default values come from constants that
 *       would be easy to change/modify/update ?
 */
void CEditor::Setup (CStyleParser * styles)
{
    CRegistry & reg = GET_REG();

    // set default highlight
    if (styles) SetStyle(wxSCI_STYLE_DEFAULT,   styles->GetStyle(_T(".default")));

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
    wxString edgeModeType = reg["editor.edgeMode"].AsString(_T("line"));
    int edgeMode = wxSCI_EDGE_NONE;
    if (edgeModeType==_T("line")) edgeMode = wxSCI_EDGE_LINE;
    else if (edgeModeType==_T("background")) edgeMode = wxSCI_EDGE_BACKGROUND;
    SetEdgeMode (edgeMode);

    // Set EOL mode
    wxString eolModeType = reg["editor.eolMode"].AsString();
    int eolMode = wxSCI_EOL_CRLF;
    if (eolModeType == _T("CR"))
        eolMode = wxSCI_EOL_CR;
    else if (eolModeType == _T("LF"))
        eolMode = wxSCI_EOL_LF;
    SetEOLMode(eolMode);

    // view EOL characters ?
    SetViewEOL(reg["editor.viewEOL"].AsBool());

    // show indent guides ?
    SetIndentationGuides(reg["editor.indentationGuides"].AsBool());

    // Show whitespaces ?
    SetViewWhiteSpace (
        reg["editor.viewWhiteSpace"].AsBool()
        ? wxSCI_WS_VISIBLEALWAYS : wxSCI_WS_INVISIBLE
    );

    // show line numbers ?
    if (reg["editor.showLineNumbers"].AsBool(true))
    {
        // line numbers
        if (styles && styles->PathExists(_T(".line-margin")))
        {
            SetStyle(wxSCI_STYLE_LINENUMBER, styles->GetStyle(_T(".line-margin")));
        }

        // calculate line widths
        bool dyn = m_data->m_dynamicLineNumberWidth = reg["editor.dynamicLineNumberWidth"].AsBool(true);
        m_data->CalcLineMarginWidth();

        SetMarginWidth (MARGIN_LINENUMBERS, dyn ? m_data->m_width3 : m_data->m_width6);

        SetMarginType(MARGIN_LINENUMBERS, wxSCI_MARGIN_NUMBER);
        m_data->m_showLineNumbers = true;
    }
    else SetMarginWidth (MARGIN_LINENUMBERS,0);


    // caret
    if (styles && styles->PathExists(_T(".caret")))
    {
        CStyleInfo info = styles->GetStyle(_T(".caret"));
        SetCaretForeground(info.fg);
        SetCaretWidth(info.width ? info.width : 1);

        if (reg["editor.highlightCurrentLine"].AsBool(true))
        {
            if (info.bg.IsOk())
            {
                SetCaretLineVisible(true);
                SetCaretLineBackground(info.bg);
                if (info.opacity)
                {
                    SetCaretLineBackAlpha(info.opacity);
                }
            }
        }
    }


    // Auto indent code
    if (m_data)
    {
        m_data->m_indent = reg["editor.autoIndent"].AsBool(false);
    }


    // Code folding
    if (reg["editor.codeFolding"].AsBool(false))
    {

        // void SetFoldMarginColour(bool useSetting, const wxColour& back);
        // void SetFoldMarginHiColour(bool useSetting, const wxColour& fore);

        CStyleInfo info = styles->GetStyle(_T(".fold-margin"));

        SetMarginWidth  (MARGIN_FOLDING, info.width ? info.width : 12);
        SetMarginType   (MARGIN_FOLDING, wxSCI_MARGIN_SYMBOL);
        SetMarginMask   (MARGIN_FOLDING, wxSCI_MASK_FOLDERS);

        wxColor & outline = info.fg;
        wxColor & fore = info.bg;
        SetFoldFlags(wxSCI_FOLDFLAG_LINEAFTER_CONTRACTED);
        MarkerDefine(wxSCI_MARKNUM_FOLDER,          wxSCI_MARK_BOXPLUS,         fore, outline);
        MarkerDefine(wxSCI_MARKNUM_FOLDEREND,       wxSCI_MARK_BOXPLUSCONNECTED,fore, outline);
        MarkerDefine(wxSCI_MARKNUM_FOLDERMIDTAIL,   wxSCI_MARK_TCORNER,         fore, outline);
        MarkerDefine(wxSCI_MARKNUM_FOLDEROPEN,      wxSCI_MARK_BOXMINUS,        fore, outline);
        MarkerDefine(wxSCI_MARKNUM_FOLDEROPENMID,   wxSCI_MARK_BOXMINUSCONNECTED,fore, outline);
        MarkerDefine(wxSCI_MARKNUM_FOLDERSUB,       wxSCI_MARK_VLINE,           fore, outline);
        MarkerDefine(wxSCI_MARKNUM_FOLDERTAIL,      wxSCI_MARK_LCORNER,         fore, outline);

        SetMarginSensitive (MARGIN_FOLDING, 1);
        SetProperty (_T("fold"),                _T("1"));
        SetProperty (_T("fold.comment"),        _T("1"));
        SetProperty (_T("fold.compact"),        _T("1"));
        SetProperty (_T("fold.preprocessor"),   _T("1"));
    }

    // Show modification margin
    if (reg["editor.showChangeMargin"].AsBool(true))
    {
        const CStyleInfo & info = styles->GetStyle(_T(".edit-margin"));
        m_data->m_changeMargin = true;

        SetMarginWidth(MARGIN_CHANGE, 4);
        SetMarginType(MARGIN_CHANGE,  wxSCI_MARGIN_SYMBOL);
        SetMarginWidth(MARGIN_CHANGE, 4);
        SetMarginMask(MARGIN_CHANGE, (1 << CEditorModMargin::MARKER_SAVED) | (1 << CEditorModMargin::MARKER_EDITED) );

        // edited marker
        MarkerDefine(CEditorModMargin::MARKER_EDITED, wxSCI_MARK_FULLRECT);
        MarkerSetBackground(CEditorModMargin::MARKER_EDITED, info.bg);

        // saved marker
        MarkerDefine(CEditorModMargin::MARKER_SAVED, wxSCI_MARK_FULLRECT);
        MarkerSetBackground(CEditorModMargin::MARKER_SAVED, info.fg);
    }

    // process TAB key manually to allow
    // adding switching between tabs ????
    // CmdKeyClear (wxSCI_KEY_TAB, 0);

}


/**
 * Set style
 */
void CEditor::SetStyle (int nr, const CStyleInfo & styleInfo)
{
    StyleSetFaceName    (nr, styleInfo.font);
    StyleSetSize        (nr, styleInfo.size);
    StyleSetItalic      (nr, styleInfo.italic);
    StyleSetBold        (nr, styleInfo.bold);
    StyleSetUnderline   (nr, styleInfo.underlined);
    StyleSetForeground  (nr, styleInfo.fg);
    StyleSetBackground  (nr, styleInfo.bg);
}



/**
 * Save the file
 */
void CEditor::SaveFile (const wxString & file)
{
    wxScintilla::SaveFile(file);
    m_data->m_modMargin->SetSavePos (  );
}



/**
 * Load file
 */
void CEditor::LoadFile (const wxString & file)
{
    m_data->m_ready = false;
    wxScintilla::LoadFile(file);
    m_data->m_ready = true;
}

