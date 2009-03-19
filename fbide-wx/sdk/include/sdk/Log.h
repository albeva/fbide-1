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

#ifndef LOG_H_INCLUDED
#define LOG_H_INCLUDED

// Log simple message
static inline void fbLogMsg(const wxString & msg)
{
    wxLogMessage(_T("%s"), msg.c_str());
}
static inline void fbLogMsg(const char * msg)
{
    fbLogMsg(wxString(msg, wxConvUTF8));
}
#define LOG_MSG(_msg) fbLogMsg(_msg);

// log integer
#define LOG_INT(_v) \
    wxLogMessage(_T("%s = %d"), _T(#_v), (int)_v);

// log message and an integer
static inline void fbLogMsg(const wxString & msg, const wxString & var, int value)
{
    // wxLogMessage(_T("%s\t\t\t(line: %d)"), msg.c_str(), line);
    wxLogMessage(_T("%s %s = %d"), msg.c_str(), var.c_str(), value);
}
static inline void fbLogMsg(const char * msg, const wxString & var, int value)
{
    fbLogMsg(wxString(msg, wxConvUTF8), var, value);
}
#define LOG_MSG_INT(_msg, _v) fbLogMsg(_msg, _T(#_v), (int)_v);

// log binary number
#define LOG_BIN(_n) \
    wxString s; for (int i = (sizeof(_n) * 8)-1; i >= 0 ; i--) \
        s << (_n & (1 << i) ? _T("1") : _T("0")); \
    wxLogMessage(_T("%s = %s"), _T(#_n), s.c_str());

#endif // LOG_H_INCLUDED
