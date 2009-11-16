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
#include "FreeBasic.h"
#include "LexFreeBasic.h"
#include "LineStates.h"

#include "../StcEditor.h"
#include "../StyleInfo.h"
#include "../Editor.h"
#include "../../Manager.h"
#include "../../EditorManager.h"
#include "../../UiManager.h"

using namespace fbi;
using namespace LEX;


FreeBasicSyntax::FreeBasicSyntax (StcEditor * stc)
    : m_stc(stc)
{
    // Set editor styling
    m_stc->SetLexer (LEX::FREEBASIC);
    StyleParser * parser = GET_EDITORMGR()->GetStyle();

    // Set m_lineStates
    m_lineStates = new CLineStates();
    wxString lineStates;
    lineStates << (int) m_lineStates;
    m_stc->SetProperty("lineStates", lineStates);

    // get registry
    auto & reg = GET_REG();

    // Set vars...
    m_lineStates->m_dialect = LEX::DIALECT_QB;
    m_lineStates->m_drawIndentedBoxes = reg["editor.drawIndentedBoxes"].AsBool(true);
    m_lineStates->m_tabWidth = reg["editor.tabWidth"].AsInt(4);
    m_lineStates->m_optionEscape = false;

    // Load the keywords
    m_stc->SetKeyWords(KEYWORD_ONE,      reg["fb.keywords.one"].AsString("asm end"));
    m_stc->SetKeyWords(KEYWORD_TWO,      reg["fb.keywords.two"].AsString("print"));
    m_stc->SetKeyWords(KEYWORD_THREE,    reg["fb.keywords.three"].AsString("and or xor not"));
    m_stc->SetKeyWords(KEYWORD_FOUR,     reg["fb.keywords.four"].AsString("abs rgb rgba"));
    m_stc->SetKeyWords(KEYWORD_PP_ONE,   reg["fb.keywords.preprocessor.one"].AsString("include ifdef macro endmacro"));
    m_stc->SetKeyWords(KEYWORD_PP_TWO,   reg["fb.keywords.preprocessor.two"].AsString("define defined"));
    m_stc->SetKeyWords(KEYWORD_ASM_ONE,  reg["fb.keywords.asm.one"].AsString("mov inc dec add sub"));
    m_stc->SetKeyWords(KEYWORD_ASM_TWO,  reg["fb.keywords.asm.two"].AsString("eax ebx ecx edx abp esp"));

    // define error indicator. (should this be moved to CEditor too?)
    m_stc->IndicatorSetStyle(0, wxSCI_INDIC_SQUIGGLE);
    m_stc->IndicatorSetForeground(0, wxColor("red"));


    // 63 possible LEX states (minus 32-39)
    m_stc->SetStyleBits(6);

    // get styles default
    StyleInfo info;
    StyleInfo defInfo  = parser->GetStyle(".default");
    StyleInfo asmInfo  = parser->GetStyle(".asm");
    StyleInfo ppInfo   = parser->GetStyle(".preprocessor");

    // normalize default
    if (!defInfo.bg.IsOk()) defInfo.bg = wxColor("white");

    // default
    m_stc->SetStyle(FB_DEFAULT, defInfo);

    // identifier
    info = parser->GetStyle(".identifier");
    bool hasBg = info.bg.IsOk();

    if (!hasBg) info.bg = defInfo.bg;
    m_stc->SetStyle(FB_IDENTIFIER,         info);
    if (!hasBg) info.bg = asmInfo.bg;
    m_stc->SetStyle(ASM_IDENTIFIER,        info);
    if (!hasBg) info.bg = ppInfo.bg;
    m_stc->SetStyle(PP_IDENTIFIER,         info);

    // comment
    info = parser->GetStyle(".comment");
    hasBg = info.bg.IsOk();

    if (!hasBg) info.bg = defInfo.bg;
    m_stc->SetStyle(FB_COMMENT,            info);
    // multiline comments should fille the line
    m_stc->StyleSetEOLFilled(FB_COMMENT,   true);
    if (!hasBg) info.bg = asmInfo.bg;
    m_stc->SetStyle(ASM_COMMENT,           info);
    if (!hasBg) info.bg = ppInfo.bg;
    m_stc->SetStyle(PP_COMMENT,            info);

    // comment url
    info = parser->GetStyle(".comment.url");
    hasBg = info.bg.IsOk();

    if (!hasBg) info.bg = defInfo.bg;
    m_stc->SetStyle(FB_COMMENT_URL,        info);
    m_stc->StyleSetEOLFilled(FB_COMMENT_URL,   true);
    if (!hasBg) info.bg = asmInfo.bg;
    m_stc->SetStyle(ASM_COMMENT_URL,       info);
    if (!hasBg) info.bg = ppInfo.bg;
    m_stc->SetStyle(PP_COMMENT_URL,        info);

    // label
    info = parser->GetStyle(".label");
    hasBg = info.bg.IsOk();

    if (!hasBg) info.bg = defInfo.bg;
    m_stc->SetStyle(FB_LABEL,              info);
    if (!hasBg) info.bg = asmInfo.bg;
    m_stc->SetStyle(ASM_LABEL,             info);
    if (!hasBg) info.bg = ppInfo.bg;
    m_stc->SetStyle(PP_LABEL,              info);

    // string
    info = parser->GetStyle(".string");
    hasBg = info.bg.IsOk();

    if (!hasBg) info.bg = defInfo.bg;
    m_stc->SetStyle(FB_STRING,             info);
    if (!hasBg) info.bg = asmInfo.bg;
    m_stc->SetStyle(ASM_STRING,            info);
    if (!hasBg) info.bg = ppInfo.bg;
    m_stc->SetStyle(PP_STRING,             info);

    // string edcapes
    info = parser->GetStyle(".string.escape");
    hasBg = info.bg.IsOk();

    if (!hasBg) info.bg = defInfo.bg;
    m_stc->SetStyle(FB_STRING_ESCAPE,      info);
    if (!hasBg) info.bg = asmInfo.bg;
    m_stc->SetStyle(ASM_STRING_ESCAPE,     info);
    if (!hasBg) info.bg = ppInfo.bg;
    m_stc->SetStyle(PP_STRING_ESCAPE,      info);

    // number
    info = parser->GetStyle(".number");
    hasBg = info.bg.IsOk();

    if (!hasBg) info.bg = defInfo.bg;
    m_stc->SetStyle(FB_NUMBER,             info);
    if (!hasBg) info.bg = asmInfo.bg;
    m_stc->SetStyle(ASM_NUMBER,            info);
    if (!hasBg) info.bg = ppInfo.bg;
    m_stc->SetStyle(PP_NUMBER,             info);

    // operator
    info = parser->GetStyle(".operator");
    hasBg = info.bg.IsOk();

    if (!hasBg) info.bg = defInfo.bg;
    m_stc->SetStyle(FB_OPERATOR,           info);
    if (!hasBg) info.bg = asmInfo.bg;
    m_stc->SetStyle(ASM_OPERATOR,          info);
    if (!hasBg) info.bg = ppInfo.bg;
    m_stc->SetStyle(PP_OPERATOR,           info);

    // FB keywords
    info = parser->GetStyle(".keyword.one");
    hasBg = info.bg.IsOk();

    if (!hasBg) info.bg = defInfo.bg;
    m_stc->SetStyle(FB_KEYWORD_ONE,        info);
    if (!hasBg) info.bg = asmInfo.bg;
    m_stc->SetStyle(ASM_FB_KEYWORD_ONE,    info);
    if (!hasBg) info.bg = ppInfo.bg;
    m_stc->SetStyle(PP_FB_KEYWORD_ONE,     info);

    info = parser->GetStyle(".keyword.two");
    hasBg = info.bg.IsOk();

    if (!hasBg) info.bg = defInfo.bg;
    m_stc->SetStyle(FB_KEYWORD_TWO,        info);
    if (!hasBg) info.bg = asmInfo.bg;
    m_stc->SetStyle(ASM_FB_KEYWORD_TWO,    info);
    if (!hasBg) info.bg = ppInfo.bg;
    m_stc->SetStyle(PP_FB_KEYWORD_TWO,     info);

    info = parser->GetStyle(".keyword.three");
    hasBg = info.bg.IsOk();

    if (!hasBg) info.bg = defInfo.bg;
    m_stc->SetStyle(FB_KEYWORD_THREE,      info);
    if (!hasBg) info.bg = asmInfo.bg;
    m_stc->SetStyle(ASM_FB_KEYWORD_THREE,  info);
    if (!hasBg) info.bg = ppInfo.bg;
    m_stc->SetStyle(PP_FB_KEYWORD_THREE,   info);

    info = parser->GetStyle(".keyword.four");
    hasBg = info.bg.IsOk();

    if (!hasBg) info.bg = defInfo.bg;
    m_stc->SetStyle(FB_KEYWORD_FOUR,       info);
    if (!hasBg) info.bg = asmInfo.bg;
    m_stc->SetStyle(ASM_FB_KEYWORD_FOUR,   info);
    if (!hasBg) info.bg = ppInfo.bg;
    m_stc->SetStyle(PP_FB_KEYWORD_FOUR,    info);

    // ASM block
    m_stc->SetStyle(ASM_DEFAULT,           asmInfo);
    m_stc->SetStyle(ASM_KEYWORD_ONE,       parser->GetStyle(".asm.keyword.one"));
    m_stc->SetStyle(ASM_KEYWORD_TWO,       parser->GetStyle(".asm.keyword.two"));
    // make all styles in ASM block fill the line
    for (int i = ASM_DEFAULT; i <= ASM_SECTION_LAST; i++)
        m_stc->StyleSetEOLFilled(i,   true);

    // PP block
    m_stc->SetStyle(PP_DEFAULT,            ppInfo);
    m_stc->SetStyle(PP_KEYWORD_ONE,        parser->GetStyle(".preprocessor.keyword.one"));
    m_stc->SetStyle(PP_KEYWORD_TWO,        parser->GetStyle(".preprocessor.keyword.two"));

    // make all pp stataments will the line
    for (int i = PP_DEFAULT; i <= PP_SECTION_LAST; i++)
        m_stc->StyleSetEOLFilled(i,   true);

}
