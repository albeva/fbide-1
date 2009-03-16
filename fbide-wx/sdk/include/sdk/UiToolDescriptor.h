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

#ifndef UITOOLDESCRIPTOR_H_INCLUDED
#define UITOOLDESCRIPTOR_H_INCLUDED

namespace fb
{

    /**
     * For menu and toolbar entries that require special handling
     *    - like fullscreen that has a checkbox...
     */
    class CUiToolDescriptor
    {
        public :

            /**
             * Degate method for Toggle tools
             * to get is toggled or not.
             */
            typedef Delegate0<bool> ToggleCallback;
            typedef Delegate<void (bool)> VoidBoolDelegate;
            typedef CMultiDelegate<void (bool)> VoidBoolMultiDelegate;
            VoidBoolMultiDelegate    m_checkEvt;

            /**
             * Supported tool types
             */
            enum
            {
                TOOL_NORMAL     = 0,
                TOOL_TOGGLE     = 1
            };

            /**
             * Is toggle tool ?
             */
            bool IsToggleTool () { return m_type == TOOL_TOGGLE; }

            /**
             * Get Tool type
             */
            int GetType () { return m_type; }

            /**
             * Is checked? (callback)
             */
            bool IsChecked ()
            {
                if (IsToggleTool()) return m_toggleCallback ();
                return false;
            }


            /**
             * Register event receaver
             */
            void Connect (VoidBoolDelegate signal)
            {
                m_checkEvt += signal;
            }


            /**
             * Send event
             */
            void Notify (bool checked)
            {
                m_checkEvt(checked);
            }


            /**
             * Create Toggle tool (2 state checked and unchecked)
             */
            static CUiToolDescriptor * CreateToggleTool (ToggleCallback callback)
            {
                CUiToolDescriptor * tool = new CUiToolDescriptor ();
                tool->m_type = TOOL_TOGGLE;
                tool->m_toggleCallback = callback;
                return tool;
            }


        private :

            int             m_type;
            ToggleCallback  m_toggleCallback;

            // use CreateXxx methods
            CUiToolDescriptor () : m_type(TOOL_NORMAL) { }
    };

}

#endif // UITOOLDESCRIPTOR_H_INCLUDED
