// Copyright 2016 Jacob Chesley

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

	wxFileName file(fileName);

	// Handle custom 16 bit image format
	if(file.GetExt().CmpNoCase("tif") == 0 || file.GetExt().CmpNoCase("tiff") == 0){
		TIFF* tif = TIFFOpen(fileName.c_str(), "r");

		// Load the tiff image with our own tiff loader
		if(ImageHandler::GetTiffBitDepth(fileName) == 16){
			ImageHandler::Read16BitTiff(fileName, image);
		}

		// use wxWidgets to load in tiff image if not 16 bit
		else{
			wxImage fileImage(fileName);
			image->SetDataFrom8(fileImage.GetData(), fileImage.GetWidth(), fileImage.GetHeight());
		}
		TIFFClose(tif);
	}
	else{
		wxImage fileImage(fileName);
		image->SetDataFrom8(fileImage.GetData(), fileImage.GetWidth(), fileImage.GetHeight());
	}
}

void ImageHandler::SaveImageToFile(wxString fileName, Image * image, int type, int quality) {

	// Save 8 bit image
	if(type == SaveType::BMP || type == SaveType::JPEG || type == SaveType::PNG || type == SaveType::TIFF8){

		int dataSize = image->GetWidth() * image->GetHeight() * 3;
		unsigned char * outData = (unsigned char *)malloc(dataSize);

		// Process image is 8 bit, keep values
		if(image->GetColorDepth() == 8){

			unsigned char * redData = image->Get8BitDataRed();
			unsigned char * greenData = image->Get8BitDataGreen();
			unsigned char * blueData = image->Get8BitDataBlue();

			int pixel = 0;
			for(int i = 0; i < dataSize; i += 3){
				outData[i] = redData[pixel];
				outData[i + 1] = greenData[pixel];
				outData[i + 2] = blueData[pixel];
				pixel += 1;
			}
		}

		// Process image is 16 bit, must scale back to 8 bit
		else{

			uint16_t * redData = image->Get16BitDataRed();
			uint16_t * greenData = image->Get16BitDataGreen();
			uint16_t * blueData = image->Get16BitDataBlue();

			int pixel = 0;
			for(int i = 0; i < dataSize; i += 3){
				outData[i] = redData[pixel]/256.0;
				outData[i + 1] = greenData[pixel]/256.0;
				outData[i + 2] = blueData[pixel]/256.0;
				pixel += 1;
			}
		}

		wxImage outImage(image->GetWidth(), image->GetHeight(), outData);
		switch(type){
			case SaveType::BMP:{
				outImage.SaveFile(fileName, wxBITMAP_TYPE_BMP);
			}
			break;

			case SaveType::JPEG:{
				outImage.SetOption(_T("quality"), quality);
				outImage.SaveFile(fileName, wxBITMAP_TYPE_JPEG);
			}
			break;

			case SaveType::PNG:{
				outImage.SaveFile(fileName, wxBITMAP_TYPE_PNG);
			}
			break;

			case SaveType::TIFF8:{
				outImage.SaveFile(fileName, wxBITMAP_TYPE_TIFF);
			}
			break;
		}
	}

	// Save 16 bit image to tiff
	else{
		ImageHandler::Write16BitTiff(fileName, image);
	}
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
	
	if(image->GetColorDepth() == 8){
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
	else{
		uint16_t * redData = image->Get16BitDataRed();
		uint16_t * greenData = image->Get16BitDataGreen();
		uint16_t * blueData = image->Get16BitDataBlue();

		int index = 0;
		for (int i = 0; i < dataSize; i += 3) {

			outArray[i] = (uint8_t)(redData[index]/256.0);
			outArray[i + 1] = (uint8_t)(greenData[index]/256.0);
			outArray[i + 2] = (uint8_t)(blueData[index]/256.0);

			index += 1;
		}
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

void ImageHandler::CopyImageFromRaw(libraw_processed_image_t * rawImage, wxImage * outImage) {

	// Bitmap in RGB values from rawImage
	if (rawImage->type == LIBRAW_IMAGE_BITMAP) {

		ushort width = rawImage->width;
		ushort height = rawImage->height;

		int dataSize = width * height * 3;
		unsigned char * rgbData = (unsigned char *)malloc(dataSize);

		// 8 bit image, copy data
		if (rawImage->bits == 8) {
			memcpy(rgbData, rawImage->data, dataSize);
			outImage->SetData(rgbData);
		}

		// 16 bit image
		else if (rawImage->bits == 16) {

			// Scale data to 8 bit
			for (int i = 0; i < dataSize; i ++) {
				rgbData[i] = rawImage->data[i] / 256;
			}
			outImage->SetData(rgbData);
			
		}
	}
}
bool ImageHandler::CheckRaw(wxString fileName){

	// Attempt to open file in LibRaw raw processor and get the error code returned
	LibRaw tempRawProc;
	int result = -1;
	#ifdef __WXMSW__
		result = tempRawProc.open_file(fileName.wc_str());
	#else
		result = tempRawProc.open_file(fileName.c_str());
	#endif
	// Return true / false based on error code
	if(result == LIBRAW_SUCCESS){ return true; }
	else{ return false; }
}

bool ImageHandler::CheckImage(wxString fileName){

	if(wxImage::CanRead(fileName)){
		wxImage fileImage(fileName);
		return fileImage.IsOk();
	}
	else{
		return false;
	}
}

int ImageHandler::GetTiffBitDepth(wxString fileName){

	// Make sure this is a tiff file
	wxFileName file(fileName);
	if(file.GetExt().CmpNoCase("tif") == 0 || file.GetExt().CmpNoCase("tiff") == 0){

		TIFF* tif = TIFFOpen(fileName.c_str(), "r");
		uint32 bitsPerSample = 0;
		TIFFGetField(tif, TIFFTAG_BITSPERSAMPLE, &bitsPerSample);
		return bitsPerSample;
	}

	return 0;
}

void ImageHandler::Read16BitTiff(wxString fileName, Image * image){

	TIFF* tif = TIFFOpen(fileName.c_str(), "r");

			
	uint32 width = 0;
	uint32 height = 0;
	uint32 samplesPerPixel = 0;
	uint32 planarConfig = 0;
	uint32 compression = 0;
	uint32 photoMetric = 0;
	uint32 rowsPerStrip = 0;

	TIFFGetField(tif, TIFFTAG_IMAGEWIDTH, &width);
	TIFFGetField(tif, TIFFTAG_IMAGELENGTH, &height);
	TIFFGetField(tif, TIFFTAG_SAMPLESPERPIXEL, &samplesPerPixel);
	TIFFGetField(tif, TIFFTAG_PLANARCONFIG, &planarConfig);

	TIFFGetField(tif, TIFFTAG_COMPRESSION, &compression);
	TIFFGetField(tif, TIFFTAG_PHOTOMETRIC, &photoMetric);
	TIFFGetField(tif, TIFFTAG_ROWSPERSTRIP, &rowsPerStrip);

	// 16 bit, uncompressed, RGB image stored in RGBRGBRGB...
	if(samplesPerPixel == 3 && planarConfig == PLANARCONFIG_CONTIG && 
		compression && COMPRESSION_NONE && photoMetric == PHOTOMETRIC_RGB){

		// Get size of a strip and number of strips
		uint32 stripSize = TIFFStripSize(tif);
		uint32 numStrips = TIFFNumberOfStrips(tif);
		uint32 bufferSize = stripSize * numStrips;
		uint8_t * buffer = new uint8_t[bufferSize];
		uint32 bufferIdx = 0;
		
		uint8_t * stripBuffer = new uint8_t[stripSize];

		// Read in current strip
		for(uint32 strip = 0; strip < numStrips; strip++){
			TIFFReadEncodedStrip(tif, strip, stripBuffer, stripSize);

			// Append strip to buffer
			for(uint32 i = 0; i < stripSize; i++){
				buffer[bufferIdx] = stripBuffer[i];

				// 6 bytes per pixel (16 bit RR GG BB)
				if(bufferIdx >= width * height * 6){
					break;
				}
				bufferIdx += 1;
			}
		}
				
		delete[]stripBuffer;

		// Read full buffer into an imageBuffer to set image data to
		uint16_t * imgBuffer = new uint16_t[width * height * 3];
		int imgIdx = 0;
		uint16_t temp=0;
		
		// Read in RRGGBB bytes to buffer
		for(uint32 i = 0; i < bufferIdx; i += 6){
			temp = (buffer[i + 0] << 8 ) | (buffer[i + 1] & 0xff);
			imgBuffer[imgIdx + 0] = (buffer[i + 1] << 8 ) | (buffer[i + 0] & 0xff);
			imgBuffer[imgIdx + 1] = (buffer[i + 3] << 8 ) | (buffer[i + 2] & 0xff);
			imgBuffer[imgIdx + 2] = (buffer[i + 5] << 8 ) | (buffer[i + 4] & 0xff);
			imgIdx += 3;
		}
		
		image->SetDataFrom16(imgBuffer, width, height);
		delete[] buffer;
		delete[] imgBuffer;
	}

	// 16 bit, uncompressed, BW image stored in BL BL BL...
	else if(samplesPerPixel == 1 && planarConfig == PLANARCONFIG_CONTIG && 
		compression == COMPRESSION_NONE && (photoMetric == PHOTOMETRIC_MINISBLACK || 
		photoMetric == PHOTOMETRIC_MINISWHITE)){
	
		// Get size of a strip and number of strips
		uint32 stripSize = TIFFStripSize(tif);
		uint32 numStrips = TIFFNumberOfStrips(tif);
		uint32 bufferSize = stripSize * numStrips;
		uint8_t * buffer = new uint8_t[bufferSize];
		uint32 bufferIdx = 0;
		
		uint8_t * stripBuffer = new uint8_t[stripSize];

		// Read in current strip
		for(uint32 strip = 0; strip < numStrips; strip++){
			TIFFReadEncodedStrip(tif, strip, stripBuffer, stripSize);

			// Append strip to buffer
			for(uint32 i = 0; i < stripSize; i++){
				buffer[bufferIdx] = stripBuffer[i];

				// 2 bytes per pixel (16 bit greyscale)
				if(bufferIdx >= width * height * 2){
					break;
				}
				bufferIdx += 1;
			}
		}
				
		delete[]stripBuffer;

		// Read full buffer into an imageBuffer to set image data to
		uint16_t * imgBuffer = new uint16_t[width * height * 3];
		int imgIdx = 0;
		uint16_t temp=0;
		
		// Read in BW bytes to buffer, using min is black
		if(photoMetric == PHOTOMETRIC_MINISBLACK){
			for(uint32 i = 0; i < bufferIdx; i += 2){
				temp = (buffer[i + 0] << 8 ) | (buffer[i + 1] & 0xff);
				imgBuffer[imgIdx + 0] = (buffer[i + 1] << 8 ) | (buffer[i + 0] & 0xff);
				imgBuffer[imgIdx + 1] = (buffer[i + 1] << 8 ) | (buffer[i + 0] & 0xff);
				imgBuffer[imgIdx + 2] = (buffer[i + 1] << 8 ) | (buffer[i + 0] & 0xff);
				imgIdx += 3;
			}
		}

		// Read in BW bytes to buffer, using min is black
		else if(photoMetric == PHOTOMETRIC_MINISWHITE){
			for(uint32 i = 0; i < bufferIdx; i += 2){
				temp = (buffer[i + 0] << 8 ) | (buffer[i + 1] & 0xff);
				imgBuffer[imgIdx + 0] = 65535 - ((buffer[i + 1] << 8 ) | (buffer[i + 0] & 0xff));
				imgBuffer[imgIdx + 1] = 65535 - ((buffer[i + 1] << 8 ) | (buffer[i + 0] & 0xff));
				imgBuffer[imgIdx + 2] = 65535 - ((buffer[i + 1] << 8 ) | (buffer[i + 0] & 0xff));
				imgIdx += 3;
			}
		}
		
		image->SetDataFrom16(imgBuffer, width, height);
		delete[] buffer;
		delete[] imgBuffer;

	}

	// Resort to wxWidgets to handle tiff if we cant (only 8 bit data will be loaded)
	else{

		wxImage fileImage(fileName);
		image->SetDataFrom8(fileImage.GetData(), fileImage.GetWidth(), fileImage.GetHeight());
	}
}

void ImageHandler::Write16BitTiff(wxString fileName, Image * image){

	uint32 width = image->GetWidth();
	uint32 height = image->GetHeight();

	TIFF* tif = TIFFOpen(fileName.c_str(), "w");
	TIFFSetField(tif, TIFFTAG_IMAGEWIDTH, width);
	TIFFSetField(tif, TIFFTAG_IMAGELENGTH, height);
	TIFFSetField(tif, TIFFTAG_BITSPERSAMPLE, 16); // 16 BIT
	TIFFSetField(tif, TIFFTAG_SAMPLESPERPIXEL, 3); // RGB
	TIFFSetField(tif, TIFFTAG_PLANARCONFIG, PLANARCONFIG_CONTIG);

	TIFFSetField(tif, TIFFTAG_COMPRESSION, COMPRESSION_NONE);
	TIFFSetField(tif, TIFFTAG_PHOTOMETRIC, PHOTOMETRIC_RGB);
	TIFFSetField(tif, TIFFTAG_ROWSPERSTRIP, height); 

	// allocate output buffer
	int outBufferSize= width * height * 6;
	uint8_t * outBuffer = new uint8_t[outBufferSize]; // Width x Height - RRGGBB (2 bytes per sample)

	// Handle 8 bit data (scale up)
	if(image->GetColorDepth() == 8){
		// Get color channel buffers of image
		uint8_t * redBuffer = image->Get8BitDataRed();
		uint8_t * greenBuffer = image->Get8BitDataGreen();
		uint8_t * blueBuffer = image->Get8BitDataBlue();

		int tempIdx = 0;

		for(int i = 0; i < outBufferSize; i += 6){

			// 2 bytes for blue
			outBuffer[i + 0] = (((redBuffer[tempIdx + 0] * 255) >> 0) & 0xff);
			outBuffer[i + 1] = (((redBuffer[tempIdx + 0] * 255) >> 8) & 0xff);

			// 2 bytes for green
			outBuffer[i + 2] = (((greenBuffer[tempIdx + 0] * 255) >> 0) & 0xff);
			outBuffer[i + 3] = (((greenBuffer[tempIdx + 0] * 255) >> 8) & 0xff);

			// 2 bytes for blue
			outBuffer[i + 4] = (((blueBuffer[tempIdx + 0] * 255) >> 0) & 0xff);
			outBuffer[i + 5] = (((blueBuffer[tempIdx + 0] * 255) >> 8) & 0xff);

			tempIdx += 1;
		}

		TIFFWriteEncodedStrip(tif, 0, outBuffer, outBufferSize);
		delete[] outBuffer;
		TIFFClose(tif);
	}

	// Handle 16 bit data (save as is)
	else{

		// Get color channel buffers of image
		uint16_t * redBuffer = image->Get16BitDataRed();
		uint16_t * greenBuffer = image->Get16BitDataGreen();
		uint16_t * blueBuffer = image->Get16BitDataBlue();

		int tempIdx = 0;
		for(int i = 0; i < outBufferSize; i += 6){
			
			// 2 bytes for blue
			outBuffer[i + 0] = ((redBuffer[tempIdx + 0] >> 0) & 0xff);
			outBuffer[i + 1] = ((redBuffer[tempIdx + 0] >> 8) & 0xff);

			// 2 bytes for green
			outBuffer[i + 2] = ((greenBuffer[tempIdx + 0] >> 0) & 0xff);
			outBuffer[i + 3] = ((greenBuffer[tempIdx + 0] >> 8) & 0xff);

			// 2 bytes for blue
			outBuffer[i + 4] = ((blueBuffer[tempIdx + 0] >> 0) & 0xff);
			outBuffer[i + 5] = ((blueBuffer[tempIdx + 0] >> 8) & 0xff);

			tempIdx += 1;
		}

		TIFFWriteEncodedStrip(tif, 0, outBuffer, outBufferSize);
		delete[] outBuffer;
		TIFFClose(tif);
	}
}