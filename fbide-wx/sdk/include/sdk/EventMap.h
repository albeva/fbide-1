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

#ifndef EVENT_MAP_H_INCLUDED
#define EVENT_MAP_H_INCLUDED

#include "sdk/MultiDelegate.h"
#include <map>

namespace fb
{

    /**
     * Provide simple EVENT_ID -> multidelegate map
     * for event broadcasting classes to implement
     */
     template<typename KEY, class MULTI_DLG>
     class CEventMapBase
     {
         public :

            // the method
            typedef typename MULTI_DLG::Signature Signature;

            // Add event to the EventMap
            void Bind (KEY event, Signature delegate)
            {
                m_evt[event] += delegate;
            }

            // Remove event from the EventMap
            void UnBind (KEY event, Signature delegate)
            {
                m_evt[event] -= delegate;
            }

            // get reference to the MULTI_DLG
            inline MULTI_DLG & GetEventMap (KEY event)
            {
                return m_evt[event];
            }

        private :
            typedef std::map<KEY, MULTI_DLG> EventMap;
            EventMap m_evt;
    };

    // Specialise CEventMap template for nice api
    template<typename KEY, typename T>
    class CEventMap;

    // R()
    template<typename KEY, typename R>
    class CEventMap < KEY,  R (  ) >
    : public CEventMapBase<KEY, CMultiDelegate<R> >
    {};

    // R(p1)
    template<typename KEY, typename R, typename P1>
    class CEventMap < KEY,  R ( P1 ) >
    : public CEventMapBase<KEY, CMultiDelegate<R(P1)> >
    {};

    // R(p1, p2)
    template<typename KEY, typename R, typename P1, typename P2>
    class CEventMap < KEY,  R ( P1, P2 ) >
    : public CEventMapBase<KEY, CMultiDelegate<R(P1, P2)> >
    {};

    // R(p1, p2, p3)
    template<typename KEY, typename R, typename P1, typename P2, typename P3>
    class CEventMap < KEY,  R ( P1, P2, P3 ) >
    : public CEventMapBase<KEY, CMultiDelegate<R(P1, P2, P3)> >
    {};

    // R(p1, p2, p3, p4)
    template<typename KEY, typename R, typename P1, typename P2, typename P3, typename P4>
    class CEventMap < KEY,  R ( P1, P2, P3, P4 ) >
    : public CEventMapBase<KEY, CMultiDelegate<R(P1, P2, P3, P4)> >
    {};

    // R(p1, p2, p3, p4, p5)
    template<typename KEY, typename R, typename P1, typename P2, typename P3, typename P4, typename P5>
    class CEventMap < KEY,  R ( P1, P2, P3, P4, P5 ) >
    : public CEventMapBase<KEY, CMultiDelegate<R(P1, P2, P3, P4, P5)> >
    {};

    // R(p1, p2, p3, p4, p5, p6)
    template<typename KEY, typename R, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6>
    class CEventMap < KEY,  R ( P1, P2, P3, P4, P5, P6 ) >
    : public CEventMapBase<KEY, CMultiDelegate<R(P1, P2, P3, P4, P5, P6)> >
    {};

} // namespace fb

#endif // EVENT_MAP_H_INCLUDED


