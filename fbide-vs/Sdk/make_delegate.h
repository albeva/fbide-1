/**
* Simple helper template functions to construct
* delegates uging boost function and boost bind
*/
#include <boost/function.hpp>

// Simple namespace shortcut. Using plain boost will need different namespaces
namespace boost
{

    /// No parameters
    template <class R, class T, class C>
    function<R()> make_delegate(R (C::*func)(), T * self) {
        return function<R()>(bind(func, self));
    }
    template <class R, class T, class C>
    function<R()> make_delegate(R (C::*func)() const, T * self) {
        return function<R()>(bind(func, self));
    }
    /// 1 parameter
    template <class R, class Arg1, class T, class C>
    function<R(Arg1)> make_delegate(R (C::*func)(Arg1), T * self) {
        return function<R(Arg1)>(bind(func, self, _1));
    }
    template <class R, class Arg1, class T, class C>
    function<R(Arg1)> make_delegate(R (C::*func)(Arg1) const, T * self) {
        return function<R(Arg1)>(bind(func, self, _1));
    }
    /// 2 parameters
    template <class R, class Arg1, class Arg2, class T, class C>
    function<R(Arg1, Arg2)> make_delegate(R (C::*func)(Arg1, Arg2), T * self) {
        return function<R(Arg1, Arg2)>(bind(func, self, _1, _2));
    }
    template <class R, class Arg1, class Arg2, class T, class C>
    function<R(Arg1, Arg2)> make_delegate(R (C::*func)(Arg1, Arg2) const, T * self) {
        return function<R(Arg1, Arg2)>(bind(func, self, _1, _2));
    }
    /// 3 parameters
    template <class R, class Arg1, class Arg2, class Arg3, class T, class C>
    function<R(Arg1, Arg2, Arg3)> make_delegate(R (C::*func)(Arg1, Arg2, Arg3), T * self) {
        return function<R(Arg1, Arg2, Arg3)>(bind(func, self, _1, _2, _3));
    }
    template <class R, class Arg1, class Arg2, class Arg3, class T, class C>
    function<R(Arg1, Arg2, Arg3)> make_delegate(R (C::*func)(Arg1, Arg2, Arg3) const, T * self) {
        return function<R(Arg1, Arg2, Arg3)>(bind(func, self, _1, _2, _3));
    }
    /// 4 parameters
    template <class R, class Arg1, class Arg2, class Arg3, class Arg4, class T, class C>
    function<R(Arg1, Arg2, Arg3, Arg4)> make_delegate(R (C::*func)(Arg1, Arg2, Arg3, Arg4), T * self) {
        return function<R(Arg1, Arg2, Arg3, Arg4)>(bind(func, self, _1, _2, _3, _4));
    }
    template <class R, class Arg1, class Arg2, class Arg3, class Arg4, class T, class C>
    function<R(Arg1, Arg2, Arg3, Arg4)> make_delegate(R (C::*func)(Arg1, Arg2, Arg3, Arg4) const, T * self) {
        return function<R(Arg1, Arg2, Arg3, Arg4)>(bind(func, self, _1, _2, _3, _4));
    }
    /// 5 parameters
    template <class R, class Arg1, class Arg2, class Arg3, class Arg4, class Arg5, class T, class C>
    function<R(Arg1, Arg2, Arg3, Arg4, Arg5)> make_delegate(R (C::*func)(Arg1, Arg2, Arg3, Arg4, Arg5), T * self) {
        return function<R(Arg1, Arg2, Arg3, Arg4, Arg5)>(bind(func, self, _1, _2, _3, _4, _5));
    }
    template <class R, class Arg1, class Arg2, class Arg3, class Arg4, class Arg5, class T, class C>
    function<R(Arg1, Arg2, Arg3, Arg4, Arg5)> make_delegate(R (C::*func)(Arg1, Arg2, Arg3, Arg4, Arg5) const, T * self) {
        return function<R(Arg1, Arg2, Arg3, Arg4, Arg5)>(bind(func, self, _1, _2, _3, _4, _5));
    }
    /// 6 parameters
    template <class R, class Arg1, class Arg2, class Arg3, class Arg4, class Arg5, class Arg6, class T, class C>
    function<R(Arg1, Arg2, Arg3, Arg4, Arg5, Arg6)> make_delegate(R (C::*func)(Arg1, Arg2, Arg3, Arg4, Arg5, Arg6), T * self) {
        return function<R(Arg1, Arg2, Arg3, Arg4, Arg5, Arg6)>(bind(func, self, _1, _2, _3, _4, _5, _6));
    }
    template <class R, class Arg1, class Arg2, class Arg3, class Arg4, class Arg5, class Arg6, class T, class C>
    function<R(Arg1, Arg2, Arg3, Arg4, Arg5, Arg6)> make_delegate(R (C::*func)(Arg1, Arg2, Arg3, Arg4, Arg5, Arg6) const, T * self) {
        return function<R(Arg1, Arg2, Arg3, Arg4, Arg5, Arg6)>(bind(func, self, _1, _2, _3, _4, _5, _6));
    }
    /// 7 parameters
    template <class R, class Arg1, class Arg2, class Arg3, class Arg4, class Arg5, class Arg6, class Arg7, class T, class C>
    function<R(Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7)> make_delegate(R (C::*func)(Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7), T * self) {
        return function<R(Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7)>(bind(func, self, _1, _2, _3, _4, _5, _6, _7));
    }
    template <class R, class Arg1, class Arg2, class Arg3, class Arg4, class Arg5, class Arg6, class Arg7, class T, class C>
    function<R(Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7)> make_delegate(R (C::*func)(Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7) const, T * self) {
        return function<R(Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7)>(bind(func, self, _1, _2, _3, _4, _5, _6, _7));
    }
    /// 8 parameters
    template <class R, class Arg1, class Arg2, class Arg3, class Arg4, class Arg5, class Arg6, class Arg7, class Arg8, class T, class C>
    function<R(Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7, Arg8)> make_delegate(R (C::*func)(Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7, Arg8), T * self) {
        return function<R(Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7, Arg8)>(bind(func, self, _1, _2, _3, _4, _5, _6, _7, _8));
    }
    template <class R, class Arg1, class Arg2, class Arg3, class Arg4, class Arg5, class Arg6, class Arg7, class Arg8, class T, class C>
    function<R(Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7, Arg8)> make_delegate(R (C::*func)(Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7, Arg8) const, T * self) {
        return function<R(Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7, Arg8)>(bind(func, self, _1, _2, _3, _4, _5, _6, _7, _8));
    }
    /// 9 parameters
    template <class R, class Arg1, class Arg2, class Arg3, class Arg4, class Arg5, class Arg6, class Arg7, class Arg8, class Arg9, class T, class C>
    function<R(Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7, Arg8, Arg9)> make_delegate(R (C::*func)(Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7, Arg8, Arg9), T * self) {
        return function<R(Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7, Arg8, Arg9)>(bind(func, self, _1, _2, _3, _4, _5, _6, _7, _8, _9));
    }
    template <class R, class Arg1, class Arg2, class Arg3, class Arg4, class Arg5, class Arg6, class Arg7, class Arg8, class Arg9, class T, class C>
    function<R(Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7, Arg8, Arg9)> make_delegate(R (C::*func)(Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7, Arg8, Arg9) const, T * self) {
        return function<R(Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7, Arg8, Arg9)>(bind(func, self, _1, _2, _3, _4, _5, _6, _7, _8, _9));
    }

}