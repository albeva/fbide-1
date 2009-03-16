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

#ifndef VARIANT_H_INCLUDED
#define VARIANT_H_INCLUDED

namespace fb
{

    /**
     * Very simple variant class for holding
     * strings, ints and booleans.
     * the reason for not using wxVariant is that
     * it tends to throw errors if value not set
     * rather than return empty value.
     */
    class DLLIMPORT CVariant
    {
        public :
            // default constructor
            CVariant () {}
            ~CVariant () {}

            // CVariant
            CVariant (const CVariant & value)
            : m_value(value.m_value)
            {}
            void operator = (const CVariant & rhs)
            { m_value = rhs.m_value; }
            void operator += (const CVariant & rhs)
            { m_value += rhs.m_value; }
            bool operator == (const CVariant & rhs)
            { return m_value == rhs.m_value; }
            bool operator != (const CVariant & rhs)
            { return m_value != rhs.m_value; }


            // wxString
            CVariant (const wxString & value)
            : m_value(value)
            {}
            wxString AsString (const wxString & defValue = _T(""))
            { return m_value.Len() ? m_value : defValue; }
            wxString AsString (const char * defValue)
            { return m_value.Len() ? m_value : wxString(defValue, wxConvUTF8); }
            void operator = (const wxString & rhs)
            { m_value = rhs; }
            void operator += (const wxString & rhs)
            { m_value += rhs; }
            bool operator == (const wxString & rhs)
            { return m_value == rhs; }
            bool operator != (const wxString & rhs)
            { return m_value != rhs; }

            // const char *
            explicit CVariant (const char * value)
            : m_value(wxString(value, wxConvUTF8))
            {}
            void operator = (const char * rhs)
            { m_value = wxString(rhs, wxConvUTF8); }
            void operator += (const char * rhs)
            { m_value += wxString(rhs, wxConvUTF8); }
            bool operator == (const char * rhs)
            { return m_value == wxString(rhs, wxConvUTF8); }
            bool operator != (const char * rhs)
            { return m_value != wxString(rhs, wxConvUTF8); }

            // const wxChar *
            explicit CVariant (const wxChar * value)
            : m_value(wxString(value, wxConvUTF8))
            {}
            void operator = (const wxChar * rhs)
            { m_value = rhs; }
            void operator += (const wxChar * rhs)
            { m_value += rhs; }
            bool operator == (const wxChar * rhs)
            { return m_value == rhs; }
            bool operator != (const wxChar * rhs)
            { return m_value != rhs; }

            // const int *
            CVariant (int value)
            { *this = value; }
            int AsInt (int defValue = 0)
            {
                if (!m_value.Len()) return defValue;
                long v;
                if (!m_value.ToLong(&v)) return (int)AsBool();
                return v;
            }
            void operator = (int rhs)
            { m_value.Empty(); m_value << rhs; }
            void operator += (int rhs)
            { *this = AsInt() + rhs; }
            bool operator == (int rhs)
            { return AsInt() == rhs; }
            bool operator != (int rhs)
            { return AsInt() != rhs; }

            // As bool
            bool AsBool (bool defValue = false)
            {
                if (!m_value.Len()) return defValue;
                if (m_value == _T("0"))
                    return false;
                if (m_value==_T("true")||m_value==_T("yes")||m_value==_T("on"))
                    return true;
                long v;
                if (m_value.ToLong(&v)) return true;
                return false;
            }
            void operator = (bool rhs)
            { m_value = rhs ? _T("true") : _T("false"); }
            bool operator == (bool rhs)
            { return AsBool() == rhs; }
            bool operator != (bool rhs)
            { return AsBool() != rhs; }


        private :
            wxString m_value;
    };
}

#endif // VARIANT_H_INCLUDED
