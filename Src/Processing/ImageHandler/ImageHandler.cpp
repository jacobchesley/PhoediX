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

	// Convert from JPEG to wxImage
	if (rawImage->type == LIBRAW_IMAGE_JPEG) {

		wxMemoryInputStream jpegStream(rawImage->data, rawImage->data_size);
		outImage->LoadFile(jpegStream, wxBITMAP_TYPE_JPEG);
	}
}


bool ImageHandler::CheckRaw(wxString fileName){

	// Attempt to open file in LibRaw raw processor and get the error code returned
	LibRaw  * tempRawProc = new LibRaw();
	int result = -1;
	#ifdef __WXMSW__
		result = tempRawProc->open_file(fileName.wc_str());
	#else
		result = tempRawProc->open_file(fileName.c_str());
	#endif
	// Return true / false based on error code

	delete tempRawProc;
	if(result == LIBRAW_SUCCESS){ return true; }
	else{ return false; }
}

bool ImageHandler::CheckImage(wxString fileName){

	// Temporarly disable logging so warnings about unsupported images don't appear
	wxLogNull noLog;

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

void ImageHandler::ReadExif(wxString fileName, Image * image) {

	if (ImageHandler::CheckRaw(fileName)) {

		// Attempt to open file in LibRaw raw processor and get the error code returned
		LibRaw  * tempRawProc = new LibRaw();
		#ifdef __WXMSW__
			tempRawProc->open_file(fileName.wc_str());
		#else
			tempRawProc->open_file(fileName.c_str());
		#endif
		ImageHandler::ReadExifFromRaw(tempRawProc, image);
		delete tempRawProc;
	}
	else {
		ImageHandler::ReadExifDataFromFile(fileName, image);
	}
}

bool ImageHandler::CheckExif(wxString fileName) {

	size_t numberBytesToCheckForExif = 50000;
	// Read in file to byte array
	wxFileInputStream fileStream(fileName);
	unsigned char * fileBytes = new unsigned char[fileStream.GetSize()];

	if (numberBytesToCheckForExif > fileStream.GetSize()) {
		numberBytesToCheckForExif = fileStream.GetSize();
	}
	fileStream.ReadAll(fileBytes, numberBytesToCheckForExif);

	size_t exifStart = 0;
	size_t exifSize = 0;

	// Look for exif header
	for (size_t i = 0; i < numberBytesToCheckForExif; i++) {

		// Exif tag found
		if (fileBytes[i] == 0xFF && fileBytes[i + 1] == 0xE1) {

			exifSize = fileBytes[i + 2] | fileBytes[i + 3] << 8;
			exifStart = i + 4;
			break;
		}
	}

	if (exifSize < 1) { 
		delete[] fileBytes;
		return false;
	}

	// Read in all exif data and delete file data after exif is read
	unsigned char * exifBytes = new unsigned char[exifSize];
	fileStream.SeekI(exifStart);
	fileStream.Read(exifBytes, exifSize);
	delete[] fileBytes;

	// Check first 6 bytes to verify exif tag
	unsigned char exifCheck[6];
	exifCheck[0] = 'E'; exifCheck[1] = 'x';
	exifCheck[2] = 'i'; exifCheck[3] = 'f';
	exifCheck[4] = 0; exifCheck[5] = 0;

	for (size_t i = 0; i < 6; i++) {
		if (exifCheck[i] != exifBytes[i]) {
			return false;
		}
	}

	// Get endianness of exif data
	bool littleEndian = true;
	if (exifBytes[6] == 'I' && exifBytes[7] == 'I') { littleEndian = true; }
	else if (exifBytes[6] == 'M' && exifBytes[7] == 'M') { littleEndian = false; }
	else { return false; }

	// Verify endianness
	if (littleEndian && (exifBytes[8] != 0x2a || exifBytes[9] != 0x00)) {
		return false;
	}
	if (!littleEndian && (exifBytes[8] != 0x00 || exifBytes[9] != 0x2a)) {
		return false;
	}

	delete[] exifBytes;

	return true;
}

void ImageHandler::ReadExifFromRaw(LibRaw * rawProcessor, Image * image) {

	(*image->GetExifMap())[0x0132] = new wxString(ctime(&rawProcessor->imgdata.other.timestamp)); // date/time
	(*image->GetExifMap())[0x013B] = new wxString(rawProcessor->imgdata.other.artist); // Artist
	(*image->GetExifMap())[0x010E] = new wxString(rawProcessor->imgdata.other.desc); // Description
	(*image->GetExifMap())[0x9211] = (void*) new uint16(rawProcessor->imgdata.other.shot_order); // Shot Order

	if (rawProcessor->imgdata.sizes.flip == 5 || rawProcessor->imgdata.sizes.flip == 6) {
		(*image->GetExifMap())[0xA002] = (void*) new uint16(rawProcessor->imgdata.sizes.height); // Width (flipped)
		(*image->GetExifMap())[0xA003] = (void*) new uint16(rawProcessor->imgdata.sizes.width); // height (flipped)
	}
	else {
		(*image->GetExifMap())[0xA002] = (void*) new uint16(rawProcessor->imgdata.sizes.width); // Width 
		(*image->GetExifMap())[0xA003] = (void*) new uint16(rawProcessor->imgdata.sizes.height); // height
	}

	// Shutter Speed calculation
	float shutterSpeed = rawProcessor->imgdata.other.shutter;
	UnsignedRational * shutterSpeedRational = new UnsignedRational();
	if (shutterSpeed > 1.0) { 
		shutterSpeedRational->numerator = shutterSpeed; 
		shutterSpeedRational->denominator = 1; 
	}
	else {
		for (int i = 1; i < 500001; i++) {
			if ((1.0f / (float)i) == shutterSpeed) {
				shutterSpeedRational->numerator = 1;
				shutterSpeedRational->denominator = i;
			}
		}
	}
	(*image->GetExifMap())[0x829A] = (void*)shutterSpeedRational; // Shutter Speed

	UnsignedRational * fNumberRational = new UnsignedRational();
	fNumberRational->numerator = rawProcessor->imgdata.other.aperture;
	fNumberRational->denominator = 1;
	(*image->GetExifMap())[0x829D] = (void*)fNumberRational; // FNumber

	(*image->GetExifMap())[0x8827] = (void*) new uint16(rawProcessor->imgdata.other.iso_speed); // ISO
	
	UnsignedRational * focalRational = new UnsignedRational();
	focalRational->numerator = rawProcessor->imgdata.other.focal_len;
	focalRational->denominator = 1;
	(*image->GetExifMap())[0x920A] = (void*)focalRational; // Focal Length

	// Flash Fired
	if (rawProcessor->imgdata.color.flash_used > 0.0) { (*image->GetExifMap())[0x9209] = (void*) new uint16(1); }
	else{ (*image->GetExifMap())[0x9209] = (void*) new uint16(0); }

	// Camera Info
	(*image->GetExifMap())[0x010F] = new wxString(rawProcessor->imgdata.idata.make); // Camera Make
	(*image->GetExifMap())[0x0110] = new wxString(rawProcessor->imgdata.idata.model); // Camera Model
	(*image->GetExifMap())[0x0131] = new wxString(rawProcessor->imgdata.idata.software); // Software
}

void ImageHandler::ReadExifDataFromFile(wxString fileName, Image * image) {

	// Read in file to byte array
	wxFileInputStream fileStream(fileName);
	unsigned char * fileBytes = new unsigned char[fileStream.GetSize()];
	fileStream.ReadAll(fileBytes, fileStream.GetSize());

	size_t exifStart = 0;
	size_t exifSize = 0;

	// Look for exif header
	for (int i = 0; i < fileStream.GetSize(); i++){

		// Exif tag found
		if (fileBytes[i] == 0xFF && fileBytes[i + 1] == 0xE1) {

			exifSize = fileBytes[i + 2] | fileBytes[i + 3] << 8;
			exifStart = i + 4;
			break;
		}
	}

	if (exifSize < 1) { return; }
	// Read in all exif data and delete file data after exif is read
	unsigned char * exifBytes = new unsigned char[exifSize];
	fileStream.SeekI(exifStart);
	fileStream.Read(exifBytes, exifSize);
	delete[] fileBytes;

	// Check first 6 bytes to verify exif tag
	unsigned char exifCheck[6];
	exifCheck[0] = 'E'; exifCheck[1] = 'x'; 
	exifCheck[2] = 'i'; exifCheck[3] = 'f';
	exifCheck[4] = 0; exifCheck[5] = 0;

	for (size_t i = 0; i < 6; i++) {
		if (exifCheck[i] != exifBytes[i]) {
			return;
		}
	}
	
	// Get endianness of exif data
	bool littleEndian = true;
	if      (exifBytes[6] == 'I' && exifBytes[7] == 'I') { littleEndian = true; }
	else if (exifBytes[6] == 'M' && exifBytes[7] == 'M') { littleEndian = false; }
	else { return; }

	// Verify endianness
	if (littleEndian && (exifBytes[8] != 0x2a || exifBytes[9] != 0x00)) {
		return;
	}
	if (!littleEndian && (exifBytes[8] != 0x00 || exifBytes[9] != 0x2a)) {
		return;
	}

	unsigned char firstIfd[4];
	firstIfd[0] = exifBytes[10]; firstIfd[1] = exifBytes[11]; 
	firstIfd[2] = exifBytes[12]; firstIfd[3] = exifBytes[13];

	size_t firstIFDOffset = (size_t)ImageHandler::Get32BitUnsigned(firstIfd, littleEndian);
	
	// exifBytes + 6 to account for start "Exif\0\0"
	ImageHandler::ParseDirectory(exifBytes + 6, firstIFDOffset, littleEndian, image);

	delete[] exifBytes;
}

void ImageHandler::ParseDirectory(unsigned char * data, size_t offset, bool littleEndian, Image * image) {

	int entryOffset = 2;
	unsigned long nextDirectory = 0;
	size_t numberDirectoryEntries = ImageHandler::Get16BitUnsigned(data + offset, littleEndian);

	for(int i = 0; i < numberDirectoryEntries; i++){

		size_t tagNumber = ImageHandler::Get16BitUnsigned(data + offset + entryOffset, littleEndian);
		if (Image::exifTags.count(tagNumber) <= 0) { continue; } // Check for unsupported tag
		size_t format = ImageHandler::Get16BitUnsigned(data + offset + 2 + entryOffset, littleEndian);
		size_t numberComponents = ImageHandler::Get32BitUnsigned(data + offset + 4 + entryOffset, littleEndian);
		int dataSize = ImageHandler::GetBytesPerComponent(format) * (int)numberComponents;
		if ((*image->GetExifMap()).count(tagNumber) > 0) { continue; }

		if (dataSize > 4) {

			// Get offset of value from start of exif data
			unsigned long valueOffset = ImageHandler::Get32BitUnsigned(data + offset + 8 + entryOffset, littleEndian);

			// Handle mutliple values for the same tag
			if (Image::exifIsGPSCoordinate(tagNumber)) {

				// Rational is 8 bytes, get number of them
				int numRationals = dataSize / 8;

				// Create new vector to push rationals to
				(*image->GetExifMap())[tagNumber] = new wxVector<UnsignedRational*>();

				for (int j = 0; j < numRationals; j++) {
					void * value = GetValueFromAddress(data, valueOffset + (j * 8), dataSize, format, littleEndian);
					((wxVector<UnsignedRational*>*)(*image->GetExifMap())[tagNumber])->push_back((UnsignedRational*)value);
				}
			}

			// Dereference map pointer from image to set value
			else {
				// Get value from address
				void * value = GetValueFromAddress(data, valueOffset, dataSize, format, littleEndian);
				(*image->GetExifMap())[tagNumber] = value;
			}
		}
		else {

			// Parse Sub IFD Directory
			if (format == 4 && tagNumber == 0x8769) {

				// Get sub dirrectory offset and parse
				nextDirectory = ImageHandler::Get32BitUnsigned(data + offset + 8 + entryOffset, littleEndian);
				ImageHandler::ParseDirectory(data, nextDirectory, littleEndian, image);
			}

			// Parse GPS IFD Directory
			else if (format == 4 && tagNumber == 0x8825) {

				// Get sub dirrectory offset and parse
				nextDirectory = ImageHandler::Get32BitUnsigned(data + offset + 8 + entryOffset, littleEndian);
				ImageHandler::ParseDirectory(data, nextDirectory, littleEndian, image);
			}

			else {

				// Dereference map pointer from image to set value
				void * value = GetValue(data + offset + 8 + entryOffset, format, littleEndian);
				(*image->GetExifMap())[tagNumber] = value;
				
			}
		}

		// Check next four bytes for offset of next directory
		if (i == numberDirectoryEntries - 1) {
			nextDirectory = ImageHandler::Get32BitUnsigned(data + offset + 12 + entryOffset, littleEndian);
			if (nextDirectory == 0) { return; }
			else { ImageHandler::ParseDirectory(data, nextDirectory, littleEndian, image); }
		}
		entryOffset += 12;
	}
}

int ImageHandler::GetBytesPerComponent(size_t format) {
	if (format == 1)  { return 1; } // Unsigned byte
	if (format == 2)  { return 1; } // Asci String
	if (format == 3)  { return 2; } // Unsigned short
	if (format == 4)  { return 4; } // Unsigned long
	if (format == 5)  { return 8; } // Unsigned rational
	if (format == 6)  { return 1; } // Signed byte
	if (format == 7)  { return 1; } // Undefined
	if (format == 8)  { return 2; } // Signed short
	if (format == 9)  { return 4; } // Signed long
	if (format == 10) { return 8; } // signed rational
	if (format == 11) { return 4; } // Single float
	if (format == 12) { return 8; } // Double float
	return 0;
}

void * ImageHandler::GetValue(unsigned char * valueData, size_t format, bool littleEndian) {

	if (format == 1) { return (void*) new int(Get8BitUnsigned(valueData, littleEndian)); }
	if (format == 3) { return (void*) new int(Get16BitUnsigned(valueData, littleEndian)); }
	if (format == 4) { return (void*) new int(Get32BitUnsigned(valueData, littleEndian)); }
	if (format == 6) { return (void* )new int(Get8BitSigned(valueData, littleEndian)); }
	if (format == 8) { return (void*) new int(Get16BitSigned(valueData, littleEndian)); }
	if (format == 9) { return (void*) new int(Get32BitSigned(valueData, littleEndian)); }
	return NULL;
}

void * ImageHandler::GetValueFromAddress(unsigned char * exifData, size_t offset, size_t dataSize, size_t format, bool littleEndian) {
	
	// Get ASCII String
	if (format == 2) {
		wxString * returnString = new wxString("");

		for (size_t i = 0; i < dataSize; i++) {
			char c = exifData[offset + i];
			*returnString += c;
		}
		return returnString;
	}

	// Get Unsigned Rational
	if (format == 5) {
		UnsignedRational * uRational = new UnsignedRational();
		uRational->numerator = Get32BitUnsigned(exifData + offset, littleEndian);
		uRational ->denominator = Get32BitUnsigned(exifData + offset + 4, littleEndian);
		return uRational;
	}

	// Get Signed Rational
	if (format == 10) {
		SignedRational * rational = new SignedRational();
		rational->numerator = Get32BitSigned(exifData + offset, littleEndian);
		rational->denominator = Get32BitSigned(exifData + offset + 4, littleEndian);
		return rational;
	}
	return NULL;
}

int ImageHandler::Get8BitSigned(unsigned char * data, bool littleEndian) {
	if (littleEndian) {
		return data[0];
	}
	else {
		return data[0];
	}
}

unsigned int ImageHandler::Get8BitUnsigned(unsigned char * data, bool littleEndian) {
	return (unsigned)ImageHandler::Get8BitSigned(data, littleEndian) & 0xFF;
}

int ImageHandler::Get16BitSigned(unsigned char * data, bool littleEndian) {
	if (littleEndian) {
		return (data[1] << 8) | data[0];
	}
	else {
		return (data[0] << 8) | data[1];
	}
}

unsigned int ImageHandler::Get16BitUnsigned(unsigned char * data, bool littleEndian) {
	return (unsigned)ImageHandler::Get16BitSigned(data, littleEndian) & 0xFFFF;
}

long ImageHandler::Get32BitSigned(unsigned char * data, bool littleEndian) {
	if (littleEndian) {
		return (data[3] << 24) | (data[2] << 16) | (data[1] << 8) | (data[0]);
	}
	else {
		return (data[0] << 24) | (data[1] << 16) | (data[2] << 8) | (data[3]);
	}
}

unsigned long ImageHandler::Get32BitUnsigned(unsigned char * data, bool littleEndian) {
	return (unsigned)ImageHandler::Get32BitSigned(data, littleEndian) & 0xFFFFFFFF;
}

