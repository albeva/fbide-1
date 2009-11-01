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

#ifndef MULTIDELEGATE_H_INCLUDED
#define MULTIDELEGATE_H_INCLUDED

#include "Delegate.h"


/**
 * To simplify MultiDelegate
 * specialized templates some common
 * stuff is in this base.
 * also individual delegates are stored
 * as void * because compiler otherwise
 * seems to prove problematic when
 * nested partial templates are parameters
 * to one - another.
 */
template<class T, class Container = std::vector<T*>> class MultiDelegateBase
{
    public :
        // convinience overload for connecting slots
        void operator += (const T & delegate) 
        {
            Connect(delegate);
        }        
        // Connect slot
        void Connect (const T & delegate)
        {
            m_list.push_back(new T(delegate));
        }

        // convinience method to disconnect
        void operator -= (const T & delegate)
        {
            Disconnect(delegate);
        }
        // disconnect a slot
        void Disconnect (const T & delegate)
        {
            auto iter = m_list.begin();
            while (iter != m_list.end())
            {
                if (**iter == delegate)
                {
                    delete *iter;
                    if (activeLevel)
                    {
                        *iter = nullptr;
                        toCleanup++;
                    }
                    else
                    {
                        iter = m_list.erase(iter);
                        continue;
                    }
                }
                iter++;
            }
        }

        /**
         * create
         */
        MultiDelegateBase () : activeLevel(0), toCleanup(0) {}

        /**
         * Clean up
         */
        virtual ~MultiDelegateBase()
        {
            for (auto iter = m_list.begin(); iter != m_list.end(); iter++)
            {
                delete *iter;
            }
        }

    protected :
        Container m_list;
        int activeLevel;
        int toCleanup;

        /**
         * Cleanup empty entries (caused by callee trying to remove itself
         * from call list
         */
        void Cleanup ()
        {
            // can't or nothing to clean up
            if (activeLevel || !toCleanup) return;
            // do the cleanup
            auto iter = m_list.begin();
            while (iter != m_list.end())
            {
                if (*iter == nullptr) iter = m_list.erase(iter);
                else iter++;
            }
            toCleanup = 0;
        }
};

/**
 * Declare this is as template class and specialize
 * later on.
 */
template <typename Signature>
class MultiDelegate;


/**
 * To provide *safe* calling of delegate functions we need to wrap things
 * up a bit. The problem being a callee wanting to erase itself from the
 * multidelegate. This cannot be done cleanly. SO this kind of wrapping
 * is required.
 */
#define DLG_CALL_START \
        parent::activeLevel++; \
        auto iter = parent::m_list.begin(); \
        while (iter != parent::m_list.end()) \
        { \
            if (nullptr != *iter)

/**
 * End block. Wrap delegate method call
 */
#define DLG_CALL_END \
            else if (parent::activeLevel == 1) \
            { \
                iter = parent::m_list.erase(iter); \
                parent::toCleanup--; \
                continue; \
            } \
            iter++; \
        } \
        parent::activeLevel--; \
        parent::Cleanup();


// N=0
// R ();
template<typename R>
class MultiDelegate < R (  ) >
: public MultiDelegateBase<Delegate0<R> >
{
    typedef Delegate0<R> type;
    typedef MultiDelegateBase<type> parent;
    public :
        typedef type Signature;
        void operator () ()
        {
            DLG_CALL_START
            {
                (**iter)();
            }
            DLG_CALL_END
        }
};


// N=1
// R (P1);
template<typename R, class P1>
class MultiDelegate < R ( P1 ) >
: public MultiDelegateBase<Delegate1<P1, R> >
{
    typedef Delegate1<P1, R> type;
    typedef MultiDelegateBase<type> parent;
    public :
        typedef type Signature;
        void operator () (P1 && p1)
        {
            DLG_CALL_START
            {
                (**iter)(std::forward<P1>(p1));
            }
            DLG_CALL_END
        }
};


// N=2
// R (P1, P2);
template<typename R, class P1, class P2>
class MultiDelegate < R ( P1, P2 ) >
: public MultiDelegateBase< Delegate2<P1, P2, R> >
{
    typedef Delegate2<P1, P2, R> type;
    typedef MultiDelegateBase<type> parent;
    public :
        typedef type Signature;
        void operator () (P1 && p1, P2 && p2)
        {
            DLG_CALL_START
            {
                (**iter)(std::forward<P1>(p1), std::forward<P2>(p2));
            }
            DLG_CALL_END
        }
};


// N=3
// R (P1, P2, P3);
template<typename R, class P1, class P2, class P3>
class MultiDelegate < R ( P1, P2, P3 ) >
: public MultiDelegateBase<Delegate3<P1, P2, P3, R> >
{
    typedef Delegate3<P1, P2, P3, R> type;
    typedef MultiDelegateBase<type> parent;
    public :
        typedef type Signature;
        void operator () (P1 && p1, P2 && p2, P3 && p3)
        {
            DLG_CALL_START
            {
                (**iter)(std::forward<P1>(p1), std::forward<P2>(p2),
                      std::forward<P3>(p3));
            }
            DLG_CALL_END
        }
};


// N=4
// R (P1, P2, P3, P4);
template<typename R, class P1, class P2, class P3, class P4>
class MultiDelegate < R ( P1, P2, P3, P4 ) >
: public MultiDelegateBase<Delegate4<P1, P2, P3, P4, R> >
{
    typedef Delegate4<P1, P2, P3, P4, R> type;
    typedef MultiDelegateBase<type> parent;
    public :
        typedef type Signature;
        void operator () (P1 && p1, P2 && p2, P3 && p3, P4 && p4)
        {
            DLG_CALL_START
            {
                (**iter)(std::forward<P1>(p1), std::forward<P2>(p2), 
                      std::forward<P3>(p3), std::forward<P4>(p4));
            }
            DLG_CALL_END
        }
};


// N=5
// R (P1, P2, P3, P4, P5);
template<typename R, class P1, class P2, class P3, class P4, class P5>
class MultiDelegate < R ( P1, P2, P3, P4, P5 ) >
: public MultiDelegateBase<Delegate5<P1, P2, P3, P4, P5, R> >
{
    typedef Delegate5<P1, P2, P3, P4, P5, R> type;
    typedef MultiDelegateBase<type> parent;
    public :
        typedef type Signature;
        void operator () (P1 && p1, P2 && p2, P3 && p3, P4 && p4, P5 && p5)
        {
            DLG_CALL_START
            {
                (**iter)(std::forward<P1>(p1), std::forward<P2>(p2),
                      std::forward<P3>(p3), std::forward<P4>(p4),
                      std::forward<P5>(p5));
            }
            DLG_CALL_END
        }
};


// N=6
// R (P1, P2, P3, P4, P5, P6);
template<typename R, class P1, class P2, class P3, class P4, class P5, class P6>
class MultiDelegate < R ( P1, P2, P3, P4, P5, P6 ) >
: public MultiDelegateBase<Delegate6<P1, P2, P3, P4, P5, P6, R> >
{
    typedef Delegate6<P1, P2, P3, P4, P5, P6, R> type;
    typedef MultiDelegateBase<type> parent;
    public :
        typedef type Signature;
        void operator () (P1 && p1, P2 && p2, P3 && p3, P4 && p4, P5 && p5, P6 && p6)
        {
            DLG_CALL_START
            {
                (**iter)(std::forward<P1>(p1), std::forward<P2>(p2),
                      std::forward<P3>(p3), std::forward<P4>(p4),
                      std::forward<P5>(p5), std::forward<P6>(p6));
            }
            DLG_CALL_END
        }
};


// N=7
// R (P1, P2, P3, P4, P5, P6, P7);
template<typename R, class P1, class P2, class P3, class P4, class P5, class P6, class P7>
class MultiDelegate < R ( P1, P2, P3, P4, P5, P6, P7 ) >
: public MultiDelegateBase<Delegate7<P1, P2, P3, P4, P5, P6, P7, R>>
{
    typedef Delegate7<P1, P2, P3, P4, P5, P6, P7, R> type;
    typedef MultiDelegateBase<type> parent;
    public :
        typedef type Signature;
        void operator () (P1 && p1, P2 && p2, P3 && p3, P4 && p4, P5 && p5, P6 && p6, P7 && p7)
        {
            DLG_CALL_START
            {
                (**iter)(std::forward<P1>(p1), std::forward<P2>(p2),
                      std::forward<P3>(p3), std::forward<P4>(p4),
                      std::forward<P5>(p5), std::forward<P6>(p6),
                      std::forward<P7>(p7));
            }
            DLG_CALL_END
        }
};


// N=8
// R (P1, P2, P3, P4, P5, P6, P7, P8);
template<typename R, class P1, class P2, class P3, class P4, class P5, class P6, class P7, class P8>
class MultiDelegate < R ( P1, P2, P3, P4, P5, P6, P7, P8 ) >
: public MultiDelegateBase<Delegate8<P1, P2, P3, P4, P5, P6, P7, P8, R>>
{
    typedef Delegate8<P1, P2, P3, P4, P5, P6, P7, P8, R> type;
    typedef MultiDelegateBase<type> parent;
    public :
        typedef type Signature;
        void operator () (P1 && p1, P2 && p2, P3 && p3, P4 && p4, P5 && p5, P6 && p6, P7 && p7, P8 && p8)
        {
            DLG_CALL_START
            {
                (**iter)(std::forward<P1>(p1), std::forward<P2>(p2),
                      std::forward<P3>(p3), std::forward<P4>(p4),
                      std::forward<P5>(p5), std::forward<P6>(p6),
                      std::forward<P7>(p7), std::forward<P8>(p8));
            }
            DLG_CALL_END
        }
};


#endif // MULTIDELEGATE_H_INCLUDED
