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
#include <wx/splitter.h>

namespace fbi
{

    /**
     * Region names for associated panes
     */
    enum MultiSplitRegion
    {
        MULTISPLIT_TOP_LEFT     = 1,
        MULTISPLIT_TOP_RIGHT    = 2,
        MULTISPLIT_BOTTOM_LEFT  = 4,
        MULTISPLIT_BOTTOM_Right = 8
    };


    /**
     * Action states
     */
    enum MultiSplitDrag
    {
        MULTISPLIT_DRAG_NONE,
        MULTISPLIT_DRAG_HORIZONTAL,
        MULTISPLIT_DRAG_VERTICAL,
        MULTISPLIT_DRAG_BOTH,
        MULTISPLIT_DRAG_LEFT_DOWN 
    };


    /**
     * This container is similar to wxSplitWindow except it
     * is able to handle 1 to 4 panes.
     * Each pane is associated with 4 corners. Any compination can be shown
     * and this class will gigure out the proper layout.
     */
    class SDK_DLL MultiSplitWindow : public wxWindow
    {
    public :
        
        // window pane numbers
        enum
        {
            PaneTopLeft,
            PaneTopRight,
            PaneBottomLeft,
            PaneBottomRight,
        };

        // pane layout flags
        enum
        {
            LayoutTopLeft           = 1 << 0,
            LayoutTopRight          = 1 << 1,
            LayoutBottomLeft        = 1 << 2,
            LayoutBottomRight       = 1 << 3,
            LayoutAll               = LayoutTopLeft | LayoutTopRight | LayoutBottomLeft | LayoutBottomRight,
            LayoutVerticalTop       = 1 << 5,
            LayoutVerticalBottom    = 1 << 6,
            LayoutVerticalFull      = LayoutVerticalTop | LayoutVerticalBottom,
            LayoutHorizontalLeft    = 1 << 8,
            LayoutHorizontalRight   = 1 << 9,
            LayoutHorizontalFull    = LayoutHorizontalLeft | LayoutHorizontalRight,
        };

        // hit test
        enum
        {
            DragVertical     = 1 << 0,
            DragHorizontal   = 1 << 1,
            DragBoth         = DragVertical | DragHorizontal
        };

        // Default constructor
        MultiSplitWindow()
        {
            Init();
        }

        // Normal constructor
        MultiSplitWindow(wxWindow *parent, wxWindowID id = wxID_ANY,
                         const wxPoint& pos = wxDefaultPosition,
                         const wxSize& size = wxDefaultSize,
                         long style = wxSP_3D,
                         const wxString& name = wxT("multisplitter"))
        {
            Init();
            Create(parent, id, pos, size, style, name);
        }
        
        // destructor
        virtual ~MultiSplitWindow();

        // manual construction
        bool Create(wxWindow *parent, wxWindowID id = wxID_ANY,
                    const wxPoint& pos = wxDefaultPosition,
                    const wxSize& size = wxDefaultSize,
                    long style = wxSP_3D,
                    const wxString& name = wxT("multisplitter"));
        
        // Get top left window
        wxWindow * GetTopLeftWindow () const { return m_managed[PaneTopLeft]; }

        // Get top right window
        wxWindow * GetTopRightWindow () const { return m_managed[PaneTopRight]; }

        // Get bottom left window
        wxWindow * GetBottomLeftWindow () const { return m_managed[PaneBottomLeft]; }

        // Get bottom right window
        wxWindow * GetBottomRightWindow () const { return m_managed[PaneBottomRight]; }

        // Generic get widnow by 0 based index
        // 0 - top left,    1 - top right
        // 2 - bottom left, 3 - bottom right
        wxWindow * GetWindow (int index) const
        {
            wxASSERT_MSG((index >= 0 && index <= 3),
                "Window index must be between 0 and 3");
            return m_managed[index];
        }

        // Show top-left window
        // optionally assign window pointer if it doesn't exist yet
        void ShowTopLeft (wxWindow * topLeft = NULL, bool show = true) { ShowWindow(PaneTopLeft, topLeft, show); }
        
        // Hide top-left window
        void HideTopLeft (bool hide = true) { HideWindow(PaneTopLeft, hide); }

        // Show top-right window
        // optionally assign window pointer if it doesn't exist yet
        void ShowTopRight (wxWindow * topRight = NULL, bool show = true) { ShowWindow(PaneTopRight, topRight, show); }

        // Hide top-right window
        void HideTopRight (bool hide = true) { HideWindow(PaneTopRight, hide); }

        // Show bottom-left window
        // optionally assign window pointer if it doesn't exist yet
        void ShowBottomLeft (wxWindow * bottomLeft = NULL, bool show = true) { ShowWindow(PaneBottomLeft, bottomLeft, show); }

        // Hide bottom-left window
        void HideBottomLeft (bool hide = true) { HideWindow(PaneBottomLeft, hide); }

        // Show bottom-right window
        // optionally assign window pointer if it doesn't exist yet
        void ShowBottomRight (wxWindow * bottomRight = NULL, bool show = true) { ShowWindow(PaneBottomRight, bottomRight, show); }

        // Hide bottom-right window
        void HideBottomRight (bool hide = true) { HideWindow(PaneBottomRight, hide); }

        // Show window by index
        // optionally assign window pointer if it doesn't exist yet
        void ShowWindow (int index, wxWindow * wnd = NULL, bool show = true);

        // Hide window by index
        void HideWindow (int index, bool hide = true) { ShowWindow(index, NULL, !hide); }

        // Is window visible ?
        bool IsVisible (int index )
        {
            wxASSERT(index >= 0 && index <= 3);
            return m_managed[index] != nullptr && m_managed[index]->IsShown();
        }

        // set minimum left size
        void SetMinLeftSize (int size) { m_minLeft = size; }

        // get minimum left size
        int GetMinLeftSize () const { return m_minLeft; }

        // set minimum right size
        void SetMinRightSize (int size) { m_minRight = size; }

        // get minimum right size
        int GetMinRightSize () const { return m_minRight; }

        // set minimum top size
        void SetMinTopSize (int size) { m_minTop = size; }

        // get minimum top size
        int GetMinTopSize () const { return m_minTop; }

        // set minimum bottom size
        void SetMinBottomSize (int size) { m_minBottom = size; }

        // get minimum bottom size
        int GetMinBottomSize () const { return m_minBottom; }

        // Resize sub windows
        virtual void SizeWindows();

        // Gets the border size
        int GetBorderSize() const;

        // Get the sash size
        int GetSashSize() const;

        // resize
        void OnSize(wxSizeEvent & event);

        // Handles mouse events
        void OnMouseEvent(wxMouseEvent& event);

        // Aborts dragging mode
        void OnMouseCaptureLost(wxMouseCaptureLostEvent& event);

        // get hash at position
        virtual int SashHitTest(int x, int y, int tolerance = 5);

        void SetNeedUpdating(bool needUpdating) { m_needUpdating = needUpdating; }
        bool GetNeedUpdating() const { return m_needUpdating ; }

    protected :

        // common part of all ctors
        void Init();

    private :

        // build visible array
        void SetVisiblePanes(int flags);

        // set sash cursor
        void SetDragCursor(int flags);

        // window panes that are managed
        // PaneXxx flags for positions
        wxWindow * m_managed[4];

        // visible window panes
        // PaneXxx flags for positions
        wxWindow * m_visible[4];

        // current state flags
        int m_stateFlags;

        // sash size. -1 means default size
        int m_sashSize;

        // vertical sash position
        int m_sashVertical;

        // horizontal sash position
        int m_sashHorizontal;

        // drag mode
        int m_dragMode;

        // need update?
        bool m_needUpdating;

        // mimum sizes
        int m_minLeft, m_minRight, m_minTop, m_minBottom;

        // cursor for vertical sash
        wxCursor    m_sashCursorWE;
        // cursor for horizontal sash
        wxCursor    m_sashCursorNS;
        // cursor for both axis
        wxCursor    m_sashCursorBoth;

        // declarations
        WX_DECLARE_CONTROL_CONTAINER();
        DECLARE_DYNAMIC_CLASS(MultiSplitWindow)
        DECLARE_EVENT_TABLE()
        wxDECLARE_NO_COPY_CLASS(MultiSplitWindow);
    };

} // ~namespace