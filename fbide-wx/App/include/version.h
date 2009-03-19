#ifndef VERSION_H
#define VERSION_H

namespace AutoVersion{
	
	//Date Version Types
	static const char DATE[] = "19";
	static const char MONTH[] = "03";
	static const char YEAR[] = "2009";
	static const double UBUNTU_VERSION_STYLE = 9.03;
	
	//Software Status
	static const char STATUS[] = "Alpha";
	static const char STATUS_SHORT[] = "a";
	
	//Standard Version Type
	static const long MAJOR = 0;
	static const long MINOR = 9;
	static const long BUILD = 484;
	static const long REVISION = 2719;
	
	//Miscellaneous Version Types
	static const long BUILDS_COUNT = 3447;
	#define RC_FILEVERSION 0,9,484,2719
	#define RC_FILEVERSION_STRING "0, 9, 484, 2719\0"
	static const char FULLVERSION_STRING[] = "0.9.484.2719";
	
	//These values are to keep track of your versioning state, don't modify them.
	static const long BUILD_HISTORY = 86;
	

}
#endif //VERSION_H
