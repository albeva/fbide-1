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

// get manager
#define GET_MGR()       fbi::Manager::GetInstance()
// get ui manager
#define GET_UIMGR()     GET_MGR()->GetUiManager()
// get script manager
#define GET_SCRIPTMGR() GET_MGR()->GetScriptManager()
// get document manager
#define GET_DOCMGR()    GET_MGR()->GetDocManager()
// get editor manager
#define GET_EDITORMGR() GET_MGR()->GetEditorManager()
// get plugin manager
#define GET_PLUGINMGR() GET_MGR()->GetPluginManager()
// get type manager
#define GET_TYPEMGR()   GET_MGR()->GetTypeManager()
// get global configuration registry
#define GET_REG()       GET_MGR()->GetRegistry()
// get translations
#define GET_LANG()      GET_MGR()->GetLang()
// Get id map
#define GET_IDMAP()     GET_MGR()->GetIdMap()


namespace fbi
{

    class UiManager;
    class ScriptManager;
    class DocManager;
    class EditorManager;
    class PluginManager;
    class TypeManager;
    class IdMap;

    /**
     * Main manager class. This is aproxy class that holds
     * the instances and other bookkeeping of the SDK
     * and should be used to access the SDK API
     *
     * This class is a singleton
     */
    class SDK_DLL Manager : private NonCopyable {
    public:
        // get version as an INT
        virtual const Version & GetVersion() = 0;

        // get global registry object
        virtual Registry & GetRegistry() = 0;

        // get language
        virtual Language & GetLang() = 0;

        // Get Id map
        virtual IdMap & GetIdMap() = 0;

        // Get UiManager
        UiManager * GetUiManager();

        // get ScriptManager
        ScriptManager * GetScriptManager();

        // get document manager
        DocManager * GetDocManager();

        // get editor manager
        EditorManager * GetEditorManager();

        // get plugin manager
        PluginManager * GetPluginManager();

        // get type manager
        TypeManager * GetTypeManager();

        // get manager instance
        static Manager * GetInstance();

        // release the manager instance ( shut down )
        static void Release();

        // private methods
        protected :

            // Release managers
            void ReleaseManagers();

            // can't create directly
            Manager() {};

            // ensure delete to fail. Use Release() instead!
            virtual ~Manager() {};
    };
}


// macro to declare a manager class in the header
#define DECLARE_MANAGER(_class) \
    protected : \
    _class () {}; \
    virtual ~_class () {}; \
    private : \
    friend class Manager; \
    static _class * GetInstance (); \
    static void Release ();

// Macro to implement Manager class logic in the source
#define IMPLEMENT_MANAGER(_baseClass, _extClass) \
    namespace { _extClass * the_instance = nullptr; } \
    _baseClass * _baseClass::GetInstance () { \
        if ( the_instance == nullptr ) the_instance = new _extClass; \
        return the_instance; \
    } \
    void _baseClass::Release () { \
        if ( the_instance == nullptr ) return; \
        delete the_instance; \
        the_instance = nullptr; \
    }
