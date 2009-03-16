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
#include "sdk/UiManager.h"


using namespace fb;



class ProjectPlugin : public CPluginBase
{
    public :

        // Attach plugin
        bool Attach ()
        {
            wxWindow * target = GET_UIMGR()->GetPaneWindow(_T("tools"));
            if (target)
            {
                wxPanel * p = new wxPanel(target);
                GET_UIMGR()->AddPane(_T("projects"), p, _T("tools"));
                return true;
            }
        }

        // Detach plugin
        bool Detach (bool force)
        {
            return true;
        }
};

namespace
{
    CPluginProvider<ProjectPlugin, PLUGINTYPE_TOOL> plugin (_T("Projects"));
}



