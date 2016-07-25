#include "ImageHandler.h"



wxString ImageHandler::allFileDialogList = "All Files (*.*)|*.*";
wxString ImageHandler::jpegFileDialogList = "JPEG (*.JPG;*.JPEG)|*.JPG;*.JPEG";
wxString ImageHandler::pngFileDialogList = "PNG (*.PNG)|*.PNG";
wxString ImageHandler::bmpFileDialogList = "BMP (*.BMP)|*.BMP";
wxString ImageHandler::tiffFileDialogList = "TIFF (*.TIF;*.TIFF)|*.TIF;*.TIFF";

wxString ImageHandler::imageOpenDialogList = 
	ImageHandler::allFileDialogList + "|" +
	ImageHandler::jpegFileDialogList + "|" + 
	ImageHandler::pngFileDialogList + "|" + 
	ImageHandler::bmpFileDialogList + "|" + 
	ImageHandler::tiffFileDialogList;
	
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

void ImageHandler::CopyImageFromRaw(libraw_processed_image_t * rawImage, Image * outImage){

	// Bitmap in RGB values from rawImage
	if(rawImage->type == LIBRAW_IMAGE_BITMAP){
	
		ushort width = rawImage->width;
		ushort height = rawImage->height;

		// 8 bit image
		if(rawImage->bits == 8){
			outImage->SetDataFrom8(rawImage->data, width, height);
		}

		// 16 bit image
		else if(rawImage->bits == 16){
			outImage->SetDataFrom16((uint8_t*)rawImage->data, width, height);
		}
	}
}

bool ImageHandler::CheckRaw(wxString fileName){

	// Attempt to open file in LibRaw raw processor and get the error code returned
	LibRaw tempRawProc;
	int result = tempRawProc.open_file(fileName.wc_str());

	// Return true / false based on error code
	if(result == LIBRAW_SUCCESS){ return true; }
	else{ return false; }
}

bool ImageHandler::CheckImage(wxString fileName){

	wxImage fileImage(fileName);
	return fileImage.IsOk();
}