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

/**
 * @todo asm /' ... \n ... '/ should decide is there a single line
 *       asm block or multiline. Implement methods "ResolveMultiLineComment"
 *
 * @todo Reseolve line continuation for ASM blocks
 *       asm _ \n could be either one liner or a multiline
 *
 * @todo end /' ... \n ... '/ asm should work. as well as
 *       end _ \n asm
 *       need to have LINE_STATE_END_PENDING ? that would persist while
 *       line is continued or a multiline comment comes between ?
 *       Don't care about that for now...
 *
 * @todo implement options to disable certain highlighting features
 *       escape sequences, block highlighting, ...
 */

#include "sdk_pch.h"

#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdio.h>
#include <stdarg.h>

#include "Platform.h"

#include "PropSet.h"
#include "Accessor.h"
#include "StyleContext.h"
#include "KeyWords.h"
#include "Scintilla.h"
#include "SciLexer.h"

#include "LineStates.h"
#include "LexFreeBasic.h"

using namespace LEX;


/**
 * Allowed URI Schemes inside comments.
 * case insensitive.
 */
static const char * const URI_SCHEMES[] = {
    "http://",
    "https://",
    "ftp://",
    "www.",
    0
};


/**
 * Allowed (2 char long) escape codes inside strings
 * preceded by \
 * case sensitive!
 */
static const char ESCAPE_CODES[] = {
    'a',        // beep
    'b',        // backspace
    'f',        // formfeed
    'l', 'n',   // new line
    'r',        // carriage return
    't',        // tab
    'v',        // vertical tab
    '\\',       // backslash
    '"',        // double quote
    '\'',       // single quote
    0
};


/**
 * Integer literal suffixes
 * Multi char
 */
static const char * const INTEGER_SUFFIXES_MC[] = {
    "ull", "ul", "ll",
    0
};
/**
 * Integer suffixes single char
 */
static const char INTEGER_SUFFIXES_SC[] = {
    'l', '%', '&', 'u',
    0
};


// Set bit flags
static inline void SET_FLAG (int & value, int flag) {value |= flag;}

// unset bit flags
static inline void UNSET_FLAG (int & value, int flag) {value &= (~flag);}

// toggle bit flags
static inline void TOGGLE_FLAG (int & value, int flag) {value ^= flag;}

#define USE_LOG

// Logging macros
#ifdef USE_LOG
    #define LOG(str, ...) wxLogMessage(str, __VA_ARGS__)
#else
    #define LOG(str, ...)
#endif

#define DUMP_INT_VAR(_v) LOG("%s = %d\t\t\t(line: %d)", #_v, (int)_v, (int)__LINE__);


/**
 * Highlight fb source code
 */
struct CFbSourceLexer
{

    /**
     * Construct the lexer object
     */
    CFbSourceLexer (StyleContext &sc, Accessor &styler, WordList *keywords[])
    :   m_sc(sc),
        m_styler(styler),
        m_keywords(keywords),
        m_section(0),
        m_visibleChars(0),
        m_indent(0),
        m_tabWidth(4),
        m_skipForward(false),
        m_indentedBoxes(false),
        m_stateStack(NULL),
        m_curNppLevel(0),
        m_curNcLevel(0),
        m_nLcLevel(0),
        m_lineCont(false)
    {

        // setup the state
        m_curLine       = styler.GetLine(m_sc.currentPos);
        m_stateStack    = (CLineStates*) styler.GetPropertyInt("lineStates", NULL);

        // can't do anything
        if (m_stateStack==0) return;
        m_dialect       = m_stateStack->m_dialect;
        m_indentedBoxes = m_stateStack->m_drawIndentedBoxes;
        m_tabWidth      = m_stateStack->m_tabWidth;


        // clear further state.
        m_stateStack->ClearFrom(m_curLine);

        // get line state
        m_lineState = m_curLine > 0 ? m_styler.GetLineState(m_curLine-1) : 0;

        // if there is line state get info...
        DUMP_INT_VAR(m_lineState);
        if (m_lineState)
        {
            // nested pp level
            m_curNppLevel   = m_lineState & 0xFF;
            m_nLcLevel      = (m_lineState >> 8) & 0xFF;
            m_curNcLevel    = (m_lineState >> 16) & 0xFF;

            m_activeState = m_stateStack->GetActiveState(m_curLine).state;
            LOG("m_activeState=%d", m_activeState);
        }
        else m_activeState = FB_DEFAULT;

        // Colorise the document
        Parse ();
    }


    /**
     * Parse the syntax
     */
    void Parse ()
    {
        // iterate through the text
        while (m_sc.More())
        {
            // handle line starts
            if (m_sc.atLineStart)
            {

                // get line info
                m_curLine = m_styler.GetLine(m_sc.currentPos);
                m_section = GetSectionType(m_activeState);
                m_sc.SetState(m_activeState);

                // zeroe the char count if not in comment
                if (!m_curNcLevel) m_visibleChars = 0;

                // parse line start
                ParseLineStart ();

                // detect indent
                m_realIndent = 0;
                for (int p = m_sc.currentPos; p < m_styler.Length(); p++)
                {
                    char ch = m_styler.SafeGetCharAt(p);
                    if (ch == ' ') m_realIndent++;
                    else if (ch == '\t') m_realIndent += m_tabWidth;
                    else break;
                }
                DUMP_INT_VAR(m_realIndent);


                // if it is a line continuation restore previous style
                if (m_lineState & LINESTATE_LINE_CONTINUATION)
                {
                    UNSET_FLAG(m_lineState, LINESTATE_LINE_CONTINUATION);
                    m_lineCont = true;
                }
            }

            // handle non default states (comments, strings, ...)
            if (!IsInDefaultSection(m_sc.state))
            {
                m_section = GetSectionType(m_sc.state);
                ParseState ();
            }

            // handle default states (fb, asm, pp)
            if (IsInDefaultSection(m_sc.state))
            {
                m_section = GetSectionType(m_sc.state);
                ParseDefault ();
            }


            // handle line ends.
            if (m_sc.atLineEnd)
            {
                m_section = GetSectionType(m_sc.state);
                ParseLineEnd ();

                // pop the multiline state
                if (m_lineCont && !(m_lineState & LINESTATE_LINE_CONTINUATION))
                {
                    m_lineCont = false;

                    while (m_nLcLevel--)
                    {
                        m_stateStack->PopState(m_curLine).state;
                    }
                    m_activeState = m_stateStack->GetActiveState(m_curLine).state;
                    LOG("Pop line continuation", 0);
                }
            }
            else
            {
                if (!IsInComment(m_sc.state) && !IsASpaceOrTab(m_sc.ch))
                    m_visibleChars++;
            }

            // skip call to Forward() ?
            if (m_skipForward)
            {
                m_skipForward = false;
                continue;
            }

            Forward();
        }

        // finalize
        m_sc.Complete();
    }


    /**
     * Handle the line beginning
     */
    void ParseLineStart ()
    {
        if (m_indentedBoxes) DrawIndentedBlocks();
    }


    /**
     * Parse the state indicated in m_sc.state
     * that is not default
     */
    void ParseState ()
    {
        if (IsInComment(m_sc.state)) ParseComment ();
    }


    /**
     * Parse default state
     */
    void ParseDefault ()
    {
        // multiline comment
        if (m_sc.Match('/', '\''))
        {
            m_activeState = m_section + FB_COMMENT;
            m_stateStack->PushState(m_curLine, m_indent, m_activeState);
            m_sc.SetState(m_activeState);
            m_curNcLevel++;
            Forward();
        }
        // single line comment
        else if (m_sc.ch == '\'')
        {
            m_sc.SetState(m_section + FB_COMMENT);
            if (m_visibleChars==0)
            {
                Forward();
                SkipSpacesAndTabs();
                ParseSingleLineComment();
            }
        }
        // single line comment using REM keyword. At line start
        // can be followed by metacommand
        else if (!IsAWordChar(m_sc.GetRelative(3)) && m_sc.MatchIgnoreCase("rem"))
        {
            m_sc.SetState(m_section + GetFbKeywordStyle("rem", FB_COMMENT));
            Forward(3);
            m_sc.SetState(m_section + FB_COMMENT);
            if (m_visibleChars==0 && IsASpaceOrTab(m_sc.ch))
            {
                SkipSpacesAndTabs();
                ParseSingleLineComment();
            }
        }
        // string literals
        else if (m_sc.ch == '"')
        {
            bool escape = false;
            if (m_dialect <= DIALECT_LITE)
            {
                escape = m_stateStack->m_optionEscape;
            }
            m_sc.SetState(m_section + FB_STRING);
            ParseString(escape);
        }
        // explicitly NON escaped string literal
        else if (m_sc.Match('$', '"'))
        {
            m_sc.SetState(m_section + FB_STRING);
            Forward();
            ParseString(false);
        }
        // explicitly escaped string literal
        else if (m_sc.Match('!', '"'))
        {
            m_sc.SetState(m_section + FB_STRING);
            Forward();
            ParseString(true);
        }
        // macro
        else if (m_sc.Match('#') && m_visibleChars == 0)
        {
            m_visibleChars++;
            ParsePreprocessor ();
        }
        // command seperator
        else if (m_sc.ch == ':')
        {
            // set to minus one because it will be incremented
            // at next at next iteration anyway
            m_visibleChars = -1;
            m_sc.SetState(m_section);
        }
        // decimal or floating point number
        else if (IsADigit(m_sc.ch)
            || (m_sc.ch == '.' && IsADigit(m_sc.chNext) && !IsAWordChar(m_sc.chPrev))
        ) {
            m_sc.SetState(m_section + FB_NUMBER);
            ParseNumber(10, m_sc.currentPos);
        }
        // hex number
        else if (m_sc.ch == '&' && tolower(m_sc.chNext) == 'h')
        {
            m_sc.SetState(m_section + FB_NUMBER);
            Forward(2);
            ParseNumber(16, m_sc.currentPos-2);
        }
        // octal number
        else if (m_sc.ch == '&' && tolower(m_sc.chNext) == 'o')
        {
            m_sc.SetState(m_section + FB_NUMBER);
            Forward(2);
            ParseNumber(8, m_sc.currentPos-2);
        }
        // binary number
        else if (m_sc.ch == '&' && tolower(m_sc.chNext) == 'b')
        {
            m_sc.SetState(m_section + FB_NUMBER);
            Forward(2);
            ParseNumber(2, m_sc.currentPos-2);
        }
        // is a asm block start ?
        else if (m_section != ASM_DEFAULT
            && !IsAWordChar(m_sc.GetRelative(3))
            && m_sc.Match("asm"))
        {
            ParseAsmStart ();
        }
        // is a asm block start ?
        else if (m_section == ASM_DEFAULT
            && !IsAWordChar(m_sc.GetRelative(3))
            && m_sc.Match("end"))
        {
            ParseAsmEnd();
        }
        // Is a line continuation
        else if (m_section != FB_DEFAULT
                && !(m_lineState & LINESTATE_LINE_CONTINUATION)
                && m_sc.ch == '_'
                && !IsAWordChar(m_sc.chNext)
                && !IsAWordChar(m_sc.chPrev))
        {
            ParseLineContinuation();
        }
        // a keyword or an identifier
        else if (IsAWordChar(m_sc.ch))
        {
            ParseIdentifier();
        }
        // is an operator ?
        else if (IsOperator(m_sc.ch))
        {
            m_sc.SetState(m_section + FB_OPERATOR);

            while (
                IsOperator(m_sc.ch)
                && !m_sc.Match('/', '\'')
                && (m_sc.ch != '.' || !IsADigit(m_sc.chNext, 10))
            ) {
                Forward();
                m_skipForward = true;
                m_visibleChars++;
            }

            m_sc.SetState(m_section);
        }
    }


    /**
     * Handle the line end
     */
    void ParseLineEnd ()
    {
        // embed pp nest level
        m_lineState = (m_lineState & 0xFFFFFF00) | m_curNppLevel;
        m_lineState = (m_lineState & 0xFFFF00FF) | (m_nLcLevel<<8);
        m_lineState = (m_lineState & 0xFF00FFFF) | (m_curNcLevel<<16);

        /// @todo optimize linestate_refresh flag!!!
        ///       as it is only needed when open/close
        ///       multiline blocks (pp, asm, ml comments)
        // TOGGLE_FLAG(m_lineState, LINESTATE_REFRESH);

        // set line state
        m_styler.SetLineState(m_curLine, m_lineState);
    }


    /**
     * Parse number
     */
    bool ParseNumber (int base, int indStart)
    {
        int fpCount = 0;
        // int indStart = m_sc.currentPos;

        // skip the word chars?
        while (IsADigit(m_sc.ch, base) || m_sc.ch == '.')
        {
            if (m_sc.ch == '.')
            {
                if (base != 10 || fpCount || m_sc.chNext == '.') break;
                else fpCount++;
            }

            MoveForward();
        }

        // set number state
        m_sc.ChangeState(m_section + FB_NUMBER);
        m_sc.SetState(m_section + FB_NUMBER);
        bool invalid = false;
        bool hasSuffix = false;

        // exponent. allow only for decimal numbers.
        if (base == 10)
        {
            // check for floating point suffixes
            // .fraction[[D|E][[+|-]exponent]][suffix]
            char ch = tolower(m_sc.ch);
            if (ch == 'e' || ch == 'd')
            {
                hasSuffix = true;
                MoveForward();
                ch = m_sc.ch;
                if (ch == '+' || ch == '-') MoveForward();

                while (IsADigit(m_sc.ch, 10))
                    MoveForward();
            }

            // type suffix
            ch = tolower(m_sc.ch);
            if (ch == '!' || ch == 'f' || ch == '#')
            {
                hasSuffix = true;
                MoveForward();
            }
        }

        // Check for integer literal suffixes
        // L, %, &, U, UL, LL, ULL
        if (!fpCount && !hasSuffix)
        {
            // have to look for multichar first
            if (int s = FindMatch(INTEGER_SUFFIXES_MC))
            {
                invalid = IsAWordChar(m_sc.GetRelative(s));
                MoveForward(s);
            }
            // check for single char suffixes
            else if (FindMatch(INTEGER_SUFFIXES_SC, true))
            {
                invalid = IsAWordChar(m_sc.chNext);
                MoveForward();
            }
        }
        invalid = IsAWordChar(m_sc.ch);

        // indicate invalid number.
        m_styler.IndicatorFill(indStart, m_sc.currentPos, 0, invalid ? 1 : 0);

        // restore state
        m_sc.SetState(m_section);
        return true;
    }


    /**
     * Parse string and highlight escape sequences (if escape=true)
     * do that in one pass. Assume that first character
     * is opening quote. So skip the first one
     */
    void ParseString (bool escape)
    {
        bool skip = false;
        while (!m_sc.atLineEnd)
        {
            if (skip) skip = false;
            else Forward();

            if (m_sc.Match('"', '"'))
            {
                m_sc.SetState(m_section+FB_STRING_ESCAPE);
                m_sc.Forward(2);
                m_sc.SetState(m_section+FB_STRING);
                skip = true;
                continue;
            }
            else if (m_sc.Match('"'))
            {
                m_sc.Forward();
                m_sc.SetState(m_section);
                m_skipForward = true;
                return;
            }

            // no escapes...
            if (!escape || m_sc.ch != '\\') continue;

            // not an escape code
            int indStart = m_sc.currentPos;
            bool matched = false;

            m_sc.SetState(m_section+FB_STRING_ESCAPE);
            Forward();

            // Simple sequences (case sensitive!)
            if (FindMatch(ESCAPE_CODES))
            {
                m_sc.ForwardSetState(m_section+FB_STRING);
                matched = true;
            }
            // unicode char in hex \uFFFF
            else if (tolower(m_sc.ch) == 'u' && IsNumberRange(1, 4, 16))
            {
                m_sc.ForwardSetState(m_section+FB_NUMBER);
                Forward(4);
                matched = true;
            }
            // ascii char in decimal \123
            else if (IsNumberRange(0, 3, 10))
            {
                m_sc.SetState(m_section+FB_NUMBER);
                Forward(3);
                matched = true;
            }
            // ascii escape sequence in FB style oct, bin or hex
            else if (m_sc.ch == '&')
            {
                m_sc.SetState(m_section+FB_NUMBER);
                Forward();
                // ascii char in hex \&hFF
                if (tolower(m_sc.ch) == 'h' && IsNumberRange(1, 2, 16))
                {
                    Forward(3);
                    matched = true;
                }
                // ascii char in octal \&o777
                else if (tolower(m_sc.ch) == 'o' && IsNumberRange(1, 3, 8))
                {
                    Forward(4);
                    matched = true;
                }
                // ascii char in binary \&b11111111
                else if (tolower(m_sc.ch) == 'b' && IsNumberRange(1, 8, 2))
                {
                    Forward(9);
                    matched = true;
                }
                else
                {
                    m_sc.ChangeState(m_section+FB_STRING);
                }
            }

            // clear or set error indicator
            m_sc.SetState(m_section+FB_STRING);
            m_styler.IndicatorFill(indStart, m_sc.currentPos, 0, matched ? 0 : 1);
        }

        // set back to string
        m_sc.SetState(m_section);
        m_skipForward = true;
    }


    /**
     * Parse identifier
     */
    void ParseIdentifier ()
    {
        m_sc.SetState(PP_DEFAULT);
        int curPos = m_sc.currentPos;

        // skip the word chars?
        while (!m_sc.atLineEnd && IsAWordChar(m_sc.ch))
            MoveForward();

        // identifier length
        size_t len = m_sc.currentPos-curPos+1;
        char * s;

        // see if identifier fits into existing buffer
        // or allocate a new one
        if (len > sizeof(m_buffer)) s = new char[len];
        else s = m_buffer;

        // get the identifier
        GetRange(curPos, m_sc.currentPos-1, s, len);

        // style it.
        if (m_section == ASM_DEFAULT)
        {
            m_sc.ChangeState(GetAsmKeywordStyle(s));
        }
        else
        {
            if (m_section == PP_DEFAULT && strcmpi(s, "once") == 0)
            {
                m_sc.ChangeState(GetPpKeywordStyle("once"));
            }
            else
            {
                if (strcmpi(s, "end") == 0 || strcmpi(s, "declare") == 0)
                {
                    int defStyle = GetFbKeywordStyle(s);

                    std::string tmp (s);

                    bool combined = false;
                    int p = FindNextWord(m_sc.currentPos, m_buffer, sizeof(m_buffer));
                    if (p)
                    {
                        tmp += '-';
                        tmp += m_buffer;
                        if (int style = GetFbKeywordStyle(tmp.c_str(), 0))
                        {
                            combined = true;
                            int move = (p-m_sc.currentPos) + strlen(m_buffer);
                            MoveForward(move);
                            m_sc.ChangeState(m_section + style);
                        }
                    }
                    // no combination
                    if (!combined)
                    {
                        m_sc.ChangeState(m_section + defStyle);
                    }
                }
                else
                {
                    m_sc.ChangeState(m_section + GetFbKeywordStyle(s));
                }
            }
        }
        m_sc.SetState(m_section);

        // delete allocated buffer.
        if (len > sizeof(m_buffer)) delete s;
    }


    /**
     * Parse asm start block. Multiline or single line
     */
    void ParseAsmStart ()
    {
        int indent = m_indent;
        m_skipForward = true;
        m_sc.SetState(ASM_DEFAULT + GetFbKeywordStyle("asm"));
        Forward(3);
        m_sc.SetState(ASM_DEFAULT);
        m_visibleChars+=3;

        SkipSpacesAndTabs();
        if (!m_sc.atLineEnd) m_skipForward = true;
        if (!IsAWordChar(m_sc.ch)
            && (!m_sc.MatchIgnoreCase("rem") || !IsAWordChar(m_sc.GetRelative(3))
        ))
        {
            if (m_nLcLevel) m_nLcLevel++;

            m_activeState = ASM_DEFAULT;
            m_stateStack->PushState(m_curLine, indent, m_activeState);
            SET_FLAG(m_lineState, LINESTATE_MULTILINE_ASM);
        }
    }


    /**
     * Parse ASM end block
     */
    void ParseAsmEnd ()
    {
        // make sure it is a multiline asm block
        /// @note ?????????????
        /// m_activeState != ASM_DEFAULT
        // int state = m_stateStack->GetActiveState(m_curLine).state;
        // if (state != ASM_DEFAULT) return;

        // check if next keyword is ASM (end asm)
        char nw[4];
        if (FindNextWord(m_sc.currentPos+4, nw, 4) && (strcmpi(nw, "asm") == 0))
        {
            int style = GetFbKeywordStyle("end-asm", 0);
            m_sc.SetState(m_section + (style ? style : GetFbKeywordStyle("end")));
            Forward(3);
            m_sc.SetState(m_section);
            SkipSpacesAndTabs();
            m_sc.SetState(m_section + (style ? style : GetFbKeywordStyle("asm")));
            Forward(3);

            UNSET_FLAG(m_lineState, LINESTATE_MULTILINE_ASM);

            // check if line end then draw asm colours
            // to the end. Otherwise end asm block abruptly
            m_stateStack->PopState(m_curLine);
            m_activeState = m_stateStack->GetActiveState(m_curLine).state;
            if (m_sc.ch != '\r' && m_sc.ch != '\n')
            {
                m_sc.SetState(m_activeState);
                m_skipForward = true;
            }
            else m_sc.SetState(m_section);
        }
    }


    /**
     * Parse line continuation _
     *
     * @todo check if line continuation is really needed ?
     */
    void ParseLineContinuation ()
    {
        m_sc.SetState (m_section);
        SET_FLAG(m_lineState, LINESTATE_LINE_CONTINUATION);

        if (!m_lineCont || m_section != m_activeState)
        {
            int indent = m_realIndent;
            LOG("Push line continuation", 0);
            m_nLcLevel++;
            m_activeState = m_section;
            m_stateStack->PushState(m_curLine, indent, m_section);
        }
        else
        {
            LOG("Nested contination", 0);
        }
    }


    /**
     * Parse a single line or a multiline comment
     * search for urls within
     */
    void ParseComment ()
    {
        //m_skipForward = true;
        for ( ; !m_sc.atLineEnd; Forward() )
        {

            // Handle multiline comment start/close
            if (m_curNcLevel)
            {
                // '/ END
                if (m_sc.Match('\'', '/'))
                {
                    if (--m_curNcLevel == 0)
                    {
                        int state = m_sc.state;
                        m_stateStack->PopState(m_curLine);
                        Forward(2);
                        m_activeState = m_stateStack->GetActiveState(m_curLine).state;

                        m_sc.SetState(m_activeState);
                        if (!IsInComment(m_activeState))
                        {
                            if (m_sc.ch == '\r' || m_sc.ch == '\n')
                                m_sc.ChangeState(state);
                            else
                                m_skipForward = true;
                        }
                        return;
                    }
                }
                // /' START
                else if (m_sc.Match('/', '\''))
                {
                    m_activeState = m_sc.state;
                    //m_stateStack->PushState(m_curLine, 0, m_activeState);
                    m_curNcLevel++;
                    Forward(2);
                }
            }

            // needs a non word char infornt
            if (IsAWordChar(m_sc.chPrev) || !IsAWordChar(m_sc.ch)) continue;

            // match uri schemes. if matched then set state
            int len = FindMatch(URI_SCHEMES);
            if (len == 0) continue;

            m_sc.SetState(m_section + FB_COMMENT_URL);
            SkipNonWhiteSpaces();
            m_sc.SetState(m_section + FB_COMMENT);

            if (m_sc.atLineEnd)
                return;
        }

    }



    /**
     * Parses single line qomment and checks if contains
     * any meta keywords (lang, dynamic, static, ...)
     * if yes then set style to PP_DEFAULT, highlight the keyword
     * and return true.
     */
    void ParseSingleLineComment ()
    {
        // check if is valid start ?
        if (m_sc.atLineEnd || m_sc.ch != '$') goto meta_error;

        // mov forth and skip any spaces
        Forward();
        SkipSpacesAndTabs ();
        if (m_sc.atLineEnd) goto meta_error;

        // check if this is a valid meta statament
        if (HighlightMetaKeyword("lang", 4)) return;

        // allowed only in QB and LITE
        if (m_dialect > DIALECT_LITE) goto meta_error;

        // include
        if (HighlightMetaKeyword("include", 7)) return;

        // dynamic
        if (HighlightMetaKeyword("dynamic", 7)) return;

        // static
        if (HighlightMetaKeyword("static", 6)) return;

        // avoid call to Forward by Parse() since we already
        // moved forward.
        meta_error:
        m_skipForward = true;
        return;
    }


    /**
     * Hightlight a QB style metakeyword inside single
     * line comments if matches
     * return true if matched.
     */
    bool HighlightMetaKeyword (const char * kw, int len)
    {
        // return if not matches or next char is a word char
        if (IsAWordChar(m_sc.GetRelative(len)) || !m_sc.MatchIgnoreCase(kw))
            return false;

        // colorise
        m_sc.Forward(len);
        m_sc.ChangeState(GetPpKeywordStyle(kw));
        m_sc.SetState(PP_DEFAULT);
        return true;
    }


    /**
     * Parse preprocessor start #
     */
    void ParsePreprocessor ()
    {
        int indent = m_indent;
        m_sc.SetState(PP_DEFAULT);
        Forward();
        SkipSpacesAndTabs();
        if (m_sc.atLineEnd) return;
        m_skipForward = true;

        // macro's can nest and cover multiple lines
        if (m_sc.MatchIgnoreCase("macro") && !IsAWordChar(m_sc.GetRelative(5)))
        {
            m_sc.ChangeState(GetPpKeywordStyle("macro"));
            Forward(5);
            m_sc.SetState(PP_DEFAULT);
            m_curNppLevel++;

            // push state
            m_activeState = PP_DEFAULT;
            m_stateStack->PushState(m_curLine, indent, PP_DEFAULT);
        }
        else if (m_sc.MatchIgnoreCase("endmacro") && !IsAWordChar(m_sc.GetRelative(8)))
        {
            m_sc.ChangeState(GetPpKeywordStyle("endmacro"));
            Forward(8);
            m_sc.SetState(PP_DEFAULT);

            if (m_curNppLevel)
            {
                m_curNppLevel--;
                int state = m_stateStack->PopState(m_curLine).state;

                // is it a nested assembly?
                if (state == ASM_DEFAULT)
                    state = m_stateStack->PopState(m_curLine).state;

                // check if line end then draw asm colours
                // to the end. Otherwise end asm block abruptly
                m_activeState = m_stateStack->GetActiveState(m_curLine).state;
                if (m_sc.ch != '\r' && m_sc.ch != '\n')
                {
                    m_sc.SetState(m_activeState);
                    m_skipForward = true;
                }
                else m_sc.SetState(m_section);
            }
        }
        else if (IsAWordChar(m_sc.ch))
        {
            // m_sc.SetState(PP_DEFAULT);
            int curPos = m_sc.currentPos;
            // skip the word chars?
            while (!m_sc.atLineEnd && IsAWordChar(m_sc.ch))
            {
                Forward();
            }
            char s[25];

            // m_sc.GetCurrent(s, sizeof(s));
            GetRange(curPos, m_sc.currentPos-1, s, sizeof(s));
            m_sc.ChangeState(GetPpKeywordStyle(s));
            m_sc.SetState(PP_DEFAULT);
        }
    }


    /**
     * Get Section type
     */
    int GetSectionType (int style)
    {
        if (style <= FB_SECTION_LAST)
            return FB_DEFAULT;

        if (style <= ASM_SECTION_LAST)
            return ASM_DEFAULT;

        if (style <= PP_SECTION_LAST)
            return PP_DEFAULT;

        return FB_DEFAULT;
    }


    /**
     * Nested and indented code blocks that have
     * a background. Draw those in proper nesting order
     */
    void DrawIndentedBlocks ()
    {
        /// @note ????
        if (m_activeState == FB_DEFAULT) return;

        int currentState = m_sc.state;
        int globalIndent = 0;

        CStateRange range = m_stateStack->GetStateRange(m_curLine);

        for ( ; range.first != range.second; )
        {
            // get inner block indent level
            int state = (range.first++)->state;
            int indent = range.first->indent;
            if (indent == 0) continue;

            // fix inner indend levels
            if (indent > globalIndent) indent-= globalIndent;
            else continue;
            globalIndent += indent;

            // get the state to colour to
            m_sc.SetState(state);

            // colour the background while there are spaces or tabs
            while (indent && IsASpaceOrTab(m_sc.ch))
            {
                if (m_sc.ch == '\t')
                {
                    if (indent > m_tabWidth) indent-= m_tabWidth;
                    else indent = 0;
                }
                else
                {
                    indent--;
                }
                Forward();
            }
        }

        // restore correct state
        m_sc.SetState(currentState);
    }


    /**
     * Skip the tabs and spaces
     * but stop at anything else
     */
    void SkipSpacesAndTabs ()
    {
        while (!m_sc.atLineEnd && IsASpaceOrTab(m_sc.ch))
            Forward();
    }


    /**
     * Skip non whitespaces. Stop ad line end or
     * a space or a tab
     */
    void SkipNonWhiteSpaces ()
    {
        while (!m_sc.atLineEnd && !IsASpaceOrTab(m_sc.ch))
            Forward();
    }


    /**
     * Move forward. Update line indent
     */
    void Forward (int cnt = 1)
    {
        while (cnt--)
        {
            m_sc.Forward();
            if (m_sc.atLineStart)
            {
                m_indent = 0;
            }
            else
            {
                if (m_sc.ch == '\t') m_indent += m_tabWidth;
                else m_indent++;
            }
        }
    }


    /**
     * Move forward and set m_skipForward to true
     * and increment visible char count
     */
    void MoveForward (int amount = 1)
    {
        Forward(amount);
        m_skipForward = true;
        m_visibleChars+=amount;
    }


    /**
     * Find the string from an array that matches
     * and return it's length
     * or 0 if none
     */
    int FindMatch (const char * const matches[])
    {
        for (int i = 0; matches[i]; i++)
            if (m_sc.MatchIgnoreCase(matches[i]))
            {
                LOG("match found %s", matches[i]);
                return strlen(matches[i]);
            }
        return 0;
    }


    /**
     * Check if any of the chars matches ?
     * return char that matches or a 0
     */
    char FindMatch(const char matches[], bool ignoreCase = false)
    {
        if (ignoreCase)
        {
            for (int i = 0; matches[i]; i++)
                if (tolower(m_sc.ch) == matches[i])
                    return matches[i];
        }
        else
        {
            for (int i = 0; matches[i]; i++)
                if (m_sc.ch == matches[i])
                    return matches[i];
        }
        return 0;
    }


    /**
     * Retreave a range of characters into a string
     */
    void GetRange(unsigned int start, unsigned int end,
            char *s, unsigned int len)
    {
        unsigned int i = 0;
        while ((i < end - start + 1) && (i < len-1))
        {
            s[i] = static_cast<char>(tolower(m_styler[start + i]));
            i++;
        }
        s[i] = '\0';
    }


    /**
     * Get non whitespace range for the lenght
     * skip any spaces or tabs and return when hitting first
     * non-printable character (space, tab, newline)
     */
    int FindNextWord (unsigned int start, char *s, unsigned int len)
    {
        // skip any spaces or tabs
        for ( ; ; start++)
        {
            char ch = m_styler[start];
            if (IsAWordChar(ch)) break;
            if (ch == '\n' || ch == '\r') return 0;
            if (!IsASpaceOrTab(ch)) return 0;
        }

        // GetRange (start, start + len, s, len);
        unsigned int i = 0;
        while ( i < len )
        {
            char ch = static_cast<char>(tolower(m_styler[start + i]));
            if (IsAWordChar(ch)) s[i] = ch;
            else break;
            i++;
        }
        s[i] = '\0';

        return start;
    }


    /**
     * Extended to accept accented characters
     */
    bool IsAWordChar(int ch)
    {
        return ch >= 0x80 ||
            (isalnum(ch) || ch == '.' || ch == '_');
    }


    /**
     * Check if the char is a valid FB operator
     */
    bool IsOperator (int ch)
    {
        return ch == '&'
            || ch == '*'
            || ch == '('
            || ch == ')'
            || ch == '-'
            || ch == '+'
            || ch == '='
            || ch == '{'
            || ch == '}'
            || ch == '['
            || ch == ']'
            || ch == ':'
            || ch == ';'
            || ch == '@'
            || ch == '#'
            || ch == '\\'
            || ch == ','
            || ch == '<'
            || ch == '>'
            || ch == '.'
            || ch == '/'
            || ch == '?'
            ;
    }


    /**
     * Check if range of characters are numbers of the base
     */
    bool IsNumberRange (int startPos, int count, int base)
    {
        for (--count; count>=0; count--)
        {
            if (!IsADigit(m_sc.GetRelative(startPos+count), base))
                return false;
        }
        return true;
    }


    /**
     * If the given keyword is a preprocessor keyword then
     * highlight it to appropriate PP keyword style
     */
    int GetPpKeywordStyle (const char * kw, int defStyle = PP_IDENTIFIER)
    {
        if (m_keywords[KEYWORD_PP_ONE]->InList(kw))
            return PP_KEYWORD_ONE;
        else if (m_keywords[KEYWORD_PP_TWO]->InList(kw))
            return PP_KEYWORD_TWO;
        return defStyle;
    }


    /**
     * Get FB keyword type. If not found FB_IDENTIFIER is returned
     */
    int GetFbKeywordStyle (const char * kw, int defStyle = FB_IDENTIFIER)
    {
        if (m_keywords[KEYWORD_ONE]->InList(kw))
            return FB_KEYWORD_ONE;
        else if (m_keywords[KEYWORD_TWO]->InList(kw))
            return FB_KEYWORD_TWO;
        else if (m_keywords[KEYWORD_THREE]->InList(kw))
            return FB_KEYWORD_THREE;
        else if (m_keywords[KEYWORD_FOUR]->InList(kw))
            return FB_KEYWORD_FOUR;
        return defStyle;
    }




    /**
     * Get ASM block keyword type
     */
    int GetAsmKeywordStyle (const char * kw, int defStyle = ASM_IDENTIFIER)
    {
        if (m_keywords[KEYWORD_ASM_ONE]->InList(kw))
            return ASM_KEYWORD_ONE;
        else if (m_keywords[KEYWORD_ASM_TWO]->InList(kw))
            return ASM_KEYWORD_TWO;
        return defStyle;
    }


    /**
     * Printf compatible function that will output to wxMessageLog
     */
    /*
    void Log (const char * msg, ...) // __attribute__((format(printf, 2, 3)))
    {
        // do thr printf
        va_list ap;
        va_start(ap, msg);
        char temp[4096];
        vsnprintf (temp, 4096, msg, ap);
        va_end(ap);

        // output the wxLog
        wxString logMsg(temp, wxConvUTF8);
        wxLogMessage(logMsg);
    }
    */



    // style context
    StyleContext  & m_sc;
    // Accessor
    Accessor      & m_styler;
    // keywords
    WordList     ** m_keywords;
    // current section
    int             m_section;
    // current line state
    int             m_lineState;
    // current line
    int             m_curLine;
    // count visible characters
    int             m_visibleChars;
    // line indentation
    int             m_indent;
    // real indent
    int             m_realIndent;
    // current FB dialect
    int             m_dialect;
    // indent amount
    int             m_tabWidth;
    // Force to skip default call to Forward()
    bool            m_skipForward;
    //
    bool            m_indentedBoxes;
    // Line states structure
    CLineStates   * m_stateStack;
    // Active state
    int             m_activeState;
    // nested pp level
    int             m_curNppLevel;
    // nested multiline comments
    int             m_curNcLevel;
    // nested line continuations
    int             m_nLcLevel;
    // line continuation
    bool            m_lineCont;
    // char buffer
    char            m_buffer[512];
};




/**
 * Colorise FB document
 */
static void ColouriseFB (
    unsigned int startPos, int length, int initStyle,
    WordList *keywordlists[], Accessor &styler )
{
    // Styler
    styler.StartAt(startPos, 63);
    StyleContext sc (startPos, length, initStyle, styler, 63);

    // Fb source lexer
    CFbSourceLexer l (sc, styler, keywordlists);
    #ifdef USE_LOG
        wxLogMessage(">>>> styled from: %d, length: %d\n"
            "---------------------------------------------------------------",
            startPos, length);
    #endif
}


static bool IsVBComment(Accessor &styler, int pos, int len) {
	return len>0 && styler[pos]=='\'';
}



/**
 * Should folding use LineStates info? In wich case the parser should
 * get states for all possible scope block starts and ends.
 */
static void FoldVBDoc(unsigned int startPos, int length, int,
						   WordList *[], Accessor &styler) {
	int endPos = startPos + length;

	// Backtrack to previous line in case need to fix its fold status
	int lineCurrent = styler.GetLine(startPos);
	if (startPos > 0) {
		if (lineCurrent > 0) {
			lineCurrent--;
			startPos = styler.LineStart(lineCurrent);
		}
	}
	int spaceFlags = 0;
	int indentCurrent = styler.IndentAmount(lineCurrent, &spaceFlags, IsVBComment);
	char chNext = styler[startPos];
	for (int i = startPos; i < endPos; i++) {
		char ch = chNext;
		chNext = styler.SafeGetCharAt(i + 1);

		if ((ch == '\r' && chNext != '\n') || (ch == '\n') || (i == endPos)) {
			int lev = indentCurrent;
			int indentNext = styler.IndentAmount(lineCurrent + 1, &spaceFlags, IsVBComment);
			if (!(indentCurrent & SC_FOLDLEVELWHITEFLAG)) {
				// Only non whitespace lines can be headers
				if ((indentCurrent & SC_FOLDLEVELNUMBERMASK) < (indentNext & SC_FOLDLEVELNUMBERMASK)) {
					lev |= SC_FOLDLEVELHEADERFLAG;
				} else if (indentNext & SC_FOLDLEVELWHITEFLAG) {
					// Line after is blank so check the next - maybe should continue further?
					int spaceFlags2 = 0;
					int indentNext2 = styler.IndentAmount(lineCurrent + 2, &spaceFlags2, IsVBComment);
					if ((indentCurrent & SC_FOLDLEVELNUMBERMASK) < (indentNext2 & SC_FOLDLEVELNUMBERMASK)) {
						lev |= SC_FOLDLEVELHEADERFLAG;
					}
				}
			}
			indentCurrent = indentNext;
			styler.SetLevel(lineCurrent, lev);
			lineCurrent++;
		}
	}
}





/**
 * Keywords
 */
static const char * const fbWordListDesc[] = {
	"FB_kw1",
	"FB_kw2",
	"FB_kw3",
	"FB_kw4",
	"FB_pp1",
	"FB_pp2",
	"FB_asm1",
	"FB_asm2",
	0
};


// Add the lexer module
LexerModule lmFB(
    FREEBASIC,         // identifier
    ColouriseFB,            // colorise proc
    "FreeBASIC",            // name
    FoldVBDoc,              // fold proc
    fbWordListDesc,         // keywords descriptor
    6                       // number of style bits used
);
