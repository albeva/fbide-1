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
#include "sdk/UiManager.h"
#include "sdk/PluginManager.h"
#include "sdk/TypeManager.h"
#include "sdk/Document.h"
#include "sdk/DocManager.h"
#include "sdk/StyleInfo.h"
#include "sdk/Editor.h"

#include "LineStates.h"
#include "LexFreeBasic.hxx"

using namespace fb;
using namespace LEX;


// this is test code...
struct FreeBasicEditor : public CDocument<CEditor, DOCUMENT_MANAGED>
{
    FreeBasicEditor (const wxString & file = _T(""))
    {
        SetDocTitle (::wxFileNameFromPath(file));

        CUiManager & ui = *CManager::Get()->GetUiManager();
        CRegistry & reg = GET_REG();

        m_loaded = false;

        // construct the editor
        CEditor::Create (
            ui.GetDocWindow(),
            wxID_ANY,
            wxDefaultPosition, wxDefaultSize,
            wxBORDER_SIMPLE
        );

        // Set editor styling
        SetLexer (LEX::FREEBASIC);
        CStyleParser * parser = GET_MGR()->GetStyleParser(_T("default"));
        if (parser) LoadStyles (parser);

        // Set m_lineStates
        wxString lineStates;
        lineStates << (int)&m_lineStates;
        SetProperty(_T("lineStates"), lineStates);

        // Set vars...
        m_lineStates.m_dialect = DIALECT_QB;
        m_lineStates.m_drawIndentedBoxes = reg["editor.drawIndentedBoxes"].AsBool(true);
        m_lineStates.m_tabWidth = reg["editor.tabWidth"].AsInt(4);
        m_lineStates.m_optionEscape = false;

        // Load the keywords
        SetKeyWords(KEYWORD_ONE,      reg["fb.keywords.one"].AsString());
        SetKeyWords(KEYWORD_TWO,      reg["fb.keywords.two"].AsString());
        SetKeyWords(KEYWORD_THREE,    reg["fb.keywords.three"].AsString());
        SetKeyWords(KEYWORD_FOUR,     reg["fb.keywords.four"].AsString());
        SetKeyWords(KEYWORD_PP_ONE,   reg["fb.keywords.preprocessor.one"].AsString());
        SetKeyWords(KEYWORD_PP_TWO,   reg["fb.keywords.preprocessor.two"].AsString());
        SetKeyWords(KEYWORD_ASM_ONE,  reg["fb.keywords.asm.one"].AsString());
        SetKeyWords(KEYWORD_ASM_TWO,  reg["fb.keywords.asm.two"].AsString());

        // define error indicator. (should this be moved to CEditor too?)
        IndicatorSetStyle(0, wxSCI_INDIC_SQUIGGLE);
        IndicatorSetForeground(0, wxColor(_T("red")));

        // Load editor configuration
        Setup(parser);

        // Load the file
        if (file.Len()) LoadFile (file);

        // mark editor as ready and fully loaded
        MarkReady(true);
    }


    /**
     * Load the styles
     */
    void LoadStyles (CStyleParser * parser)
    {

        // 63 possible LEX states (minus 32-39)
        SetStyleBits(6);

        // get styles default
        CStyleInfo info;
        CStyleInfo defInfo  = parser->GetStyle(_T(".default"));
        CStyleInfo asmInfo  = parser->GetStyle(_T(".asm"));
        CStyleInfo ppInfo   = parser->GetStyle(_T(".preprocessor"));

        // normalize default
        if (!defInfo.bg.IsOk()) defInfo.bg = wxColor(_T("white"));

        // default
        SetStyle(FB_DEFAULT,            defInfo);

        // identifier
        info = parser->GetStyle(_T(".identifier"));
        bool hasBg = info.bg.IsOk();

        if (!hasBg) info.bg = defInfo.bg;
        SetStyle(FB_IDENTIFIER,         info);
        if (!hasBg) info.bg = asmInfo.bg;
        SetStyle(ASM_IDENTIFIER,        info);
        if (!hasBg) info.bg = ppInfo.bg;
        SetStyle(PP_IDENTIFIER,         info);

        // comment
        info = parser->GetStyle(_T(".comment"));
        hasBg = info.bg.IsOk();

        if (!hasBg) info.bg = defInfo.bg;
        SetStyle(FB_COMMENT,            info);
        // multiline comments should fille the line
        StyleSetEOLFilled(FB_COMMENT,   true);
        if (!hasBg) info.bg = asmInfo.bg;
        SetStyle(ASM_COMMENT,           info);
        if (!hasBg) info.bg = ppInfo.bg;
        SetStyle(PP_COMMENT,            info);

        // comment url
        info = parser->GetStyle(_T(".comment.url"));
        hasBg = info.bg.IsOk();

        if (!hasBg) info.bg = defInfo.bg;
        SetStyle(FB_COMMENT_URL,        info);
        StyleSetEOLFilled(FB_COMMENT_URL,   true);
        if (!hasBg) info.bg = asmInfo.bg;
        SetStyle(ASM_COMMENT_URL,       info);
        if (!hasBg) info.bg = ppInfo.bg;
        SetStyle(PP_COMMENT_URL,        info);

        // label
        info = parser->GetStyle(_T(".label"));
        hasBg = info.bg.IsOk();

        if (!hasBg) info.bg = defInfo.bg;
        SetStyle(FB_LABEL,              info);
        if (!hasBg) info.bg = asmInfo.bg;
        SetStyle(ASM_LABEL,             info);
        if (!hasBg) info.bg = ppInfo.bg;
        SetStyle(PP_LABEL,              info);

        // string
        info = parser->GetStyle(_T(".string"));
        hasBg = info.bg.IsOk();

        if (!hasBg) info.bg = defInfo.bg;
        SetStyle(FB_STRING,             info);
        if (!hasBg) info.bg = asmInfo.bg;
        SetStyle(ASM_STRING,            info);
        if (!hasBg) info.bg = ppInfo.bg;
        SetStyle(PP_STRING,             info);

        // string edcapes
        info = parser->GetStyle(_T(".string.escape"));
        hasBg = info.bg.IsOk();

        if (!hasBg) info.bg = defInfo.bg;
        SetStyle(FB_STRING_ESCAPE,      info);
        if (!hasBg) info.bg = asmInfo.bg;
        SetStyle(ASM_STRING_ESCAPE,     info);
        if (!hasBg) info.bg = ppInfo.bg;
        SetStyle(PP_STRING_ESCAPE,      info);

        // number
        info = parser->GetStyle(_T(".number"));
        hasBg = info.bg.IsOk();

        if (!hasBg) info.bg = defInfo.bg;
        SetStyle(FB_NUMBER,             info);
        if (!hasBg) info.bg = asmInfo.bg;
        SetStyle(ASM_NUMBER,            info);
        if (!hasBg) info.bg = ppInfo.bg;
        SetStyle(PP_NUMBER,             info);

        // operator
        info = parser->GetStyle(_T(".operator"));
        hasBg = info.bg.IsOk();

        if (!hasBg) info.bg = defInfo.bg;
        SetStyle(FB_OPERATOR,           info);
        if (!hasBg) info.bg = asmInfo.bg;
        SetStyle(ASM_OPERATOR,          info);
        if (!hasBg) info.bg = ppInfo.bg;
        SetStyle(PP_OPERATOR,           info);

        // FB keywords
        info = parser->GetStyle(_T(".keyword.one"));
        hasBg = info.bg.IsOk();

        if (!hasBg) info.bg = defInfo.bg;
        SetStyle(FB_KEYWORD_ONE,        info);
        if (!hasBg) info.bg = asmInfo.bg;
        SetStyle(ASM_FB_KEYWORD_ONE,    info);
        if (!hasBg) info.bg = ppInfo.bg;
        SetStyle(PP_FB_KEYWORD_ONE,     info);

        info = parser->GetStyle(_T(".keyword.two"));
        hasBg = info.bg.IsOk();

        if (!hasBg) info.bg = defInfo.bg;
        SetStyle(FB_KEYWORD_TWO,        info);
        if (!hasBg) info.bg = asmInfo.bg;
        SetStyle(ASM_FB_KEYWORD_TWO,    info);
        if (!hasBg) info.bg = ppInfo.bg;
        SetStyle(PP_FB_KEYWORD_TWO,     info);

        info = parser->GetStyle(_T(".keyword.three"));
        hasBg = info.bg.IsOk();

        if (!hasBg) info.bg = defInfo.bg;
        SetStyle(FB_KEYWORD_THREE,      info);
        if (!hasBg) info.bg = asmInfo.bg;
        SetStyle(ASM_FB_KEYWORD_THREE,  info);
        if (!hasBg) info.bg = ppInfo.bg;
        SetStyle(PP_FB_KEYWORD_THREE,   info);

        info = parser->GetStyle(_T(".keyword.four"));
        hasBg = info.bg.IsOk();

        if (!hasBg) info.bg = defInfo.bg;
        SetStyle(FB_KEYWORD_FOUR,       info);
        if (!hasBg) info.bg = asmInfo.bg;
        SetStyle(ASM_FB_KEYWORD_FOUR,   info);
        if (!hasBg) info.bg = ppInfo.bg;
        SetStyle(PP_FB_KEYWORD_FOUR,    info);

        // ASM block
        SetStyle(ASM_DEFAULT,           asmInfo);
        SetStyle(ASM_KEYWORD_ONE,       parser->GetStyle(_T(".asm.keyword.one")));
        SetStyle(ASM_KEYWORD_TWO,       parser->GetStyle(_T(".asm.keyword.two")));
        // make all styles in ASM block fill the line
        for (int i = ASM_DEFAULT; i <= ASM_SECTION_LAST; i++)
            StyleSetEOLFilled(i,   true);

        // PP block
        SetStyle(PP_DEFAULT,            ppInfo);
        SetStyle(PP_KEYWORD_ONE,        parser->GetStyle(_T(".preprocessor.keyword.one")));
        SetStyle(PP_KEYWORD_TWO,        parser->GetStyle(_T(".preprocessor.keyword.two")));

        // make all pp stataments will the line
        for (int i = PP_DEFAULT; i <= PP_SECTION_LAST; i++)
            StyleSetEOLFilled(i,   true);

    }


    /**
     * Save file
     */
    virtual bool SaveDoc (const wxString & file)
    {
        CEditor::SaveFile(file);
        SetDocTitle(::wxFileNameFromPath(file));
    }


    DECLARE_EVENT_TABLE();
    CLineStates             m_lineStates;
    bool                    m_loaded;
};


BEGIN_EVENT_TABLE (FreeBasicEditor, CEditor)
    // EVT_KEY_DOWN (FreeBasicEditor::OnKeyDown)
    // EVT_SCI_MARGINCLICK (-1, FreeBasicEditor::OnMarginClick)
    // EVT_SCI_MODIFIED    (-1, FreeBasicEditor::OnModified)
END_EVENT_TABLE()


/**
 * the plugin class
 */
class FBIdePlugin : public CPluginBase
{
    public :

        // Attach plugin
        bool Attach ()
        {
            CTypeManager * tm = GET_TYPEMGR();
            tm->AddType(_T("text/fb"), _T("Freebasic files"), new CTypeRegistrant<FreeBasicEditor>);
            tm->BindExtensions(_T("text/fb"), _T("bas;bi"));

            if (!tm->TypeExists(_T("default")))
                tm->BindAlias(_T("default"), _T("text/fb"));
            return true;
        }

        // Detach plugin
        bool Detach (bool force)
        {
            return true;
        }
};

namespace
{
    CPluginProvider<FBIdePlugin, PLUGINTYPE_EDITOR> plugin (_T("FBIdePlugin"));
}
