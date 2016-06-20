#include "ImageHandler.h"

void ImageHandler::LoadImageFromFile(wxString fileName, Image * image) {

	wxImage fileImage(fileName);
	image->SetDataFrom8(fileImage.GetData(), fileImage.GetWidth(), fileImage.GetHeight());
}

void ImageHandler::LoadImageFromwxImage(wxImage* inImage, Image * image) {

	if (inImage->IsOk()) {
		image->SetDataFrom8(inImage->GetData(), inImage->GetWidth(), inImage->GetHeight());
	}
	else {
		image->SetDataFrom8(NULL, 0, 0);
	}
}

void ImageHandler::CopyImageData8(Image * image, uint8_t * outArray) {
	
	int dataSize = image->GetWidth() * image->GetHeight() * 3;
	
	uint8_t * redData = image->Get8BitDataRed();
	uint8_t * greenData = image->Get8BitDataGreen();
	uint8_t * blueData = image->Get8BitDataBlue();

	int index = 0;
	for (int i = 0; i < dataSize; i += 3) {

		outArray[i] = redData[index];
		outArray[i + 1] = greenData[index];
		outArray[i + 2] = blueData[index];

		index += 1;
	}
}

void ImageHandler::CopyImageData16(Image * image, uint16_t * outArray) {

	if (image->GetColorDepth() != 16) {
		return;
	}

	int dataSize = image->GetWidth() * image->GetHeight() * 3;

	uint16_t * redData = image->Get16BitDataRed();
	uint16_t * greenData = image->Get16BitDataGreen();
	uint16_t * blueData = image->Get16BitDataBlue();

	int index = 0;
	for (int i = 0; i < dataSize; i += 3) {

		outArray[i] = redData[index];
		outArray[i + 1] = greenData[index];
		outArray[i + 2] = blueData[index];

		index += 1;
	}
}