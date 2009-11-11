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
#include "sdk/TypeManager.h"
#include "sdk/DocManager.h"
#include "sdk/PluginManager.h"
#include "sdk/EditorManager.h"
#include "sdk/StyleInfo.h"

#include <wx/gdicmn.h>

using namespace fb;

/**
 * Private data of CManager class
 */
struct CManager::CData
{
    CData () : /*m_uiManager(0L), */m_typeManager(0L),
               m_docManager(0L), m_pluginManager(0L),
               m_frame(0L)
    {}

    // common typedefs
    typedef std::map<wxString, int> IdMap;
    typedef std::map<wxString, wxString> StrMap;
    typedef std::map<wxString, CStyleParser *> CStyleMap;

    // public properties
    // CUiManager      * m_uiManager;
    CTypeManager    * m_typeManager;
    CDocManager     * m_docManager;
    CPluginManager  * m_pluginManager;
    wxFrame         * m_frame;
    CStyleMap       m_stylesMap;
    CRegistry       m_registry;
    IdMap           m_idMap;
    StrMap          m_langMap;
};



 /**
  * Manager Constructor
  */
CManager::CManager () : m_data(new CManager::CData), m_shutdown(false)
{
    return;
}



/**
 * Manager Destructor
 *
 * @todo Clean up all active sub managers...
 */
CManager::~CManager ()
{
    m_shutdown = true;
    if (m_data->m_pluginManager != 0L)  delete m_data->m_pluginManager;
    if (m_data->m_docManager != 0L)     delete m_data->m_docManager;
    CUiManager::Free();
    if (m_data->m_typeManager != 0L)    delete m_data->m_typeManager;
    CEditorManager::Free();
}



/**
 * return CManager instance
 */
CManager * CManager::Get ()
{
    static CManager * m_instance = 0L;
    if (m_instance == 0L) m_instance = new CManager;
    return m_instance;
}



/**
 * release CManager instance
 */
void CManager::Free ()
{
    delete CManager::Get();
}


/**
 * Get style info
 */
CStyleParser * CManager::GetStyleParser (const wxString & name)
{
    return m_data->m_stylesMap[name];
}


/**
 * Add style info
 */
void CManager::SetStyleParser (const wxString & name, CStyleParser * parser)
{
    m_data->m_stylesMap[name] = parser;
}


/**
 * return registry object
 */
CRegistry & CManager::GetReg ()
{
    return m_data->m_registry;
}



/**
 * return text for value name.
 */
const wxString & CManager::GetLang (const wxString & name)
{
    CData::StrMap::iterator iter = m_data->m_langMap.find(name);
    if (iter == m_data->m_langMap.end()) return name;
    return iter->second;
}



/**
 * Register ID by name
 */
void CManager::RegisterId (const wxString & name, int id)
{
    m_data->m_idMap[name] = id;
}



/**
 * Get ID by name
 */
int CManager::GetId (const wxString & name)
{
    CData::IdMap::iterator iter = m_data->m_idMap.find(name);
    if (iter == m_data->m_idMap.end()) return wxID_ANY;
    return iter->second;
}



/**
 * Set application frame
 */
void CManager::SetFrame (wxFrame * frame)
{
    if (0L == m_data->m_frame) m_data->m_frame = frame;
    else throw EXCEPTION(_T("Frame already set"));
}



/**
 * return application frame
 */
wxFrame * CManager::GetFrame ()
{
    return m_data->m_frame;
}



/**
 * Return instance of CUiManager
 */
CUiManager * CManager::GetUiManager()
{
    return CUiManager::Get();
}



/**
 * return instance of CTypeManager
 */
CTypeManager * CManager::GetTypeManager ()
{
    if (m_data->m_typeManager == 0L) m_data->m_typeManager = new CTypeManager;
    return m_data->m_typeManager;
}



/**
 * Return instance of CDocManager
 */
CDocManager * CManager::GetDocManager ()
{
    if (m_data->m_docManager == 0L) m_data->m_docManager = new CDocManager;
    return m_data->m_docManager;
}



/**
 * Return instance of CPluginMmanager
 */
CPluginManager * CManager::GetPluginManager ()
{
    if (m_data->m_pluginManager == 0L) m_data->m_pluginManager = new CPluginManager;
    return m_data->m_pluginManager;
}


/**
 * Return editor manager
 */
CEditorManager  * CManager::GetEditorManager ()
{
    return CEditorManager::Get();
}



/**
 * Load language file
 */
void CManager::LoadLang (const wxString & file)
{
    if (!wxFileExists(file)) throw EXCEPTION(_T("Language file '") + file + _T("' not found!"));
    wxFileInputStream is (file);
    wxFileConfig ini(is);

    long index = 0;
    wxString entry;
    if (!ini.GetFirstEntry (entry, index)) return;

    do
    {
        m_data->m_langMap[entry] = ini.Read(entry, _T(""));
    } while (ini.GetNextEntry(entry, index));
}



/**
 * Load config file
 */
void CManager::LoadConfig (const wxString & file)
{
    if (!wxFileExists(file)) throw EXCEPTION(_T("Config file '") + file + _T("' not found!"));
    wxFileInputStream is (file);
    wxFileConfig ini(is);

    long index = 0;
    wxString entry;
    if (!ini.GetFirstEntry (entry, index)) return;

    do
    {
        wxString value = ini.Read(entry, _T(""));
        m_data->m_registry[entry] = value;
    } while (ini.GetNextEntry(entry, index));
}



/**
 *
 */
void CManager::SaveConfig (const wxString & file)
{
    wxFileInputStream is (file);
    wxFileConfig ini(is);

    CRegistry::iterator iter = m_data->m_registry.begin();
    while (iter != m_data->m_registry.end())
    {
        ini.Write(iter->first, iter->second.AsString());
        iter++;
    }

    wxFileOutputStream os(file);
    ini.Save(os);
}


/**
 * Load color palette. Extend the wxColourDatabase
 * to include CSS style colour names.
 * @todo in the future palette should be loaded from file!
 */
void CManager::SetupPalette (const wxString & filename)
{
	wxTheColourDatabase->AddColour(_T("AliceBlue"),				wxColour(_T("#F0F8FF")));
	wxTheColourDatabase->AddColour(_T("AntiqueWhite"),			wxColour(_T("#FAEBD7")));
	wxTheColourDatabase->AddColour(_T("Aqua"),					wxColour(_T("#00FFFF")));
	wxTheColourDatabase->AddColour(_T("Aquamarine"),			wxColour(_T("#7FFFD4")));
	wxTheColourDatabase->AddColour(_T("Azure"),					wxColour(_T("#F0FFFF")));
	wxTheColourDatabase->AddColour(_T("Beige"),					wxColour(_T("#F5F5DC")));
	wxTheColourDatabase->AddColour(_T("Bisque"),				wxColour(_T("#FFE4C4")));
	wxTheColourDatabase->AddColour(_T("Black"),					wxColour(_T("#000000")));
	wxTheColourDatabase->AddColour(_T("BlanchedAlmond"),		wxColour(_T("#FFEBCD")));
	wxTheColourDatabase->AddColour(_T("Blue"),					wxColour(_T("#0000FF")));
	wxTheColourDatabase->AddColour(_T("BlueViolet"),			wxColour(_T("#8A2BE2")));
	wxTheColourDatabase->AddColour(_T("Brown"),					wxColour(_T("#A52A2A")));
	wxTheColourDatabase->AddColour(_T("BurlyWood"),				wxColour(_T("#DEB887")));
	wxTheColourDatabase->AddColour(_T("CadetBlue"),				wxColour(_T("#5F9EA0")));
	wxTheColourDatabase->AddColour(_T("Chartreuse"),			wxColour(_T("#7FFF00")));
	wxTheColourDatabase->AddColour(_T("Chocolate"),				wxColour(_T("#D2691E")));
	wxTheColourDatabase->AddColour(_T("Coral"),					wxColour(_T("#FF7F50")));
	wxTheColourDatabase->AddColour(_T("CornflowerBlue"),		wxColour(_T("#6495ED")));
	wxTheColourDatabase->AddColour(_T("Cornsilk"),				wxColour(_T("#FFF8DC")));
	wxTheColourDatabase->AddColour(_T("Crimson"),				wxColour(_T("#DC143C")));
	wxTheColourDatabase->AddColour(_T("Cyan"),					wxColour(_T("#00FFFF")));
	wxTheColourDatabase->AddColour(_T("DarkBlue"),				wxColour(_T("#00008B")));
	wxTheColourDatabase->AddColour(_T("DarkCyan"),				wxColour(_T("#008B8B")));
	wxTheColourDatabase->AddColour(_T("DarkGoldenRod"),			wxColour(_T("#B8860B")));
	wxTheColourDatabase->AddColour(_T("DarkGray"),				wxColour(_T("#A9A9A9")));
	wxTheColourDatabase->AddColour(_T("DarkGreen"),				wxColour(_T("#006400")));
	wxTheColourDatabase->AddColour(_T("DarkKhaki"),				wxColour(_T("#BDB76B")));
	wxTheColourDatabase->AddColour(_T("DarkMagenta"),			wxColour(_T("#8B008B")));
	wxTheColourDatabase->AddColour(_T("DarkOliveGreen"),		wxColour(_T("#556B2F")));
	wxTheColourDatabase->AddColour(_T("Darkorange"),			wxColour(_T("#FF8C00")));
	wxTheColourDatabase->AddColour(_T("DarkOrchid"),			wxColour(_T("#9932CC")));
	wxTheColourDatabase->AddColour(_T("DarkRed"),				wxColour(_T("#8B0000")));
	wxTheColourDatabase->AddColour(_T("DarkSalmon"),			wxColour(_T("#E9967A")));
	wxTheColourDatabase->AddColour(_T("DarkSeaGreen"),			wxColour(_T("#8FBC8F")));
	wxTheColourDatabase->AddColour(_T("DarkSlateBlue"),			wxColour(_T("#483D8B")));
	wxTheColourDatabase->AddColour(_T("DarkSlateGray"),			wxColour(_T("#2F4F4F")));
	wxTheColourDatabase->AddColour(_T("DarkTurquoise"),			wxColour(_T("#00CED1")));
	wxTheColourDatabase->AddColour(_T("DarkViolet"),			wxColour(_T("#9400D3")));
	wxTheColourDatabase->AddColour(_T("DeepPink"),				wxColour(_T("#FF1493")));
	wxTheColourDatabase->AddColour(_T("DeepSkyBlue"),			wxColour(_T("#00BFFF")));
	wxTheColourDatabase->AddColour(_T("DimGray"),				wxColour(_T("#696969")));
	wxTheColourDatabase->AddColour(_T("DodgerBlue"),			wxColour(_T("#1E90FF")));
	wxTheColourDatabase->AddColour(_T("FireBrick"),				wxColour(_T("#B22222")));
	wxTheColourDatabase->AddColour(_T("FloralWhite"),			wxColour(_T("#FFFAF0")));
	wxTheColourDatabase->AddColour(_T("ForestGreen"),			wxColour(_T("#228B22")));
	wxTheColourDatabase->AddColour(_T("Fuchsia"),				wxColour(_T("#FF00FF")));
	wxTheColourDatabase->AddColour(_T("Gainsboro"),				wxColour(_T("#DCDCDC")));
	wxTheColourDatabase->AddColour(_T("GhostWhite"),			wxColour(_T("#F8F8FF")));
	wxTheColourDatabase->AddColour(_T("Gold"),					wxColour(_T("#FFD700")));
	wxTheColourDatabase->AddColour(_T("GoldenRod"),				wxColour(_T("#DAA520")));
	wxTheColourDatabase->AddColour(_T("Gray"),					wxColour(_T("#808080")));
	wxTheColourDatabase->AddColour(_T("Green"),					wxColour(_T("#008000")));
	wxTheColourDatabase->AddColour(_T("GreenYellow"),			wxColour(_T("#ADFF2F")));
	wxTheColourDatabase->AddColour(_T("HoneyDew"),				wxColour(_T("#F0FFF0")));
	wxTheColourDatabase->AddColour(_T("HotPink"),				wxColour(_T("#FF69B4")));
	wxTheColourDatabase->AddColour(_T("IndianRed "),			wxColour(_T("#CD5C5C")));
	wxTheColourDatabase->AddColour(_T("Indigo "),				wxColour(_T("#4B0082")));
	wxTheColourDatabase->AddColour(_T("Ivory"),					wxColour(_T("#FFFFF0")));
	wxTheColourDatabase->AddColour(_T("Khaki"),					wxColour(_T("#F0E68C")));
	wxTheColourDatabase->AddColour(_T("Lavender"),				wxColour(_T("#E6E6FA")));
	wxTheColourDatabase->AddColour(_T("LavenderBlush"),			wxColour(_T("#FFF0F5")));
	wxTheColourDatabase->AddColour(_T("LawnGreen"),				wxColour(_T("#7CFC00")));
	wxTheColourDatabase->AddColour(_T("LemonChiffon"),			wxColour(_T("#FFFACD")));
	wxTheColourDatabase->AddColour(_T("LightBlue"),				wxColour(_T("#ADD8E6")));
	wxTheColourDatabase->AddColour(_T("LightCoral"),			wxColour(_T("#F08080")));
	wxTheColourDatabase->AddColour(_T("LightCyan"),				wxColour(_T("#E0FFFF")));
	wxTheColourDatabase->AddColour(_T("LightGoldenRodYellow"),	wxColour(_T("#FAFAD2")));
	wxTheColourDatabase->AddColour(_T("LightGrey"),				wxColour(_T("#D3D3D3")));
	wxTheColourDatabase->AddColour(_T("LightGreen"),			wxColour(_T("#90EE90")));
	wxTheColourDatabase->AddColour(_T("LightPink"),				wxColour(_T("#FFB6C1")));
	wxTheColourDatabase->AddColour(_T("LightSalmon"),			wxColour(_T("#FFA07A")));
	wxTheColourDatabase->AddColour(_T("LightSeaGreen"),			wxColour(_T("#20B2AA")));
	wxTheColourDatabase->AddColour(_T("LightSkyBlue"),			wxColour(_T("#87CEFA")));
	wxTheColourDatabase->AddColour(_T("LightSlateGray"),		wxColour(_T("#778899")));
	wxTheColourDatabase->AddColour(_T("LightSteelBlue"),		wxColour(_T("#B0C4DE")));
	wxTheColourDatabase->AddColour(_T("LightYellow"),			wxColour(_T("#FFFFE0")));
	wxTheColourDatabase->AddColour(_T("Lime"),					wxColour(_T("#00FF00")));
	wxTheColourDatabase->AddColour(_T("LimeGreen"),				wxColour(_T("#32CD32")));
	wxTheColourDatabase->AddColour(_T("Linen"),					wxColour(_T("#FAF0E6")));
	wxTheColourDatabase->AddColour(_T("Magenta"),				wxColour(_T("#FF00FF")));
	wxTheColourDatabase->AddColour(_T("Maroon"),				wxColour(_T("#800000")));
	wxTheColourDatabase->AddColour(_T("MediumAquaMarine"),		wxColour(_T("#66CDAA")));
	wxTheColourDatabase->AddColour(_T("MediumBlue"),			wxColour(_T("#0000CD")));
	wxTheColourDatabase->AddColour(_T("MediumOrchid"), 			wxColour(_T("#BA55D3")));
	wxTheColourDatabase->AddColour(_T("MediumPurple"),			wxColour(_T("#9370D8")));
	wxTheColourDatabase->AddColour(_T("MediumSeaGreen"),		wxColour(_T("#3CB371")));
	wxTheColourDatabase->AddColour(_T("MediumSlateBlue"),		wxColour(_T("#7B68EE")));
	wxTheColourDatabase->AddColour(_T("MediumSpringGreen"),		wxColour(_T("#00FA9A")));
	wxTheColourDatabase->AddColour(_T("MediumTurquoise"),		wxColour(_T("#48D1CC")));
	wxTheColourDatabase->AddColour(_T("MediumVioletRed"),		wxColour(_T("#C71585")));
	wxTheColourDatabase->AddColour(_T("MidnightBlue"),			wxColour(_T("#191970")));
	wxTheColourDatabase->AddColour(_T("MintCream"),				wxColour(_T("#F5FFFA")));
	wxTheColourDatabase->AddColour(_T("MistyRose"),				wxColour(_T("#FFE4E1")));
	wxTheColourDatabase->AddColour(_T("Moccasin"),				wxColour(_T("#FFE4B5")));
	wxTheColourDatabase->AddColour(_T("NavajoWhite"),			wxColour(_T("#FFDEAD")));
	wxTheColourDatabase->AddColour(_T("Navy"),					wxColour(_T("#000080")));
	wxTheColourDatabase->AddColour(_T("OldLace"),				wxColour(_T("#FDF5E6")));
	wxTheColourDatabase->AddColour(_T("Olive"),					wxColour(_T("#808000")));
	wxTheColourDatabase->AddColour(_T("OliveDrab"),				wxColour(_T("#6B8E23")));
	wxTheColourDatabase->AddColour(_T("Orange"),				wxColour(_T("#FFA500")));
	wxTheColourDatabase->AddColour(_T("OrangeRed"),				wxColour(_T("#FF4500")));
	wxTheColourDatabase->AddColour(_T("Orchid"),				wxColour(_T("#DA70D6")));
	wxTheColourDatabase->AddColour(_T("PaleGoldenRod"),			wxColour(_T("#EEE8AA")));
	wxTheColourDatabase->AddColour(_T("PaleGreen"),				wxColour(_T("#98FB98")));
	wxTheColourDatabase->AddColour(_T("PaleTurquoise"),			wxColour(_T("#AFEEEE")));
	wxTheColourDatabase->AddColour(_T("PaleVioletRed"),			wxColour(_T("#D87093")));
	wxTheColourDatabase->AddColour(_T("PapayaWhip"),			wxColour(_T("#FFEFD5")));
	wxTheColourDatabase->AddColour(_T("PeachPuff"),				wxColour(_T("#FFDAB9")));
	wxTheColourDatabase->AddColour(_T("Peru"),					wxColour(_T("#CD853F")));
	wxTheColourDatabase->AddColour(_T("Pink"),					wxColour(_T("#FFC0CB")));
	wxTheColourDatabase->AddColour(_T("Plum"),					wxColour(_T("#DDA0DD")));
	wxTheColourDatabase->AddColour(_T("PowderBlue"),			wxColour(_T("#B0E0E6")));
	wxTheColourDatabase->AddColour(_T("Purple"),				wxColour(_T("#800080")));
	wxTheColourDatabase->AddColour(_T("Red"),					wxColour(_T("#FF0000")));
	wxTheColourDatabase->AddColour(_T("RosyBrown"),				wxColour(_T("#BC8F8F")));
	wxTheColourDatabase->AddColour(_T("RoyalBlue"),				wxColour(_T("#4169E1")));
	wxTheColourDatabase->AddColour(_T("SaddleBrown"),			wxColour(_T("#8B4513")));
	wxTheColourDatabase->AddColour(_T("Salmon"),				wxColour(_T("#FA8072")));
	wxTheColourDatabase->AddColour(_T("SandyBrown"),			wxColour(_T("#F4A460")));
	wxTheColourDatabase->AddColour(_T("SeaGreen"),				wxColour(_T("#2E8B57")));
	wxTheColourDatabase->AddColour(_T("SeaShell"),				wxColour(_T("#FFF5EE")));
	wxTheColourDatabase->AddColour(_T("Sienna"),				wxColour(_T("#A0522D")));
	wxTheColourDatabase->AddColour(_T("Silver"),				wxColour(_T("#C0C0C0")));
	wxTheColourDatabase->AddColour(_T("SkyBlue"),				wxColour(_T("#87CEEB")));
	wxTheColourDatabase->AddColour(_T("SlateBlue"),				wxColour(_T("#6A5ACD")));
	wxTheColourDatabase->AddColour(_T("SlateGray"),				wxColour(_T("#708090")));
	wxTheColourDatabase->AddColour(_T("Snow"),					wxColour(_T("#FFFAFA")));
	wxTheColourDatabase->AddColour(_T("SpringGreen"),			wxColour(_T("#00FF7F")));
	wxTheColourDatabase->AddColour(_T("SteelBlue"),				wxColour(_T("#4682B4")));
	wxTheColourDatabase->AddColour(_T("Tan"),					wxColour(_T("#D2B48C")));
	wxTheColourDatabase->AddColour(_T("Teal"),					wxColour(_T("#008080")));
	wxTheColourDatabase->AddColour(_T("Thistle"),				wxColour(_T("#D8BFD8")));
	wxTheColourDatabase->AddColour(_T("Tomato"),				wxColour(_T("#FF6347")));
	wxTheColourDatabase->AddColour(_T("Turquoise"),				wxColour(_T("#40E0D0")));
	wxTheColourDatabase->AddColour(_T("Violet"),				wxColour(_T("#EE82EE")));
	wxTheColourDatabase->AddColour(_T("Wheat"),					wxColour(_T("#F5DEB3")));
	wxTheColourDatabase->AddColour(_T("White"),					wxColour(_T("#FFFFFF")));
	wxTheColourDatabase->AddColour(_T("WhiteSmoke"),			wxColour(_T("#F5F5F5")));
	wxTheColourDatabase->AddColour(_T("Yellow"),				wxColour(_T("#FFFF00")));
	wxTheColourDatabase->AddColour(_T("YellowGreen"),			wxColour(_T("#9ACD32")));
}
