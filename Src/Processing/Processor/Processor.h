#ifndef PROCESSOR_H
#define PROCESSOR_H

#include "Processing\Image\Image.h"

#ifdef CHECK_MEMORY_LEAK
#define CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>

#ifdef _DEBUG
#ifndef DBG_NEW
#define DBG_NEW new ( _NORMAL_BLOCK , __FILE__ , __LINE__ )
#define new DBG_NEW
#endif
#endif  // _DEBUG
#endif

class Processor {
public:
	Processor();
	~Processor();

	Image * GetImage();

private:
	Image * img;
};

#endif 