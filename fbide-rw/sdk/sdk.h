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

// version information
#define SDK_VERSION_MAJOR       0
#define SDK_VERSION_MINOR       5
#define SDK_VERSION_RELEASE     1
#define SDK_VERSION_STRING      "0.5.1"

// macro function to check sdk version
#define SDK_CHECK_VERSION(major, minor, release) \
    ((SDK_VERSION_MAJOR > (major) || \
    (SDK_VERSION_MAJOR == (major) && SDK_VERSION_MINOR > (minor)) || \
    (SDK_VERSION_MAJOR == (major) && SDK_VERSION_MINOR == (minor) && SDK_VERSION_RELEASE >= (release))))

// macros for DLL import / export clauses
#ifdef __WINDOWS__
#   ifdef EXPORT_SDK
#       define SDK_DLL __declspec (dllexport)
#   else
#       define SDK_DLL __declspec (dllimport)
#   endif
#   ifdef EXPORT_PLUGIN
#       define PLUGIN_DLL __declspec (dllexport)
#   else
#       define PLUGIN_DLL __declspec (dllimport)
#   endif
#else
#   define SDK_DLL
#   define PLUGIN_DLL
#endif
