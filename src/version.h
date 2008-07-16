#ifndef VERSION_H
#define VERSION_H

namespace AutoVersion{
	
	//Date Version Types
	static const char DATE[] = "16";
	static const char MONTH[] = "07";
	static const char YEAR[] = "2008";
	static const double UBUNTU_VERSION_STYLE = 8.07;
	
	//Software Status
	static const char STATUS[] = "Release";
	static const char STATUS_SHORT[] = "r";
	
	//Standard Version Type
	static const long MAJOR = 1;
	static const long MINOR = 3;
	static const long BUILD = 1;
	static const long REVISION = 2;
	
	//Miscellaneous Version Types
	static const long BUILDS_COUNT = 8;
	#define RC_FILEVERSION 1,3,1,2
	#define RC_FILEVERSION_STRING "1, 3, 1, 2\0"
	static const char FULLVERSION_STRING[] = "1.3.1.2";
	
	//These values are to keep track of your versioning state, don't modify them.
	static const long BUILD_HISTORY = 1;
	

}
#endif //VERSION_H
