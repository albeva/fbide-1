// tests.cpp : Defines the entry point for the console application.
//

// #include "stdafx.h"
#include <iostream>
#include <map>
using namespace std;

struct Right {};
template<typename T1, typename T2, class N1 = Right> class BiMap
{
    public:

    private:
        std::map<T1, T2> m_left;
        std::map<T2, T1> m_right;
};


int main(int argc, char * argv[])
{
    
    typedef BiMap<int, int> IntBridge;
    // IntBridge::

	return 0;
}

