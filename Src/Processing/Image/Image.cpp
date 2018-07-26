// Copyright 2016 Jacob Chesley

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
	errorMessage = "";
	exifMap = std::map<size_t, void*>();
}

Image::Image(const Image& imageToCopy) {

	uint32_t origWidth = width;
	uint32_t origHeight = height;

	width = imageToCopy.width;
	height = imageToCopy.height;
	bit16Enabled = imageToCopy.bit16Enabled;
	errorMessage = "";
	exifMap = imageToCopy.exifMap;
	
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
		if (origWidth > 0 && origHeight > 0){
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
		if (origWidth > 0 && origHeight > 0) {
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

	this->Destroy();
	width = inWidth;
	height = inHeight;
	uint64_t size = width * height * 3;
	errorMessage = "";

	// Convert the 8 bit data into 16 bit data
	if (bit16Enabled) {

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

	this->Destroy();
	width = inWidth;
	height = inHeight;
	int size = width * height * 3;
	errorMessage = "";


	// Copy the 16 bit data
	if (bit16Enabled) {

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

	this->Destroy();
	width = inWidth;
	height = inHeight;
	int size = width * height * 3;

	// Copy the 16 bit data
	if (bit16Enabled) {

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
		// Flip bytes
		int index = 0;
		for (int i = 0; i < size * 2; i += 6) {
			imageDataRed16[index] = inData[i + 1] << 8 |  inData[i];
			imageDataGreen16[index] = inData[i + 3] << 8 |  inData[i + 2];
			imageDataBlue16[index] = inData[i + 5] << 8 |  inData[i + 4];
			index += 1;
		}
	}

	// Set the 8 bit data
	else{
	
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
	errorMessage = "";
	if (size < 1) { return; }
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

	int size = width * height;
	errorMessage = "";
	if (size < 1) { return; }


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

	int size = width * height;
	errorMessage = "";

	if (size < 1) { return; }

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

std::map<size_t, void*> * Image::GetExifMap() {
	return &exifMap;
}

std::map<size_t, wxString> Image::exifTags = std::map<size_t, wxString>();
std::map<size_t, int> Image::exifFormats = std::map<size_t, int>();

void Image::InitExif(){

	// Tags used by IFD0 (main image)
	exifTags[0x010E] = "ImageDescription"; exifFormats[0x010E] = Image::ExifType::ASCII;
	exifTags[0x010F] = "Make"; exifFormats[0x010F] = Image::ExifType::ASCII;
	exifTags[0x0110] = "Model"; exifFormats[0x0110] = Image::ExifType::ASCII;
	exifTags[0x0112] = "Orientation"; exifFormats[0x0112] = Image::ExifType::U_SHORT;
	exifTags[0x011A] = "XResolution"; exifFormats[0x011A] = Image::ExifType::U_RATIONAL;
	exifTags[0x011B] = "YResolution"; exifFormats[0x011B] = Image::ExifType::U_RATIONAL;
	exifTags[0x0128] = "ResolutionUnit"; exifFormats[0x0128] = Image::ExifType::U_SHORT;
	exifTags[0x0131] = "Software"; exifFormats[0x0131] = Image::ExifType::ASCII;
	exifTags[0x0132] = "DateTime"; exifFormats[0x0132] = Image::ExifType::ASCII;
	exifTags[0x013E] = "WhitePoint"; exifFormats[0x013E] = Image::ExifType::U_RATIONAL;
	exifTags[0x013F] = "PrimaryChromaticities"; exifFormats[0x013F] = Image::ExifType::U_RATIONAL;
	exifTags[0x0211] = "YCbCrCoefficients"; exifFormats[0x0211] = Image::ExifType::U_RATIONAL;
	exifTags[0x0213] = "YCbCrPositioning"; exifFormats[0x0213] = Image::ExifType::U_SHORT;
	exifTags[0x0214] = "ReferenceBlackWhite"; exifFormats[0x0214] = Image::ExifType::U_RATIONAL;
	exifTags[0x8298] = "Copyright"; exifFormats[0x8298] = Image::ExifType::ASCII;
	exifTags[0x8769] = "ExifOffset"; exifFormats[0x8769] = Image::ExifType::U_LONG;

	// Tags used by Exif SubIFD
	exifTags[0x829A] = "ExposureTime"; exifFormats[0x829A] = Image::ExifType::U_RATIONAL;
	exifTags[0x829D] = "FNumber"; exifFormats[0x829D] = Image::ExifType::U_RATIONAL;
	exifTags[0x8822] = "ExposureProgram"; exifFormats[0x8822] = Image::ExifType::U_SHORT;
	exifTags[0x8827] = "ISOSpeedRatings"; exifFormats[0x8827] = Image::ExifType::U_SHORT;
	exifTags[0x9000] = "ExifVersion"; exifFormats[0x9000] = Image::ExifType::UNDEFINED;
	exifTags[0x9003] = "DateTimeOriginal"; exifFormats[0x9003] = Image::ExifType::ASCII;
	exifTags[0x9004] = "DateTimeDigitized"; exifFormats[0x9004] = Image::ExifType::ASCII;
	exifTags[0x9101] = "ComponentConfiguration"; exifFormats[0x9101] = Image::ExifType::UNDEFINED;
	exifTags[0x9102] = "CompressedBitsPerPixel"; exifFormats[0x9102] = Image::ExifType::U_RATIONAL;
	exifTags[0x9201] = "ShutterSpeedValue"; exifFormats[0x9201] = Image::ExifType::S_RATIONAL;
	exifTags[0x9202] = "AperatureValue"; exifFormats[0x9202] = Image::ExifType::U_RATIONAL;
	exifTags[0x9203] = "BrightnessValue"; exifFormats[0x9203] = Image::ExifType::S_RATIONAL;
	exifTags[0x9204] = "ExposureBiasValue"; exifFormats[0x9204] = Image::ExifType::S_RATIONAL;
	exifTags[0x9205] = "MaxApertureValue"; exifFormats[0x9205] = Image::ExifType::U_RATIONAL;
	exifTags[0x9206] = "SubjectDistance"; exifFormats[0x9206] = Image::ExifType::S_RATIONAL;
	exifTags[0x9207] = "MeteringMode"; exifFormats[0x9207] = Image::ExifType::U_SHORT;
	exifTags[0x9208] = "LightSource"; exifFormats[0x9208] = Image::ExifType::U_SHORT;
	exifTags[0x9209] = "Flash"; exifFormats[0x9209] = Image::ExifType::U_SHORT;
	exifTags[0x920A] = "FocalLength"; exifFormats[0x920A] = Image::ExifType::U_RATIONAL;
	exifTags[0x927C] = "MakerNote"; exifFormats[0x927C] = Image::ExifType::UNDEFINED;
	exifTags[0x9286] = "UserComment"; exifFormats[0x9286] = Image::ExifType::UNDEFINED;
	exifTags[0xA000] = "FlashPixVersion"; exifFormats[0XA000] = Image::ExifType::UNDEFINED;
	exifTags[0xA001] = "ColorSpace"; exifFormats[0xA001] = Image::ExifType::U_SHORT;
	exifTags[0xA002] = "ExifImageWidth"; exifFormats[0xA002] = Image::ExifType::U_LONG;
	exifTags[0xA003] = "ExifImageHeight";  exifFormats[0xA003] = Image::ExifType::U_LONG;
	exifTags[0xA004] = "RelatedSoundFile"; exifFormats[0xA004] = Image::ExifType::ASCII;
	exifTags[0xA005] = "ExifInteroperabilityOffset"; exifFormats[0xA005] = Image::ExifType::U_LONG;
	exifTags[0xA20E] = "FocalPlaneXResolution"; exifFormats[0xA20E] = Image::ExifType::U_RATIONAL;
	exifTags[0xA20F] = "FocalPlaneYResolution"; exifFormats[0xA20F] = Image::ExifType::U_RATIONAL;
	exifTags[0xA210] = "FocalPlaneResolutionUnit"; exifFormats[0xA210] = Image::ExifType::U_SHORT;
	exifTags[0xA217] = "SensingMethod"; exifFormats[0xA217] = Image::ExifType::U_SHORT;
	exifTags[0xA300] = "FileSource"; exifFormats[0xA300] = Image::ExifType::UNDEFINED;
	exifTags[0xA301] = "SceneType"; exifFormats[0xA301] = Image::ExifType::UNDEFINED;

	// Tags used by IFD1 (thumbnail image)
	exifTags[0x0100] = "ImageWidth"; exifFormats[0x0100] = Image::ExifType::U_LONG;
	exifTags[0x0101] = "ImageLength"; exifFormats[0x0101] = Image::ExifType::U_LONG;
	exifTags[0x0102] = "BitsPerSample"; exifFormats[0x0102] = Image::ExifType::U_SHORT;
	exifTags[0x0103] = "Compression"; exifFormats[0x0103] = Image::ExifType::U_SHORT;
	exifTags[0x0106] = "PhotometricInterpretation"; exifFormats[0x0106] = Image::ExifType::U_SHORT;
	exifTags[0x0111] = "StripOffset";  exifFormats[0x0111] = Image::ExifType::U_LONG;
	exifTags[0x0115] = "SamplesPerPixel"; exifFormats[0x0115] = Image::ExifType::U_SHORT;
	exifTags[0x0116] = "RowsPerStrip"; exifFormats[0x0116] = Image::ExifType::U_LONG;
	exifTags[0x0117] = "StripByteCounts"; exifFormats[0x0117] = Image::ExifType::U_LONG;
	exifTags[0x011A] = "XResolution"; exifFormats[0x011A] = Image::ExifType::U_RATIONAL;
	exifTags[0x011B] = "YResolution"; exifFormats[0x011B] = Image::ExifType::U_RATIONAL;
	exifTags[0x011C] = "PlanarConfiguration"; exifFormats[0x011C] = Image::ExifType::U_SHORT;
	exifTags[0x0128] = "ResolutionUnit"; exifFormats[0x0128] = Image::ExifType::U_SHORT;
	exifTags[0x0201] = "JpegIFOffset"; exifFormats[0x0201] = Image::ExifType::U_LONG;
	exifTags[0x0202] = "JpegIFByteCount"; exifFormats[0x0202] = Image::ExifType::U_LONG;
	exifTags[0x0211] = "YCbCrCoefficients"; exifFormats[0x0203] = Image::ExifType::U_RATIONAL;
	exifTags[0x0212] = "YCbCrSubSampling"; exifFormats[0x0212] = Image::ExifType::U_SHORT;
	exifTags[0x0213] = "YCbCrPositioning"; exifFormats[0x0213] = Image::ExifType::U_SHORT;
	exifTags[0x0214] = "ReferenceBlackWhite"; exifFormats[0x0214] = Image::ExifType::U_RATIONAL;

	// Misc Tags
	exifTags[0x00FE] = "NewSubfileType"; exifFormats[0x00FE] = Image::ExifType::U_LONG;
	exifTags[0x00FF] = "SubfileType"; exifFormats[0x00FF] = Image::ExifType::U_SHORT;
	exifTags[0x012D] = "TransferFunction"; exifFormats[0x012D] = Image::ExifType::U_SHORT;
	exifTags[0x013B] = "Artist"; exifFormats[0x013B] = Image::ExifType::ASCII;
	exifTags[0x013D] = "Predictor"; exifFormats[0x013D] = Image::ExifType::U_SHORT;
	exifTags[0x0142] = "TileWidth"; exifFormats[0x0142] = Image::ExifType::U_SHORT;
	exifTags[0x0143] = "TileLength"; exifFormats[0x0143] = Image::ExifType::U_SHORT;
	exifTags[0x0144] = "TileOffsets"; exifFormats[0x0144] = Image::ExifType::U_LONG;
	exifTags[0x0145] = "TileByteCounts"; exifFormats[0x0145] = Image::ExifType::U_SHORT;
	exifTags[0x014A] = "SubIFDs"; exifFormats[0x014A] = Image::ExifType::U_LONG;
	exifTags[0x015B] = "JPEGTables"; exifFormats[0x015B] = Image::ExifType::UNDEFINED;
	exifTags[0x828D] = "CFARepeatPatternDim"; exifFormats[0x828D] = Image::ExifType::U_SHORT;
	exifTags[0x828E] = "CFAPattern"; exifFormats[0x828E] = Image::ExifType::U_BYTE;
	exifTags[0x828F] = "BatteryLevel"; exifFormats[0x828F] = Image::ExifType::U_RATIONAL;
	exifTags[0x83BB] = "IPTC/NAA"; exifFormats[0x83BB] = Image::ExifType::U_LONG;
	exifTags[0x8773] = "InterColorProfile"; exifFormats[0x8773] = Image::ExifType::UNDEFINED;
	exifTags[0x8824] = "SpectralSensitivity"; exifFormats[0x8824] = Image::ExifType::ASCII;
	exifTags[0x8825] = "GPSInfo"; exifFormats[0x8825] = Image::ExifType::U_LONG;
	exifTags[0x8828] = "OECF"; exifFormats[0x8828] = Image::ExifType::UNDEFINED;
	exifTags[0x8829] = "Interlace"; exifFormats[0x8829] = Image::ExifType::U_SHORT;
	exifTags[0x882A] = "TimeZoneOffset"; exifFormats[0x882A] = Image::ExifType::S_SHORT;
	exifTags[0x882B] = "SelfTimerMode"; exifFormats[0x882B] = Image::ExifType::U_SHORT;
	exifTags[0x920B] = "FlashEnergy"; exifFormats[0x920B] = Image::ExifType::U_RATIONAL;
	exifTags[0x920C] = "SpatialFrequencyResponse"; exifFormats[0x920C] = Image::ExifType::UNDEFINED;
	exifTags[0x920D] = "Noise"; exifFormats[0x920D] = Image::ExifType::UNDEFINED;
	exifTags[0x9211] = "ImageNumber"; exifFormats[0x9211] = Image::ExifType::U_LONG;
	exifTags[0x9212] = "SecurityClassification"; exifFormats[0x9212] = Image::ExifType::ASCII;
	exifTags[0x9213] = "ImageHistory"; exifFormats[0x9213] = Image::ExifType::ASCII;
	exifTags[0x9214] = "SubjectLocation"; exifFormats[0x9214] = Image::ExifType::U_SHORT;
	exifTags[0x9215] = "ExposureIndex"; exifFormats[0x9215] = Image::ExifType::U_RATIONAL;
	exifTags[0x9216] = "TIFF/EPStandardID"; exifFormats[0x9216] = Image::ExifType::U_BYTE;
	exifTags[0x9290] = "SubSecTime"; exifFormats[0x9290] = Image::ExifType::ASCII;
	exifTags[0x9291] = "SubSecTimeOriginal"; exifFormats[0x9291] = Image::ExifType::ASCII;
	exifTags[0x9292] = "SubSecTimeDigitized"; exifFormats[0x9292] = Image::ExifType::ASCII;
	exifTags[0xA20B] = "FlashEnergy"; exifFormats[0xA20B] = Image::ExifType::U_RATIONAL;
	exifTags[0xA20C] = "SpatialFrequencyResponse"; exifFormats[0xA20C] = Image::ExifType::U_SHORT;
	exifTags[0xA214] = "SubjectLocation"; exifFormats[0xA214] = Image::ExifType::U_SHORT;
	exifTags[0xA215] = "ExposureIndex"; exifFormats[0xA215] = Image::ExifType::U_RATIONAL;
	exifTags[0xA302] = "CFAPattern"; exifFormats[0xA302] = Image::ExifType::UNDEFINED;

	// GPS Info
	exifTags[0x0000] = "GPSVersionID"; exifFormats[0x0000] = Image::ExifType::U_BYTE;
	exifTags[0x0001] = "GPSLatitudeRef"; exifFormats[0x0001] = Image::ExifType::ASCII;
	exifTags[0x0002] = "GPSLatitude"; exifFormats[0x0002] = Image::ExifType::U_RATIONAL;
	exifTags[0x0003] = "GPSLongitudeRef"; exifFormats[0x0003] = Image::ExifType::ASCII;
	exifTags[0x0004] = "GPSLongitude"; exifFormats[0x0004] = Image::ExifType::U_RATIONAL;
	exifTags[0x0005] = "GPSAltitudeRef"; exifFormats[0x0005] = Image::ExifType::U_BYTE;
	exifTags[0x0006] = "GPSAltitude"; exifFormats[0x0006] = Image::ExifType::U_RATIONAL;
	exifTags[0x0007] = "GPSTimeStamp"; exifFormats[0x0007] = Image::ExifType::U_RATIONAL;
	exifTags[0x0008] = "GPSSatellites"; exifFormats[0x0008] = Image::ExifType::ASCII;
	exifTags[0x0009] = "GPSStatus"; exifFormats[0x0009] = Image::ExifType::ASCII;
	exifTags[0x000A] = "GPSMeasureMode"; exifFormats[0x000a] = Image::ExifType::ASCII;
	exifTags[0x000B] = "GPSDOP"; exifFormats[0x000B] = Image::ExifType::U_RATIONAL;
	exifTags[0x000C] = "GPSSpeedRef"; exifFormats[0x000C] = Image::ExifType::ASCII;
	exifTags[0x000D] = "GPSSpeed"; exifFormats[0x000D] = Image::ExifType::U_RATIONAL;
	exifTags[0x000E] = "GPSTrackRef"; exifFormats[0x000E] = Image::ExifType::ASCII;
	exifTags[0x000F] = "GPSTrack"; exifFormats[0x000F] = Image::ExifType::U_RATIONAL;
	exifTags[0x0010] = "GPSImgDirectionRef"; exifFormats[0x0010] = Image::ExifType::ASCII;
	exifTags[0x0011] = "GPSImgDirection"; exifFormats[0x0011] = Image::ExifType::U_RATIONAL;
	exifTags[0x0012] = "GPSMapDatum"; exifFormats[0x0012] = Image::ExifType::ASCII;
	exifTags[0x0013] = "GPSDestLatitudeRef"; exifFormats[0x0013] = Image::ExifType::ASCII;
	exifTags[0x0014] = "GPSDestLatitude"; exifFormats[0x0014] = Image::ExifType::U_RATIONAL;
	exifTags[0x0015] = "GPSDestLongitudeRef"; exifFormats[0x0015] = Image::ExifType::ASCII;
	exifTags[0x0016] = "GPSDestLongitude"; exifFormats[0x0016] = Image::ExifType::U_RATIONAL;
	exifTags[0x0017] = "GPSDestBearingRef"; exifFormats[0x0017] = Image::ExifType::ASCII;
	exifTags[0x0018] = "GPSDestBearing"; exifFormats[0x0018] = Image::ExifType::U_RATIONAL;
	exifTags[0x0019] = "GPSDestDistanceRef"; exifFormats[0x0019] = Image::ExifType::ASCII;
	exifTags[0x001A] = "GPSDestDistance"; exifFormats[0x001A] = Image::ExifType::U_RATIONAL;
	exifTags[0x001B] = "GPSProcessingMethod"; exifFormats[0x001B] = Image::ExifType::UNDEFINED;
	exifTags[0x001C] = "GPSAreaInformation"; exifFormats[0x001C] = Image::ExifType::UNDEFINED;
	exifTags[0x001D] = "GPSDateStamp"; exifFormats[0x001D] = Image::ExifType::ASCII;
	exifTags[0x001E] = "GPSDifferential"; exifFormats[0x001E] = Image::ExifType::U_SHORT;
	exifTags[0x001F] = "GPSHPositioningError"; exifFormats[0x001F] = Image::ExifType::U_RATIONAL;
}

bool Image::exifIsGPSCoordinate(size_t tag) {
	if (tag == 0x0002 || tag == 0x0004 || tag== 0x0007 || tag == 0x0014 || tag == 0x0016) {
		return true;
	}
	return false;
}