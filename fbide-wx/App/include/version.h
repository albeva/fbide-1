#ifndef VERSION_H
#define VERSION_H

namespace AutoVersion{
	
	//Date Version Types
	static const char DATE[] = "18";
	static const char MONTH[] = "03";
	static const char YEAR[] = "2009";
	static const double UBUNTU_VERSION_STYLE = 9.03;
	
	//Software Status
	static const char STATUS[] = "Alpha";
	static const char STATUS_SHORT[] = "a";
	
	//Standard Version Type
	static const long MAJOR = 0;
	static const long MINOR = 9;
	static const long BUILD = 462;
	static const long REVISION = 2592;
	
	//Miscellaneous Version Types
	static const long BUILDS_COUNT = 3252;
	#define RC_FILEVERSION 0,9,462,2592
	#define RC_FILEVERSION_STRING "0, 9, 462, 2592\0"
	static const char FULLVERSION_STRING[] = "0.9.462.2592";
	
	//These values are to keep track of your versioning state, don't modify them.
	static const long BUILD_HISTORY = 64;
	

}
#endif //VERSION_H
