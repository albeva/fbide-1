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

#ifndef SINGLETON_H_INCLUDED
#define SINGLETON_H_INCLUDED

namespace fb
{

    template <class T> class CSingleton
    {
        private :
            static T * m_instance;

            // disable any copy constructions...
            explicit CSingleton (const CSingleton<T>&) { };
            CSingleton<T>& operator = (CSingleton<T> const&) { };


        protected :
            // private constructor
            CSingleton()
            {
                assert(CSingleton<T>::m_instance == 0L);
            }

            // private destructor
            virtual ~CSingleton()
            {
                CSingleton<T>::m_instance = NULL;
            }


        public:

            /**
             * Return true if Singleton instance exists
             */
            static inline bool IsCreated () { return m_instance != 0L; }


            /**
             * Get singleton class instance. Create if doesn't exist
             */
            static inline T * GetInstance()
            {
                if(m_instance == 0L) m_instance = new T();
                return m_instance;
            }


            /**
             * Release existing singleton instance
             */
            static inline void Release()
            {
                if (!CSingleton::IsCreated()) return;
                delete m_instance;
                m_instance = 0L;
            }
    };
    template<class T>T * CSingleton<T>::m_instance = 0L;

}

#endif // SINGLETON_H_INCLUDED
