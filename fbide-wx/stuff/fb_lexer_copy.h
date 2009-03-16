
/*
 * Allowed uri schemes inside the comments
 */
const char * UriSchemes[] = {
    "http://",
    "https://",
    "ftp://",
    "www.",
    0
};


const char EscapeCodes[] = {
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
 * Is it a comment?
 */
static bool IsComment(Accessor &styler, int pos, int len)
{
	return len > 0 && styler[pos] == '\'';
}


/**
 * Is it a legal type suffix ?
 */
static inline bool IsTypeCharacter(int ch)
{
	return ch == '%' || ch == '&' || ch == '@' || ch == '!' || ch == '#' ;
}


/**
 * Extended to accept accented characters
 */
static inline bool IsAWordChar(int ch)
{
	return ch >= 0x80 ||
	       (isalnum(ch) || ch == '.' || ch == '_' || ch == '$');
}

/**
 * Start of teh identifier ?
 */
static inline bool IsAWordStart(int ch)
{
	return ch >= 0x80 ||
	       (isalnum(ch) || ch == '_' || ch == '.');
}


/**
 * Skip the tabs and spaces
 * but stop at anything else
 */
static inline void SkipSpacesAndTabs (StyleContext & sc)
{
    while (!sc.atLineEnd && IsASpaceOrTab(sc.ch))
        sc.Forward();
}


/**
 * Hightlight a QB style metakeyword inside single
 * line comments if matches
 * return true if matched.
 */
static bool HighlightMetaKeyword (
    StyleContext & sc,
    const char * kw,
    int len
) {
    // return if not matches or next char is printable
    if (IsAWordChar(sc.GetRelative(len)) || !sc.MatchIgnoreCase(kw))
        return false;

    // colorise
    sc.ChangeState(PP_KEYWORD_META);
    sc.SetState(PP_KEYWORD_META);
    sc.Forward(len);
    sc.SetState(PP_DEFAULT);
    return true;
}


/**
 * Parse single line comments
 * a comment may appear in:
 * - normal code
 * - preprocessor
 * - asm block
 *
 * may start with ' or REM keyword
 * may be followed by qb compatible
 * metacommand in wich case turn it
 * into a preprocessor statament
 */
static inline void ParseSingleLineComment (
    int dialect,
    StyleContext & sc,
    Accessor & styler
) {
    // skip any spaces or tabs...
    SkipSpacesAndTabs (sc);

    // return if this line can't be a meta statament
    if (sc.atLineEnd || sc.ch != '$') return;

    // mov forth and skip any spaces
    sc.Forward();
    SkipSpacesAndTabs (sc);
    if (sc.atLineEnd) return;

    // check if this is a valid meta statament
    if (HighlightMetaKeyword(sc, "lang", 4))
        return;

    // allowed only in QB and LITE
    if (dialect == DIALECT_QB || dialect == DIALECT_LITE)
    {
        if (HighlightMetaKeyword(sc, "include", 7))
            return;

        if (HighlightMetaKeyword(sc, "dynamic", 7))
            return;

        if (HighlightMetaKeyword(sc, "static", 6))
            return;
    }
}


/**
 * Get the Keyword style
 * return FB_IDENTIFIER if not found
 */
static inline int GetFBKeywordStyle (
    WordList *keywordlists[],
    const char * kw,
    int defStyle = FB_IDENTIFIER
) {
    if (keywordlists[KEYWORD_ONE]->InList(kw))
        return FB_KEYWORD_ONE;
    else if (keywordlists[KEYWORD_TWO]->InList(kw))
        return FB_KEYWORD_TWO;
    else if (keywordlists[KEYWORD_THREE]->InList(kw))
        return FB_KEYWORD_THREE;
    else if (keywordlists[KEYWORD_FOUR]->InList(kw))
        return FB_KEYWORD_FOUR;
    return defStyle;
}


/**
 * Find the string from an array that matches
 * and return it's length
 * or 0 if none
 */
static inline int FindMatch (StyleContext & sc, const char * matches[])
{
    for (int i = 0; matches[i]; i++)
        if (sc.MatchIgnoreCase(matches[i]))
            return strlen(matches[i]);
    return 0;
}


/**
 * Check if any of the chars matches ?
 * return char that matches or a 0
 */
static inline char FindMatch(StyleContext & sc, const char matches[])
{
    for (int i = 0; matches[i]; i++)
        if (sc.ch == matches[i])
            return matches[i];
    return 0;
}


/**
 * If is URL inside the comment
 * set the style, move forward and return
 * true
 */
static inline bool IsCommentUrl (int section, StyleContext & sc, Accessor &styler)
{
    if (!IsAWordChar(styler[sc.currentPos-1]))
    {
        if (int len = FindMatch(sc, UriSchemes))
        {
            sc.SetState(section + FB_COMMENT_URL);
            sc.Forward(len);
            return true;
        }
    }
    return false;
}


/**
 * If this is default section
 */
static inline bool IsDefaultSection (int style)
{
    return style == FB_DEFAULT
        || style == ASM_DEFAULT
        || style == PP_DEFAULT
        ;
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
 * Find previous section type
 */
int GetPrevSectionType (StyleContext & sc, Accessor &styler)
{
    int p = sc.currentPos;
    if (!p) return SECTION_FB;
    int curSection = GetSectionType(sc.state);

    while (--p)
    {
        int prevSection = GetSectionType(styler.StyleAt(p));
        if (prevSection != curSection)
            return prevSection;
    }

    return SECTION_FB;
}


/**
 * Highlight part of the source code
 */
static void ColouriseFB (
    unsigned int startPos,
    int length, int initStyle,
    WordList *keywordlists[],
    Accessor &styler
)
{

    // init
	styler.StartAt(startPos, 63);
	StyleContext sc(startPos, length, initStyle, styler, 63);

	// get dialect
	int dialect = styler.GetPropertyInt("dialect", LEX::DIALECT_FB);
    int charCount = 0;
    int curLine = styler.GetLine(startPos);
    int lineState = curLine > 0 ? styler.GetLineState(curLine-1): 0;
    int curNcLevel = lineState & 0xFF;

    // do the highlighting
	for ( ; sc.More(); sc.Forward())
	{

	    // get current section
	    int section = GetSectionType(sc.state);

        // section might have changed...
        section = GetSectionType(sc.state);
        if (sc.atLineStart)
        {
            /*
            // Set previous line state
            lineState = (lineState & 0xFFFFFF00) | curNcLevel;
            styler.SetLineState(curLine, lineState);

            // this line
            curLine = styler.GetLine(sc.currentPos);

            if (!curNcLevel)
            {
                if (section != SECTION_FB)
                {
                    sc.SetState(GetPrevSectionType(sc, styler));
                }
                else sc.SetState(SECTION_FB);
            }
            else
            {
                sc.SetState(section + FB_COMMENT_MULTILINE);
            }
            */

            // no visible chars on a new line
            charCount = 0;
        }
        else if (!IsASpaceOrTab(sc.ch))
        {
            if (!IsInMultiLineComment(sc.state))
                charCount++;
        }


        // handle single line comments
        if (IsInSingleLineComment(sc.state))
        {
            if (IsCommentUrl(section, sc, styler))
                continue;
        }
        // is URL inside the comment ?
        else if (IsInCommentUrl(sc.state))
        {
            if (sc.atLineEnd || IsASpace(sc.ch))
            {
                if (curNcLevel)
                    sc.SetState(section + FB_COMMENT_MULTILINE);
                else
                    sc.SetState(section + FB_COMMENT);
            }
        }
	    // is it a multiline ?
        else if (IsInMultiLineComment(sc.state))
        {
            if (sc.Match('\'', '/'))
            {
                if (curNcLevel > 0) curNcLevel--;
                sc.Forward();
                if (curNcLevel == 0)
                {
                    sc.ForwardSetState(section);
                }
            }
            else if (sc.Match('/','\'')) {
                curNcLevel++;
                sc.Forward();
            }
            else if (IsCommentUrl(section, sc, styler))
                continue;
        }
        // handle string literals
        else if (IsInString(sc.state))
        {
            if (sc.Match('"', '"'))
            {
                sc.SetState(section+FB_STRING_ESCAPE);
                sc.Forward(2);
                sc.SetState(section+FB_STRING);
                continue;
            }
            else if (sc.Match('"'))
            {
                sc.Forward();
                sc.SetState(section);
                lineState &= (~LINESTATE_ESCAPE_STRING);
                curLine = styler.GetLine(sc.currentPos);
                styler.SetLineState(curLine, lineState);

            }
            else if (sc.ch == '\\' && lineState & LINESTATE_ESCAPE_STRING)
            {
                sc.SetState(section+FB_STRING_ESCAPE);
                sc.Forward();
                // Simplel sequences
                if (FindMatch(sc, EscapeCodes))
                {
                    sc.ForwardSetState(section+FB_STRING);
                    continue;
                }
                // unicode char in hex \uFFFF
                else if (sc.ch=='u' &&
                    IsADigit(sc.chNext, 16) &&
                    IsADigit(sc.GetRelative(2), 16) &&
                    IsADigit(sc.GetRelative(3), 16) &&
                    IsADigit(sc.GetRelative(4), 16)
                ) {
                    sc.ForwardSetState(section+FB_NUMBER_HEX);
                    sc.Forward(3);
                    sc.ForwardSetState(section+FB_STRING);
                }
                // ascii char in decimal \123
                else if (
                    IsADigit(sc.ch, 10) &&
                    IsADigit(sc.chNext, 10) &&
                    IsADigit(sc.GetRelative(2), 10)
                ) {
                    sc.SetState(section+FB_NUMBER);
                    sc.Forward(2);
                    sc.ForwardSetState(section+FB_STRING);
                }
                // ascii char in hex \&hFF
                else if (sc.ch == '&' && sc.chNext == 'h'
                    && IsADigit(sc.GetRelative(2), 16)
                    && IsADigit(sc.GetRelative(3), 16)
                ) {
                    sc.SetState(section+FB_NUMBER_HEX);
                    sc.Forward(3);
                    sc.ForwardSetState(section+FB_STRING);
                }
                // ascii char in octal \&o777
                else if (sc.ch == '&' && sc.chNext == 'o'
                    && IsADigit(sc.GetRelative(2), 8)
                    && IsADigit(sc.GetRelative(3), 8)
                    && IsADigit(sc.GetRelative(4), 8)
                ) {
                    sc.SetState(section+FB_NUMBER_OCT);
                    sc.Forward(4);
                    sc.ForwardSetState(section+FB_STRING);
                }
                // ascii char in binary \&b11111111
                else if (sc.ch == '&' && sc.chNext == 'b'
                    && IsADigit(sc.GetRelative(2), 2)
                    && IsADigit(sc.GetRelative(3), 2)
                    && IsADigit(sc.GetRelative(4), 2)
                    && IsADigit(sc.GetRelative(5), 2)
                    && IsADigit(sc.GetRelative(6), 2)
                    && IsADigit(sc.GetRelative(7), 2)
                    && IsADigit(sc.GetRelative(8), 2)
                    && IsADigit(sc.GetRelative(9), 2)
                ) {
                    sc.SetState(section+FB_NUMBER_BIN);
                    sc.Forward(9);
                    sc.ForwardSetState(section+FB_STRING);
                }
                // invalid sequence. maybe highlight it properly?
                else
                {
                    sc.SetState(section+FB_STRING);
                }
            }
        }
        */

	    // default mode
        if (IsDefaultSection(sc.state))
        {
            // is it a comment ?
            if (sc.Match('\''))
            {
                
                /*
                sc.SetState(section + FB_COMMENT);

                // if is first on the line it might
                // be metacommand
                if (charCount == 0)
                {
                    ParseSingleLineComment (dialect, sc, styler);
                }
                */

            }
            // REM keyword
            else if (
                !IsAWordChar(sc.GetRelative(3))
                && sc.MatchIgnoreCase("rem")
            ) {
                // highlight REM as a keyword if it's in the list
                sc.SetState(section + GetFBKeywordStyle(
                    keywordlists, "rem", FB_COMMENT
                ));

                if (IsASpace(sc.GetRelative(3)))
                    sc.Forward(4);
                else
                    sc.Forward(3);
                sc.SetState(section + FB_COMMENT);

                if (charCount == 0)
                    ParseSingleLineComment (dialect, sc, styler);

            }
            // multiline comment ?
            else if (sc.Match('/', '\''))
            {
                sc.SetState(section + FB_COMMENT_MULTILINE);
                curNcLevel++;
                sc.Forward();
            }
            // a string ?
            else if (sc.ch == '"')
            {
                if (dialect == DIALECT_QB || dialect == DIALECT_LITE)
                {
                    if (styler.GetPropertyInt("option.escape", false))
                    {
                        curLine = styler.GetLine(sc.currentPos);
                        lineState |= LINESTATE_ESCAPE_STRING;
                        styler.SetLineState(curLine, lineState);
                    }
                }
                sc.SetState(section + FB_STRING);
            }
            // explicitly NON escaped string literal
            else if (sc.Match('$', '"'))
            {
                sc.SetState(section + FB_STRING);
                sc.Forward();
            }
            // explicitly escaped string literal
            else if (sc.Match('!', '"'))
            {
                lineState |= LINESTATE_ESCAPE_STRING;
                curLine = styler.GetLine(sc.currentPos);
                styler.SetLineState(curLine, lineState);
                sc.SetState(section + FB_STRING);
                sc.Forward();
            }
            else if (sc.Match('#') && !charCount)
            {
                sc.SetState(PP_DEFAULT);
            }
            */
        }

        // reached the end of the line...
        if (sc.atLineEnd)
        {
            sc.SetState(GetSectionType(sc.state));
        }
	}

	// end
	sc.Complete();
}