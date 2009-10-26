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

namespace fbi
{

    /**
     * Simple SDK specific variant class
     *
     * internally all are stored as strings so conversions
     * between the types are possible.
     *
     * supported types
     * - string, int, bool, float
     * - wxSize, wxColour, wxPoint
     */
    struct SDK_DLL Variant
    {
        // default constructor and destructor
        Variant () {};
        ~Variant () {};

        // Variant
        Variant (const Variant & value);
        void operator = (const Variant & rhs);
        bool operator == (const Variant & rhs) const;
        bool operator != (const Variant & rhs) const;

        // wxString
        Variant (const wxString & value);
        void operator = (const wxString & rhs);
        bool operator == (const wxString & rhs) const;
        bool operator != (const wxString & rhs) const;
        wxString AsString (const wxString & def = "") const;
        inline operator wxString () const { return AsString(); }

        // const char *
        Variant (const char * value);
        void operator = (const char * rhs);
        bool operator == (const char * rhs) const;
        bool operator != (const char * rhs) const;

        // const wxChar *
        Variant (const wxChar * value);
        void operator = (const wxChar * rhs);
        bool operator == (const wxChar * rhs) const;
        bool operator != (const wxChar * rhs) const;

        // integer ( works for bools too )
        Variant (int value);
        void operator = (int rhs);
        bool operator == (int rhs) const;
        bool operator != (int rhs) const;
        int AsInt (int def = 0) const;
        inline operator int () const { return AsInt(); }

        // boolean values
        bool AsBool (bool def = false) const;

        // double
        Variant (double value);
        void operator = (double rhs);
        bool operator == (double rhs) const;
        bool operator != (double rhs) const;
        double AsDouble (double def = 0.0) const;
        inline operator double () const { return AsDouble(); }

        // wxSize {w, h}
        Variant (const wxSize & size);
        void operator = (const wxSize & rhs);
        bool operator == (const wxSize & rhs) const;
        bool operator != (const wxSize & rhs) const;
        wxSize AsSize (const wxSize & def = wxDefaultSize) const;
        static wxString MakeString (const wxSize & size);
        inline operator wxSize () const { return AsSize(); }

        // wxPoint {x, y}
        Variant (const wxPoint & point);
        void operator = (const wxPoint & rhs);
        bool operator == (const wxPoint & rhs) const;
        bool operator != (const wxPoint & rhs) const;
        wxPoint AsPoint (const wxPoint & def = wxDefaultPosition) const;
        static wxString MakeString (const wxPoint & point);
        inline operator wxPoint () const { return AsPoint(); }

        // wxColour
        Variant (const wxColour & colour);
        void operator = (const wxColour & rhs);
        bool operator == (const wxColour & rhs) const;
        bool operator != (const wxColour & rhs) const;
        wxColour AsColour (const wxColour & def = *wxBLACK) const;
        static wxString MakeString (const wxColour & colour);
        inline operator wxColour () const { return AsColour(); }

        private :
            wxString m_value;
    };


    /**
     * Associative hashmap for variants
     */
    typedef std::unordered_map<
        wxString, Variant,
        wxStringHash, wxStringEqual
    > VariantHashMap;
}
