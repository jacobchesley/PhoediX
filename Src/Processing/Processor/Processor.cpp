#include "Processor.h"

Processor::Processor() {
	img = new Image();
}

Processor::~Processor() {
	delete img;
}

Image * Processor::GetImage() {
	return img;
}