#include "Processor.h"

Processor::Processor() {
	img = new Image();
}

Image * Processor::GetImage() {
	return img;
}