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

#ifndef EDITOR_EVENT_H_INCLUDED
#define EDITOR_EVENT_H_INCLUDED


namespace fb
{

    /**
     * Simple class to represent CEditor events
     */
    class DLLIMPORT CEditorEvent
    {
        public :

            // Type enums
            enum Type
            {
                // whenever user inserts text
                TYPE_TEXT_INSERT,
                // when ever user deletes text
                TYPE_TEXT_DELETE,
                // undo/redo blocks. called at the end of each
                TYPE_BLOCK_MODIFY,
                // user performed uno
                TYPE_BLOCK_UNDO,
                // user performed redo
                TYPE_BLOCK_REDO,
                // Document save
                TYPE_SAVE
            };

            // Constructor
            CEditorEvent (
                CEditorEvent::Type t,
                int start,
                int end
            ) : type(t), startLine(start), endLine(end) {}

            // type of event
            CEditorEvent::Type type;
            // the start line
            int startLine;
            // the end line
            int endLine;
    };
}

#endif // EDITOR_EVENT_H_INCLUDED
