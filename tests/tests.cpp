// tests.cpp : Defines the entry point for the console application.
//

// #include "stdafx.h"
#include <iostream>

//#include "make_delegate.h"
//#include "multi_delegate.h"
#include "boost/function.hpp"
#include "boost/signals2.hpp"



using namespace std;

int main(int argc, char * argv[])
{
    auto fn = [](void) { cout << "hi\n"; };
    boost::signals2::signal<void ()> sig;
    sig.connect(fn);
    sig();


}

