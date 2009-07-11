#ifndef VERSION_H
#define VERSION_H

namespace AutoVersion{
	
	//Date Version Types
	static const char DATE[] = "10";
	static const char MONTH[] = "07";
	static const char YEAR[] = "2009";
	static const double UBUNTU_VERSION_STYLE = 9.07;
	
	//Software Status
	static const char STATUS[] = "Alpha";
	static const char STATUS_SHORT[] = "a";
	
	//Standard Version Type
	static const long MAJOR = 0;
	static const long MINOR = 10;
	static const long BUILD = 507;
	static const long REVISION = 2850;
	
	//Miscellaneous Version Types
	static const long BUILDS_COUNT = 3740;
	#define RC_FILEVERSION 0,10,507,2850
	#define RC_FILEVERSION_STRING "0, 10, 507, 2850\0"
	static const char FULLVERSION_STRING[] = "0.10.507.2850";
	
	//These values are to keep track of your versioning state, don't modify them.
	static const long BUILD_HISTORY = 9;
	

}
#endif //VERSION_H
