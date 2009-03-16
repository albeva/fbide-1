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

#ifndef SDK_H_INCLUDED
#define SDK_H_INCLUDED

#define SDK_VERSION_MAJOR 0
#define SDK_VERSION_MINOR 5
#define SDK_VERSION_RELEASE 0
#define SDK_VERSION_STRING _T("FBIde SDK 0.5.0")

#define SDK_CHECK_VERSION(major, minor, release) \
    (SDK_VERSION_MAJOR > (major) || \
    (SDK_VERSION_MAJOR == (major) && SDK_VERSION_MINOR > (minor)) || \
    (SDK_VERSION_MAJOR == (major) && SDK_VERSION_MINOR == (minor) && SDK_VERSION_RELEASE >= (release)))


#ifdef __WINDOWS__
    #if EXPORT_LIB
        #define DLLIMPORT __declspec (dllexport)
    #else
         #define DLLIMPORT __declspec (dllimport)
    #endif
    #define EXPORT_PLUGIN __declspec (dllexport)
#else
    #define DLLIMPORT
    #define EXPORT_PLUGIN
#endif


#endif // SDK_H_INCLUDED
