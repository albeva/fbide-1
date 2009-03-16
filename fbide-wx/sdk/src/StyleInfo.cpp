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
#include "sdk/StyleInfo.h"
#include <wx/tokenzr.h>

using namespace fb;

// data structure holding style info
struct CssRule
{
    wxString            inheritFrom;
    CStringStringHashMap    styles;
};

// private data
struct CStyleParser::CData
{

    // Hold the CSS rule blocks
    typedef std::map<wxString, CssRule> CCssRuleMap;

    // cache styleinfo based on paths
    typedef std::map<wxString, CStyleInfo> CStyleInfoMap;

    CStyleInfoMap   m_styleInfoMap;
    CCssRuleMap     m_cssRuleMap;
    CStyleInfo      m_defaultStyle;
    CStyleParser  * m_owner;

    /**
     * Find rule value
     */
    wxString GetRule (CssRule & info, const wxString & rule, const wxString & selector)
    {
        // check if this rule has a value
        if (info.styles[rule].Len() && info.styles[rule] != _T("inherit"))
            return info.styles[rule];

        // no inherit or try to inherit from self?
        if (!info.inheritFrom.Len() || info.inheritFrom == selector) return _T("");

        // check if inherit exists
        if (!m_owner->PathExists(info.inheritFrom)) return _T("");

        // call recursievly
        return GetRule(m_cssRuleMap[info.inheritFrom], rule, info.inheritFrom);
    }


    // Reset styleinfo's so that changes would reflect in the
    // next call to get styles
    void resetStyleInfoCache ()
    {
        CStyleInfoMap::iterator iter = m_styleInfoMap.begin();
        for ( ; iter != m_styleInfoMap.end(); iter++)
            iter->second.isOk = false;
    }
};



/**
 * Construct new style info object
 */
CStyleParser::CStyleParser (const wxString & filename)
    : m_data(new CStyleParser::CData)
{
    // set owner to the pimpl idiom
    m_data->m_owner = this;

    // this style is returned as default
    m_data->m_defaultStyle.isOk = true;

    // Load CSS
    LoadFile(filename);
}


/**
 * Parse the CSS file.
 * This is a very simple and primitive CSS
 * parser capable only function within FBIDE
 * styling needs.
 */
void CStyleParser::LoadFile (const wxString & filename)
{
    if (!filename.Len()) return;

    if (!::wxFileExists(filename))
    {
        wxMessageBox(_T("File not found"));
        return;
    }

    // load the file
    wxTextFile file;
    file.Open(filename);

    // parse through the file
    wxString buffer;
    for (wxString str = file.GetFirstLine(); !file.Eof(); str = file.GetNextLine() )
        buffer << str << _T("\n");

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
        error.Printf(_T("Invalid character at %d:%d (%d)"), line, col, __LINE__); \
        wxMessageBox(error); \
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
void CStyleParser::SetCssRule (
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

void CStyleParser::SetCssRule (
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
    m_data->resetStyleInfoCache();

    // split classes
    wxStringTokenizer tkz(selector, _T("."));

    // iterate through the classes
    wxString partSelector;
    wxString token;
    while ( tkz.HasMoreTokens() )
    {
        // skip if empty
        token = tkz.GetNextToken();
        if (!token.Len()) continue;

        // the path to current class
        wxString path = partSelector + _T(".") + token;

        // if there are more tokens then it means that current
        // selector becomes a parent to the next one
        if (tkz.HasMoreTokens())
        {
            if (!PathExists(path))
            {
                m_data->m_cssRuleMap[path].inheritFrom = inherit;
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
            CssRule & info = m_data->m_cssRuleMap[path];
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
CStyleInfo CStyleParser::GetStyle (const wxString & selector, const wxString & inherit)
{
    // invalid path?
    if (!PathExists(selector))
    {
        SetCssRule(selector, _T("font-size"), _T("inherit"), inherit.Len() ? inherit : _T(".default"));
    }

    // get styleinfo from cache. if it exists then return it
    CStyleInfo & style = m_data->m_styleInfoMap[selector];
    if (style.isOk) return style; // creates a copy

    // get data
    CssRule & info = m_data->m_cssRuleMap[selector];
    wxString value;

    // get bold
    value = m_data->GetRule(info, _T("font-weight"), selector);
    if (value == _T("bold")) style.bold = true;

    // get italic
    value = m_data->GetRule(info, _T("font-style"), selector);
    if (value == _T("italic")) style.italic = true;

    // get underlined
    value = m_data->GetRule(info, _T("text-decoration"), selector);
    if (value == _T("underlined")) style.underlined = true;

    // get font size
    value = m_data->GetRule(info, _T("font-size"), selector);
    if (value.Len())
    {
        long size = 0;
        value.ToLong(&size);
        style.size = (int)size;
    }
    else style.size = STYLE_DEFAULT_FONT_SIZE;

    // get font size
    value = m_data->GetRule(info, _T("width"), selector);
    if (value.Len())
    {
        long width = 0;
        value.ToLong(&width);
        style.width = (int)width;
    }

    // get font size
    value = m_data->GetRule(info, _T("opacity"), selector);
    if (value.Len())
    {
        long opacity = 0;
        value.ToLong(&opacity);
        style.opacity = (int)opacity;
    }

    // get font family
    value = m_data->GetRule(info, _T("font-family"), selector);
    if (value.Len()) style.font = value;
    else style.font = STYLE_DEFAULT_FONT;

    // get color
    value = m_data->GetRule(info, _T("color"), selector);
    if (value.Len()) style.fg.Set(value);
    else style.fg.Set(STYLE_DEFAULT_FG);

    // get background color
    value = m_data->GetRule(info, _T("background-color"), selector);
    if (value.Len())
    {
        if (value != _T("none") && value != _T("transparent"))
            style.bg.Set(value);
    }
    else style.bg.Set(STYLE_DEFAULT_BG);

    // outline color
    value = m_data->GetRule(info, _T("outline-color"), selector);
    if (value.Len())
    {
        if (value != _T("none") && value != _T("transparent"))
            style.outline.Set(value);
    }
    else style.outline.Set(STYLE_DEFAULT_BG);

    style.isOk = true;
    return style; // creates a copy
}


/**
 * Check if CSS path exists
 */
bool CStyleParser::PathExists (const wxString & selector)
{
    return m_data->m_cssRuleMap.find(selector) != m_data->m_cssRuleMap.end();
}




