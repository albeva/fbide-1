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

#ifndef EDITOR_MANAGER_H_INCLUDED
#define EDITOR_MANAGER_H_INCLUDED

#include "EventMap.h"

namespace fb
{
    // forward reference ...
    class CEditor;
    class CManager;

    /**
     * Manage editors
     */
    class DLLIMPORT CEditorManager
        : public CEventMap<int, void(int, CEditor &) >
    {
        public :

            // declare events
            enum Event
            {
                // open new document
                EVT_OPEN = 1,
                // close document
                EVT_CLOSE
            };

            // Register new editor with the manager
            virtual void RegisterEditor ( CEditor * editor ) = 0;

        private :
            // allow CManager to access ...
            friend class CManager;
            // get singleton instance
            static CEditorManager * Get ();
            // free singleton instance
            static void Free ();

        protected :
            virtual ~CEditorManager() {}

    };

}

#endif // EDITOR_MANAGER_H_INCLUDED
