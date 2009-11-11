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
#include "StyleInfo.h"

using namespace fbi;


/**
 * Construct style info and set default values
 */
StyleInfo::StyleInfo () :
    isOk(false),
    bold(false), italic(false), underlined(false),
    size(StyleDefaultFontSize), width(0), opacity(0),
    font(StyleDefaultFont),
    fg(StyleDefaultForeColor), bg(StyleDefaultBackColor)
{}


/**
 * Get style info as a string
 */
wxString StyleInfo::AsString () const
{
    wxString tmp;

    // font-weight
    tmp << _T("font-weight : ");
    if (bold) tmp << _T("bold");
    else tmp << _T("normal");
    tmp << _T(";\n");

    // font-style
    tmp << _T("font-style : ");
    if (italic) tmp << _T("italic");
    else tmp << _T("normal");
    tmp << _T(";\n");

    // text decoration
    tmp << _T("text-decoration : ");
    if (underlined) tmp << _T("underline");
    else tmp << _T("none");
    tmp << _T(";\n");

    // font size
    tmp << _T("font-size : ");
    tmp << size << _T("px");
    tmp << _T(";\n");

    // width
    tmp << _T("width : ");
    tmp << width << _T("px");
    tmp << _T(";\n");

    // fg
    if (fg.IsOk())
    {
        tmp << _T("color : ");
        tmp << fg.GetAsString(wxC2S_NAME | wxC2S_HTML_SYNTAX);
        tmp << _T(";\n");
    }

    // bg
    if (bg.IsOk())
    {
        tmp << _T("background-color : ");
        tmp << bg.GetAsString(wxC2S_NAME | wxC2S_HTML_SYNTAX);
        tmp << _T(";\n");
    }

    // border
    if (outline.IsOk())
    {
        tmp << _T("outline-color : ");
        tmp << outline.GetAsString(wxC2S_NAME | wxC2S_HTML_SYNTAX);
        tmp << _T(";\n");
    }

    // opacity
    tmp << _T("opacity : ");
    tmp << opacity;
    tmp << _T(";");

    return tmp;
}



/**
 * Construct new style info object
 */
StyleParser::StyleParser (const wxString & filename)
{
    // this style is returned as default
    m_defaultStyle.isOk = true;

    // Load CSS
    LoadFile(filename);
}


/**
 * Find rule value
 */
wxString StyleParser::GetRule (CssRule & info, const wxString & rule, const wxString & selector)
{
    // check if this rule has a value
    if (info.styles[rule].Len() && info.styles[rule] != "inherit")
        return info.styles[rule];

    // no inherit or try to inherit from self?
    if (!info.inheritFrom.Len() || info.inheritFrom == selector) return "";

    // check if inherit exists
    if (!PathExists(info.inheritFrom)) return "";

    // call recursievly
    return GetRule(m_cssRuleMap[info.inheritFrom], rule, info.inheritFrom);
}


// Reset styleinfo's so that changes would reflect in the
// next call to get styles
void StyleParser::ResetStyleInfoCache ()
{
    for (auto iter = m_styleInfoMap.begin(); iter != m_styleInfoMap.end(); iter++)
        iter->second.isOk = false;
}


/**
 * Parse the CSS file.
 * This is a very simple and primitive CSS
 * parser capable only function within FBIDE
 * styling needs.
 */
void StyleParser::LoadFile (const wxString & filename)
{
    if (!filename.Len()) return;

    if (!::wxFileExists(filename))
    {
        wxLogWarning("Style file '%s' not found", filename);
        return;
    }

    // load the file
    wxTextFile file;
    file.Open(filename);

    // parse through the file
    wxString buffer;
    for (wxString str = file.GetFirstLine(); !file.Eof(); str = file.GetNextLine() )
        buffer << str << "\n";

    // parse states
    bool inComment  = false,
         inString   = false,
         inSection  = false,
         inDefine   = false,
         inIdentifier=false;

    wxChar cCh = 0;
    wxChar pCh = 0;
    wxChar nCh = 0;
    wxString section, key, value;
    wxArrayString values;
    int line = 1, col = 0;

    #define CSS_ERROR() \
        { \
            wxString error; \
            wxLogWarning("Invalid character at %d:%d (%d)", line, col, __LINE__); \
            return; \
        }

    for (size_t i = 0; i < buffer.Len(); i++)
    {
        pCh = cCh;
        cCh = buffer[i];
        nCh = (i < buffer.Len()-1) ? buffer[i+1] : 0;

        // count lines and columns
        if (cCh == 10) { line++; col=0; }
        else col++;

        // if inside a comment
        if (inComment)
        {
            if (cCh == '*' && nCh == '/')
            {
                inComment = false;
                col++, i++;
            }
            continue;
        }
        // in quoted text ?
        else if (inString)
        {
            if (cCh == '"')
            {
                inString = false;
                values.Add(value);
                value = _T("");
            }
            else value << cCh;
            continue;
        }
        // identifier
        else if (inIdentifier)
        {
            if ((cCh >= 'a' && cCh <= 'z')
                || (cCh >= 'A' && cCh <= 'Z')
                || (cCh >= '0' && cCh <= '9')
                || cCh == '-' || cCh == '.' || cCh == '_')
            {
                if (inSection)
                {
                    if (inDefine) value << cCh;
                    else key << cCh;
                }
                else section << cCh;
                continue;
            }
            else if (inSection && inDefine)
            {
                values.Add(value);
                value = _T("");
            }
            inIdentifier = false;
        }

        // a comment ?
        if (cCh == '/' && nCh == '*')
        {
            inComment = true;
            col++, i++;
            continue;
        }
        // a whitespace ?
        if (cCh == ' ' || cCh == '\t' || cCh == 10 || cCh == 13)
        {
            continue;
        }
        // section start ?
        if (cCh == '{')
        {
            if (inSection || !section.Len()) CSS_ERROR();
            inSection = true;
            continue;
        }
        // section end ?
        if (cCh == '}')
        {
            if (!inSection || inDefine) CSS_ERROR();
            inSection = false;
            section = _T("");
            continue;
        }
        // couted text ?
        if (cCh == '"')
        {
            if (!inDefine) CSS_ERROR();
            inString = true;
            continue;
        }
        if (cCh == ':')
        {
            if (!inSection || inDefine || !key.Len())
                CSS_ERROR();

            inDefine = true;
            continue;
        }
        if (cCh == ';')
        {
            if (!inSection || !inDefine || !values.Count())
                CSS_ERROR();

            inDefine = false;
            SetCssRule (section, key, values);
            key = _T("");
            values.Clear();
            continue;
        }
        // identifier ?
        if ((cCh >= 'a' && cCh <= 'z')
            || (cCh >= 'A' && cCh <= 'Z')
            || (cCh >= '0' && cCh <= '9')
            || cCh == '-' || cCh == '.'
            || cCh == '_' || cCh == '#')
        {
            inIdentifier = true;
            if (inSection)
            {
                if (inDefine) value << cCh;
                else key << cCh;
            }
            else section << cCh;
        }
    }
    #undef CSS_ERROR
}



// Add CSS rule
void StyleParser::SetCssRule (
    const wxString & selector,
    const wxString & rule,
    const wxString & value,
    const wxString & inheritFrom
)
{
    wxArrayString values;
    values.Add(value);
    SetCssRule(selector, rule, values, inheritFrom);
}

void StyleParser::SetCssRule (
    const wxString & selector,
    const wxString & rule,
    const wxArrayString & values,
    const wxString & inheritFrom
)
{
    if (!selector.Len() || !rule.Len() || !values.Count())
        return;

    // inherit default values from class...
    wxString inherit = inheritFrom;

    // reset style cache
    ResetStyleInfoCache();

    // split classes
    wxStringTokenizer tkz(selector, ".");

    // iterate through the classes
    wxString partSelector;
    wxString token;
    while ( tkz.HasMoreTokens() )
    {
        // skip if empty
        token = tkz.GetNextToken();
        if (!token.Len()) continue;

        // the path to current class
        wxString path = partSelector + "." + token;

        // if there are more tokens then it means that current
        // selector becomes a parent to the next one
        if (tkz.HasMoreTokens())
        {
            if (!PathExists(path))
            {
                m_cssRuleMap[path].inheritFrom = inherit;
            }
            inherit = path;
            partSelector = path;
        }
        else
        {
            // need to add inherit info ?
            bool addInherit = false;

            // get the style info
            if (!PathExists(path)) addInherit = true;
            CssRule & info = m_cssRuleMap[path];
            if (addInherit) info.inheritFrom = inherit;

            // parse CSS rules
            wxString value = values[0];

            // color
            if (rule == _T("color"))
                info.styles[rule] = value;
            // background
            else if (rule == _T("background") || rule == _T("background-color"))
                info.styles[_T("background-color")] = value;
            // outline
            else if (rule == _T("outline-color"))
                info.styles[rule] = value;
            // font size
            else if (rule == _T("font-size"))
            {
                if (value.Right(2) == _T("px"))
                    info.styles[rule] = value.Left(value.Len()-2);
            }
            // font family
            else if (rule == _T("font-family"))
                info.styles[rule] = value;
            // font style
            else if (rule == _T("font-style"))
                info.styles[rule] = value;
            // font weight
            else if (rule == _T("font-weight"))
                info.styles[rule] = value;
            // text decorations
            else if (rule == _T("text-decoration"))
                info.styles[rule] = value;
            else if (rule == _T("opacity"))
                info.styles[rule] = value;
            else if (rule == _T("width"))
            {
                if (value.Right(2) == _T("px"))
                    info.styles[rule] = value.Left(value.Len()-2);
            }

        }
    }
}


/**
 * Get styles
 * If selector is not defined try to inherit styles from it's base
 * or default selector.
 */
const StyleInfo & StyleParser::GetStyle (const wxString & selector, const wxString & inherit)
{
    // invalid path?
    if (!PathExists(selector))
    {
        SetCssRule(selector, "font-size", "inherit", inherit.Len() ? inherit : ".default");
    }

    // get styleinfo from cache. if it exists then return it
    StyleInfo & style = m_styleInfoMap[selector];
    if (style.isOk) return style;

    // get data
    CssRule & info = m_cssRuleMap[selector];
    wxString value;

    // get bold
    value = GetRule(info, "font-weight", selector);
    if (value == "bold") style.bold = true;

    // get italic
    value = GetRule(info, "font-style", selector);
    if (value == "italic") style.italic = true;

    // get underlined
    value = GetRule(info, "text-decoration", selector);
    if (value == "underlined") style.underlined = true;

    // get font size
    value = GetRule(info, "font-size", selector);
    if (value.Len())
    {
        long size = 0;
        value.ToLong(&size);
        style.size = (int)size;
    }
    else style.size = StyleDefaultFontSize;

    // get font size
    value = GetRule(info, "width", selector);
    if (value.Len())
    {
        long width = 0;
        value.ToLong(&width);
        style.width = (int)width;
    }

    // get font size
    value = GetRule(info, "opacity", selector);
    if (value.Len())
    {
        long opacity = 0;
        value.ToLong(&opacity);
        style.opacity = (int)opacity;
    }

    // get font family
    value = GetRule(info, "font-family", selector);
    if (value.Len()) style.font = value;
    else style.font = StyleDefaultFont;

    // get color
    value = GetRule(info, "color", selector);
    if (value.Len()) style.fg.Set(value);
    else style.fg.Set(StyleDefaultForeColor);

    // get background color
    value = GetRule(info, "background-color", selector);
    if (value.Len())
    {
        if (value != "none" && value != "transparent") style.bg.Set(value);
    }
    else style.bg.Set(StyleDefaultBackColor);

    // outline color
    value = GetRule(info, "outline-color", selector);
    if (value.Len())
    {
        if (value != "none" && value != "transparent")
            style.outline.Set(value);
    }
    else style.outline.Set(StyleDefaultBackColor);

    style.isOk = true;
    return style;
}


/**
 * Check if CSS path exists
 */
bool StyleParser::PathExists (const wxString & selector)
{
    return m_cssRuleMap.find(selector) != m_cssRuleMap.end();
}
