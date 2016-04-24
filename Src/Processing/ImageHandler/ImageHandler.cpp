#include "ImageHandler.h"

void ImageHandler::LoadImageFromFile(wxString fileName, Image * image) {

	wxImage fileImage(fileName);
	image->SetDataFrom8(fileImage.GetData(), fileImage.GetWidth(), fileImage.GetHeight());
}

void ImageHandler::CopyImageData8(Image * image, unsigned char * outArray) {
	
	int dataSize = image->GetWidth() * image->GetHeight() * 3;
	
	uint8_t * redData = image->Get8BitDataRed();
	uint8_t * greenData = image->Get8BitDataGreen();
	uint8_t * blueData = image->Get8BitDataBlue();

	int index = 0;
	for (int i = 0; i < dataSize; i += 3) {

		outArray[i] = redData[index];
		outArray[i + 1] = greenData[index];
		outArray[i + 2] = blueData[index];

		int test = redData[index];
		index += 1;
	}
}