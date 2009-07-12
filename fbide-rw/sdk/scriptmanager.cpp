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
#include "manager.h"
#include "scriptmanager.h"
#include "v8/v8.h"


using namespace fb;
using namespace v8;

/**
 * Manager class implementation
 */
struct TheScriptManager : ScriptManager
{
    // create
    TheScriptManager ()
    {}


    // destroy
    ~TheScriptManager ()
    {}


    // execute arbitraru javascript
    void Execute (const wxString & code)
    {
        // Create a stack-allocated handle scope.
        HandleScope handle_scope;

        // Create a new context.
        Persistent<Context> context = Context::New();

        // Enter the created context for compiling and
        // running the hello world script.
        Context::Scope context_scope(context);

        // Create a string containing the JavaScript source code.
        Handle<String> source = String::New(code.c_str());

        // Compile the source code.
        Handle<Script> script = Script::Compile(source);

        // Run the script to get the result.
        Handle<Value> result = script->Run();

        // Dispose the persistent context.
        context.Dispose();

        // Convert the result to an ASCII string and print it.
        String::AsciiValue ascii(result);

        wxMessageBox(*ascii);
    }

};


// Implement Manager
IMPLEMENT_MANAGER(ScriptManager, TheScriptManager)
