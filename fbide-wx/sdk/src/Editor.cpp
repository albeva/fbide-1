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

#include <stdlib.h>

// #include "sdk/LexFreeBasic.hxx"

using namespace fb;

/**
 * Constants
 */
enum
{
    MARKER_SAVED    = 0,
    MARKER_EDITED   = 1,
    MARKER_FLAGS    = (1 << MARKER_SAVED) | (1 << MARKER_EDITED),

    MARGIN_LINENUMBERS  = 0,
    MARGIN_CHANGE       = 1,
    MARGIN_FOLDING      = 2
};


/*
 * Simplified logging macros
 */
#define LOG_MSG(_msg)           wxLogMessage(_T("%s\t\t\t(line: %d)"), _T(_msg), (int)__LINE__);
#define LOG_INT(_v)             wxLogMessage(_T("%s = %d\t\t\t(line: %d)"), _T(#_v), (int)_v, (int)__LINE__);
#define LOG_MSG_INT(_msg, _v)   wxLogMessage(_T("%s %s = %d\t\t\t(line: %d)"), _T(_msg), _T(#_v), (int)_v, (int)__LINE__);


/**
 * Private data implementation for the CEditor
 */
struct CEditor::CData : public wxEvtHandler
{

    /**
     * Track changes per line. Needed for
     * modification margin
     */
    struct CChangeHistory
    {

        // Number of elements allocaed per block.
        enum {
            STATES_IN_CELL  = 16,
            BITS_PER_STATE  = 2,
            STATES_FLAG     = (2 << BITS_PER_STATE) - 1
        };

        /**
         * Construct new history object
         */
        CChangeHistory () {}

        // Set bit flags
        static inline void SET_FLAG (int & value, int flag) {value |= flag;}

        // unset bit flags
        static inline void UNSET_FLAG (int & value, int flag) {value &= (~flag);}


        /**
         * Pop topmost and check next value
         */
        int Pop (int line)
        {
            // no state here
            if ((int)m_lines.size() <= line) return false;

            // get mod
            IntVector & mod = m_lines[line];

            // if it's too small?
            if (mod.size() < 2) return false;

            // no states? pop topmost
            if (mod[0] == 0) return false;
            int cellCount = --mod[0];
            LOG_MSG_INT("Pop:", cellCount);

            // get offset and index into bitset
            int offset = (cellCount % STATES_IN_CELL) * BITS_PER_STATE;
            int index  = (cellCount / STATES_IN_CELL) + 1;

            // get the bit value
            return (mod[index] >> offset) & STATES_FLAG;
        }


        /**
         * check current state and "undo"
         */
        int CheckAndRedo (int line)
        {
            // no state here
            if ((int)m_lines.size() <= line) return 0;

            // get mod
            IntVector & mod = m_lines[line];

            // if it's too small?
            if (mod.size() < 2) return 0;

            // get counter
            int cellCount = ++mod[0];
            LOG_MSG_INT("CheckAndRedo:", cellCount);
            // if (cellCount == 0) return 0;

            // get offset and index into bitset
            int offset = (cellCount % STATES_IN_CELL) * BITS_PER_STATE;
            int index  = (cellCount / STATES_IN_CELL) + 1;

            // get the bit value
            return (mod[index] >> offset) & STATES_FLAG;
        }


        /**
         * Push line state
         */
        void Push(int line, int value)
        {
            if (line >= (int)m_lines.size())
            {
                // m_lines.resize(m_lines.capacity() + BUFFER_SIZE);
                for (int i = m_lines.size(); i <= line; i++)
                    m_lines.push_back(IntVector());
            }

            // get mod
            IntVector & mod = m_lines[line];

            // get the cell or add a new entry
            if (mod.size() < (size_t)2)
            {
                mod.push_back(0);
                mod.push_back(0);
            }

            // get counter and increment count
            int cellCount = mod[0]++;
            LOG_MSG_INT("Push:", cellCount);

            // get offset (bit offset) and index
            int offset = (cellCount % STATES_IN_CELL) * BITS_PER_STATE;
            int index  = (cellCount / STATES_IN_CELL) + 1;

            // expand array if needed
            if ((int)mod.size() <= index) mod.push_back(0);

            // set cell value
            mod[index] = (mod[index] & (~(STATES_FLAG<<offset))) | ((value & STATES_FLAG) << offset);
        }


        /**
         * Set current state to given value
         */
        void SetCurrent (int line, int value)
        {
            // no state for the line?
            if (line >= (int)m_lines.size()) return;

            // get mod
            IntVector & mod = m_lines[line];

            // no state for the line yet?
            if (mod.size() < (size_t)2) return;

            // no states?
            int cellCount = mod[0];
            LOG_MSG_INT("SetCurrent:", cellCount);
            // if (cellCount == 0) return;
            // cellCount--;

            // get offset (bit offset) and index
            int offset = (cellCount % STATES_IN_CELL) * BITS_PER_STATE;
            int index  = (cellCount / STATES_IN_CELL) + 1;

            // set cell value
            mod[index] = (mod[index] & (~(STATES_FLAG<<offset))) | ((value & STATES_FLAG) << offset);
        }


        /**
         * Set all states to
         */
        void SetAllAndCurrentTo (CEditor * editor, int state, int currentState)
        {
            // generate common flag
            int flag = (state & STATES_FLAG);
            for (int offset = BITS_PER_STATE;
                 offset < (STATES_IN_CELL * BITS_PER_STATE);
                 offset += BITS_PER_STATE)
                flag |= (state & STATES_FLAG) << offset;

            // iterate through the lines
            for (int line = 0; line < m_lines.size(); line++)
            {
                for (int cell = 1; cell < m_lines[line].size(); cell++)
                {
                    m_lines[line][cell] = flag;
                }
                if (m_lines[line].size() > 1)
                {
                    SetCurrent(line, currentState);
                    SetMargins(editor, line, currentState);
                }
            }
        }


        /**
         * Add line to pending lines.
         */
        void AddPendingLine (int line)
        {
            // add the line
            m_pending.push_back(line);
        }


        /**
         * Commit pending lines
         */
        void PushAndMarkPending (CEditor * editor, int markers)
        {
            for ( int i = m_pending.size(); i ; )
            {
                int line = m_pending[--i];
                Push (line, editor->MarkerGet(line) & MARKER_FLAGS);
                SetMargins(editor, line, markers);
            }

            // reset pending
            m_pending.clear();
        }


        /**
         * Pop pending
         */
        void PopPending (CEditor * editor)
        {
            // Push the states
            // LOG_MSG_INT("PopPending:", m_pending.size());
            for ( int i = m_pending.size(); i ; )
            {
                int line = m_pending[--i];
                SetCurrent(line, editor->MarkerGet(line) & MARKER_FLAGS);
                SetMargins(editor, line, Pop(line));
            }

            // reset pending
            m_pending.clear();
        }


        /**
         * Undo pending
         */
        void UndoPending (CEditor * editor)
        {
            // Push the states
            // LOG_MSG_INT("UndoPending:", m_pending.size());
            for ( int i = m_pending.size(); i ; )
            {
                int line = m_pending[--i];
                SetMargins(editor, line, CheckAndRedo(line));
            }

            // reset pending
            m_pending.clear();
        }


        /**
         * Set editor margins
         */
        void SetMargins (CEditor * editor, int line, int markers)
        {
            // update margin
            if (markers & (1 << MARKER_EDITED))
                editor->MarkerAdd(line, MARKER_EDITED);
            else
                editor->MarkerDelete(line, MARKER_EDITED);

            // set saved
            if (markers & (1 << MARKER_SAVED))
                editor->MarkerAdd(line, MARKER_SAVED);
            else
                editor->MarkerDelete(line, MARKER_SAVED);
        }

        // vector of int's
        typedef std::vector<int>            IntVector;

        // the lines vector
        typedef std::vector<IntVector>      LinesVector;

        LinesVector m_lines;
        IntVector   m_pending;
    };


    // simple constructor
    CData (CEditor * parent)
        : m_parent(parent),
          m_dynamicLineNumberWidth(false),
          m_changeMargin(false),
          m_commitChanges(false),
          m_width3(0), m_width4(0), m_width5(0),
          m_width6(0), m_width7(0)
    {}


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
        int editFlags = wxSCI_MOD_INSERTTEXT | wxSCI_MOD_DELETETEXT;
        int mod = event.GetModificationType();

        // content has changed
        if (mod & editFlags)
        {
            // update change margin
            if (m_changeMargin) updateChangeMargin (event);
        }
    }


    // Manage change margin
    void updateChangeMargin (wxScintillaEvent & event)
    {
        // modification type
        int mod = event.GetModificationType();

        // get line range affected
        int startLine = m_parent->LineFromPosition(event.GetPosition());
        int linesModified = event.GetLinesAdded();

        // append affected lines
        m_changes.AddPendingLine(startLine);

        if (linesModified > 0)
        {
            for ( ; linesModified > 0; linesModified-- )
                m_changes.AddPendingLine(startLine + linesModified);
        }
        else if (linesModified < 0)
        {
            int delLines = abs(linesModified);
            if (mod & wxSCI_PERFORMED_USER)
            {
                for ( ; delLines; delLines--)
                {
                    int line = startLine + delLines;
                    LOG_INT(line);
                    m_changes.Push (
                        line,
                        m_parent->MarkerGet(line) & MARKER_FLAGS
                    );
                }
            }
            else
            {
                LOG_MSG_INT("----------->", delLines);
                for ( ; delLines; delLines--)
                    m_changes.AddPendingLine(startLine + delLines);
            }
        }

        // performed by user (edit, delete, insert)
        if (mod & wxSCI_PERFORMED_USER)
        {
            m_changes.PushAndMarkPending(m_parent, 1 << MARKER_EDITED);
        }
        // performed UNDO
        else if (mod & wxSCI_PERFORMED_UNDO)
        {
            m_changes.PopPending(m_parent);
        }
        // performed REDO
        else if (mod & wxSCI_PERFORMED_REDO)
        {
            m_changes.UndoPending(m_parent);
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

    bool        m_commitChanges;

    int         m_width3;
    int         m_width4;
    int         m_width5;
    int         m_width6;
    int         m_width7;

    CChangeHistory m_changes;

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

    // collect changes.
    // SetChangeCollection(true);

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
        CStyleInfo info = styles->GetStyle(_T(".edit-margin"));
        m_data->m_changeMargin = true;

        SetMarginWidth(MARGIN_CHANGE, 4);
        SetMarginType(MARGIN_CHANGE,  wxSCI_MARGIN_SYMBOL);
        SetMarginWidth(MARGIN_CHANGE, 4);
        SetMarginMask(MARGIN_CHANGE, (1 << MARKER_SAVED) | (1 << MARKER_EDITED) );

        // edited marker
        MarkerDefine(MARKER_EDITED, wxSCI_MARK_FULLRECT);
        MarkerSetBackground(MARKER_EDITED, info.bg);

        // saved marker
        MarkerDefine(MARKER_SAVED, wxSCI_MARK_FULLRECT);
        MarkerSetBackground(MARKER_SAVED, info.fg);
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

    // update lines
    /*for (int line = 0; line < GetLineCount(); line++)
    {
        if (MarkerGet(line) & (1 << MARKER_EDITED))
        {
            // all lines uniqe no need to check...
            m_data->m_changes.AddPendingLine(line);
            // remove modified markers and set saved
            // MarkerDelete(line, MARKER_EDITED);
            // MarkerAdd(line, MARKER_SAVED);
        }
    }*/

    // commit edit changed...
    m_data->m_changes.SetAllAndCurrentTo (
        this,
        (1 << MARKER_EDITED),
        (1 << MARKER_SAVED)
    );
    // m_data->m_changes.ChangeCurrent(this, (1 << MARKER_EDITED));
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

