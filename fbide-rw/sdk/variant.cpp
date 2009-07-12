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
#include "variant.h"
#include <wx/regex.h>

using namespace fb;

namespace
{
    wxRegEx reSizePoint("^\\{([0-9]+)[ ]*,[ ]*([0-9]+)\\}$");
}


//------------------------------------------------------------------------------
// Variant
//------------------------------------------------------------------------------
Variant::Variant (const Variant & value)
: m_value(value.m_value) {}

void Variant::operator = (const Variant & rhs)
{ m_value = rhs.m_value; }

bool Variant::operator == (const Variant & rhs) const
{ return m_value == rhs.m_value; }

bool Variant::operator != (const Variant & rhs) const
{ return m_value != rhs.m_value; }


//------------------------------------------------------------------------------
// wxString
//------------------------------------------------------------------------------
Variant::Variant (const wxString & value)
: m_value(value) {}

void Variant::operator = (const wxString & rhs)
{ m_value = rhs; }

bool Variant::operator == (const wxString & rhs) const
{ return m_value == rhs; }

bool Variant::operator != (const wxString & rhs) const
{ return m_value != rhs; }

wxString Variant::AsString (const wxString & def) const
{ return m_value.Len() ? m_value : def; }


//------------------------------------------------------------------------------
// const char *
//------------------------------------------------------------------------------
Variant::Variant (const char * value)
: m_value(value) {}

void Variant::operator = (const char * rhs)
{ m_value = rhs; }

bool Variant::operator == (const char * rhs) const
{ return m_value == rhs; }

bool Variant::operator != (const char * rhs) const
{ return m_value != rhs; }


//------------------------------------------------------------------------------
// const wxChar *
//------------------------------------------------------------------------------
Variant::Variant (const wxChar * value)
: m_value(value) {}

void Variant::operator = (const wxChar * rhs)
{ m_value = rhs; }

bool Variant::operator == (const wxChar * rhs) const
{ return m_value == rhs; }

bool Variant::operator != (const wxChar * rhs) const
{ return m_value != rhs; }


//------------------------------------------------------------------------------
// int
//------------------------------------------------------------------------------
Variant::Variant (int value)
{ *this = value; }

void Variant::operator = (int rhs)
{ m_value.Empty(); m_value << rhs; }

bool Variant::operator == (int rhs) const
{ return AsInt() == rhs; }

bool Variant::operator != (int rhs) const
{ return AsInt() != rhs; }

int Variant::AsInt (int def) const
{
    if (!m_value.Len()) return def;

    long v; if (m_value.ToLong(&v)) return v;

    if (m_value == "true" || m_value == "yes" || m_value== "on")
        return (int)true;

    return def;
}


//------------------------------------------------------------------------------
// bool
//------------------------------------------------------------------------------
bool Variant::AsBool (bool def) const
{
    if (!m_value.Len()) return def;

    if (m_value == "0") return false;

    if (m_value == "true" || m_value == "yes" || m_value== "on")
        return true;

    long v; if (m_value.ToLong(&v)) return (bool)v;

    return def;
}

//------------------------------------------------------------------------------
// double
//------------------------------------------------------------------------------
Variant::Variant (double value)
{ *this = value; }

void Variant::operator = (double rhs)
{ m_value.Empty(); m_value << rhs; }

bool Variant::operator == (double rhs) const
{ return AsDouble() == rhs; }

bool Variant::operator != (double rhs) const
{ return AsDouble() != rhs; }

double Variant::AsDouble (double def) const
{
    if (!m_value.Len()) return def;

    double v; if (m_value.ToDouble(&v)) return v;

    return def;
}

//------------------------------------------------------------------------------
// wxSize {w, h}
//------------------------------------------------------------------------------
Variant::Variant (const wxSize & value)
{ *this = value; }

void Variant::operator = (const wxSize & rhs)
{
    m_value.Empty();
    m_value = MakeString(rhs);
}

bool Variant::operator == (const wxSize & rhs) const
{ return m_value == MakeString(rhs); }

bool Variant::operator != (const wxSize & rhs) const
{ return m_value != MakeString(rhs); }

wxSize Variant::AsSize (const wxSize & def) const
{
    if (!m_value.Len()) return def;
    if (reSizePoint.Matches(m_value))
    {
        auto w_str = reSizePoint.GetMatch(m_value, 1);
        auto h_str = reSizePoint.GetMatch(m_value, 2);
        long w = 0, h = 0;
        w_str.ToLong(&w);
        h_str.ToLong(&h);
        return wxSize(w, h);
    }
    return def;
}

wxString Variant::MakeString (const wxSize & size)
{
    wxString tmp;
    tmp << "{" << size.GetWidth()
        << ", " << size.GetHeight() << "}"
        ;
    return tmp;
}


//------------------------------------------------------------------------------
// wxPoint {w, h}
//------------------------------------------------------------------------------
Variant::Variant (const wxPoint & value)
{ *this = value; }

void Variant::operator = (const wxPoint & rhs)
{
    m_value.Empty();
    m_value = MakeString(rhs);
}

bool Variant::operator == (const wxPoint & rhs) const
{ return m_value == MakeString(rhs); }

bool Variant::operator != (const wxPoint & rhs) const
{ return m_value != MakeString(rhs); }

wxPoint Variant::AsPoint (const wxPoint & def) const
{
    if (!m_value.Len()) return def;
    if (reSizePoint.Matches(m_value))
    {
        auto x_str = reSizePoint.GetMatch(m_value, 1);
        auto y_str = reSizePoint.GetMatch(m_value, 2);
        long x = 0, y = 0;
        x_str.ToLong(&x);
        y_str.ToLong(&y);
        return wxPoint(x, y);
    }
    return def;
}

wxString Variant::MakeString (const wxPoint & point)
{
    wxString tmp;
    tmp << "{" << point.x
        << ", " << point.y << "}"
        ;
    return tmp;
}


//------------------------------------------------------------------------------
// wxColour css notation
//------------------------------------------------------------------------------
Variant::Variant (const wxColour & value)
{ *this = value; }

void Variant::operator = (const wxColour & rhs)
{
    m_value.Empty();
    m_value = MakeString(rhs);
}

bool Variant::operator == (const wxColour & rhs) const
{ return m_value == MakeString(rhs); }

bool Variant::operator != (const wxColour & rhs) const
{ return m_value != MakeString(rhs); }

wxColour Variant::AsColour (const wxColour & def) const
{
    if (!m_value.Len()) return def;
    auto c = wxColour(m_value);
    if (c.IsOk ()) return c;
    return def;
}

wxString Variant::MakeString (const wxColour & colour)
{
    return colour.GetAsString(wxC2S_HTML_SYNTAX);
}
