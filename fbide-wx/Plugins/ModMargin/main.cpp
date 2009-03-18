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
#include "sdk/PluginManager.h"
#include "sdk/PluginArtProvider.h"
#include "sdk/EditorManager.h"
#include "sdk/Editor.h"
#include "sdk/EditorEvent.h"
#include "sdk/StyleInfo.h"


using namespace fb;



class ModMarginPlugin : public CPluginBase
{
    public :

        enum {
            MARGIN_NUMBER   = 1,
            MARKER_SAVED    = 0,
            MARKER_EDITED   = 1,
            MARKER_FLAGS    = (1 << MARKER_SAVED) | (1 << MARKER_EDITED),
        };

        // Attach plugin
        bool Attach ()
        {
            // register handler
            GET_EDITORMGR()->Bind(
                CEditorManager::EVT_OPEN,
                MakeDelegate(this, &ModMarginPlugin::NewDocument)
            );

            return true;
        }


        /**
         * Open new editor
         */
        void NewDocument (int evt, CEditor & editor)
        {

            const CStyleInfo & info = GET_MGR()->GetStyleParser(_T("default"))
                              ->GetStyle(_T(".edit-margin"));


            editor.SetMarginWidth(MARGIN_NUMBER, 4);
            editor.SetMarginType(MARGIN_NUMBER,  wxSCI_MARGIN_SYMBOL);
            editor.SetMarginWidth(MARGIN_NUMBER, 4);
            editor.SetMarginMask(MARGIN_NUMBER, (1 << MARKER_SAVED) | (1 << MARKER_EDITED) );

            // edited marker
            editor.MarkerDefine(MARKER_EDITED, wxSCI_MARK_FULLRECT);
            editor.MarkerSetBackground(MARKER_EDITED, info.bg);

            // saved marker
            editor.MarkerDefine(MARKER_SAVED, wxSCI_MARK_FULLRECT);
            editor.MarkerSetBackground(MARKER_SAVED, info.fg);


            // insert text event
            editor.Bind (
                CEditorEvent::TYPE_TEXT_INSERT,
                MakeDelegate(this, &ModMarginPlugin::ModifyText)
            );

            // delete text event
            editor.Bind (
                CEditorEvent::TYPE_TEXT_DELETE,
                MakeDelegate(this, &ModMarginPlugin::ModifyText)
            );
        }


        /**
         * Text modified by a user
         */
        void ModifyText (CEditor & editor, CEditorEvent & event)
        {
            // update margin
            editor.MarkerAdd(event.startLine, MARKER_EDITED);
        }


        // Detach plugin
        bool Detach (bool force)
        {
            // unregister handler
            GET_EDITORMGR()->UnBind(
                CEditorManager::EVT_OPEN,
                MakeDelegate(this, &ModMarginPlugin::NewDocument)
            );

            return true;
        }
};

namespace
{
    CPluginProvider<ModMarginPlugin, PLUGINTYPE_TOOL> plugin (_T("ModMargin"));
}


#if 0
    /**
     * Track changes per line. Needed for
     * modification margin
     */
    struct CChangeHistory
    {

        // Number of elements allocaed per block.
        enum {
            STATES_IN_CELL  = 16,
            BITS_PER_STATE  = 2,
            STATES_FLAG     = (2 << BITS_PER_STATE) - 1
        };

        /**
         * Construct new history object
         */
        CChangeHistory () {}

        // Set bit flags
        static inline void SET_FLAG (int & value, int flag) {value |= flag;}

        // unset bit flags
        static inline void UNSET_FLAG (int & value, int flag) {value &= (~flag);}


        /**
         * Pop topmost and check next value
         */
        int Pop (int line)
        {
            // no state here
            if ((int)m_lines.size() <= line) return false;

            // get mod
            IntVector & mod = m_lines[line];

            // if it's too small?
            if (mod.size() < 2) return false;

            // no states? pop topmost
            if (mod[0] == 0) return false;
            int cellCount = --mod[0];
            LOG_MSG_INT("Pop:", cellCount);

            // get offset and index into bitset
            int offset = (cellCount % STATES_IN_CELL) * BITS_PER_STATE;
            int index  = (cellCount / STATES_IN_CELL) + 1;

            // get the bit value
            return (mod[index] >> offset) & STATES_FLAG;
        }


        /**
         * check current state and "undo"
         */
        int CheckAndRedo (int line)
        {
            // no state here
            if ((int)m_lines.size() <= line) return 0;

            // get mod
            IntVector & mod = m_lines[line];

            // if it's too small?
            if (mod.size() < 2) return 0;

            // get counter
            int cellCount = ++mod[0];
            LOG_MSG_INT("CheckAndRedo:", cellCount);
            // if (cellCount == 0) return 0;

            // get offset and index into bitset
            int offset = (cellCount % STATES_IN_CELL) * BITS_PER_STATE;
            int index  = (cellCount / STATES_IN_CELL) + 1;

            // get the bit value
            return (mod[index] >> offset) & STATES_FLAG;
        }


        /**
         * Push line state
         */
        void Push(int line, int value)
        {
            if (line >= (int)m_lines.size())
            {
                // m_lines.resize(m_lines.capacity() + BUFFER_SIZE);
                for (int i = m_lines.size(); i <= line; i++)
                    m_lines.push_back(IntVector());
            }

            // get mod
            IntVector & mod = m_lines[line];

            // get the cell or add a new entry
            if (mod.size() < (size_t)2)
            {
                mod.push_back(0);
                mod.push_back(0);
            }

            // get counter and increment count
            int cellCount = mod[0]++;
            LOG_MSG_INT("Push:", cellCount);

            // get offset (bit offset) and index
            int offset = (cellCount % STATES_IN_CELL) * BITS_PER_STATE;
            int index  = (cellCount / STATES_IN_CELL) + 1;

            // expand array if needed
            if ((int)mod.size() <= index) mod.push_back(0);

            // set cell value
            mod[index] = (mod[index] & (~(STATES_FLAG<<offset))) | ((value & STATES_FLAG) << offset);
        }


        /**
         * Set current state to given value
         */
        void SetCurrent (int line, int value)
        {
            // no state for the line?
            if (line >= (int)m_lines.size()) return;

            // get mod
            IntVector & mod = m_lines[line];

            // no state for the line yet?
            if (mod.size() < (size_t)2) return;

            // no states?
            int cellCount = mod[0];
            LOG_MSG_INT("SetCurrent:", cellCount);
            // if (cellCount == 0) return;
            // cellCount--;

            // get offset (bit offset) and index
            int offset = (cellCount % STATES_IN_CELL) * BITS_PER_STATE;
            int index  = (cellCount / STATES_IN_CELL) + 1;

            // set cell value
            mod[index] = (mod[index] & (~(STATES_FLAG<<offset))) | ((value & STATES_FLAG) << offset);
        }


        /**
         * Set all states to
         */
        void SetAllAndCurrentTo (CEditor * editor, int state, int currentState)
        {
            // generate common flag
            int flag = (state & STATES_FLAG);
            for (int offset = BITS_PER_STATE;
                 offset < (STATES_IN_CELL * BITS_PER_STATE);
                 offset += BITS_PER_STATE)
                flag |= (state & STATES_FLAG) << offset;

            // iterate through the lines
            for (int line = 0; line < m_lines.size(); line++)
            {
                for (int cell = 1; cell < m_lines[line].size(); cell++)
                {
                    m_lines[line][cell] = flag;
                }
                if (m_lines[line].size() > 1)
                {
                    SetCurrent(line, currentState);
                    SetMargins(editor, line, currentState);
                }
            }
        }


        /**
         * Add line to pending lines.
         */
        void AddPendingLine (int line)
        {
            // add the line
            m_pending.push_back(line);
        }


        /**
         * Commit pending lines
         */
        void PushAndMarkPending (CEditor * editor, int markers)
        {
            for ( int i = m_pending.size(); i ; )
            {
                int line = m_pending[--i];
                Push (line, editor->MarkerGet(line) & MARKER_FLAGS);
                SetMargins(editor, line, markers);
            }

            // reset pending
            m_pending.clear();
        }


        /**
         * Pop pending
         */
        void PopPending (CEditor * editor)
        {
            // Push the states
            // LOG_MSG_INT("PopPending:", m_pending.size());
            for ( int i = m_pending.size(); i ; )
            {
                int line = m_pending[--i];
                SetCurrent(line, editor->MarkerGet(line) & MARKER_FLAGS);
                SetMargins(editor, line, Pop(line));
            }

            // reset pending
            m_pending.clear();
        }


        /**
         * Undo pending
         */
        void UndoPending (CEditor * editor)
        {
            // Push the states
            // LOG_MSG_INT("UndoPending:", m_pending.size());
            for ( int i = m_pending.size(); i ; )
            {
                int line = m_pending[--i];
                SetMargins(editor, line, CheckAndRedo(line));
            }

            // reset pending
            m_pending.clear();
        }


        /**
         * Set editor margins
         */
        void SetMargins (CEditor * editor, int line, int markers)
        {
            // update margin
            if (markers & (1 << MARKER_EDITED))
                editor->MarkerAdd(line, MARKER_EDITED);
            else
                editor->MarkerDelete(line, MARKER_EDITED);

            // set saved
            if (markers & (1 << MARKER_SAVED))
                editor->MarkerAdd(line, MARKER_SAVED);
            else
                editor->MarkerDelete(line, MARKER_SAVED);
        }

        // vector of int's
        typedef std::vector<int>            IntVector;

        // the lines vector
        typedef std::vector<IntVector>      LinesVector;

        LinesVector m_lines;
        IntVector   m_pending;
    };
#endif


#if 0
    // Manage change margin
    void updateChangeMargin (wxScintillaEvent & event)
    {
        // modification type
        int mod = event.GetModificationType();

        // get line range affected
        int startLine = m_parent->LineFromPosition(event.GetPosition());
        int linesModified = event.GetLinesAdded();

        // append affected lines
        m_changes.AddPendingLine(startLine);

        if (linesModified > 0)
        {
            for ( ; linesModified > 0; linesModified-- )
                m_changes.AddPendingLine(startLine + linesModified);
        }
        /*
        else if (linesModified < 0)
        {
            int delLines = abs(linesModified);
            if (mod & wxSCI_PERFORMED_USER)
            {
                for ( ; delLines; delLines--)
                {
                    int line = startLine + delLines;
                    LOG_INT(line);
                    m_changes.Push (
                        line,
                        m_parent->MarkerGet(line) & MARKER_FLAGS
                    );
                }
            }
            else
            {
                LOG_MSG_INT("----------->", delLines);
                for ( ; delLines; delLines--)
                    m_changes.AddPendingLine(startLine + delLines);
            }
        }
        */

        // performed by user (edit, delete, insert)
        if (mod & wxSCI_PERFORMED_USER)
        {
            m_changes.PushAndMarkPending(m_parent, 1 << MARKER_EDITED);
        }
        // performed UNDO
        else if (mod & wxSCI_PERFORMED_UNDO)
        {
            m_changes.PopPending(m_parent);
        }
        // performed REDO
        else if (mod & wxSCI_PERFORMED_REDO)
        {
            m_changes.UndoPending(m_parent);
        }

    }
#endif

    /*
    MARKER_SAVED    = 0,
    MARKER_EDITED   = 1,
    MARKER_FLAGS    = (1 << MARKER_SAVED) | (1 << MARKER_EDITED),
    */
