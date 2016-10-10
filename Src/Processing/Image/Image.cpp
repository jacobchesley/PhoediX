#include "Image.h"
#include <string.h>

Image::Image() {

	bit16Enabled = false;

	imageDataRed8 = NULL;
	imageDataGreen8 = NULL;
	imageDataBlue8 = NULL;

	imageDataRed16 = NULL;
	imageDataGreen16 = NULL;
	imageDataBlue16 = NULL;
	width = 0;
	height = 0;
}

Image::Image(const Image& imageToCopy) {

	width = imageToCopy.width;
	height = imageToCopy.height;
	bit16Enabled = imageToCopy.bit16Enabled;

	int size = width * height;
	if(size < 1){
		return;
	}

	imageDataRed8 = NULL;
	imageDataGreen8 = NULL;
	imageDataBlue8 = NULL;

	imageDataRed16 = NULL;
	imageDataGreen16 = NULL;
	imageDataBlue16 = NULL;

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
		try{
			imageDataRed16 = new uint16_t[size];
			imageDataGreen16 = new uint16_t[size];
			imageDataBlue16 = new uint16_t[size];
		}
		catch(std::bad_alloc& badAllocation){
			errorMessage = "Image memory allocation failure: " + wxString(badAllocation.what());
			width = 0;
			height = 0;
			return;
		}

		// Go through each pixel containing red, green and blue, and copy 16 bit data
		uint16_t * redData = imageToCopy.imageDataRed16;
		uint16_t * greenData = imageToCopy.imageDataGreen16;
		uint16_t * blueData = imageToCopy.imageDataBlue16;

		if(redData == NULL || greenData == NULL || blueData == NULL){
			return;
		}

		for (int i = 0; i < size; i++) {
			imageDataRed16[i] = (uint16_t)(redData[i]);
			imageDataGreen16[i] = (uint16_t)(greenData[i]);
			imageDataBlue16[i] = (uint16_t)(blueData[i]);
		}
	}

	// Copy the 8 bit data
	else{
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
		try{
			imageDataRed8 = new uint8_t[size];
			imageDataGreen8 = new uint8_t[size];
			imageDataBlue8 = new uint8_t[size];
		}
		catch(std::bad_alloc& badAllocation){
			errorMessage = "Image memory allocation failure: " + wxString(badAllocation.what());
			width = 0;
			height = 0;
			return;
		}

		// Go through each pixel containing red, green and blue, and copy to 8 bit data
		uint8_t * redData = imageToCopy.imageDataRed8;
		uint8_t * greenData = imageToCopy.imageDataGreen8;
		uint8_t * blueData = imageToCopy.imageDataBlue8;

		if(redData == NULL || greenData == NULL || blueData == NULL){
			return;
		}

		for (int i = 0; i < size; i++) {
			imageDataRed8[i] = (uint8_t)(redData[i]);
			imageDataGreen8[i] = (uint8_t)(greenData[i]);
			imageDataBlue8[i] = (uint8_t)(blueData[i]);
		}
	}
}

Image::~Image() {

	this->Destroy();
}

void Image::Destroy() {

	if (width < 1 || height < 1) { return; }

	// Delete current 16 bit image data if it exists
	if(bit16Enabled){
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
	}

	// Delete current 8 bit image data if it exists
	else{
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
	}

	width = 0;
	height = 0;
}

void Image::SetDataFrom8(uint8_t * inData, int inWidth, int inHeight) {

	width = inWidth;
	height = inHeight;
	int size = width * height * 3;

	if(width == 0 || height == 0){
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
		}
		else{
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
		}
		return;
	}


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
		try{
			imageDataRed16 = new uint16_t[size / 3];
			imageDataGreen16 = new uint16_t[size / 3];
			imageDataBlue16 = new uint16_t[size / 3];
		}
		catch(std::bad_alloc& badAllocation){
			errorMessage = "Image memory allocation failure: " + wxString(badAllocation.what());
			width = 0;
			height = 0;
			return;
		}

		// Go through each pixel containing red, green and blue, and scale to 16 bit data
		int index = 0;
		for (int i = 0; i < size; i += 3) {
			imageDataRed16[index] = (uint16_t)(inData[i] * 256.0);
			imageDataGreen16[index] = (uint16_t)(inData[i + 1] * 256.0);
			imageDataBlue16[index] = (uint16_t)(inData[i + 2] * 256.0);
			index += 1;
		}
	}

	// Copy the 8 bit data
	else{
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
		try{
			imageDataRed8 = new uint8_t[size / 3];
			imageDataGreen8 = new uint8_t[size / 3];
			imageDataBlue8 = new uint8_t[size / 3];
		}
		catch(std::bad_alloc& badAllocation){
			errorMessage = "Image memory allocation failure: " + wxString(badAllocation.what());
			width = 0;
			height = 0;
			return;
		}

		// Go through each pixel containing red, green and blue, and copy to 8 bit data
		int index = 0;
		for (int i = 0; i < size; i += 3) {
			imageDataRed8[index] = inData[i];
			imageDataGreen8[index] = inData[i + 1];
			imageDataBlue8[index] = inData[i + 2];
			index += 1;
		}
	}
}

void Image::SetDataFrom16(uint16_t * inData, int inWidth, int inHeight) {

	width = inWidth;
	height = inHeight;
	int size = width * height * 3;

	if(width == 0 || height == 0){
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
		}
		else{
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
		}
	}

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
		try{
			imageDataRed16 = new uint16_t[size / 3];
			imageDataGreen16 = new uint16_t[size / 3];
			imageDataBlue16 = new uint16_t[size / 3];
		}
		catch(std::bad_alloc& badAllocation){
			errorMessage = "Image memory allocation failure: " + wxString(badAllocation.what());
			width = 0;
			height = 0;
			return;
		}

		// Go through each pixel containing red, green and blue, and copy to 16 bit data
		int index = 0;
		for (int i = 0; i < size; i += 3) {
			imageDataRed16[index] = inData[i];
			imageDataGreen16[index] = inData[i + 1];
			imageDataBlue16[index] = inData[i + 2];
			index += 1;
		}
	}

	// Set the 8 bit data
	else{

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
		try{
			imageDataRed8 = new uint8_t[size / 3];
			imageDataGreen8 = new uint8_t[size / 3];
			imageDataBlue8 = new uint8_t[size / 3];
		}
		catch(std::bad_alloc& badAllocation){
			errorMessage = "Image memory allocation failure: " + wxString(badAllocation.what());
			width = 0;
			height = 0;
			return;
		}

		// Go through each pixel containing red, green and blue, and scale to 8 bit data
		int index = 0;
		for (int i = 0; i < size; i += 3) {
			imageDataRed8[index] = (uint8_t) (inData[i] / 256);
			imageDataGreen8[index] = (uint8_t) (inData[i + 1] / 256);
			imageDataBlue8[index] = (uint8_t) (inData[i + 2] / 256);
			index += 1;
		}
	}
}

void Image::SetDataFrom16(uint8_t * inData, int inWidth, int inHeight) {

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
		try{
			imageDataRed16 = new uint16_t[size / 3];
			imageDataGreen16 = new uint16_t[size / 3];
			imageDataBlue16 = new uint16_t[size / 3];
		}
		catch(std::bad_alloc& badAllocation){
			errorMessage = "Image memory allocation failure: " + wxString(badAllocation.what());
			width = 0;
			height = 0;
			return;
		}

		// Go through each pixel containing red, green and blue, and copy to 16 bit data.  2 bytes per pixel color.
		int index = 0;
		for (int i = 0; i < size * 2; i += 6) {
			imageDataRed16[index] = inData[i] << 8 |  inData[i+1];
			imageDataGreen16[index] = inData[i+2] << 8 |  inData[i+3];;
			imageDataBlue16[index] = inData[i+4] << 8 |  inData[i+5];;
			index += 1;
		}
	}

	// Set the 8 bit data
	else{
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
		try{
			imageDataRed8 = new uint8_t[size / 3];
			imageDataGreen8 = new uint8_t[size / 3];
			imageDataBlue8 = new uint8_t[size / 3];
		}
		catch(std::bad_alloc& badAllocation){
			errorMessage = "Image memory allocation failure: " + wxString(badAllocation.what());
			width = 0;
			height = 0;
			return;
		}

		// Go through each pixel containing red, green and blue, and scale to 8 bit data
		int index = 0;
		for (int i = 0; i < size * 2; i += 6) {
			imageDataRed8[index] = (uint8_t) ((inData[i] << 8 |  inData[i+1]) / 256);
			imageDataGreen8[index] = (uint8_t) ((inData[i+2] << 8 |  inData[i+3]) / 256);
			imageDataBlue8[index] = (uint8_t) ((inData[i+4] << 8 |  inData[i+5]) / 256);
			index += 1;
		}
	}
	
}

void Image::SetWidth(int newWidth) {
	width = newWidth;
}

void Image::SetHeight(int newHeight) {
	height = newHeight;
}

void Image::InitImage() {

	int size = width * height * 3;

	// Init 16 bit data
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
		try{
			imageDataRed16 = new uint16_t[size / 3];
			imageDataGreen16 = new uint16_t[size / 3];
			imageDataBlue16 = new uint16_t[size / 3];
		}
		catch(std::bad_alloc& badAllocation){
			errorMessage = "Image memory allocation failure: " + wxString(badAllocation.what());
			width = 0;
			height = 0;
			return;
		}

		// Go through each pixel containing red, green and blue, and init to 0
		int index = 0;
		for (int i = 0; i < size; i += 3) {
			imageDataRed16[index] = 0;
			imageDataGreen16[index] = 0;
			imageDataBlue16[index] = 0;
			index += 1;
		}
	}

	// Init 8 bit data
	else{
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
		try{
			imageDataRed8 = new uint8_t[size / 3];
			imageDataGreen8 = new uint8_t[size / 3];
			imageDataBlue8 = new uint8_t[size / 3];
		}
		catch(std::bad_alloc& badAllocation){
			errorMessage = "Image memory allocation failure: " + wxString(badAllocation.what());
			width = 0;
			height = 0;
			return;
		}

		// Go through each pixel containing red, green and blue, and init to 0
		int index = 0;
		for (int i = 0; i < size; i += 3) {
			imageDataRed8[index] = 0;
			imageDataGreen8[index] = 0;
			imageDataBlue8[index] = 0;
			index += 1;
		}
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

wxString Image::GetErrorStr() {
	return errorMessage;
}

void Image::Enable16Bit() {
	bit16Enabled = true;

	int size = width * height;;

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
	try{
		imageDataRed16 = new uint16_t[size];
		imageDataGreen16 = new uint16_t[size];
		imageDataBlue16 = new uint16_t[size];
	}
	catch(std::bad_alloc& badAllocation){
			errorMessage = "Image memory allocation failure: " + wxString(badAllocation.what());
			width = 0;
			height = 0;
			return;
		}

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
}

void Image::Disable16Bit() {
	bit16Enabled = false;

	int size = width * height;;

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
	try{
		imageDataRed8 = new uint8_t[size];
		imageDataGreen8 = new uint8_t[size];
		imageDataBlue8 = new uint8_t[size];
	}
	catch(std::bad_alloc& badAllocation){
			errorMessage = "Image memory allocation failure: " + wxString(badAllocation.what());
			width = 0;
			height = 0;
			return;
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
}