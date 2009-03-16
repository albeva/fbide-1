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

#ifndef PLUGINARTPROVIDER_H_INCLUDED
#define PLUGINARTPROVIDER_H_INCLUDED

#include "sdk/PluginManager.h"
#include "sdk/ArtProvider.h"
#include "sdk/Manager.h"
#include "sdk/UiManager.h"

namespace fb {

    /**
     * Simple Template to ease the Artproviders plugins
     */
    template<class T> class CPluginArtProviderBase : public CPluginBase
    {
        protected :
            Delegate<void(IArtProvider*)> m_onChange;
            T m_provider;
            typedef CPluginArtProviderBase<T> Self;

        public :
            /**
             * Activate
             */
            bool Attach ()
            {
                m_onChange = MakeDelegate(this, &Self::OnChange);
                GET_UIMGR()->SetArtProvider(&m_provider);
                GET_UIMGR()->onArtProvderChange += m_onChange;
                return true;
            }


            /**
             * Deactivate
             */
            bool Detach (bool force)
            {
                if (GET_UIMGR()->GetArtProvider() == &m_provider)
                {
                    GET_UIMGR()->onArtProvderChange -= m_onChange;
                    GET_UIMGR()->SetArtProvider(NULL);
                }
                return true;
            }


            /**
             * Event callback
             */
            void OnChange (IArtProvider * provider)
            {
                if (provider == &m_provider) return;
                GET_UIMGR()->onArtProvderChange -= m_onChange;
            }

    };


    /**
     * Plugin registrant shorthand.
     * use:
     * CPluginArtProvider<ArtProviderClass> plugin (_T("PluginName"));
     */
    template<class T> class CPluginArtProvider :
        public CPluginProvider<CPluginArtProviderBase<T>, PLUGINTYPE_ARTPROVIDER>
    {
        private :
            typedef CPluginProvider<CPluginArtProviderBase<T>, PLUGINTYPE_ARTPROVIDER> parent;

        public :
            CPluginArtProvider<T> (const wxString& name) : parent(name) {}
            virtual ~CPluginArtProvider<T> () {}
    };
}

#endif // PLUGINARTPROVIDER_H_INCLUDED
