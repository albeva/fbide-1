/**
 * FBIde main application
 */
#include "app_pch.h"
#include "Sdk/Manager.h"
#include "Sdk/UiManager.h"
#include "Sdk/ScriptManager.h"
#include "Sdk/CmdManager.h"

using namespace fbi;

/**
 * Main application class
 */
class FBIdeApp : public wxApp
{
    
    // Entry point
    virtual bool OnInit () {
        // base init
        if (!wxApp::OnInit()) return false;

        // app base
        wxStandardPathsBase & sp = this->GetTraits()->GetStandardPaths();
        auto path = ::wxPathOnly(sp.GetExecutablePath());

        // Load Language
        Language & lang = GET_LANG();
        lang.Load(path + "/ide/en.ini");

        auto ui = GET_UIMGR();
        if (!ui->Load()) return false;

        // GET_SCRIPTMGR()->Execute(path + "/test.js");
        ui->LoadLayout(path + "/ide/layout.xml");
        ui->GetFrame()->Show();

        // success
        return true;
    }

};
IMPLEMENT_APP(FBIdeApp);
