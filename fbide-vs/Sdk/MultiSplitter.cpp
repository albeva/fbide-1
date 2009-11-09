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
#include <wx/renderer.h>
#include "MultiSplitter.h"

using namespace fbi;


// implementations
IMPLEMENT_DYNAMIC_CLASS(MultiSplitWindow, wxWindow)
WX_DELEGATE_TO_CONTROL_CONTAINER(MultiSplitWindow, wxWindow)


// the event table
BEGIN_EVENT_TABLE(MultiSplitWindow, wxWindow)
    WX_EVENT_TABLE_CONTROL_CONTAINER(MultiSplitWindow)
    EVT_SIZE(MultiSplitWindow::OnSize)
    EVT_MOUSE_EVENTS(MultiSplitWindow::OnMouseEvent)
    EVT_MOUSE_CAPTURE_LOST(MultiSplitWindow::OnMouseCaptureLost)
END_EVENT_TABLE()


/**
 * Destructor
 */
MultiSplitWindow::~MultiSplitWindow()
{
}


/**
 * Initalize the multisplitter settings
 */
void MultiSplitWindow::Init()
{
    WX_INIT_CONTROL_CONTAINER();

    // init m_managed array to zeroes
    memset(m_managed, 0, sizeof(m_managed));
    // init m_visible array to zeroes
    memset(m_visible, 0, sizeof(m_visible));
    // state flags. set to 0
    m_stateFlags = 0;
    // default size
    m_sashSize = -1;
    // sash positions
    m_sashVertical = m_sashHorizontal = 0;
    // cursors
    m_sashCursorWE = wxCursor(wxCURSOR_SIZEWE);
    m_sashCursorNS = wxCursor(wxCURSOR_SIZENS);
    m_sashCursorBoth = wxCursor(wxCURSOR_SIZING);
    // drag mode
    m_dragMode = 0;
    // no need to update
    m_needUpdating = false;
    // set minimum sizes
    m_minLeft = m_minRight = m_minTop = m_minBottom = 0;

    // temporary!
    m_sashVertical = 200;
    m_sashHorizontal = 100;
}


/**
 * manual construction
 */
bool MultiSplitWindow::Create(wxWindow *parent, wxWindowID id, 
             const wxPoint& pos,const wxSize& size,
             long style, const wxString& name)
{
    // allow TABbing from one window to the other
    style |= wxTAB_TRAVERSAL;

    // we draw our border ourselves to blend the sash with it
    style &= ~wxBORDER_MASK;
    style |= wxBORDER_NONE;

    if ( !wxWindow::Create(parent, id, pos, size, style, name) )
        return false;

    return true;
}


/**
 * Show the pabe by index and assign a window pointer to it
 * window must not be already assigned!
 */
void MultiSplitWindow::ShowWindow (int index, wxWindow * wnd, bool show)
{
    // ensure index is correct
    wxASSERT_MSG((index >= 0 && index <= 3), "Window index must be from 0 to 3");

    // if window pointer is passed make sure its not yet assigned!
    if (wnd != NULL)
    {
        wxASSERT((m_managed[index] == NULL), "Window already assigned to the region");
        m_managed[index] = wnd;
    }
    else
    {
        wxASSERT((m_managed[index] != NULL), "No window assigned");
        wnd = m_managed[index];
    }

    // get location flag
    int locFlag = 0;
    switch (index)
    {
        case PaneTopLeft :      locFlag = LayoutTopLeft;    break;
        case PaneTopRight :     locFlag = LayoutTopRight;   break;
        case PaneBottomLeft :   locFlag = LayoutBottomLeft; break;
        case PaneBottomRight :  locFlag = LayoutBottomRight;break;
    }

    int flags = m_stateFlags;
    if (show)
    {
        // already visible
        if (flags & locFlag) return;
        flags |= locFlag;
    }
    else
    {
        // not visible anyway
        if (!(flags & locFlag)) return;
        flags &= ~locFlag;
    }

    SetVisiblePanes(flags);
    wnd->Show(show);
    SizeWindows();
}


/**
 * Handle size events
 */
void MultiSplitWindow::OnSize(wxSizeEvent & event)
{
    // get client size
    int w, h;
    GetClientSize(&w, &h);
    SizeWindows();
}


// Handles mouse events
void MultiSplitWindow::OnMouseEvent(wxMouseEvent& event)
{
    int x = (int)event.GetX(),
        y = (int)event.GetY();
    
    if (event.LeftDown())
    {
        if (int sash = SashHitTest(x, y))
        {
            m_dragMode = sash;
            CaptureMouse();
            SetDragCursor(sash);
        }
    }
    else if (event.LeftUp() && m_dragMode != 0)
    {
        m_dragMode = 0;
        ReleaseMouse();
        SetCursor(*wxSTANDARD_CURSOR);
        SizeWindows();
    }
    else if ((event.Moving() || event.Leaving() || event.Entering()) && (m_dragMode == 0))
    {
        int sash = SashHitTest(x, y);
        if ( event.Leaving() || !sash )
        {
            SetCursor(*wxSTANDARD_CURSOR);
        }
        else
        {
            SetDragCursor(sash);
        }
    }
    else if (event.Dragging() && (m_dragMode != 0))
    {
        if (m_dragMode & DragVertical)
            m_sashVertical = x;
        if (m_dragMode & DragHorizontal)
            m_sashHorizontal = y;
        SizeWindows();
    }
    else
    {
        event.Skip();
    }
}


// Aborts dragging mode
void MultiSplitWindow::OnMouseCaptureLost(wxMouseCaptureLostEvent& event)
{
    if (!m_dragMode) return;
    m_dragMode = 0;
    SetCursor(* wxSTANDARD_CURSOR);
}


/**
 * Set drag cursor
 */
void MultiSplitWindow::SetDragCursor(int flags)
{
    switch (flags)
    {
        case DragVertical   : SetCursor(m_sashCursorWE);    break;
        case DragHorizontal : SetCursor(m_sashCursorNS);    break;
        case DragBoth       : SetCursor(m_sashCursorBoth);  break;
    }
}


/**
 * Find hash at position
 */
int MultiSplitWindow::SashHitTest(int x, int y, int tolerance)
{
    int flags = 0;
    int sash_s = GetSashSize();
    tolerance += sash_s / 2;
    
    // vertical
    if ((m_stateFlags & LayoutVerticalFull) == LayoutVerticalFull || m_visible[1])
    {
        if ( x >= m_sashVertical - tolerance && x <= m_sashVertical + tolerance )
            flags |= DragVertical;
    }
    else if (m_stateFlags & LayoutVerticalTop)
    {
        if (y <= (m_sashHorizontal+tolerance))
            if (x >= m_sashVertical - tolerance && x <= m_sashVertical + tolerance)
                flags |= DragVertical;
    }
    else if (m_stateFlags & LayoutVerticalBottom)
    {
        if (y >= (m_sashHorizontal-tolerance))
            if (x >= m_sashVertical - tolerance && x <= m_sashVertical + tolerance)
                flags |= DragVertical;
    }

    // horizontal
    if ((m_stateFlags & LayoutHorizontalFull) == LayoutHorizontalFull || m_visible[2])
    {
        if ( y >= m_sashHorizontal - tolerance && y <= m_sashHorizontal + tolerance )
            flags |= DragHorizontal;
    }
    else if (m_stateFlags & LayoutHorizontalLeft)
    {
        if (x <= (m_sashVertical+tolerance))
            if ( y >= m_sashHorizontal - tolerance && y <= m_sashHorizontal + tolerance )
                flags |= DragHorizontal;
    }
    else if (m_stateFlags & LayoutHorizontalRight)
    {
        if (x >= (m_sashVertical-tolerance))
            if ( y >= m_sashHorizontal - tolerance && y <= m_sashHorizontal + tolerance )
                flags |= DragHorizontal;
    }

    return flags;
}


/**
 * Position and size subwindows.
 * Note that the border size applies to each subwindow, not
 * including the edges next to the sash.
 */
void MultiSplitWindow::SizeWindows()
{
    // no visible windows
    if (!m_visible[0]) return;

    // get client size
    int w, h;
    GetClientSize(&w, &h);

    // vars
    int border_s = GetBorderSize();
    int sash_s = GetSashSize();
    int size_p = (sash_s/2) + (3*border_s);
    int size_m = (sash_s/2) - (2*border_s);

    // rects for every corner
    wxRect tl, tr, bl, br;
    
    // default
    tl.x = border_s, tl.y = border_s;

    // top-right
    if (m_visible[1])
    {
        tr.x = m_sashVertical + size_p;
        tr.y = border_s;
        tl.width = m_sashVertical - size_m;
        tr.width = w - tl.width;
    }
    else tl.width = w - (2*border_s);
    
    // bottom-left
    if (m_visible[2])
    {
        bl.x = border_s;
        bl.y = m_sashHorizontal + size_p;
        tl.height = m_sashHorizontal - size_m;
        bl.height = h - tl.height;
        // bottom-right
        if (m_visible[3])
        {
            br.x = m_sashVertical + size_p;
            br.y = bl.y;
            if (m_visible[1]) bl.width = tl.width, br.width = tr.width;
            else
            {
                bl.width = m_sashVertical - size_m;
                br.width = w - bl.width;
            }
            br.height = bl.height;
        }
        else bl.width = w - (2*border_s);
    }
    else tl.height = h - (2*border_s);
    tr.height = tl.height;

    m_visible[0]->SetSize(tl);
    if (m_visible[1]) m_visible[1]->SetSize(tr);
    if (m_visible[2]) m_visible[2]->SetSize(bl);
    if (m_visible[3]) m_visible[3]->SetSize(br);


    SetNeedUpdating(false);
}


/**
 * get border size
 */
int MultiSplitWindow::GetBorderSize() const
{
    return wxRendererNative::Get().GetSplitterParams(this).border;
}

/**
 * Get sash splitter size
 */
int MultiSplitWindow::GetSashSize() const
{
    return m_sashSize > -1 ? m_sashSize : wxRendererNative::Get().GetSplitterParams(this).widthSash;
}


/**
 * Build m_visible array based on visiblity flags
 */
void MultiSplitWindow::SetVisiblePanes(int flags)
{
    // cells in m_visible array
    int tl = 0, tr = 0,
        bl = 0, br = 0;
    // counters
    int hCnt = 0, vCnt = 0;

    // reset data
    memset(m_visible, 0, sizeof(m_visible));
    m_stateFlags = 0;

    // show top left ?
    if (flags & LayoutTopLeft && m_managed[PaneTopLeft] != NULL)
    {
        m_stateFlags |= LayoutTopLeft;
        m_visible[tl] = m_managed[PaneTopLeft];
        tr = 1, bl = 2, br = 2;
    }
    // show top right ?
    if (flags & LayoutTopRight && m_managed[PaneTopRight] != NULL)
    {
        m_stateFlags |= LayoutTopRight;
        m_visible[tr] = m_managed[PaneTopRight];
        bl = 2, br = 2;
    }
    // show bottom left ?
    if (flags & LayoutBottomLeft && m_managed[PaneBottomLeft] != NULL)
    {
        m_stateFlags |= LayoutBottomLeft;
        m_visible[bl] = m_managed[PaneBottomLeft];
        br = 3;
    }
    // show bottom right ?
    if (flags & LayoutBottomRight && m_managed[PaneBottomRight] != NULL)
    {
        m_stateFlags |= LayoutBottomRight;
        m_visible[br] = m_managed[PaneBottomRight];
    }

    // seperation flags
    if (m_visible[1])
    {
        m_stateFlags |= LayoutVerticalTop;
        if (m_visible[2]) m_stateFlags |= LayoutHorizontalRight;
    }
    else if (m_visible[2]) m_stateFlags |= LayoutHorizontalRight;
    if (m_visible[2])
    {
        m_stateFlags |= LayoutHorizontalLeft;
        if (m_visible[3]) m_stateFlags |= LayoutVerticalBottom;
    }
    
}
