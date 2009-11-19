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

        // set registry
        auto & reg = GET_REG();
        reg["path.ide.data"] = path + "/ide/";

        auto ui = GET_UIMGR();
        if (!ui->Load()) return false;

        // ensure all managers are loaded
        GET_CMDMGR();
        GET_DOCMGR();
        GET_TYPEMGR();
        GET_EDITORMGR();
        GET_PLUGINMGR();
        GET_SCRIPTMGR();//->Execute(path + "/test.js");      

        // GET_SCRIPTMGR()->Execute(path + "/test.js");
        ui->LoadLayout(path + "/ide/layout.xml");
        auto frame = ui->GetFrame();
        frame->Show();

        // set logger
        auto * logger = new wxLogWindow(frame, "logs");
        //logger->SetLogLevel(wxLOG_Info);
        logger->Resume();
        wxLog::SetActiveTarget ( logger );
        
        // success
        return true;
    }

};
IMPLEMENT_APP(FBIdeApp);
