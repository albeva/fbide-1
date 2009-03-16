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
#include "sdk/Singleton.h"
#include "sdk/UiManager.h"
#include "sdk/UiToolDescriptor.h"
#include "sdk/ArtProvider.h"
#include "sdk/Document.h"
#include "sdk/DocManager.h"


using namespace fb;


/**
 * Ui is private class and implements the CUiManager class
 */
class Ui : public CUiManager, wxEvtHandler, public CSingleton<Ui>
{

    friend class CSingleton<Ui>;
    friend class CUiManager;


    /**
     * This small class is used to fix
     * a) wxToolBarToolBase::Toggle and wxMenuToolBase::Check are not compatable
     * b) wxToolBarToolBase::Toggle does not refresh the UI
     * In order to use delegate based fast and nice event dispatching
     * this buffer class is required to solve the problem
     *
     * doesn't seem to work on linux...
     */
    struct UiToolbarItemCheckFixer
    {
        wxAuiToolBar        * m_tbar;
        wxAuiToolBarItem    * m_tool;
        void Check (bool check)
        {
            m_tbar->ToggleTool(m_tool->GetId(), check);
            m_tbar->Realize();
        }
        UiToolbarItemCheckFixer (wxAuiToolBar * tbar, wxAuiToolBarItem * tool) : m_tbar(tbar), m_tool(tool)
        {
            Ui::GetInstance()->m_toolbarCheckFixs.push_back(this);
        }
    };


    typedef std::map<wxString, CUiToolDescriptor *> UiToolMap;
    typedef std::map<wxString, wxMenu *>            MenuMap;
    typedef std::map<wxString, wxAuiToolBar *>      ToolBarMap;
    typedef std::vector<UiToolbarItemCheckFixer *>  ToolCheckFixVector;
    typedef std::map<int, void *>                   IntVoidMap;
    typedef std::map<int, wxAuiToolBarItem*>        ToolBarItemsMap;
    typedef std::map<int, wxString>                 IntStringMap;
    typedef std::map<wxString, wxAuiNotebook*>      NotebookMap;

    IntStringMap        m_idNameMap;
    ToolBarItemsMap     m_toolBarItems;
    IntVoidMap          m_AuiPaneMenuBindMap;
    ToolCheckFixVector  m_toolbarCheckFixs;
    UiToolMap           m_uiToolDescMap;
    MenuMap             m_menuMap;
    ToolBarMap          m_toolBarMap;
    wxAuiManager        * m_aui;
    IArtProvider        * m_artProvider;
    wxMenuBar           * m_menuBar;
    wxMenu              * m_toolbarsMenu;
    wxStatusBar         * m_statusBar;
    wxAuiNotebook       * m_docArea;
    bool                m_showToolbars;
    NotebookMap         m_paneNotebooks;



    // Load the toolbars from XML
    void LoadToolbars (wxXmlNode * xml);

    // new tool to the toolbar
    void AddToolBarItem (const wxString & name, wxAuiToolBar * toolbar);

    // Load the menus from XML
    void LoadMenus (wxXmlNode * xml, wxMenu * parent);

    // Create menu entry
    void AddMenuItem (const wxString & name, wxMenu * parent);

    // Menu Fullscreen selected
    void OnFullScreen (wxCommandEvent & event);

    // When document tab becomes active
    void OnDocumentChanged (wxAuiNotebookEvent & event);

    // When document is closed
    void OnDocumentClosed (wxAuiNotebookEvent & event);

    // About to close document
    void OnDocumentClose (wxAuiNotebookEvent & event);

    // Aui pane close (toolbar or some panel)
    void OnAuiPaneClose (wxAuiManagerEvent & event);

    // Toolbar show/hide menu selected
    void OnAuiPaneShowHide (wxCommandEvent & event);

    // Toggle toolbars on/off
    void OnShowToolbars (wxCommandEvent & event);

    // Set Application frame title
    void SetFrameTitle ();

    // Document events
    void OnDocumentEvent (CDocumentBase * doc, int evt);

    // Are toolbars visible?
    bool IsToolbarVisible () { return m_showToolbars; }

    // Get number of visible toolbars
    int GetVisibleToolbarCount ();

    // Find toolbar name
    wxString GetToolBarName (wxAuiToolBar * tbar);

    // Create UI
    Ui ();

    // Final clean up
    ~Ui ();

    public :

        // Register new tool descriptor
        void AddToolDescriptor (const wxString & name, CUiToolDescriptor * toolDesc);

        // get doucment "parent" window
        wxWindow * GetDocWindow ();

        // load layout specified by xml
        void LoadLayout (const wxString & file);

        // Clean up ui before application close
        void Unload ();

        // Get menu with given name
        wxMenu * GetMenu (const wxString & name);

        // Add new menu into system
        void AddMenu (const wxString & name, wxMenu * menu, bool show = false);

        // Get toolbar by name
        wxAuiToolBar * GetToolBar (const wxString & name);

        // add new toolbar
        void AddToolBar (const wxString & name, wxAuiToolBar * toolbar, bool show = false);

        // Add document into Document view
        void AddDocument (CDocumentBase * doc, bool show);

        // Get Active document
        CDocumentBase * GetActiveDocument ();

        // Remove document
        void RemoveDocument (CDocumentBase * doc);

        // Set art provider
        void SetArtProvider (IArtProvider * provider);

        // Get art provider
        IArtProvider * GetArtProvider ();

        // Add pane
        bool AddPane (const wxString & id, wxWindow * wnd, const wxString & group);

        // Get parent for the pane area
        wxWindow * GetPaneWindow (const wxString & group);

        // Get pane by id
        wxWindow * GetPane (const wxString & id);

        // remove pane
        void RemovePane (const wxString & id);

        // Load the UI (finalize loading process)
        void Load ();


    DECLARE_EVENT_TABLE()
};



// SOME IDs
const int ID_FULLSCREEN     = ::wxNewId ();
const int ID_DOC_AREA       = ::wxNewId ();
const int ID_SHOW_TOOLBARS  = ::wxNewId ();



// Event table for Ui
BEGIN_EVENT_TABLE(Ui, wxEvtHandler)
    EVT_AUI_PANE_CLOSE          (                   Ui::OnAuiPaneClose)
    EVT_AUINOTEBOOK_PAGE_CHANGED(ID_DOC_AREA,       Ui::OnDocumentChanged)
    EVT_AUINOTEBOOK_PAGE_CLOSE  (ID_DOC_AREA,       Ui::OnDocumentClose)
    EVT_AUINOTEBOOK_PAGE_CLOSED (ID_DOC_AREA,       Ui::OnDocumentClosed)
    EVT_MENU                    (ID_FULLSCREEN,     Ui::OnFullScreen)
    EVT_MENU                    (ID_SHOW_TOOLBARS,  Ui::OnShowToolbars)
END_EVENT_TABLE()



// Constructor
Ui::Ui ()
{
    // vars
    CManager * mgr = GET_MGR();
    wxFrame * frame = mgr->GetFrame();

    // Register UI specific IDs
    mgr->RegisterId(_T("fullscreen"), ID_FULLSCREEN);
    AddToolDescriptor (
        _T("fullscreen"),
        CUiToolDescriptor::CreateToggleTool (MakeDelegate(frame, &wxFrame::IsFullScreen))
    );

    m_showToolbars = true;
    mgr->RegisterId(_T("toolbars"), ID_SHOW_TOOLBARS);
    AddToolDescriptor (
        _T("toolbars"),
        CUiToolDescriptor::CreateToggleTool (MakeDelegate(this, &Ui::IsToolbarVisible))
    );


    // set event handler
    frame->PushEventHandler (this);

    // Create Ui stuff
    m_aui       = new wxAuiManager (frame);
    m_menuBar   = new wxMenuBar ();
    m_statusBar = new wxStatusBar (frame);

    // Assign status and menu bar to the frame
    frame->SetStatusBar(m_statusBar);
    frame->SetMenuBar(m_menuBar);

    // Toolbars menu
    m_toolbarsMenu = new wxMenu ();
    AddMenu(_T("group.toolbars"), m_toolbarsMenu);

    int tabStyles = wxAUI_NB_TAB_MOVE | wxAUI_NB_SCROLL_BUTTONS |
        wxAUI_NB_WINDOWLIST_BUTTON | wxAUI_NB_TOP | wxAUI_NB_TAB_SPLIT;

    // Create document area
    m_docArea = new wxAuiNotebook (
        frame,
        ID_DOC_AREA,
        wxDefaultPosition, wxDefaultSize,
        tabStyles | wxAUI_NB_CLOSE_BUTTON | wxAUI_NB_CLOSE_ON_ACTIVE_TAB
    );
    m_aui->AddPane (
        m_docArea,
        wxAuiPaneInfo()
        .Name(_T("fbUiMgrDocArea"))
        .CenterPane()
        .PaneBorder(false)
    );


    // management panes
    wxAuiNotebook * nb;
    nb = new wxAuiNotebook (
        frame,
        -1,
        wxDefaultPosition, wxSize(150, -1),
        tabStyles
    );
    m_aui->AddPane (
        nb,
        wxAuiPaneInfo()
        .Name(_T("fbUiToolsArea"))
        .Left()
        .PaneBorder(false)
        .Hide()
    );
    m_paneNotebooks[_T("tools")] = nb;


    // log panes
    nb = new wxAuiNotebook (
        frame,
        -1,
        wxDefaultPosition, wxDefaultSize,
        tabStyles
    );
    m_aui->AddPane (
        nb,
        wxAuiPaneInfo()
        .Name(_T("fbUiLogsArea"))
        .Bottom()
        .PaneBorder(false)
        .Hide()
    );
    m_paneNotebooks[_T("logs")] = nb;



    // Refresh title
    SetFrameTitle ();
}



// Final clean up
Ui::~Ui ()
{
    for (UiToolMap::iterator iter = m_uiToolDescMap.begin(); iter != m_uiToolDescMap.end(); iter++)
    {
        delete iter->second;
    }
    for ( ToolCheckFixVector::iterator iter = m_toolbarCheckFixs.begin();
          iter != m_toolbarCheckFixs.end();)
    {
        delete *iter;
        iter = m_toolbarCheckFixs.erase(iter);
    }
}



/**
 * Toggle fullscreen
 */
void Ui::OnFullScreen (wxCommandEvent & event)
{
    int style = wxFULLSCREEN_NOBORDER | wxFULLSCREEN_NOCAPTION;
    GET_MGR()->GetFrame()->ShowFullScreen(event.IsChecked(), style);

    wxMessageBox(event.IsChecked() ? _T("checked") : _T("not"));

    // Notify connected items
    CUiToolDescriptor * toolDesc = m_uiToolDescMap[_T("fullscreen")];
    if (toolDesc) toolDesc->Notify(event.IsChecked());
}


/**
 * Show hide all toolbars
 */
void Ui::OnShowToolbars (wxCommandEvent & event)
{
    m_showToolbars = event.IsChecked();
    for (ToolBarMap::iterator toolbars = m_toolBarMap.begin();
         toolbars != m_toolBarMap.end();
         toolbars++)
    {
        wxAuiToolBar * tbar = toolbars->second;
        wxAuiPaneInfo & pane = m_aui->GetPane(tbar);
        if (m_showToolbars) pane.Show();
        else pane.Hide();

        wxMenuItem * item = (wxMenuItem *)m_AuiPaneMenuBindMap[tbar->GetId()];
        if (item) item->Check(m_showToolbars);

        wxString name = _T("toolbar.") + GetToolBarName(tbar);
        CUiToolDescriptor * tool = m_uiToolDescMap[name];
        if (tool)
        {
            tool->Notify(m_showToolbars);
        }
    }
    CUiToolDescriptor * tool = m_uiToolDescMap[_T("toolbars")];
    if (tool)
    {
        tool->Notify(m_showToolbars);
    }

    m_aui->Update();
}


/**
 * Get number of toolbars visible
 */
int Ui::GetVisibleToolbarCount ()
{
    //IsShown
    int count = 0;
    ToolBarMap::iterator iter = m_toolBarMap.begin();
    for ( ; iter != m_toolBarMap.end(); iter++ )
    {
        wxAuiToolBar * tbar = iter->second;
        if (m_aui->GetPane(tbar).IsShown()) count++;
    }
    return count;
}


/**
 * New tab is active
 */
void Ui::OnDocumentChanged (wxAuiNotebookEvent & event)
{
    wxWindow * wnd = m_docArea->GetPage(event.GetSelection());
    CDocumentBase * doc = GET_DOCMGR()->FindDocument(wnd);
    doc->SendDocEvent(DocEvent::FOCUS);
    SetFrameTitle();
}



/**
 * Document event
 */
void Ui::OnDocumentEvent (CDocumentBase * doc, int evt)
{
    if (evt == DocEvent::RENAME)
    {
        wxWindow * win = doc->GetDocWindow();
        int index = m_docArea->GetPageIndex(win);
        if (index == wxNOT_FOUND) return;

        m_docArea->SetPageText(index, doc->GetDocTitle());
        if (index == m_docArea->GetSelection()) SetFrameTitle();
    }
}



/**
 * Document closed
 */
void Ui::OnDocumentClosed (wxAuiNotebookEvent & event)
{
    if (m_docArea->GetPageCount() == 0) SetFrameTitle();
}


/**
 * Close document ?
 */
void Ui::OnDocumentClose (wxAuiNotebookEvent & event)
{
    // wxMessageBox(_T("About to close..."));
    // event.Skip(true);
}



/**
 * On aui pane close (toolbar or some view)
 */
void Ui::OnAuiPaneClose (wxAuiManagerEvent & event)
{
    wxAuiPaneInfo * pane = event.GetPane();
    if (pane->IsToolbar())
    {
        int id = pane->window->GetId();
        wxMenuItem * item = reinterpret_cast<wxMenuItem *>(m_AuiPaneMenuBindMap[id]);
        if (!item) return;

        item->Check(false);

        if (!(GetVisibleToolbarCount()-1))
        {
            CUiToolDescriptor * tool = m_uiToolDescMap[_T("toolbars")];
            if (tool) tool->Notify(false);
        }

        wxString name = _T("toolbar.") + GetToolBarName(dynamic_cast<wxAuiToolBar *>(pane->window));
        CUiToolDescriptor * tool = m_uiToolDescMap[name];
        if (tool)
        {
            tool->Notify(false);
        }
    }
}



/**
 * Show or Hide the toolbar
 */
void Ui::OnAuiPaneShowHide (wxCommandEvent & event)
{
    wxAuiToolBar * toolbar = reinterpret_cast<wxAuiToolBar *>(m_AuiPaneMenuBindMap[event.GetId()]);
    if (toolbar)
    {
        bool show = event.IsChecked();
        m_aui->GetPane(toolbar).Show(show);
        m_aui->Update();
        if (show)
        {
            if (!m_showToolbars)
            {
                CUiToolDescriptor * tool = m_uiToolDescMap[_T("toolbars")];
                if (tool) tool->Notify(true);
            }
        }
        else if (!GetVisibleToolbarCount())
        {
            CUiToolDescriptor * tool = m_uiToolDescMap[_T("toolbars")];
            if (tool) tool->Notify(false);
        }

        wxString name = _T("toolbar.") + GetToolBarName(toolbar);
        CUiToolDescriptor * tool = m_uiToolDescMap[name];
        if (tool)
        {
            tool->Notify(show);
        }
    }
}


/**
 * Register new Ui tool descriptor
 */
void Ui::AddToolDescriptor (const wxString & name, CUiToolDescriptor * toolDesc)
{
    if (m_uiToolDescMap[name] != NULL)
    {
        wxLogError(_T("Tool descriptor for id '%s' already exists"), name.c_str());
        return;
    }
    m_uiToolDescMap[name] = toolDesc;
}



void Ui::SetFrameTitle ()
{
    wxString title(_T("FBIde"));
    if (m_docArea->GetPageCount())
    {
        title << _T(" - ") << m_docArea->GetPageText(m_docArea->GetSelection());
    }
    GET_MGR()->GetFrame()->SetTitle(title);
}



// Get document parent window (tab area)
wxWindow * Ui::GetDocWindow ()
{
    return m_docArea;
}



// Add document into Document view
void Ui::AddDocument (CDocumentBase * doc, bool show)
{
    if (doc->GetDocType() != DOCUMENT_MANAGED)
        throw EXCEPTION(_T("Invalid document type"));

    wxWindow * wnd = dynamic_cast<wxWindow *>(doc);
    wxASSERT(wnd != NULL);

    m_docArea->AddPage (wnd, doc->GetDocTitle(), show);
    doc->evt[DocEvent::RENAME] += MakeDelegate(this, &Ui::OnDocumentEvent);
}


// get currently active document
CDocumentBase * Ui::GetActiveDocument ()
{
    if (!m_docArea->GetPageCount()) return NULL;
    wxWindow * wnd = m_docArea->GetPage(m_docArea->GetSelection());
    return dynamic_cast<CDocumentBase*>(wnd);
}



// Remove document
void Ui::RemoveDocument (CDocumentBase * doc)
{
    if (doc->GetDocType() != DOCUMENT_MANAGED) return;

    wxWindow * wnd = dynamic_cast<wxWindow *>(doc);
    wxASSERT(wnd != NULL);

    int page = m_docArea->GetPageIndex(wnd);
    if (page == wxNOT_FOUND) return;

    // unregister event handler
    doc->evt[DocEvent::RENAME] -= MakeDelegate(this, &Ui::OnDocumentEvent);
    m_docArea->RemovePage (page);
}



// load layout specified by xml
void Ui::LoadLayout (const wxString & file)
{
    // the xml object
    wxXmlDocument xml;

    // Normalize the file path
    wxFileName f(file);
    f.Normalize();
    wxString layoutFile = f.GetFullPath();

    // Check for errors ...
    if (!f.FileExists() || !xml.Load(layoutFile))
    {
        wxLogError(_T("Could not load layout file \"%s\""), layoutFile.c_str());
        return;
    }
    if (!xml.IsOk() || xml.GetRoot()->GetName() != _T("fbide"))
    {
        wxLogError(_T("Malformed layout file \"%s\""), layoutFile.c_str());
        return;
    }

    // load layout
    wxXmlNode * child = xml.GetRoot()->GetChildren();
    for ( ; child; child = child->GetNext())
    {
        if (child->GetName() == _T("menus"))
        {
            LoadMenus(child->GetChildren(), 0L);
        }
        else if (child->GetName() == _T("toolbars"))
        {
            LoadToolbars(child->GetChildren());
        }
    }
}



// Load the toolbars from XML
void Ui::LoadToolbars (wxXmlNode * xml)
{
    // vars
    CManager * mgr = GET_MGR();
    wxString toolbarId;
    wxString itemId;
    int style = m_artProvider == NULL
              ? wxAUI_TB_TEXT | wxAUI_TB_GRIPPER | wxAUI_TB_HORZ_LAYOUT // wxTB_FLAT | wxTB_NODIVIDER | wxTB_NOICONS | wxTB_TEXT
              : wxAUI_TB_GRIPPER | wxAUI_TB_HORZ_LAYOUT; // wxTB_FLAT | wxTB_NODIVIDER;
    wxXmlNode * child;


    // iterate through xml and add entries
    for ( ; xml; xml = xml->GetNext())
    {
        if (xml->GetName() != _T("toolbar")) continue;
        if (!xml->GetPropVal(_T("id"), &toolbarId)) continue;
        bool addTb = false;
        bool show = xml->GetPropVal(_T("show"), _T("yes")) == _T("yes");

        wxAuiToolBar * tbar = GetToolBar(toolbarId);
        if (tbar == NULL)
        {
            tbar = new wxAuiToolBar (
                mgr->GetFrame(), wxID_ANY,
                wxDefaultPosition, wxDefaultSize,
                style
            );
            if (m_artProvider)
            {
                tbar->SetToolBitmapSize(m_artProvider->GetIconSize());
            }
            addTb = true;
        }

        // iterate through children (if any) and add them to the toolbar
        for (child = xml->GetChildren(); child; child = child->GetNext())
        {
            wxString tag = child->GetName();

            // seperator
            if (tag == _T("separator"))
            {
                tbar->AddSeparator ();
            }
            else if (tag == _T("item"))
            {
                if(!child->GetPropVal(_T("id"), &itemId)) continue;
                AddToolBarItem (itemId, tbar);
            }
        }

        // refresh the toolbar
        tbar->Realize ();
        if (addTb) AddToolBar (toolbarId, tbar, show);
    }
}




// Get toolbar by name
wxAuiToolBar * Ui::GetToolBar (const wxString & name)
{
    return m_toolBarMap[name];
}


// Get toolbar by name
wxString Ui::GetToolBarName (wxAuiToolBar * tbar)
{
    ToolBarMap::iterator iter = m_toolBarMap.begin();
    for ( ; iter != m_toolBarMap.end(); iter++)
    {
        if (iter->second == tbar) return iter->first;
    }
    return _T("");
}



// add new toolbar
void Ui::AddToolBar (const wxString & name, wxAuiToolBar * toolbar, bool show)
{
    if (GetToolBar(name))
    {
        wxLogWarning(_T("Toolbar with id '%s' already exists"), name.c_str());
        return;
    }

    m_toolBarMap[name] = toolbar;

    // vars
    CManager * mgr = GET_MGR();
    wxString label = mgr->GetLang(_T("toolbar.") + name);

    // Add menu entry for toolbar
    int menuId = ::wxNewId();
    wxMenuItem * item = m_toolbarsMenu->AppendCheckItem(menuId, label);
    item->Check(show);
    Connect(menuId, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(Ui::OnAuiPaneShowHide));
    m_AuiPaneMenuBindMap[menuId] = toolbar;
    m_AuiPaneMenuBindMap[toolbar->GetId()] = item;

    wxString toolBarEntry = _T("toolbar.");
    toolBarEntry += name;
    AddToolDescriptor (
        toolBarEntry,
        CUiToolDescriptor::CreateToggleTool (MakeDelegate(item, &wxMenuItemBase::IsChecked))
    );
    mgr->RegisterId(toolBarEntry, menuId);
    m_uiToolDescMap[toolBarEntry]->Connect(
        CUiToolDescriptor::VoidBoolDelegate(item, &wxMenuItemBase::Check)
    );


    // Add to aui
    m_aui->AddPane(
        toolbar,
        wxAuiPaneInfo().Name(name).Caption(label)
        .ToolbarPane().Top().Dockable(true).Show(show)
    );
}



/**
 * Add an item to the toolbar
 */
void Ui::AddToolBarItem (const wxString & name, wxAuiToolBar * toolbar)
{
    // vars
    CManager * mgr  = GET_MGR();
    wxItemKind style= wxITEM_NORMAL;
    bool checked    = false;
    int id          = mgr->GetId(name);
    wxString label  = mgr->GetLang(name);
    wxString help   = mgr->GetLang(name + _T(".help"));
    wxAuiToolBarItem * tool = NULL;
    CUiToolDescriptor * toolDesc;

    // check if is a special tool
    toolDesc = m_uiToolDescMap[name];
    if (toolDesc != NULL)
    {
        if (toolDesc->GetType() == CUiToolDescriptor::TOOL_TOGGLE)
        {
            checked = toolDesc->IsChecked();
            style = wxITEM_CHECK;
        }
    }

    wxBitmap bmp = m_artProvider ? m_artProvider->GetIcon(name) : wxNullBitmap;
    toolbar->AddTool(id, label, bmp, help, style);
    tool = toolbar->FindTool(id);

    m_toolBarItems[id] = tool;
    m_idNameMap[id] = name;

    if (style == wxITEM_CHECK)
    {
        tool->SetKind(wxITEM_CHECK);
        UiToolbarItemCheckFixer * t = new UiToolbarItemCheckFixer (toolbar, tool);
        toolDesc->Connect (
            CUiToolDescriptor::VoidBoolDelegate(t, &UiToolbarItemCheckFixer::Check)
        );
        toolbar->ToggleTool(tool->GetId(), checked);
    }

    tool->SetLongHelp(help);
    toolbar->Realize();
}



/**
 * Load the menus from XML
 */
void Ui::LoadMenus (wxXmlNode * xml, wxMenu * parent)
{
    wxString idName;
    CManager & mgr = *GET_MGR();

    for ( ; xml; xml = xml->GetNext())
    {
        if (xml->GetName() == _T("menu"))
        {
            if(!xml->GetPropVal(_T("id"), &idName)) continue;

            idName = _T("group.") + idName;
            wxMenu * menu = GetMenu(idName);
            if (menu == NULL)
            {
                menu = new wxMenu();
                AddMenu(idName, menu, parent == 0L);
            }
            if (parent != 0L)
            {
                parent->AppendSubMenu(menu, mgr.GetLang(idName));
            }
            else if(m_menuBar->FindMenu(mgr.GetLang(idName)) == wxNOT_FOUND)
            {
                m_menuBar->Append(menu, mgr.GetLang(idName));
            }

            if (wxXmlNode * children = xml->GetChildren())
            {
                LoadMenus(children, menu);
            }
        }
        else if (xml->GetName() == _T("item"))
        {
            if(parent && xml->GetPropVal(_T("id"), &idName)) AddMenuItem (idName, parent);
        }
        else if (xml->GetName() == _T("separator"))
        {
            if (parent != 0L) parent->AppendSeparator();
        }
    }
}



// Get menu with given name
wxMenu * Ui::GetMenu (const wxString & name)
{
    return m_menuMap[name];
}



// Add new menu into system
void Ui::AddMenu (const wxString & name, wxMenu * menu, bool show)
{
    if (GetMenu(name) != NULL)
    {
        wxLogError (_T("Menu with id '%s' already registered"), name.c_str());
        return;
    }
    m_menuMap[name] = menu;
    if (show) m_menuBar->Append(menu, GET_MGR()->GetLang(name));
}



// Add menu item
void Ui::AddMenuItem (const wxString & name, wxMenu * parent)
{
    // vars
    CManager * mgr  = GET_MGR();
    wxItemKind style= wxITEM_NORMAL;
    bool checked    = false;
    int id          = mgr->GetId(name);
    wxString label  = mgr->GetLang(name);
    wxString help   = mgr->GetLang(name + _T(".help"));
    wxMenuItem * item = NULL;
    CUiToolDescriptor * toolDesc;

    // check if is a special tool
    toolDesc = m_uiToolDescMap[name];
    if (toolDesc != NULL)
    {
        if (toolDesc->GetType() == CUiToolDescriptor::TOOL_TOGGLE)
        {
            checked = toolDesc->IsChecked();
            style = wxITEM_CHECK;
        }
    }

    // Create menu item
    item = new wxMenuItem (parent, id, label, help, style);
    if (style == wxITEM_NORMAL)
    {
        if (m_artProvider) item->SetBitmap(m_artProvider->GetIcon(name));
        parent->Append(item);
    }
    else if (style == wxITEM_CHECK)
    {
        parent->Append(item);
        item->Check(checked);
        toolDesc->Connect(
            CUiToolDescriptor::VoidBoolDelegate(item, &wxMenuItemBase::Check)
        );
    }
}



// Load the UI (finalize loading process)
void Ui::Load ()
{
    m_aui->Update();
    GET_MGR()->GetFrame()->Show();
}



// to be removed
void Ui::Unload ()
{
    CManager::Get()->GetDocManager()->CloseAll();
    #ifdef AUI_USE_PERSPECTIVES
        if (m_aui != 0L) GET_REG()[_T("ui.perspective")] = m_aui->SavePerspective();
    #endif
}




/**
 * Set art provider. Takes ownership?? maybe no... (might be a plugin class
 * that directly implements the interface. Should send an event...)
 *
 * @todo if provider is changed then should refresh the UI
 */
void Ui::SetArtProvider (IArtProvider * provider)
{
    // if setting self no reason to linger here...
    if (provider == m_artProvider) return;

    // vars
    CManager * mgr = GET_MGR();

    // check if we are killing ourselves
    if (mgr->IsShuttingDown()) return;

    m_artProvider = provider;
    onArtProvderChange (provider);

    int style = m_artProvider == NULL
              ? wxTB_FLAT | wxTB_NODIVIDER | wxTB_NOICONS | wxTB_TEXT
              : wxTB_FLAT | wxTB_NODIVIDER;

    // Update toolbars
    for (ToolBarMap::iterator toolbars = m_toolBarMap.begin();
         toolbars != m_toolBarMap.end();
         toolbars++)
    {
        // get the toolbar and name
        wxAuiToolBar * toolbar = toolbars->second;
        wxString name = toolbars->first;

        m_aui->DetachPane(toolbar);

        // if we have art provider change bitmaps
        if (m_artProvider)
        {
            toolbar->SetToolBitmapSize(m_artProvider->GetIconSize());
            // iterate through items
            for (ToolBarItemsMap::iterator items = m_toolBarItems.begin();
                 items != m_toolBarItems.end(); items++)
            {
                wxAuiToolBarItem * tool = items->second;
                tool->SetBitmap(m_artProvider->GetIcon(m_idNameMap[items->first]));
                //toolbar->SetToolNormalBitmap (items->first, m_artProvider->GetIcon(m_idNameMap[items->first]));
            }
        }
        toolbar->SetWindowStyle(style);
        toolbar->Realize();

        m_aui->AddPane(
            toolbar,
            wxAuiPaneInfo().Name(name).Caption(mgr->GetLang(_T("toolbar.") + name))
            .ToolbarPane().Top().LeftDockable(false).RightDockable(false)
            .Show(true)
        );
    }
    m_aui->Update();
}



// Get art provider
IArtProvider * Ui::GetArtProvider ()
{
    return m_artProvider;
}



// Add pane
bool Ui::AddPane (const wxString & id, wxWindow * wnd, const wxString & group)
{
    wxAuiNotebook * nb = m_paneNotebooks[group];
    if (NULL == nb)
    {
        wxLogMessage(_T("Pane group '%s' does not exist!"), group.c_str());
        return false;
    }
    nb->AddPage(wnd, id, true);
    m_aui->GetPane(nb).Show();

    return true;
}

// Get parent for the pane area
wxWindow * Ui::GetPaneWindow (const wxString & group)
{
    return m_paneNotebooks[group];
}


// Get pane by id
wxWindow * Ui::GetPane (const wxString & id)
{
    return 0;
}

// remove pane
void Ui::RemovePane (const wxString & id)
{

}



// Get and Free
CUiManager * CUiManager::Get () { return Ui::GetInstance(); }
void CUiManager::Free () { Ui::Release (); }
