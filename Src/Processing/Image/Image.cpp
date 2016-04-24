#include "Image.h"

Image::Image() {

	bit16Enabled = true;

	imageDataRed8 = NULL;
	imageDataGreen8 = NULL;
	imageDataBlue8 = NULL;

	imageDataRed16 = NULL;
	imageDataGreen16 = NULL;
	imageDataBlue16 = NULL;
}

Image::~Image() {

	this->Destroy();
}

void Image::Destroy() {

	if (imageDataRed8 != NULL) {
		delete[] imageDataRed8;
		imageDataRed8 = NULL;
	}

	if (imageDataGreen8 != NULL) {
		delete[] imageDataGreen8;
		imageDataGreen8 = NULL;
	}

	if (imageDataBlue8 != NULL) {
		delete[] imageDataBlue8;
		imageDataBlue8 = NULL;
	}

	// Delete current 16 bit image data if it exists
	if (imageDataRed16 != NULL) {
		delete[] imageDataRed16;
		imageDataRed16 = NULL;
	}

	if (imageDataGreen16 != NULL) {
		delete[] imageDataGreen16;
		imageDataGreen16 = NULL;
	}

	if (imageDataBlue16 != NULL) {
		delete[] imageDataBlue16;
		imageDataBlue16 = NULL;
	}

	width = 0;
	height = 0;
}
void Image::SetDataFrom8(unsigned char * inData, int inWidth, int inHeight) {

	width = inWidth;
	height = inHeight;
	int size = width * height * 3;

	// Convert the 8 bit data into 16 bit data
	if (bit16Enabled) {

		// Delete current 16 bit image data if it exists
		if (imageDataRed16 != NULL) {
			delete[] imageDataRed16;
			imageDataRed16 = NULL;
		}

		if (imageDataGreen16 != NULL) {
			delete[] imageDataGreen16;
			imageDataGreen16 = NULL;
		}

		if (imageDataBlue16 != NULL) {
			delete[] imageDataBlue16;
			imageDataBlue16 = NULL;
		}

		// Create new image data
		imageDataRed16 = new uint16_t[size / 3];
		imageDataGreen16 = new uint16_t[size / 3];
		imageDataBlue16 = new uint16_t[size / 3];

		// Go through each pixel containing red, green and blue, and scale to 16 bit data
		int index = 0;
		for (int i = 0; i < size; i += 3) {
			imageDataRed16[index] = inData[i] * 256;
			imageDataGreen16[index] = inData[i + 1] * 256;
			imageDataBlue16[index] = inData[i + 2] * 256;
			index += 1;
		}
	}

	// Always need to have 8 bit data available for display

	// Copy the 8 bit data

	// Delete current 16 bit image data if it exists
	if (imageDataRed8 != NULL) {
		delete[] imageDataRed8;
		imageDataRed8 = NULL;
	}

	if (imageDataGreen8 != NULL) {
		delete[] imageDataGreen8;
		imageDataGreen8 = NULL;
	}

	if (imageDataBlue8 != NULL) {
		delete[] imageDataBlue8;
		imageDataBlue8 = NULL;
	}

	// Create new image data
	imageDataRed8 = new uint8_t[size / 3];
	imageDataGreen8 = new uint8_t[size / 3];
	imageDataBlue8 = new uint8_t[size / 3];

	// Go through each pixel containing red, green and blue, and scale to 16 bit data
	int index = 0;
	for (int i = 0; i < size; i += 3) {
		imageDataRed8[index] = inData[i];
		imageDataGreen8[index] = inData[i + 1];
		imageDataBlue8[index] = inData[i + 2];
		index += 1;
	}
}

void Image::SetDataFrom16(int16_t * inData, int inWidth, int inHeight) {

	width = inWidth;
	height = inHeight;
	int size = width * height * 3;

	// Copy the 16 bit data
	if (bit16Enabled) {

		// Delete current 16 bit image data if it exists
		if (imageDataRed16 != NULL) {
			delete[] imageDataRed16;
			imageDataRed16 = NULL;
		}

		if (imageDataGreen16 != NULL) {
			delete[] imageDataGreen16;
			imageDataGreen16 = NULL;
		}

		if (imageDataBlue16 != NULL) {
			delete[] imageDataBlue16;
			imageDataBlue16 = NULL;
		}

		// Create new image data
		imageDataRed16 = new uint16_t[size / 3];
		imageDataGreen16 = new uint16_t[size / 3];
		imageDataBlue16 = new uint16_t[size / 3];

		// Go through each pixel containing red, green and blue, and scale to 16 bit data
		int index = 0;
		for (int i = 0; i < size; i += 3) {
			imageDataRed16[index] = inData[i];
			imageDataGreen16[index] = inData[i + 1];
			imageDataBlue16[index] = inData[i + 2];
			index += 1;
		}
	}

	// Always need to have 8 bit data available for display

	// Delete current 8 bit image data if it exists
	if (imageDataRed8 != NULL) {
		delete[] imageDataRed8;
		imageDataRed8 = NULL;
	}

	if (imageDataGreen8 != NULL) {
		delete[] imageDataGreen8;
		imageDataGreen8 = NULL;
	}

	if (imageDataBlue8 != NULL) {
		delete[] imageDataBlue8;
		imageDataBlue8 = NULL;
	}

	// Create new image data
	imageDataRed8 = new uint8_t[size / 3];
	imageDataGreen8 = new uint8_t[size / 3];
	imageDataBlue8 = new uint8_t[size / 3];

	// Go through each pixel containing red, green and blue, and scale to 16 bit data
	int index = 0;
	for (int i = 0; i < size; i += 3) {
		imageDataRed8[index] = (int8_t) (inData[i] / 256);
		imageDataGreen8[index] = (int8_t) (inData[i + 1] / 256);
		imageDataBlue8[index] = (int8_t) (inData[i + 2] / 256);
		index += 1;
	}
	
}

int Image::GetWidth() {
	return width;
}

int Image::GetHeight() {
	return height;
}

uint8_t * Image::Get8BitDataRed() {
	return imageDataRed8;
}

uint8_t * Image::Get8BitDataGreen() {
	return imageDataGreen8;
}

uint8_t * Image::Get8BitDataBlue() {
	return imageDataBlue8;
}

uint16_t * Image::Get16BitDataRed() {
	return imageDataRed16;
}

uint16_t * Image::Get16BitDataGreen() {
	return imageDataGreen16;
}

uint16_t * Image::Get16BitDataBlue() {
	return imageDataBlue16;
}

int Image::GetColorDepth() {
	return bit16Enabled ? 16 : 8;
}

void Image::Enable16Bit() {
	bit16Enabled = true;
}

void Image::Disable16Bit() {
	bit16Enabled = false;
}