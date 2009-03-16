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
#include "sdk/Document.h"
#include "sdk/TypeManager.h"

using namespace fb;

struct CTypeManager::CData
{
    struct TypeData
    {
        CTypeRegistrantBase * creator;
        wxString description;
        wxString name;
    };
    typedef std::map<wxString, TypeData*> CExtMap;
    // typedef std::pair<wxString, TypeData*> CExtPair;

    WX_DECLARE_STRING_HASH_MAP(TypeData, CTypeList);
    CExtMap             m_extMap;
    CTypeList           m_list;
    CStringStringHashMap    m_aliasMap;



    /**
     * Resolve type alias if needed. Return real type
     */
    wxString ResolveType (const wxString & base)
    {
        CTypeList::iterator iter = m_list.find(base);
        if (iter != m_list.end()) return base;

        CStringStringHashMap::iterator aliasIter = m_aliasMap.find(base);
        if (aliasIter != m_aliasMap.end()) return ResolveType (aliasIter->second);
        return _T("");
    }
};




/**
 * PHP like explode function by Ryan Norton
 * from http://wxforum.shadonet.com/viewtopic.php?p=12929#12929
 */
void Explode( const wxString& s, wxArrayString& retArray,  const wxChar* cpszExp,
              const size_t& crnStart = 0,    const size_t& crnCount = (size_t)-1,
              const bool& crbCIComp = false)
{
    wxASSERT_MSG(cpszExp != NULL, wxT("Invalid value for First Param of wxString::Split (cpszExp)"));

    retArray.Clear();
    size_t  nOldPos = crnStart, nPos = crnStart;

    wxString szComp, szExp = cpszExp;

    if (crbCIComp)
    {
        szComp = s.Lower();
        szExp.MakeLower();
    }
    else
        szComp = s;

    if(crnCount == (size_t)-1)
    {
        for (; (nPos = szComp.find(szExp, nPos)) != wxString::npos;)
        {
            retArray.Add(s.Mid(nOldPos, nPos - nOldPos));
            nOldPos = nPos += szExp.Length();
        }

    }
    else
    {
        for (int i = crnCount; (nPos = szComp.find(szExp, nPos)) != wxString::npos && i != 0; --i)
        {
            retArray.Add(s.Mid(nOldPos, nPos - nOldPos));
            nOldPos = nPos += szExp.Length();
        }
    }

    if (nOldPos != s.Length()) retArray.Add(s.Mid(nOldPos) );
}




/**
 * construct. Can only be called
 * from CManager class
 */
CTypeManager::CTypeManager () : m_data(new CTypeManager::CData())
{
    return;
}


/**
 * Clean things up
 */
CTypeManager::~CTypeManager ()
{
    CData::CTypeList::iterator iter = m_data->m_list.begin();
    for (; iter != m_data->m_list.end(); iter++)
    {
        delete iter->second.creator;
    }
    return;
}


/**
 * register new Document type and associate it with the name
 */
void CTypeManager::AddType (const wxString & name, const wxString & desc, CTypeRegistrantBase * type)
{
    if (TypeExists(name)) throw EXCEPTION(_T("Document type '") + name + _T("' already registered!"));

    // add
    CData::TypeData & data = m_data->m_list[name];
    data.creator = type;
    data.name = name;
    data.description = desc;
}


/**
 * Register type alias. Useful for example binding certain
 * type identifiers to other types. For example set "default" type
 */
void CTypeManager::BindAlias (const wxString & alias, const wxString & type)
{
    if (TypeExists(alias)) throw EXCEPTION(_T("Document type '") + alias + _T("' already registered! Can't register alias."));
    if (!TypeExists(type)) throw EXCEPTION(_T("Document type '") + type + _T("' does not exist. Can't register alias."));
    m_data->m_aliasMap[alias] = type;
}


/**
 * Associate file types with the document types
 *
 * @todo allow registering multiple documents for the same
 *       extension later on.
 */
void CTypeManager::BindExtensions (const wxString & type, const wxString & ext)
{
    wxString t = m_data->ResolveType(type);
    CData::CTypeList::iterator iter = m_data->m_list.find(t);
    if (iter == m_data->m_list.end()) throw EXCEPTION(_T("Document type '") + type + _T("' not registered."));

    wxArrayString strArr;
    Explode (ext, strArr, _T(";"));

    for (size_t i = 0; i < strArr.Count(); i++)
    {
        m_data->m_extMap[strArr[i].Lower()] = &iter->second;
    }
}



/**
 * Check if given type exists
 */
bool CTypeManager::TypeExists (const wxString & type)
{
    CData::CTypeList::iterator iter = m_data->m_list.find(type);
    if (iter != m_data->m_list.end()) return true;

    CStringStringHashMap::iterator aliasIter = m_data->m_aliasMap.find(type);
    return (aliasIter != m_data->m_aliasMap.end());
}



/**
 * Create document from file extension
 */
CDocumentBase * CTypeManager::LoadFile (const wxString & file)
{
    wxFileName fn(file);

    // If file doesn't exist show the error message
    // and return NULL
    if (!fn.FileExists())
    {
        wxString msg = GET_MGR()->GetLang(_T("error.file-not-found"));
        msg.Replace(_T("{filename}"), fn.GetFullPath());
        wxMessageBox(msg);
        return NULL;
    }

    // find Creator
    CData::CExtMap::iterator iter = m_data->m_extMap.find(fn.GetExt().Lower());

    // if no loader found show error message:
    /// @todo In future provide an options for:
    ///       - open with existing handler (and allow binding the file)
    ///       - load with external program
    if (iter == m_data->m_extMap.end())
    {
        wxString msg = GET_MGR()->GetLang(_T("error.type-not-supported"));
        msg.Replace(_T("{filename}"), fn.GetFullPath());
        wxMessageBox(msg);
        return NULL;
    }

    // Create the document and set the file.
    CDocumentBase * doc = iter->second->creator->Create(fn.GetFullPath());
    doc->SetDocFile (fn.GetFullPath());
    return doc;
}



/**
 * create document from type name
 */
CDocumentBase * CTypeManager::NewDocument (const wxString & type)
{
    wxString t = m_data->ResolveType(type);
    CData::CTypeList::iterator iter = m_data->m_list.find(t);
    if (iter == m_data->m_list.end()) return 0L;
    return iter->second.creator->Create(_T(""));
}



/**
 * Get filter string for file open / save dialog
 */
wxString CTypeManager::GetFileFilters (bool allFiles)
{
    wxString filter;
    for (CData::CTypeList::iterator iter = m_data->m_list.begin(); iter != m_data->m_list.end(); iter++)
    {
        wxString ext;
        for (CData::CExtMap::iterator ext_iter = m_data->m_extMap.begin(); ext_iter != m_data->m_extMap.end(); ext_iter++)
        {
            if (ext_iter->second == &iter->second)
            {
                if (ext.Len()) ext << _T(";");
                ext << _T("*.") << ext_iter->first;
            }
        }
        if (ext.Len())
        {
            if (filter.Len()) filter << _T("|");
            filter << iter->second.description << _T(" (") << ext << _T(")|") << ext;
        }
    }
    if (allFiles)
    {
        if (filter.Len()) filter << _T("|");
        filter << _T("All files (*.*)|*.*");
    }
    return filter;
}


