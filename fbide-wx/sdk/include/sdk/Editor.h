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

#ifndef EDITOR_H_INCLUDED
#define EDITOR_H_INCLUDED

#include "sdk/EventMap.h"

namespace fb
{

    /**
     * Forward reference StyleInfo
     */
    class CStyleInfo;
    class CStyleParser;
    class CEditorEvent;


    /**
     * Base class for the editor
     */
    class DLLIMPORT CEditor : public wxScintilla,
        public CEventMap<int, void (CEditor &, CEditorEvent &)>
    {
        public :

            /**
             * Default constructor
             */
            CEditor () : m_data(NULL) {}

            /**
             * construct the editor. same constructor
             * as the base class
             */
            CEditor (
                wxWindow *parent, wxWindowID id=wxID_ANY,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = 0,
                const wxString& name = _T("")
            );

            /**
             * Destructor
             */
            virtual ~CEditor();

            /**
             * Create the editor. Will call Setup()
             */
            bool Create(
                wxWindow *parent, wxWindowID id=wxID_ANY,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize, long style = 0,
                const wxString& name = _T("")
            );

            /**
             * Set style
             */
            void SetStyle (int nr, const CStyleInfo & styleInfo);

            /**
             * Save file
             */
            void SaveFile (const wxString & file);

            /**
             * Load file
             */
            void LoadFile (const wxString & file);

            /**
             * Mark editor as ready and fully loaded
             */
            void MarkReady (bool state);

            /**
             * Get is ready state?
             */
            bool GetReady ();

            /**
             * Setup the editor. Load general config
             */
            void Setup (CStyleParser * styles);

        private :
            struct CData;
            friend struct CData;
            CData * m_data;
    };

}

#endif // EDITOR_H_INCLUDED
