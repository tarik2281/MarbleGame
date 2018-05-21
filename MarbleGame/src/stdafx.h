#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>

#ifdef _DEBUG   
#ifndef DBG_NEW
#define DBG_NEW new ( _NORMAL_BLOCK , __FILE__ , __LINE__ )
#endif
#else
#define DBG_NEW new
#endif  // _DEBUG


#ifndef SAFE_RELEASE
#define SAFE_RELEASE(p) if (p != nullptr) { \
	delete p;\
	p = nullptr;\
	}
#endif