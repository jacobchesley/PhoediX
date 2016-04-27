#ifndef PROCESSOR_H
#define PROCESSOR_H

#include "Processing\Image\Image.h"
#include "Debugging\MemoryLeakCheck.h"

class Processor {
public:
	Processor();
	~Processor();

	Image * GetImage();

private:
	Image * img;
};

#endif 