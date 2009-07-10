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

#ifndef EDITORSTYLEINFO_H_INCLUDED
#define EDITORSTYLEINFO_H_INCLUDED

#define STYLE_DEFAULT_FONT_SIZE   10
#define STYLE_DEFAULT_FG          _T("black")
#define STYLE_DEFAULT_BG          _T("white")
#define STYLE_DEFAULT_FONT        _T("Courier New")

namespace fb
{

    /**
     * Contains info about the styling
     *
     * @todo add opacity and width
     */
    struct DLLIMPORT CStyleInfo
    {
        bool        isOk;
        bool        bold;
        bool        italic;
        bool        underlined;
        int         size;
        int         width;
        int         opacity;
        wxString    font;
        wxColor     fg;
        wxColor     bg;
        wxColor     outline;

        CStyleInfo ()
            : isOk(false),
              bold(false), italic(false), underlined(false),
              size(STYLE_DEFAULT_FONT_SIZE), width(0), opacity(0),
              font(STYLE_DEFAULT_FONT)
        {}


        /**
         * Get style info as string
         */
        wxString AsString () const
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
            tmp << _T("color : ");
            tmp << fg.GetAsString(wxC2S_NAME | wxC2S_HTML_SYNTAX);
            tmp << _T(";\n");

            // bg
            tmp << _T("background-color : ");
            tmp << bg.GetAsString(wxC2S_NAME | wxC2S_HTML_SYNTAX);
            tmp << _T(";\n");

            // border
            tmp << _T("outline-color : ");
            tmp << outline.GetAsString(wxC2S_NAME | wxC2S_HTML_SYNTAX);
            tmp << _T(";\n");

            // opacity
            tmp << _T("opacity : ");
            tmp << opacity;
            tmp << _T(";");

            return tmp;
        }
    };

    /**
     * Styles manager
     */
    class DLLIMPORT CStyleParser
    {
        public :

            // construct the style info object
            CStyleParser (const wxString & filename);

            // Parse the file
            void LoadFile (const wxString & filename);

            // Add CSS rule
            void SetCssRule (
                const wxString & selector,
                const wxString & rule,
                const wxArrayString & values,
                const wxString & inheritFrom = _T(".default")
            );

            // Add CSS rule
            // Same as above but only one value
            void SetCssRule (
                const wxString & selector,
                const wxString & rule,
                const wxString & value,
                const wxString & inheritFrom = _T(".default")
            );

            // check if path exists
            bool PathExists (const wxString & selector);

            // Get styles for the selector
            CStyleInfo GetStyle (const wxString & selector, const wxString & inherit = _T(".default"));

        private :

            // private data implementation "pimpl"
            struct CData;
            std::auto_ptr<CData> m_data;

    };

};

#endif // EDITORSTYLEINFO_H_INCLUDED
