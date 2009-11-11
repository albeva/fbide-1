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
#pragma once


namespace fbi
{
    
    /**
     * Default values
     */
    static const int    StyleDefaultFontSize   = 10;
    static const char * StyleDefaultForeColor  = "black";
    static const char * StyleDefaultBackColor  = "white";
    static const char * StyleDefaultFont       = "Courier New";


    
    /**
     * Contain styling information for
     * a single rule
     */
    struct SDK_DLL StyleInfo
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

        // default constructor
        StyleInfo ();

        // get styleinfo as a string
        wxString AsString () const;
    };


    /**
     * Styl parser.
     */
    class SDK_DLL StyleParser
    {
        public :

            // construct the style info object
            StyleParser (const wxString & filename = "");

            // Parse the file
            void LoadFile (const wxString & filename);

            // Add CSS rule
            void SetCssRule (
                const wxString & selector,
                const wxString & rule,
                const wxArrayString & values,
                const wxString & inheritFrom = ".default"
            );

            // Add CSS rule
            // Same as above but only one value
            void SetCssRule (
                const wxString & selector,
                const wxString & rule,
                const wxString & value,
                const wxString & inheritFrom = ".default"
            );

            // check if path exists
            bool PathExists (const wxString & selector);

            // Get styles for the selector
            const StyleInfo & GetStyle (const wxString & selector, const wxString & inherit = ".default");

        private :

            // data structure holding style info
            struct CssRule
            {
                wxString        inheritFrom;
                StringHashMap   styles;
            };

            // get rule
            wxString GetRule (CssRule & info, const wxString & rule, const wxString & selector);

            // as name says
            void ResetStyleInfoCache ();

            // Hold the CSS rule blocks
            typedef HashMap<CssRule> CssRuleMap;

            // cache styleinfo based on paths
            typedef HashMap<StyleInfo> StyleInfoMap;
            
            // data
            StyleInfoMap    m_styleInfoMap;
            CssRuleMap      m_cssRuleMap;
            StyleInfo       m_defaultStyle;
    };
}
