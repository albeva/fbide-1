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

#include "sdk/FastDelegate.h"

using namespace fastdelegate;

/**
 * To simplify CMultiDelegate
 * specialized templates some common
 * stuff is in this base.
 * also individual delegates are stored
 * as void * because compiler otherwise
 * seems to prove problematic when
 * nested partial templates are parameters
 * to one - another.
 */
template<class T> class CMultiDelegateBase
{
    public :
        void Add (T delegate)
        {
            m_list.push_back(new T(delegate));
        }
        void operator += (T delegate)
        {
            m_list.push_back(new T(delegate));
        }

        void Remove (T delegate)
        {
            Remove(&delegate);
        }
        void operator -= (T delegate)
        {
            Remove(&delegate);
        }

        /**
         * create
         */
        CMultiDelegateBase () : activeLevel(0), toCleanup(0)
        {
        }

        /**
         * Clean up
         */
        virtual ~CMultiDelegateBase()
        {
            iterator iter = m_list.begin();
            while (iter != m_list.end())
            {
                T * dg = reinterpret_cast<T *>(*iter);
                delete dg;
                iter++;
            }
        }
    protected :
        typedef std::vector<void *> list;
        typedef std::vector<void *>::iterator iterator;
        list m_list;
        int activeLevel; //
        int toCleanup;


        /**
         * Cleanup empty entries (caused by callee trying to remove itself
         * from call list
         */
        void Cleanup ()
        {
            if (activeLevel || !toCleanup) return;

            iterator iter = m_list.begin();
            while (iter != m_list.end())
            {
                if (*iter == NULL)
                {
                    iter = m_list.erase(iter);
                }
                else
                {
                    iter++;
                }
            }

            toCleanup = 0;
        }

    private :
        void Remove (T * delegate)
        {
            iterator iter = m_list.begin();
            while (iter != m_list.end())
            {
                T * dg = reinterpret_cast<T *>(*iter);
                if ((*dg) == *delegate)
                {
                    delete dg;
                    if (activeLevel)
                    {
                        *iter = NULL;
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
};

/**
 * Declare this is as template class and specialize
 * later on.
 */
template <typename Signature>
class CMultiDelegate;


/**
 * To provide *safe* calling of delegate functions we need to wrap things
 * up a bit. The problem being a callee wanting to erase itself from the
 * multidelegate. This cannot be done cleanly. SO this kind of wrapping
 * is required.
 */
#define DLG_CALL_START \
        parent::activeLevel++; \
        std::vector<void *>::iterator iter = parent::m_list.begin(); \
        while (iter != parent::m_list.end()) \
        { \
            type * dg = reinterpret_cast<type *>(*iter); \
            if (NULL != dg)

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


/**
 * N=0
 * R ();
 */
template<typename R>
class CMultiDelegate < R (  ) >
: public CMultiDelegateBase<Delegate0<R> >
{
    typedef Delegate0<R> type;
    typedef CMultiDelegateBase<type> parent;
    public :
        void operator () ()
        {
            DLG_CALL_START
            {
                (*dg)();
            }
            DLG_CALL_END
        }
};

/**
 * N=1
 * R (P1);
 */
template<typename R, class P1>
class CMultiDelegate < R ( P1 ) >
: public CMultiDelegateBase<Delegate1<P1, R> >
{
    typedef Delegate1<P1, R> type;
    typedef CMultiDelegateBase<type> parent;
    public :
        void operator () (P1 p1)
        {
            DLG_CALL_START
            {
                (*dg)(p1);
            }
            DLG_CALL_END
        }
};

/**
 * N=2
 * R (P1, P2);
 */
template<typename R, class P1, class P2>
class CMultiDelegate < R ( P1, P2 ) >
: public CMultiDelegateBase<Delegate2<P1, P2, R> >
{
    typedef Delegate2<P1, P2, R> type;
    typedef CMultiDelegateBase<type> parent;
    public :
        void operator () (P1 p1, P2 p2)
        {
            DLG_CALL_START
            {
                (*dg)(p1, p2);
            }
            DLG_CALL_END
        }
};

/**
 * N=3
 * R (P1, P2, P3);
 */
template<typename R, class P1, class P2, class P3>
class CMultiDelegate < R ( P1, P2, P3 ) >
: public CMultiDelegateBase<Delegate3<P1, P2, P3, R> >
{
    typedef Delegate3<P1, P2, P3, R> type;
    typedef CMultiDelegateBase<type> parent;
    public :
        void operator () (P1 p1, P2 p2, P3 p3)
        {
            DLG_CALL_START
            {
                (*dg)(p1, p2, p3);
            }
            DLG_CALL_END
        }
};

/**
 * N=4
 * R (P1, P2, P3, P4);
 */
template<typename R, class P1, class P2, class P3, class P4>
class CMultiDelegate < R ( P1, P2, P3, P4 ) >
: public CMultiDelegateBase<Delegate4<P1, P2, P3, P4, R> >
{
    typedef Delegate4<P1, P2, P3, P4, R> type;
    typedef CMultiDelegateBase<type> parent;
    public :
        void operator () (P1 p1, P2 p2, P3 p3, P4 p4)
        {
            DLG_CALL_START
            {
                (*dg)(p1, p2, p3, p4);
            }
            DLG_CALL_END
        }
};

/**
 * N=5
 * R (P1, P2, P3, P4, P5);
 */
template<typename R, class P1, class P2, class P3, class P4, class P5>
class CMultiDelegate < R ( P1, P2, P3, P4, P5 ) >
: public CMultiDelegateBase<Delegate5<P1, P2, P3, P4, P5, R> >
{
    typedef Delegate5<P1, P2, P3, P4, P5, R> type;
    typedef CMultiDelegateBase<type> parent;
    public :
        void operator () (P1 p1, P2 p2, P3 p3, P4 p4, P5 p5)
        {
            DLG_CALL_START
            {
                (*dg)(p1, p2, p3, p4, p5);
            }
            DLG_CALL_END
        }
};

#endif // MULTIDELEGATE_H_INCLUDED
