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

#ifndef EXCEPTION_H_INCLUDED
#define EXCEPTION_H_INCLUDED



    /**
     * Global base Exception object used in FBIde
     *
     * Other classes in FBIde are encoriged to derive from
     * this base class
     */
    class DLLIMPORT CException
    {
        public :
            CException (const wxString & message, const wxString & file = _T(""), int line = 0, const wxString & method = _T(""))
                : m_message(message), m_file(file), m_line(line), m_method(method)
            {}

            CException (const char * message, const wxString & file = _T(""), int line = 0, const wxString & method = _T(""))
                : m_message(wxString(message, wxConvUTF8)), m_file(file), m_line(line), m_method(method)
            {}

            const wxString & GetMessage () const
            {
                return m_message;
            }

            const wxString & GetFile () const
            {
                return m_file;
            }

            int GetLine () const
            {
                return m_line;
            }

            const wxString & GetMethod () const
            {
                return m_method;
            }

        private :
            wxString    m_message;
            wxString    m_file;
            int         m_line;
            wxString    m_method;
    };


    /**
     * For creating CException instance.
     * This automatically adds file, line and method
     * to exception message.
     */
    #define EXCEPTION(msg) ::CException((msg), wxString(__FILE__, wxConvUTF8), __LINE__, wxString(__PRETTY_FUNCTION__, wxConvUTF8));

    /**
     * FBIde specific TRY ... CATCH block that
     * traps most exceptions in a consistant
     * form
     */
    #define FBIDE_TRY try
    #define FBIDE_CATCH()                   \
        catch (CException e)                \
        {                                   \
            ::ShowException (e);            \
        }                                   \
        catch (CException * e)              \
        {                                   \
            ::ShowException (*e);           \
            delete e;                       \
        }                                   \
        catch (wxString msg)                \
        {                                   \
            ::ShowException (msg);          \
        }                                   \
        catch (int code)                    \
        {                                   \
            wxString msg = _T("Code : ");   \
            msg << code;                    \
            ::ShowException(msg);           \
        }                                   \
        catch (...)                         \
        {                                   \
            ::ShowException(_T("Unknown exception was thrown!")); \
        }


/**
 * Show exception message window
 */
void DLLIMPORT ShowException (const wxString & msg);
void DLLIMPORT ShowException (const CException & ex);

#endif // EXCEPTION_H_INCLUDED
