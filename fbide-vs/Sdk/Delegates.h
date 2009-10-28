/**
* Simple helper template functions to construct
* delegates uging boost function and boost bind
*/
#include <functional>


// Simple namespace shortcut. Using plain boost will need different namespaces
namespace _X
{
    using namespace std;
    using namespace std::placeholders;
}

/// No parameters
template <class R, class C, class T>
_X::function<R()> MakeDelegate(R (C::*func)(), T * self) {
    return _X::function<R()>(_X::bind(func, self));
}
template <class R, class C, class T>
_X::function<R()> MakeDelegate(R (C::*func)() const, T * self) {
    return _X::function<R()>(_X::bind(func, self));
}
/// 1 parameter
template <class R, class C, class Arg1, class T>
_X::function<R(Arg1)> MakeDelegate(R (C::*func)(Arg1), T * self) {
    return _X::function<R(Arg1)>(_X::bind(func, self, _X::_1));
}
template <class R, class C, class Arg1, class T>
_X::function<R(Arg1)> MakeDelegate(R (C::*func)(Arg1) const, T * self) {
    return _X::function<R(Arg1)>(_X::bind(func, self, _X::_1));
}
/// 2 parameters
template <class R, class C, class Arg1, class Arg2, class T>
_X::function<R(Arg1, Arg2)> MakeDelegate(R (C::*func)(Arg1, Arg2), T * self) {
    return _X::function<R(Arg1, Arg2)>(_X::bind(func, self, _X::_1, _X::_2));
}
template <class R, class C, class Arg1, class Arg2, class T>
_X::function<R(Arg1, Arg2)> MakeDelegate(R (C::*func)(Arg1, Arg2) const, T * self) {
    return _X::function<R(Arg1, Arg2)>(_X::bind(func, self, _X::_1, _X::_2));
}
/// 3 parameters
template <class R, class C, class Arg1, class Arg2, class Arg3, class T>
_X::function<R(Arg1, Arg2, Arg3)> MakeDelegate(R (C::*func)(Arg1, Arg2, Arg3), T * self) {
    return _X::function<R(Arg1, Arg2, Arg3)>(_X::bind(func, self, _X::_1, _X::_2, _X::_3));
}
template <class R, class C, class Arg1, class Arg2, class Arg3, class T>
_X::function<R(Arg1, Arg2, Arg3)> MakeDelegate(R (C::*func)(Arg1, Arg2, Arg3) const, T * self) {
    return _X::function<R(Arg1, Arg2, Arg3)>(_X::bind(func, self, _X::_1, _X::_2, _X::_3));
}
/// 4 parameters
template <class R, class C, class Arg1, class Arg2, class Arg3, class Arg4, class T>
_X::function<R(Arg1, Arg2, Arg3, Arg4)> MakeDelegate(R (C::*func)(Arg1, Arg2, Arg3, Arg4), T * self) {
    return _X::function<R(Arg1, Arg2, Arg3, Arg4)>(_X::bind(func, self, _X::_1, _X::_2, _X::_3, _X::_4));
}
template <class R, class C, class Arg1, class Arg2, class Arg3, class Arg4, class T>
_X::function<R(Arg1, Arg2, Arg3, Arg4)> MakeDelegate(R (C::*func)(Arg1, Arg2, Arg3, Arg4) const, T * self) {
    return _X::function<R(Arg1, Arg2, Arg3, Arg4)>(_X::bind(func, self, _X::_1, _X::_2, _X::_3, _X::_4));
}
/// 5 parameters
template <class R, class C, class Arg1, class Arg2, class Arg3, class Arg4, class Arg5, class T>
_X::function<R(Arg1, Arg2, Arg3, Arg4, Arg5)> MakeDelegate(R (C::*func)(Arg1, Arg2, Arg3, Arg4, Arg5), T * self) {
    return _X::function<R(Arg1, Arg2, Arg3, Arg4, Arg5)>(_X::bind(func, self, _X::_1, _X::_2, _X::_3, _X::_4, _X::_5));
}
template <class R, class C, class Arg1, class Arg2, class Arg3, class Arg4, class Arg5, class T>
_X::function<R(Arg1, Arg2, Arg3, Arg4, Arg5)> MakeDelegate(R (C::*func)(Arg1, Arg2, Arg3, Arg4, Arg5) const, T * self) {
    return _X::function<R(Arg1, Arg2, Arg3, Arg4, Arg5)>(_X::bind(func, self, _X::_1, _X::_2, _X::_3, _X::_4, _X::_5));
}
/// 6 parameters
template <class R, class C, class Arg1, class Arg2, class Arg3, class Arg4, class Arg5, class Arg6, class T>
_X::function<R(Arg1, Arg2, Arg3, Arg4, Arg5, Arg6)> MakeDelegate(R (C::*func)(Arg1, Arg2, Arg3, Arg4, Arg5, Arg6), T * self) {
    return _X::function<R(Arg1, Arg2, Arg3, Arg4, Arg5, Arg6)>(_X::bind(func, self, _X::_1, _X::_2, _X::_3, _X::_4, _X::_5, _X::_6));
}
template <class R, class C, class Arg1, class Arg2, class Arg3, class Arg4, class Arg5, class Arg6, class T>
_X::function<R(Arg1, Arg2, Arg3, Arg4, Arg5, Arg6)> MakeDelegate(R (C::*func)(Arg1, Arg2, Arg3, Arg4, Arg5, Arg6) const, T * self) {
    return _X::function<R(Arg1, Arg2, Arg3, Arg4, Arg5, Arg6)>(_X::bind(func, self, _X::_1, _X::_2, _X::_3, _X::_4, _X::_5, _X::_6));
}
/// 7 parameters
template <class R, class C, class Arg1, class Arg2, class Arg3, class Arg4, class Arg5, class Arg6, class Arg7, class T>
_X::function<R(Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7)> MakeDelegate(R (C::*func)(Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7), T * self) {
    return _X::function<R(Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7)>(_X::bind(func, self, _X::_1, _X::_2, _X::_3, _X::_4, _X::_5, _X::_6, _X::_7));
}
template <class R, class C, class Arg1, class Arg2, class Arg3, class Arg4, class Arg5, class Arg6, class Arg7, class T>
_X::function<R(Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7)> MakeDelegate(R (C::*func)(Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7) const, T * self) {
    return _X::function<R(Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7)>(_X::bind(func, self, _X::_1, _X::_2, _X::_3, _X::_4, _X::_5, _X::_6, _X::_7));
}
/// 8 parameters
template <class R, class C, class Arg1, class Arg2, class Arg3, class Arg4, class Arg5, class Arg6, class Arg7, class Arg8, class T>
_X::function<R(Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7, Arg8)> MakeDelegate(R (C::*func)(Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7, Arg8), T * self) {
    return _X::function<R(Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7, Arg8)>(_X::bind(func, self, _X::_1, _X::_2, _X::_3, _X::_4, _X::_5, _X::_6, _X::_7, _X::_8));
}
template <class R, class C, class Arg1, class Arg2, class Arg3, class Arg4, class Arg5, class Arg6, class Arg7, class Arg8, class T>
_X::function<R(Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7, Arg8)> MakeDelegate(R (C::*func)(Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7, Arg8) const, T * self) {
    return _X::function<R(Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7, Arg8)>(_X::bind(func, self, _X::_1, _X::_2, _X::_3, _X::_4, _X::_5, _X::_6, _X::_7, _X::_8));
}
/// 9 parameters
template <class R, class C, class Arg1, class Arg2, class Arg3, class Arg4, class Arg5, class Arg6, class Arg7, class Arg8, class Arg9, class T>
_X::function<R(Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7, Arg8, Arg9)> MakeDelegate(R (C::*func)(Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7, Arg8, Arg9), T * self) {
    return _X::function<R(Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7, Arg8, Arg9)>(_X::bind(func, self, _X::_1, _X::_2, _X::_3, _X::_4, _X::_5, _X::_6, _X::_7, _X::_8, _X::_9));
}
template <class R, class C, class Arg1, class Arg2, class Arg3, class Arg4, class Arg5, class Arg6, class Arg7, class Arg8, class Arg9, class T>
_X::function<R(Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7, Arg8, Arg9)> MakeDelegate(R (C::*func)(Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7, Arg8, Arg9) const, T * self) {
    return _X::function<R(Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7, Arg8, Arg9)>(_X::bind(func, self, _X::_1, _X::_2, _X::_3, _X::_4, _X::_5, _X::_6, _X::_7, _X::_8, _X::_9));
}
