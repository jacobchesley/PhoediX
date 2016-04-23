#ifndef PROCESSOR_H
#define PROCESSOR_H

#include "Processing\Image\Image.h"

class Processor {
public:
	Processor();

	Image * GetImage();

private:
	Image * img;
};

#endif 