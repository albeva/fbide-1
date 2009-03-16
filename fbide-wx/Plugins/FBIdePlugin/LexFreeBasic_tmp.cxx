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
 * @todo When macro or asm block is closed line is highlighted to the end
 *       of the line. However ':' character should revert to previous
 *       section style
 *
 * @todo asm /' ... \n ... '/ should decide is there a single line
 *       asm block or multiline. Implement methods "ResolveMultiLineComment"
 *
 * @todo Reseolve line continuation for ASM blocks
 *       asm _ \n could be either one liner or a multiline
 *
 * @todo end (asm|if|while|...) matching. in keyword list have end+asm
 *       or something
 *
 * @todo end /' ... \n ... '/ asm should work. as well as
 *       end _ \n asm
 *
 * @todo asm and macro block nesting
 *
 * @todo highlight code blocks that have background based on
 *       indentation. (looks mich nicer with nested blocks)
 *
 * @todo implement options to disable certain highlighting features
 *       escape sequences, block highlighting, ...
 */

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

#include "wx_pch.h"
#include "LexFreeBasic.hxx"

using namespace LEX;


/**
 * Allowed URI Schemes inside comments.
 * case insensitive.
 */
static const char * URI_SCHEMES[] = {
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
const char ESCAPE_CODES[] = {
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


// Set bit flags
static inline void SET_FLAG (int & value, int flag)
{
    value |= flag;
}

// unset bit flags
static inline void UNSET_FLAG (int & value, int flag)
{
    value &= (~flag);
}

// toggle bit flags
static inline void TOGGLE_FLAG (int & value, int flag)
{
    value ^= flag;
}



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
        m_lineState(0),
        m_curNcLevel(0),
        m_curNppLevel(0),
        m_visibleChars(0),
        m_skipForward(false)
    {
        // setup the state
        m_dialect = styler.GetPropertyInt("dialect", LEX::DIALECT_FB);
        m_curLine = styler.GetLine(m_sc.currentPos);

        // if not the first line then get the state of
        // the previous line.
        if (m_curLine > 0)
        {
            m_lineState  = m_styler.GetLineState(m_curLine-1);
            m_curNcLevel = m_lineState & 0xFF;
            m_curNppLevel= (m_lineState >> 8) & 0xFF;
        }

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
                m_curLine = m_styler.GetLine(m_sc.currentPos);
                m_lineState  = m_curLine ? m_styler.GetLineState(m_curLine-1) : 0;
                m_section = GetSectionType (m_sc.state);
                ParseLineStart ();

                // do not reset if is a multiline comment
                if (m_curNcLevel == 0) m_visibleChars = 0;
            }

            // handle non default states (comments, strings, ...)
            if (!IsInDefaultSection(m_sc.state))
            {
                m_section = GetSectionType (m_sc.state);
                ParseState ();
            }

            // handle default states (fb, asm, pp)
            if (IsInDefaultSection(m_sc.state))
            {
                m_section = GetSectionType (m_sc.state);
                ParseDefault ();
            }

            // handle line ends.
            if (m_sc.atLineEnd)
            {
                m_section = GetSectionType (m_sc.state);
                ParseLineEnd ();
            }
            else
            {
                if (!IsInAnyComment(m_sc.state) && !IsASpaceOrTab(m_sc.ch))
                    m_visibleChars++;
            }

            // skip call to m_sc.Forward() ?
            if (m_skipForward)
            {
                m_skipForward = false;
                continue;
            }
            m_sc.Forward();
        }

        // finalize
        m_sc.Complete();
    }


    /**
     * Handle the line beginning
     */
    void ParseLineStart ()
    {
        Log("<<< ParseLineStart : m_section=%d", m_section);
        Log("<<< ParseLineStart : m_lineState=%d", m_lineState);

        if (m_curNcLevel > 0)
        {
            m_sc.SetState(m_section + FB_COMMENT);
        }
        else if (IsInComment(m_sc.state))
        {
            m_sc.SetState(m_section);
        }
        else if (m_section == PP_DEFAULT)
        {
            if (m_lineState & LINESTATE_LINE_CONTINUATION)
            {
                UNSET_FLAG(m_lineState, LINESTATE_LINE_CONTINUATION);
                m_sc.SetState(PP_DEFAULT);
            }
            else
            {
                if (m_curNppLevel==0)
                    m_sc.SetState(GetPrevSectionType());
            }
        }
        else if (m_lineState & LINESTATE_MULTILINE_ASM)
        {
            m_sc.SetState(ASM_DEFAULT);
        }
        else if (m_section == ASM_DEFAULT)
        {
            if (m_lineState & LINESTATE_LINE_CONTINUATION)
            {
                UNSET_FLAG(m_lineState, LINESTATE_LINE_CONTINUATION);
                m_sc.SetState(ASM_DEFAULT);
            }
            else
            {
                m_sc.SetState(GetPrevSectionType());
            }
        }
        else
            m_sc.SetState(FB_DEFAULT);
    }


    /**
     * Parse the state indicated in m_sc.state
     * that is not default
     */
    void ParseState ()
    {
        // multiline comment
        if (IsInAnyComment(m_sc.state))
            ParseComment();
        else if (IsInString(m_sc.state))
            ParseString();
    }


    /**
     * Parse default state
     */
    void ParseDefault ()
    {
        // a single line comment. at line start may be followed
        // by QB style metacommand
        if (m_sc.ch == '\'')
        {
            m_sc.SetState(m_section + FB_COMMENT);
            if (m_visibleChars==0)
            {
                m_sc.Forward();
                SkipSpacesAndTabs();
                ParseSingleLineComment();
            }
        }
        // single line comment using REM keyword. At line start
        // can be followed by metacommand
        else if (!IsAWordChar(m_sc.GetRelative(3)) && m_sc.Match("rem"))
        {
            m_sc.SetState(m_section + GetFbKeywordType("rem", FB_COMMENT));
            m_sc.Forward(3);
            m_sc.SetState(m_section + FB_COMMENT);
            if (m_visibleChars==0 && IsASpaceOrTab(m_sc.ch))
            {
                SkipSpacesAndTabs();
                m_sc.SetState(m_section + FB_COMMENT);
                ParseSingleLineComment();
            }
        }
        // multiline comment. Can be nested!
        else if (m_sc.Match('/', '\''))
        {
            m_sc.SetState(m_section + FB_COMMENT);
            m_curNcLevel++;
            m_sc.Forward();
        }
        // string literals
        else if (m_sc.ch == '"')
        {
            if (m_dialect <= DIALECT_LITE)
            {
                if (m_styler.GetPropertyInt("option.escape", false))
                {
                    m_lineState |= LINESTATE_ESCAPE_STRING;
                }
            }
            m_sc.SetState(m_section + FB_STRING);
        }
        // explicitly NON escaped string literal
        else if (m_sc.Match('$', '"'))
        {
            m_sc.SetState(m_section + FB_STRING);
            m_sc.Forward();
        }
        // explicitly escaped string literal
        else if (m_sc.Match('!', '"'))
        {
            m_lineState |= LINESTATE_ESCAPE_STRING;
            m_sc.SetState(m_section + FB_STRING);
            m_sc.Forward();
        }
        // macro
        else if (m_sc.Match('#') && m_visibleChars == 0)
        {
            ParsePreprocessor ();
        }
        // decimal or floating point number
        else if (IsADigit(m_sc.ch) ||
            (m_sc.ch == '.' && IsADigit(m_sc.chNext) && !IsADigit(m_sc.chPrev))
        ) {
            ParseNumber(10);
        }
        // hex number
        else if (m_sc.ch == '&' && tolower(m_sc.chNext) == 'h')
        {
            m_sc.Forward(2);
            ParseNumber(16);
        }
        // octal number
        else if (m_sc.ch == '&' && tolower(m_sc.chNext) == 'o')
        {
            m_sc.Forward(2);
            ParseNumber(8);
        }
        // binary number
        else if (m_sc.ch == '&' && tolower(m_sc.chNext) == 'b')
        {
            m_sc.Forward(2);
            ParseNumber(2);
        }
        // is a asm block start ?
        else if (m_section != ASM_DEFAULT
            && !IsAWordChar(m_sc.GetRelative(3))
            && m_sc.Match("asm"))
        {
            m_skipForward = true;
            m_sc.SetState(ASM_DEFAULT + GetFbKeywordType("asm"));
            m_sc.Forward(3);
            m_sc.SetState(ASM_DEFAULT);
            m_visibleChars+=3;

            SkipSpacesAndTabs();
            if (!m_sc.atLineEnd) m_skipForward = true;
            if (!IsAWordChar(m_sc.ch)
                && (!m_sc.MatchIgnoreCase("rem") || !IsAWordChar(m_sc.GetRelative(3))
            ))
            {
                m_lineState |= LINESTATE_MULTILINE_ASM;
            }
        }
        // is a asm block start ?
        else if (m_section == ASM_DEFAULT
            && (m_lineState & LINESTATE_MULTILINE_ASM)
            && !IsAWordChar(m_sc.GetRelative(3))
            && m_sc.Match("end"))
        {
            char nw[4];
            GetNextInput(m_sc.currentPos+4, nw, 4);
            if (GetNextInput(m_sc.currentPos+4, nw, 4) && (strcmpi(nw, "asm") == 0))
            {
                m_sc.SetState(m_section + GetFbKeywordType("end"));
                m_sc.Forward(3);
                m_sc.SetState(m_section);
                SkipSpacesAndTabs();
                m_sc.SetState(m_section + GetFbKeywordType("asm"));
                m_sc.Forward(3);
                m_sc.SetState(m_section);
                UNSET_FLAG(m_lineState, LINESTATE_MULTILINE_ASM);
            }
        }
        // Is a line continuation
        else if (m_sc.ch == '_' &&
                !IsAWordChar(m_sc.chNext) && !IsAWordChar(m_sc.chPrev)
        ) {
            //m_visibleChars = 0;
            m_lineState |= LINESTATE_LINE_CONTINUATION;
            m_sc.SetState(m_section);
            // if (m_section == PP_DEFAULT)
        }
        // command seperator
        else if (m_sc.ch == ':')
        {
            // set to minus one because it will be incremented
            // at next at next iteration anyway
            m_visibleChars = -1;
            m_sc.SetState(m_section);
        }
        // a keyword or an identifier
        else if (IsAWordChar(m_sc.ch))
        {
            // m_sc.SetState(PP_DEFAULT);
            int curPos = m_sc.currentPos;

            // skip the word chars?
            while (!m_sc.atLineEnd && IsAWordChar(m_sc.ch))
            {
                m_sc.Forward();
                m_skipForward = true;
                m_visibleChars++;
            }
            char s[25];
            getRange(curPos, m_sc.currentPos-1, s, sizeof(s));
            if (m_section == ASM_DEFAULT)
            {
                m_sc.ChangeState(GetAsmKeywordType(s));
            }
            else
            {
                if (m_section == PP_DEFAULT && strcmpi(s, "once") == 0)
                {
                    m_sc.ChangeState(GetPpKeywordStyle("once"));
                }
                else
                {
                    m_sc.ChangeState(m_section + GetFbKeywordType(s));
                }
            }
            m_sc.SetState(m_section);
        }
        else if (IsOperator(m_sc.ch))
        {
            m_sc.SetState(m_section + FB_OPERATOR);

            while (
                IsOperator(m_sc.ch)
                && !m_sc.Match('/', '\'')
                && (m_sc.ch != '.' || !IsADigit(m_sc.chNext, 10))
            ) {
                m_sc.Forward();
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
        // set to default section so proper section background
        // colour could fill the screen as needed.
        // in case of multiline non-default styles they
        // will be continued in ParseLineStart
        if (!IsInComment(m_sc.state)) m_sc.SetState(m_section);

        Log(">>> ParseLineEnd : m_section=%d", m_section);
        Log(">>> ParseLineEnd : m_lineState=%d", m_lineState);

        // set comment nest level
        m_lineState = (m_lineState & 0xFFFFFF00) | m_curNcLevel;
        m_lineState = (m_lineState & 0xFFFF00FF) | (m_curNppLevel<<8);

        // set current line state
        m_styler.SetLineState(m_curLine, m_lineState);
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
     * Parse number
     */
    bool ParseNumber (int base)
    {
        // m_sc.SetState(PP_DEFAULT);
        int curPos = m_sc.currentPos;
        int fpCount = 0;

        // skip the word chars?
        while (!m_sc.atLineEnd &&
            (IsADigit(m_sc.ch, base)
             || (m_sc.ch == '.' && base == 10 && !(fpCount++)))
        ) {
            m_sc.Forward();
            m_skipForward = true;
            m_visibleChars++;
        }
        char s[25];
        getRange(curPos, m_sc.currentPos-1, s, sizeof(s));
        m_sc.ChangeState(m_section + FB_NUMBER);
        m_sc.SetState(m_section);
        return true;
    }


    /**
     * Retreave a range of characters into a string
     */
    void getRange(unsigned int start, unsigned int end,
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
    bool GetNextInput (unsigned int start, char *s, unsigned int len)
    {
        unsigned int i = 0;
        s[i] = '\0';

        // skip any spaces or tabs
        for ( ; ; start++)
        {
            char ch = m_styler[start];
            if (IsAWordChar(ch)) break;
            if (ch == '\n' || ch == '\r') return false;
        }

        getRange (start, start + len, s, len);
        return true;
    }


    /**
     * Parse preprocessor block beginning.
     */
    void ParsePreprocessor ()
    {
        m_sc.SetState(PP_DEFAULT);
        m_sc.Forward();
        SkipSpacesAndTabs();
        if (m_sc.atLineEnd) return;
        m_skipForward = true;

        // macro's can nest and cover multiple lines
        if (m_sc.MatchIgnoreCase("macro"))
        {
            m_curNppLevel++;
            m_sc.ChangeState(GetPpKeywordStyle("macro"));
            m_sc.Forward(5);
            m_sc.SetState(PP_DEFAULT);
        }
        else if (m_sc.MatchIgnoreCase("endmacro"))
        {
            if (m_curNppLevel) m_curNppLevel--;
            m_sc.ChangeState(GetPpKeywordStyle("endmacro"));
            m_sc.Forward(8);
            if (m_curNppLevel)
            {
                m_sc.SetState(PP_DEFAULT);
                // try to style till the end of the line.
                // looks better.
            }
            else
            {
                /// @todo figure out if it is inside ASM block or not
                //m_sc.SetState(FB_DEFAULT);
                // try to style till the end of the line.
                // looks better.
                m_sc.SetState(PP_DEFAULT);
            }
            m_skipForward = true;
        }
        else if (IsAWordChar(m_sc.ch))
        {
            // m_sc.SetState(PP_DEFAULT);
            int curPos = m_sc.currentPos;
            // skip the word chars?
            while (!m_sc.atLineEnd && IsAWordChar(m_sc.ch))
            {
                m_sc.Forward();
            }
            char s[25];

            // m_sc.GetCurrent(s, sizeof(s));
            getRange(curPos, m_sc.currentPos-1, s, sizeof(s));
            m_sc.ChangeState(GetPpKeywordStyle(s));
            m_sc.SetState(PP_DEFAULT);
        }
    }


    /**
     * Parse string literals
     */
    void ParseString ()
    {
        if (m_sc.Match('"', '"'))
        {
            m_sc.SetState(m_section+FB_STRING_ESCAPE);
            m_sc.Forward(2);
            m_sc.SetState(m_section+FB_STRING);
        }
        else if (m_sc.Match('"'))
        {
            m_sc.Forward();
            m_sc.SetState(m_section);
            m_lineState &= (~LINESTATE_ESCAPE_STRING);
            return;
        }

        // not an escape code
        int indStart = m_sc.currentPos;
        bool matched = false;

        if (m_sc.ch != '\\' || !(m_lineState & LINESTATE_ESCAPE_STRING))
            return;

        m_sc.SetState(m_section+FB_STRING_ESCAPE);
        m_sc.Forward();

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
            m_sc.Forward(4);
            matched = true;
        }
        // ascii char in decimal \123
        else if (IsNumberRange(0, 3, 10))
        {
            m_sc.SetState(m_section+FB_NUMBER);
            m_sc.Forward(3);
            matched = true;
        }
        // ascii escape sequence in FB style oct, bin or hex
        else if (m_sc.ch == '&')
        {
            m_sc.SetState(m_section+FB_NUMBER);
            m_sc.Forward();
            // ascii char in hex \&hFF
            if (tolower(m_sc.ch) == 'h' && IsNumberRange(1, 2, 16))
            {
                m_sc.Forward(3);
                matched = true;
            }
            // ascii char in octal \&o777
            else if (tolower(m_sc.ch) == 'o' && IsNumberRange(1, 3, 8))
            {
                m_sc.Forward(4);
                matched = true;
            }
            // ascii char in binary \&b11111111
            else if (tolower(m_sc.ch) == 'b' && IsNumberRange(1, 8, 2))
            {
                m_sc.Forward(9);
                matched = true;
            }
            else
            {
                m_sc.ChangeState(m_section+FB_STRING);
            }
        }

        // clear or set error indicator
        m_styler.IndicatorFill(indStart, m_sc.currentPos, 0, matched ? 0 : 1);

        // set back to string
        m_sc.SetState(m_section+FB_STRING);
        m_skipForward = true;
    }


    /**
     * Check if range of characters are numbers of the base
     */
    bool IsNumberRange (int startPos, int count, int base)
    {
        for (--count; count>=0; count--)
        {
            Log("count=%d", count);
            if (!IsADigit(m_sc.GetRelative(startPos+count), base))
                return false;
        }
        return true;
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
        m_sc.Forward();
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
     * Parse the comments and look for urls inside
     */
    void ParseComment ()
    {
        // if this is a nested comment check if it ends ?
        if (m_curNcLevel)
        {
            if (m_sc.Match('\'', '/'))
            {
                if (m_curNcLevel) m_curNcLevel--;
                m_sc.Forward(2);
                if (m_curNcLevel == 0)
                {
                    //SkipSpacesAndTabs();
                    //if (!m_sc.atLineEnd)
                    //{
                        m_sc.SetState(m_section);
                    //}
                    return;
                }
            }
            else if (m_sc.Match('/', '\''))
            {
                m_curNcLevel++;
                m_sc.Forward(2);
            }
        }

        if (IsInCommentUrl(m_sc.state))
        {
            if (IsASpaceOrTab(m_sc.ch) || m_sc.atLineEnd)
                m_sc.SetState(m_section + FB_COMMENT);
        }
        else
        {
            // needs a non word char infornt
            if (IsAWordChar(m_sc.chPrev)) return;

            // match uri schemes. if matched then set state
            if (int len = FindMatch(URI_SCHEMES))
            {
                m_sc.SetState(m_section + FB_COMMENT_URL);
                m_sc.Forward(len);
            }
        }
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
     * If the given keyword is a preprocessor keyword then
     * highlight it to appropriate PP keyword style
     */
    int GetPpKeywordStyle (const char * kw)
    {
        if (m_keywords[KEYWORD_PP_ONE]->InList(kw))
            return PP_KEYWORD_ONE;
        else if (m_keywords[KEYWORD_PP_ONE]->InList(kw))
            return PP_KEYWORD_TWO;
        return PP_IDENTIFIER;
    }


    /**
     * Get FB keyword type. If not found FB_IDENTIFIER is returned
     */
    int GetFbKeywordType (const char * kw, int defStyle = FB_IDENTIFIER)
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
    int GetAsmKeywordType (const char * kw, int defStyle = ASM_IDENTIFIER)
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
    void Log (const char * msg, ...) __attribute__((format(printf, 2, 3)))
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


    /**
     * Get Section type
     */
    int GetSectionType (int style)
    {
        if (style <= FB_SECTION_LAST)
            return SECTION_FB;

        if (style <= ASM_SECTION_LAST)
            return SECTION_ASM;

        if (style <= PP_SECTION_LAST)
            return SECTION_PP;

        return SECTION_FB;
    }


    /**
     * Find previous section
     */
    int GetPrevSectionType ()
    {
        int p = m_sc.currentPos;
        if (!p) return FB_DEFAULT;

        while (--p)
        {
            int prevSection = GetSectionType(m_styler.StyleAt(p));
            if (prevSection != m_section)
                return prevSection;
        }

        return SECTION_FB;
    }

    /**
     * Skip the tabs and spaces
     * but stop at anything else
     */
    void SkipSpacesAndTabs ()
    {
        while (!m_sc.atLineEnd && IsASpaceOrTab(m_sc.ch))
            m_sc.Forward();
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
     * Find the string from an array that matches
     * and return it's length
     * or 0 if none
     */
    int FindMatch (const char * matches[])
    {
        for (int i = 0; matches[i]; i++)
            if (m_sc.MatchIgnoreCase(matches[i]))
                return strlen(matches[i]);
        return 0;
    }


    /**
     * Check if any of the chars matches ?
     * return char that matches or a 0
     */
    char FindMatch(const char matches[])
    {
        for (int i = 0; matches[i]; i++)
            if (m_sc.ch == matches[i])
                return matches[i];
        return 0;
    }


    // style context
    StyleContext  & m_sc;
    // Accessor
    Accessor      & m_styler;
    // keywords
    WordList     ** m_keywords;
    // current section
    int             m_section;
    // current line
    int             m_curLine;
    // current line state
    int             m_lineState;
    // current nested comment level
    int             m_curNcLevel;
    // nested pp level
    int             m_curNppLevel;
    // count visible characters
    int             m_visibleChars;
    // current FB dialect
    int             m_dialect;
    // Force to skip default call to Forward()
    bool            m_skipForward;
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
    l.Log(">>>> styled from: %d, length: %d\n---------------------------------------------------------------", startPos, length);
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
    NULL,                   // fold proc
    fbWordListDesc,         // keywords descriptor
    6                       // number of style bits used
);
