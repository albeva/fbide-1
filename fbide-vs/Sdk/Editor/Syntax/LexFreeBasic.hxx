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
 *
 * This file is based on VB lexer for Scintilla by
 * Neil Hodgson <neilh@scintilla.org>
 */

#ifndef LEXFREEBASIC_HXX_INCLUDED
#define LEXFREEBASIC_HXX_INCLUDED

namespace LEX
{

    // the lexer ID
    const int FREEBASIC = 100;

    // lexer states
    enum
    {
        // Lexical states for FB code
        FB_DEFAULT              = 0,
        FB_IDENTIFIER           = 1,
        FB_COMMENT              = 2,
        FB_COMMENT_URL          = 3,
        FB_LABEL                = 4,
        FB_STRING               = 5,
        FB_STRING_ESCAPE        = 6,
        FB_NUMBER               = 7,
        FB_OPERATOR             = 8,
        FB_KEYWORD_ONE          = 9,
        FB_KEYWORD_TWO          = 10,
        FB_KEYWORD_THREE        = 11,
        FB_KEYWORD_FOUR         = 12,
        FB_SECTION_LAST         = FB_KEYWORD_FOUR,

        // the lexical states for ASM code
        ASM_DEFAULT             = 17,
        ASM_IDENTIFIER          = 18,
        ASM_COMMENT             = 19,
        ASM_COMMENT_URL         = 20,
        ASM_LABEL               = 21,
        ASM_STRING              = 22,
        ASM_STRING_ESCAPE       = 23,
        ASM_NUMBER              = 24,
        ASM_OPERATOR            = 25,
        ASM_FB_KEYWORD_ONE      = 26,
        ASM_FB_KEYWORD_TWO      = 27,
        ASM_FB_KEYWORD_THREE    = 28,
        ASM_FB_KEYWORD_FOUR     = 29,
        ASM_KEYWORD_ONE         = 30,
        ASM_KEYWORD_TWO         = 31,
        ASM_SECTION_LAST        = ASM_KEYWORD_TWO,

        // Lexical states for preprocessor blocks
        PP_DEFAULT              = 40,
        PP_IDENTIFIER           = 41,
        PP_COMMENT              = 42,
        PP_COMMENT_URL          = 43,
        PP_LABEL                = 44,
        PP_STRING               = 45,
        PP_STRING_ESCAPE        = 46,
        PP_NUMBER               = 47,
        PP_OPERATOR             = 48,
        PP_FB_KEYWORD_ONE       = 49,
        PP_FB_KEYWORD_TWO       = 50,
        PP_FB_KEYWORD_THREE     = 51,
        PP_FB_KEYWORD_FOUR      = 52,
        PP_KEYWORD_ONE          = 53,
        PP_KEYWORD_TWO          = 54,
        PP_SECTION_LAST         = 55
    };



    /**
     * Test if style is a single line comment
     */
    static inline bool IsInComment (int style)
    {
        return style == FB_COMMENT
            || style == ASM_COMMENT
            || style == PP_COMMENT
            ;
    }


    /**
     * returns true if this style is a comment of any kind
     */
    static inline bool IsInAnyComment (int style)
    {
        return style == FB_COMMENT
            || style == FB_COMMENT_URL
            || style == ASM_COMMENT
            || style == ASM_COMMENT_URL
            || style == PP_COMMENT
            || style == PP_COMMENT_URL
            ;
    }

    /**
     * Test if style is comment url
     */
    static inline bool IsInCommentUrl (int style)
    {
        return style == FB_COMMENT_URL
            || style == ASM_COMMENT_URL
            || style == PP_COMMENT_URL
            ;
    }

    /**
     * Test if style is a string literal
     */
    static inline bool IsInString (int style)
    {
        return style == FB_STRING
            || style == ASM_STRING
            || style == PP_STRING
            ;
    }


    /**
     * Check if is inside default section
     */
    static inline bool IsInDefaultSection (int style)
    {
        return style == FB_DEFAULT
            || style == ASM_DEFAULT
            || style == PP_DEFAULT
            ;
    }

    // Keyword types
    enum
    {
        KEYWORD_ONE     = 0,
        KEYWORD_TWO     = 1,
        KEYWORD_THREE   = 2,
        KEYWORD_FOUR    = 3,
        KEYWORD_PP_ONE  = 4,
        KEYWORD_PP_TWO  = 5,
        KEYWORD_ASM_ONE = 6,
        KEYWORD_ASM_TWO = 7
    };

    // FB DIALECT TYPE
    enum
    {
        DIALECT_QB,
        DIALECT_LITE,
        DIALECT_DEPRECATED,
        DIALECT_FB
    };

    // Line state bits
    enum
    {
        /* bits 0 to 23 reserved */

        LINESTATE_MULTILINE_ASM     = 1 << 29,
        LINESTATE_LINE_CONTINUATION = 1 << 30,
        LINESTATE_REFRESH           = 1 << 31,  // force editor refresh
    };
}

#endif // LEXFREEBASIC_HXX_INCLUDED
