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
#include "Manager.h"
#include "UiManager.h"
#include "TypeManager.h"
#include "Document.h"

using namespace fbi;


/**
 * Manager class implementation
 */
struct TheTypeManager : TypeManager
{

    // forward ref
    struct TypeInfo;

    // Hold a list of typeinfos
    typedef std::vector<TypeInfo*> TypeInfoVector;


    // Register new type
    virtual void Register (const wxString & type,
                           const wxString & desc,
                           DocCreatorFn creator )
    {
        // type already registered?
        if (IsRegistered(type))
        {
            wxLogError("Type '%s' already registered with TypeManager", type);
            return;
        }

        // create type and set info
        auto & info = m_typeMap[type];
        info.isAlias    = false;
        info.type       = type;
        info.desc       = desc;
        info.creator    = creator;
    }


    // Bind file extensions to the type
    virtual void BindExtensions (const wxString & type,
                                 const wxString & extensions)
    {
        // find one level only
        auto info = FindType(type, false);
        if (info == nullptr)
        {
            wxLogError("Type '%s' is not registered with TypeManager", type);
            return;
        }

        // explode extensions into an array
        wxArrayString arr;
        ExplodeString(extensions, arr, ";");

        // add the extensions
        for (int i = 0; i < arr.Count(); i++)
        {
            // Get the extension anc clean it
            wxString ext = arr[i].Lower().Trim();
            //if (!ext.Len()) continue;

            // avoid duplicates
            if (info->exts.Index(ext) != wxNOT_FOUND) continue;

            // Bind the extenions
            info->exts.Add(ext);
            m_extMap[ext].push_back(info);
        }
    }


    // Bind type alias to an existing type
    virtual void BindAlias (const wxString & alias,
                            const wxString & target,
                            bool overwrite)
    {
        // Get target type. However One alias may point to another
        // and later alias might change the target. So resolve one level only
        auto targetInfo = FindType(target, false);
        if (targetInfo == nullptr)
        {
            wxLogError("Target '%s' is not registered with TypeManager", target);
            return;
        }

        // Get alias type
        auto aliasInfo = FindType(alias, false);
        if (aliasInfo != nullptr)
        {
            // type exists ...
            if (!overwrite || !aliasInfo->isAlias)
            {
                wxLogError("Cannot convert '%s' into an alias in TypeManager", alias);
                return;
            }

            // overwrite the alias
            auto oldTarget = aliasInfo->alias;
            aliasInfo->alias = targetInfo;

            // check for circular references
            auto check = aliasInfo;
            while (check->isAlias)
            {
                // circular reference
                if (check->alias = aliasInfo)
                {
                    wxLogError("Alias '%s' creates a circular reference with '%s' in TypeManager", alias, target);
                    aliasInfo->alias = oldTarget;
                    return;
                }
                check = check->alias;
            }
            return;
        }

        // add new
        auto & entry = m_typeMap[alias];
        entry.isAlias = true;
        entry.alias = targetInfo;
    }

    
    // Create new document. Deduct type from extension
    // if failed show a dialog with an option to open the file
    // with any other registered loaders or use OS to open it
    // return nullptr if not opened in the editor
    virtual Document * CreateFromFile (const wxString & file)
    {
        // get the extension and clean it
        wxString ext = wxFileName(file).GetExt().Lower().Trim();
        
        // if extension is already registered ?
        auto extIter = m_extMap.find(ext);
        if (extIter != m_extMap.end())
        {
            auto doc =_createFromExt( extIter->second, true );;
            if (doc != nullptr) _loadFile( doc, file );
            return doc;
        }

        // Get different loaders and ignore aliases
        TypeInfoVector list;
        for (auto iter = m_typeMap.begin(); iter != m_typeMap.end(); iter++)
        {
            TypeInfo * info = &iter->second;
            while (info->isAlias) info = info->alias;
            if (std::find(list.begin(), list.end(), info) == list.end()) list.push_back(info);
        }

        // choises.
        wxArrayString choices;
        for (int i = 0; i < list.size(); i++) choices.Add(list[i]->desc);

        // Allow user to select the document type to create
        // todo - add option to open with external program
        // todo - add option to use shell to open
        auto & lang = GET_LANG();
        wxSingleChoiceDialog select(
            GET_FRAME(),
            lang.Get("open.document.type.message",  "file", file),
            lang.Get("open.document.type.title",    "file", file),
            choices
        );
        if (select.ShowModal() == wxID_CANCEL) return nullptr;

        // create the document and load file
        auto doc = list[select.GetSelection()]->creator();
        if (doc != nullptr) _loadFile( doc, file );
        return doc;
    }


    // Load file
    void _loadFile ( Document * doc, const wxString & file )
    {
        auto & lang = GET_LANG();

        wxFileName f(file);
        if (!f.FileExists())
        {
            wxMessageBox(lang.Get("error.file-not-found", "filename", file));
            return;
        }

        if (doc->LoadDocFile(file))
        {
            doc->SetDocFilename(file);
            doc->SetDocTitle(f.GetFullName());
        }
    }


    // Create new document using type name. If not registred
    // return nullptr and log a warning.
    virtual Document * CreateFromType (const wxString & type)
    {
        auto info = FindType(type, true);
        if (info == nullptr)
        {
            wxLogError("Type '%s' is not registered with TypeManager", type);
            return nullptr;
        }
        return info->creator();
    }


    // Create new document by registered extenions.
    // if multiple creators are registered show a dialog allowing
    // to select the proper type.
    // return nullptr and log a warning if no extension is registered
    // with the type
    virtual Document * CreateFromExtension (const wxString & ext)
    {
        auto extIter = m_extMap.find(ext);
        if (extIter == m_extMap.end())
        {
            wxLogError("File extension '%s' is not registered with TypeManager", ext);
            return nullptr;
        }

        // create
        return _createFromExt( extIter->second, false );
    }


    // Create from extension
    // internal implementation
    Document * _createFromExt (TypeInfoVector & types, bool fromFile)
    {

        // there is only one registered type then return it
        if (types.size() == 1) return types[0]->creator();

        // get the types and elimiate duplicates
        TypeInfoVector list;
        for (auto iter = types.begin(); iter != types.end(); iter++)
        {
            auto info = *iter;
            while (info->isAlias) info = info->alias;
            if (std::find(list.begin(), list.end(), info) == list.end()) list.push_back(info);
        }

        // is only one type left ?
        if (list.size() == 1) return list[0]->creator();

        // choises.
        // NB! don't sort this. order is important as
        // index is used to match
        wxArrayString choices;
        for (int i = 0; i < list.size(); i++) choices.Add(list[i]->desc);

        // Allow user to select the document type to create
        auto & lang = GET_LANG();
        wxSingleChoiceDialog select(
            GET_FRAME(),
            lang["select.document.type.message"],
            lang["select.document.type.title"],
            choices
        );
        if (select.ShowModal() == wxID_CANCEL) return nullptr;

        // and create the document
        return list[select.GetSelection()]->creator();
    }


    // check if type is registered. Will resolve aliases to the 
    // actual type
    virtual bool IsRegistered ( const wxString & type )
    {
        return m_typeMap.find(type) != m_typeMap.end();
    }


    // Get file filters to be used with file load / save dialogs
    virtual wxString GetFileFilters ( bool incAllFiles )
    {
        // Collect all information needed into a map
        std::map<TypeInfo *, wxString> map;

        // iterate through the extensions
        for (auto extIter = m_extMap.begin(); extIter != m_extMap.end(); extIter++)
        {
            // get list where the extension can possibly belong
            auto & list = extIter->second;
            for (auto typeIter = list.begin(); typeIter != list.end(); typeIter++)
            {
                // resolve aliases
                TypeInfo * info = *typeIter;
                while (info->isAlias) info = info->alias;

                // add extension to the map
                wxString & exts = map[info];
                if (exts.Len()) exts << ";";
                exts << "*." << extIter->first;
            }
        }

        // generate the string
        wxString result;
        for (auto iter = map.begin(); iter != map.end(); iter++)
        {
            if (result.Len()) result << "|";
            result << iter->first->desc << " (" << iter->second << ")|" << iter->second;
        }
        
        // add All files
        if (incAllFiles)
        {
            if (result.Len()) result << "|";
            result << GET_LANG()["all-files"] << " (*.*)|*.*";
        }

        return result;
    }


    // find type by name and return TypeInfo * or nullptr
    // if not found.
    TypeInfo * FindType ( const wxString & type, bool resolveAlias )
    {
        // find
        auto iter = m_typeMap.find(type);
        if (iter == m_typeMap.end()) return nullptr;
        
        // don't care about aliases
        if (!resolveAlias) return &iter->second;
        
        // resolve alias
        TypeInfo * info = &iter->second;
        while (info->isAlias) info = info->alias;
        return info;
    }


    /**
     * PHP like explode function by Ryan Norton
     * from http://wxforum.shadonet.com/viewtopic.php?p=12929#12929
     */
    void ExplodeString( const wxString& s,
                        wxArrayString& retArray,
                        const char * cpszExp,
                        const size_t& crnStart = 0,
                        const size_t& crnCount = (size_t)-1,
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


    // hold type information
    struct TypeInfo
    {
        // is this an alias to another type ?
        bool isAlias;
        // type name
        wxString type;
        // type description
        wxString desc;
        // type extensions
        wxArrayString exts;
        // if is alias then a pointer to typeinfo it points to
        // otherwise document creaor function pointer
        union
        {
            TypeInfo * alias;
            DocCreatorFn creator;
        };
    };

    // hold types
    HashMap<TypeInfo> m_typeMap;
    
    // hold extension to typeinfo
    HashMap<TypeInfoVector> m_extMap;
};


// Implement Manager
IMPLEMENT_MANAGER(TypeManager, TheTypeManager)
