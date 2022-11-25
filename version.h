#ifndef VERSION_H
#define VERSION_H

	//Date Version Types
	static const char DATE[] = "25";
	static const char MONTH[] = "11";
	static const char YEAR[] = "2022";
	static const char UBUNTU_VERSION_STYLE[] =  "22.11";
	
	//Software Status
	static const char STATUS[] =  "Alpha";
	static const char STATUS_SHORT[] =  "a";
	
	//Standard Version Type
	static const long MAJOR  = 0;
	static const long MINOR  = 7;
	static const long BUILD  = 0;
	static const long REVISION  = 0;
	
	//Miscellaneous Version Types
	static const long BUILDS_COUNT  = 1;
	#define RC_FILEVERSION 0,7,0,0
	#define RC_FILEVERSION_STRING "0, 7, 0, 0\0"
	static const char FULLVERSION_STRING [] = "0.7.0.0";
	
	//These values are to keep track of your versioning state, don't modify them.
	static const long BUILD_HISTORY  = 0;
	

#endif //VERSION_H