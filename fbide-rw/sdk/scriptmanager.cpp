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
#include <wx/textfile.h>
#include "manager.h"
#include "scriptmanager.h"
#include "v8/v8.h"

using namespace fb;
using namespace v8;


// The callback that is invoked by v8 whenever the JavaScript 'print'
// function is called. Prints its arguments on stdout separated by
// spaces and ending with a newline.
Handle<Value> JsPrint(const Arguments& args)
{
    wxString msg = "";
    bool first = true;
    for (int i = 0; i < args.Length(); i++)
    {
        HandleScope handle_scope;
        if (first)
        {
            first = false;
        }
        else
        {
            msg += "\n";
        }
        //convert the args[i] type to normal char* string
        String::AsciiValue str(args[i]);
        msg += *str;
    }

    wxMessageBox(msg);

    //returning Undefined is the same as returning void...
    return v8::Undefined();
}


/**
 * Show JavaScript errors
 */
void jsMessages (Handle<Message> message, Handle<Value> data)
{
    String::AsciiValue str(message->Get());
    wxString msg;
    msg << *str << " on line " << message->GetLineNumber() << "\n"
        << *String::AsciiValue(message->GetSourceLine())
        ;
    wxMessageBox(msg, "JavaScript error");
}



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
    void Execute (const wxString & fileName)
    {
        // check if file exists
        if (!::wxFileExists(fileName))
        {
            wxLogMessage("file '" + fileName + "' is not found");
            return;
        }

        // read file in
        wxTextFile file(fileName);
        if (!file.Open())
        {
            wxLogMessage("could not open '" + fileName + "'");
            return;
        }

        // get the content
        wxString code = file.GetFirstLine();
        for ( ; !file.Eof(); code += "\n" + file.GetNextLine() );

        // Register the error callback
        V8::AddMessageListener(jsMessages);

        // Create a stack-allocated handle scope.
        HandleScope handle_scope;

        // Create a template for the global object and set the
        // built-in global functions.
        Handle<ObjectTemplate> global = ObjectTemplate::New();
        global->Set(String::New("print"), FunctionTemplate::New(JsPrint));

        // Each processor gets its own context so different processors
        // do not affect each other.
        Persistent<Context> context = Context::New(NULL, global);

        // Enter the created context for compiling and
        // running the hello world script.
        Context::Scope context_scope(context);

        // Create a string containing the JavaScript source code.
        Handle<String> source = String::New(code.c_str());

        // Compile the source code.
        Handle<Script> script = Script::Compile(source);
        if (script.IsEmpty()) return;

        // javascript exceptions
        TryCatch trycatch;
        Handle<Value> result = script->Run();
        if (result.IsEmpty())
        {
            Handle<Value> exception = trycatch.Exception();
            String::AsciiValue exception_str(exception);
            wxMessageBox(*exception_str, "Exception");
        }

        // Dispose the persistent context.
        context.Dispose();

    }

};


// Implement Manager
IMPLEMENT_MANAGER(ScriptManager, TheScriptManager)
