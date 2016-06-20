#include "Processor.h"

wxDEFINE_EVENT(PROCESSOR_MESSAGE_EVENT, wxCommandEvent);

double Processor::pi = 3.141592653589793238463;

Processor::Processor(wxWindow * parent) {
	img = new Image();
	didUpdate = false;
	locked = false;
	parWindow = parent;
}

Processor::~Processor() {
	delete img;
	delete originalImg;
	this->DeleteEdits();
}

Image * Processor::GetImage() {
	return img;
}

void Processor::SetOriginalImage(Image *newOriginalImage) {
	originalImg = new Image(*newOriginalImage);
}

Image* Processor::GetOriginalImage() {
	return originalImg;
}

void  Processor::RevertToOriginalImage() {
	
	delete img;
	img = new Image(*originalImg);
	didUpdate = true;
}

void Processor::AddEdit(ProcessorEdit * edit) {
	editListInternal.push_back(edit);
}

void Processor::DeleteEdits() {

	// Delete all edits in the internal vector
	for (size_t i = 0; i < editListInternal.size(); i++) {
		delete editListInternal.at(i);
	}

	// Clear the vector
	editListInternal.clear();
}

void Processor::Enable16Bit() {
	img->Enable16Bit();
	originalImg->Enable16Bit();
}

void Processor::Disable16Bit() {
	img->Disable16Bit();
	originalImg->Disable16Bit();
}

void Processor::ProcessEdits() {
	
	if (this->GetLocked()) { return; }
	this->ProcessEdits(editListInternal);
}

void Processor::ProcessEdit(ProcessorEdit * edit) {

	if (this->GetLocked()) { return; }
	ProcessThread * procThread = new ProcessThread(this, edit);
	procThread->Run();

}

void Processor::ProcessEdits(wxVector<ProcessorEdit*> editList) {

	if (this->GetLocked()) { return; }
	ProcessThread * procThread = new ProcessThread(this, editList);
	procThread->Run();
	procThread->SetPriority(75);
}

void Processor::Lock() {
	{
		wxCriticalSectionLocker locker(lock);
		locked = true;
	}
}

void Processor::Unlock() {
	{
		wxCriticalSectionLocker locker(lock);
		locked = false;
	}
}

bool Processor::GetLocked() {
	{
		wxCriticalSectionLocker locker(lock);
		return locked;
	}
}

void Processor::SetUpdated(bool updated) {
	didUpdate = updated;
}

bool Processor::GetUpdated() {
	return didUpdate;
}

void Processor::SendMessageToParent(wxString message) {

	// Send add edit event with edit ID to parent window (the edit panel)
	if (parWindow != NULL) {
		wxCommandEvent evt(PROCESSOR_MESSAGE_EVENT, ID_PROCESSOR_MESSAGE);
		evt.SetString(message);
		wxPostEvent(parWindow, evt);
	}
}

void Processor::ShiftBrightness(double all, double red, double green, double blue, int dataStart, int dataEnd) {

	// Calculate 8 bit shift values.  Need 16 bits because this is now signed
	int16_t all8 = (int16_t)(all * 255);
	int16_t red8 = (int16_t)(red * 255);
	int16_t green8 = (int16_t)(green * 255);
	int16_t blue8 = (int16_t)(blue * 255);

	// Get number of pixels for the image
	int dataSize = img->GetWidth() * img->GetHeight();
	if (dataStart < 0 || dataEnd < 0) {
		dataStart = 0;
		dataEnd = dataSize;
	}

	// Get pointers to 8 bit data
	uint8_t * redData8 = img->Get8BitDataRed();
	uint8_t * greenData8 = img->Get8BitDataGreen();
	uint8_t * blueData8 = img->Get8BitDataBlue();

	// Create a temporary set of pixels that will allow handling of overflow
	int tempRed;
	int tempGreen;
	int tempBlue;

	for (int i = dataStart; i < dataEnd; i++) {

		// Get red, green and blue temporary pixels
		tempRed = redData8[i];
		tempGreen = greenData8[i];
		tempBlue = blueData8[i];

		// add the 8 bit value to the pixels
		tempRed += (red8 + all8);
		tempGreen += (green8 + all8);
		tempBlue += (blue8 + all8);

		// handle overflow or underflow
		tempRed = (tempRed > 255) ? 255 : tempRed;
		tempGreen = (tempGreen > 255) ? 255 : tempGreen;
		tempBlue = (tempBlue > 255) ? 255 : tempBlue;

		tempRed = (tempRed < 0) ? 0 : tempRed;
		tempGreen = (tempGreen < 0) ? 0 : tempGreen;
		tempBlue = (tempBlue < 0) ? 0 : tempBlue;
		
		// Set the new pixel to the 8 bit data
		redData8[i] = (uint8_t)tempRed;
		greenData8[i] = (uint8_t)tempGreen;
		blueData8[i] = (uint8_t)tempBlue;
	}

	// If 16 bit image is enabled
	if (img->GetColorDepth() == 16) {

		// Calculate 16 bit shift values.  Need 32 bits because this is now signed.
		int32_t all16 = (int32_t)(all * 65535);
		int32_t red16 = (int32_t)(red * 65535);
		int32_t green16 = (int32_t)(green * 65535);
		int32_t blue16 = (int32_t)(blue * 65535);

		// Get pointers to 16 bit data
		uint16_t * redData16 = img->Get16BitDataRed();
		uint16_t * greenData16 = img->Get16BitDataGreen();
		uint16_t * blueData16 = img->Get16BitDataBlue();

		for (int i = 0; i < dataSize; i++) {

			// Get red, green and blue temporary pixels
			tempRed = redData16[i];
			tempGreen = greenData16[i];
			tempBlue = blueData16[i];

			// add the 16 bit value to the pixels
			tempRed += (red16 + all16);
			tempGreen += (green16 + all16);
			tempBlue += (blue16 + all16);

			// handle overflow or underflow
			tempRed = (tempRed > 65535) ? 65535 : tempRed;
			tempGreen = (tempGreen > 65535) ? 65535 : tempGreen;
			tempBlue = (tempBlue > 65535) ? 65535 : tempBlue;

			tempRed = (tempRed < 0) ? 0 : tempRed;
			tempGreen = (tempGreen < 0) ? 0 : tempGreen;
			tempBlue = (tempBlue < 0) ? 0 : tempBlue;

			// Set the new pixel to the 16 bit data
			redData16[i] = (uint16_t)tempRed;
			greenData16[i] = (uint16_t)tempGreen;
			blueData16[i] = (uint16_t)tempBlue;
		}
	}
}

void Processor::ScaleBrightness(double all, double red, double green, double blue, int dataStart, int dataEnd) {

	// Get number of pixels for the image
	int dataSize = img->GetWidth() * img->GetHeight();
	if (dataStart < 0 || dataEnd < 0) {
		dataStart = 0;
		dataEnd = dataSize;
	}

	// Get pointers to 8 bit data
	uint8_t * redData8 = img->Get8BitDataRed();
	uint8_t * greenData8 = img->Get8BitDataGreen();
	uint8_t * blueData8 = img->Get8BitDataBlue();

	// Create a temporary set of pixels that will allow handling of overflow
	int tempRed;
	int tempGreen;
	int tempBlue;

	for (int i = dataStart; i < dataEnd; i++) {

		// Get red, green and blue temporary pixels
		tempRed = redData8[i];
		tempGreen = greenData8[i];
		tempBlue = blueData8[i];

		// scale the 8 bit values
		tempRed *= (all * red);
		tempGreen *= (all * green);
		tempBlue *= (all * blue);

		// handle overflow or underflow
		tempRed = (tempRed > 255) ? 255 : tempRed;
		tempGreen = (tempGreen > 255) ? 255 : tempGreen;
		tempBlue = (tempBlue > 255) ? 255 : tempBlue;

		tempRed = (tempRed < 0) ? 0 : tempRed;
		tempGreen = (tempGreen < 0) ? 0 : tempGreen;
		tempBlue = (tempBlue < 0) ? 0 : tempBlue;

		// Set the new pixel to the 8 bit data
		redData8[i] = (uint8_t)tempRed;
		greenData8[i] = (uint8_t)tempGreen;
		blueData8[i] = (uint8_t)tempBlue;
	}

	// If 16 bit image is enabled
	if (img->GetColorDepth() == 16) {

		// Get pointers to 16 bit data
		uint16_t * redData16 = img->Get16BitDataRed();
		uint16_t * greenData16 = img->Get16BitDataGreen();
		uint16_t * blueData16 = img->Get16BitDataBlue();

		for (int i = 0; i < dataSize; i++) {

			// Get red, green and blue temporary pixels
			tempRed = redData16[i];
			tempGreen = greenData16[i];
			tempBlue = blueData16[i];

			// scale the 16 bit values
			tempRed *= (all * red);
			tempGreen *= (all * green);
			tempBlue *= (all * blue);

			// handle overflow or underflow
			tempRed = (tempRed > 65535) ? 65535 : tempRed;
			tempGreen = (tempGreen > 65535) ? 65535 : tempGreen;
			tempBlue = (tempBlue > 65535) ? 65535 : tempBlue;

			tempRed = (tempRed < 0) ? 0 : tempRed;
			tempGreen = (tempGreen < 0) ? 0 : tempGreen;
			tempBlue = (tempBlue < 0) ? 0 : tempBlue;

			// Set the new pixel to the 16 bit data
			redData16[i] = (uint16_t)tempRed;
			greenData16[i] = (uint16_t)tempGreen;
			blueData16[i] = (uint16_t)tempBlue;
		}
	}
}

void Processor::AdjustContrast(double allContrast, double redContrast, double greenContrast, double blueContrast, int dataStart, int dataEnd) {

	// Get number of pixels for the image
	int dataSize = img->GetWidth() * img->GetHeight();
	if (dataStart < 0 || dataEnd < 0) {
		dataStart = 0;
		dataEnd = dataSize;
	}

	// Get pointers to 8 bit data
	uint8_t * redData8 = img->Get8BitDataRed();
	uint8_t * greenData8 = img->Get8BitDataGreen();
	uint8_t * blueData8 = img->Get8BitDataBlue();

	// Create a temporary set of pixels that will allow handling of overflow
	int tempRed;
	int tempGreen;
	int tempBlue;

	for (int i = dataStart; i < dataEnd; i++) {

		// Perform whole contrast on red, green and blue
		tempRed = (allContrast * (redData8[i] - 128)) + 128;
		tempGreen = (allContrast * (greenData8[i] - 128)) + 128;
		tempBlue = (allContrast * (blueData8[i] - 128)) + 128;

		// Perform individual contrast on red, green and blue
		tempRed = (redContrast * (tempRed - 128)) + 128;
		tempGreen = (greenContrast * (tempGreen - 128)) + 128;
		tempBlue = (blueContrast * (tempBlue - 128)) + 128;

		// handle overflow or underflow
		tempRed = (tempRed > 255) ? 255 : tempRed;
		tempGreen = (tempGreen > 255) ? 255 : tempGreen;
		tempBlue = (tempBlue > 255) ? 255 : tempBlue;

		tempRed = (tempRed < 0) ? 0 : tempRed;
		tempGreen = (tempGreen < 0) ? 0 : tempGreen;
		tempBlue = (tempBlue < 0) ? 0 : tempBlue;

		// Set the new pixel to the 8 bit data
		redData8[i] = (uint8_t)tempRed;
		greenData8[i] = (uint8_t)tempGreen;
		blueData8[i] = (uint8_t)tempBlue;
	}

	// If 16 bit image is enabled
	if (img->GetColorDepth() == 16) {

		// Get pointers to 16 bit data
		uint16_t * redData16 = img->Get16BitDataRed();
		uint16_t * greenData16 = img->Get16BitDataGreen();
		uint16_t * blueData16 = img->Get16BitDataBlue();

		for (int i = 0; i < dataSize; i++) {

			// Perform whole contrast on red, green and blue
			tempRed = (allContrast * (redData16[i] - 32768)) + 32768;
			tempGreen = (allContrast * (greenData16[i] - 32768)) + 32768;
			tempBlue = (allContrast * (blueData16[i] - 32768)) + 32768;

			// Perform individual contrast on red, green and blue
			tempRed = (redContrast * (tempRed - 32768)) + 32768;
			tempGreen = (greenContrast * (tempGreen - 32768)) + 32768;
			tempBlue = (blueContrast * (tempBlue - 32768)) + 32768;

			// handle overflow or underflow
			tempRed = (tempRed > 65535) ? 65535 : tempRed;
			tempGreen = (tempGreen > 65535) ? 65535 : tempGreen;
			tempBlue = (tempBlue > 65535) ? 65535 : tempBlue;

			tempRed = (tempRed < 0) ? 0 : tempRed;
			tempGreen = (tempGreen < 0) ? 0 : tempGreen;
			tempBlue = (tempBlue < 0) ? 0 : tempBlue;

			// Set the new pixel to the 16 bit data
			redData16[i] = (uint16_t)tempRed;
			greenData16[i] = (uint16_t)tempGreen;
			blueData16[i] = (uint16_t)tempBlue;
		}
	}
}

void Processor::ConvertGreyscale(double redScale, double greenScale, double blueScale, int dataStart, int dataEnd) {

	// Get number of pixels for the image
	int dataSize = img->GetWidth() * img->GetHeight();
	if (dataStart < 0 || dataEnd < 0) {
		dataStart = 0;
		dataEnd = dataSize;
	}

	// Get pointers to 8 bit data
	uint8_t * redData8 = img->Get8BitDataRed();
	uint8_t * greenData8 = img->Get8BitDataGreen();
	uint8_t * blueData8 = img->Get8BitDataBlue();

	// Create a temporary set of pixels that will allow handling of overflow
	int tempGrey;

	for (int i = dataStart; i < dataEnd; i++) {

		// Perform greyscale conversion
		tempGrey = (redScale * (redData8[i])) + (greenScale * (greenData8[i])) + (blueScale * (blueData8[i]));
	
		// handle overflow or underflow
		tempGrey = (tempGrey > 255) ? 255 : tempGrey;
		tempGrey = (tempGrey < 0) ? 0 : tempGrey;

		// Set the new pixel to the 8 bit data
		redData8[i] = (uint8_t)tempGrey;
		greenData8[i] = (uint8_t)tempGrey;
		blueData8[i] = (uint8_t)tempGrey;
	}

	// If 16 bit image is enabled
	if (img->GetColorDepth() == 16) {

		// Get pointers to 16 bit data
		uint16_t * redData16 = img->Get16BitDataRed();
		uint16_t * greenData16 = img->Get16BitDataGreen();
		uint16_t * blueData16 = img->Get16BitDataBlue();

		for (int i = 0; i < dataSize; i++) {

			// Perform greyscale conversion
			tempGrey = (redScale * (redData16[i])) + (greenScale * (greenData16[i])) + (blueScale * (blueData16[i]));

			// handle overflow or underflow
			tempGrey = (tempGrey > 65535) ? 65535 : tempGrey;
			tempGrey = (tempGrey < 0) ? 0 : tempGrey;

			// Set the new pixel to the 16 bit data
			redData16[i] = (uint16_t)tempGrey;
			greenData16[i] = (uint16_t)tempGrey;
			blueData16[i] = (uint16_t)tempGrey;
		}
	}
}

void Processor::ChannelScale(double redRedScale, double redGreenScale, double redBlueScale,
	double greenRedScale, double greenGreenScale, double greenBlueScale,
	double blueRedScale, double blueGreenScale, double blueBlueScale,
	int dataStart, int dataEnd) {

	// Get number of pixels for the image
	int dataSize = img->GetWidth() * img->GetHeight();
	if (dataStart < 0 || dataEnd < 0) {
		dataStart = 0;
		dataEnd = dataSize;
	}

	// Get pointers to 8 bit data
	uint8_t * redData8 = img->Get8BitDataRed();
	uint8_t * greenData8 = img->Get8BitDataGreen();
	uint8_t * blueData8 = img->Get8BitDataBlue();

	int32_t tempRed;
	int32_t tempGreen;
	int32_t tempBlue;

	for (int i = dataStart; i < dataEnd; i++) {

		// Perform transform
		tempRed = (redRedScale * (redData8[i])) + (redGreenScale * (greenData8[i])) + (redBlueScale * (blueData8[i]));
		tempGreen = (greenRedScale * (redData8[i])) + (greenGreenScale * (greenData8[i])) + (greenBlueScale * (blueData8[i]));
		tempBlue = (blueRedScale * (redData8[i])) + (blueGreenScale * (greenData8[i])) + (blueBlueScale * (blueData8[i]));

		// handle overflow or underflow
		tempRed = (tempRed > 255) ? 255 : tempRed;
		tempGreen = (tempGreen > 255) ? 255 : tempGreen;
		tempBlue = (tempBlue > 255) ? 255 : tempBlue;

		tempRed = (tempRed < 0) ? 0 : tempRed;
		tempGreen = (tempGreen < 0) ? 0 : tempGreen;
		tempBlue = (tempBlue < 0) ? 0 : tempBlue;

		// Set the new pixel to the 8 bit data
		redData8[i] = (uint8_t)tempRed;
		greenData8[i] = (uint8_t)tempGreen;
		blueData8[i] = (uint8_t)tempBlue;
	}

	// If 16 bit image is enabled
	if (img->GetColorDepth() == 16) {

		// Get pointers to 16 bit data
		uint16_t * redData16 = img->Get16BitDataRed();
		uint16_t * greenData16 = img->Get16BitDataGreen();
		uint16_t * blueData16 = img->Get16BitDataBlue();

		for (int i = 0; i < dataSize; i++) {

			// Perform transform
			tempRed = (redRedScale * (redData16[i])) + (redGreenScale * (greenData16[i])) + (redBlueScale * (blueData16[i]));
			tempGreen = (greenRedScale * (redData16[i])) + (greenGreenScale * (greenData16[i])) + (greenBlueScale * (blueData16[i]));
			tempBlue = (blueRedScale * (redData16[i])) + (blueGreenScale * (greenData16[i])) + (blueBlueScale * (blueData16[i]));

			// handle overflow or underflow
			tempRed = (tempRed > 65535) ? 65535 : tempRed;
			tempGreen = (tempGreen > 65535) ? 65535 : tempGreen;
			tempBlue = (tempBlue > 65535) ? 65535 : tempBlue;

			tempRed = (tempRed < 0) ? 0 : tempRed;
			tempGreen = (tempGreen < 0) ? 0 : tempGreen;
			tempBlue = (tempBlue < 0) ? 0 : tempBlue;

			// Set the new pixel to the 16 bit data
			redData16[i] = (uint16_t)tempRed;
			greenData16[i] = (uint16_t)tempGreen;
			blueData16[i] = (uint16_t)tempBlue;
		}
	}
}

void Processor::Rotate90CW() {

	int width = img->GetWidth();
	int height = img->GetHeight();
	int dataSize = width * height;

	int x = 0;
	int y = 0;
	int offset = 0;

	// Get pointers to 8 bit data
	uint8_t * redData8 = img->Get8BitDataRed();
	uint8_t * greenData8 = img->Get8BitDataGreen();
	uint8_t * blueData8 = img->Get8BitDataBlue();

	Image * tempImage = new Image(*img);
	uint8_t * redData8Dup = tempImage->Get8BitDataRed();
	uint8_t * greenData8Dup = tempImage->Get8BitDataGreen();
	uint8_t * blueData8Dup = tempImage->Get8BitDataBlue();

	
	for (int i = 0; i < dataSize; i++) {

		// Get x and y coordinates from current index of data
		x = i % width;
		y = i / width;
		offset = height * x + y;

		redData8[offset] = redData8Dup[width * (height - 1 - y) + x];
		greenData8[offset] = greenData8Dup[width * (height - 1 - y) + x];
		blueData8[offset] = blueData8Dup[width * (height - 1 - y) + x];
	}

	// If 16 bit image is enabled
	if (img->GetColorDepth() == 16) {

		// Get pointers to 16 bit data
		uint16_t * redData16 = img->Get16BitDataRed();
		uint16_t * greenData16 = img->Get16BitDataGreen();
		uint16_t * blueData16 = img->Get16BitDataBlue();

		uint16_t * redData16Dup = tempImage->Get16BitDataRed();
		uint16_t * greenData16Dup = tempImage->Get16BitDataGreen();
		uint16_t * blueData16Dup = tempImage->Get16BitDataBlue();

		for (int i = 0; i < dataSize; i++) {

			// Get x and y coordinates from current index of data
			x = i % width;
			y = i / width;
			offset = height * x + y;

			redData16[offset] = redData16Dup[width * (height - 1 - y) + x];
			greenData16[offset] = greenData16Dup[width * (height - 1 - y) + x];
			blueData16[offset] = blueData16Dup[width * (height - 1 - y) + x];
		}
	}

	img->SetWidth(height);
	img->SetHeight(width);

	delete tempImage;
}

void Processor::Rotate180() {

	int width = img->GetWidth();
	int height = img->GetHeight();
	int dataSize = width * height;

	// Get pointers to 8 bit data
	uint8_t * redData8 = img->Get8BitDataRed();
	uint8_t * greenData8 = img->Get8BitDataGreen();
	uint8_t * blueData8 = img->Get8BitDataBlue();

	Image * tempImage = new Image(*img);
	uint8_t * redData8Dup = tempImage->Get8BitDataRed();
	uint8_t * greenData8Dup = tempImage->Get8BitDataGreen();
	uint8_t * blueData8Dup = tempImage->Get8BitDataBlue();

	for (int i = 0; i < dataSize; i++) {

		redData8[i] = redData8Dup[dataSize -i -1];
		greenData8[i] = greenData8Dup[dataSize - i - 1];
		blueData8[i] = blueData8Dup[dataSize - i - 1];
	}

	// If 16 bit image is enabled
	if (img->GetColorDepth() == 16) {

		// Get pointers to 16 bit data
		uint16_t * redData16 = img->Get16BitDataRed();
		uint16_t * greenData16 = img->Get16BitDataGreen();
		uint16_t * blueData16 = img->Get16BitDataBlue();

		uint16_t * redData16Dup = tempImage->Get16BitDataRed();
		uint16_t * greenData16Dup = tempImage->Get16BitDataGreen();
		uint16_t * blueData16Dup = tempImage->Get16BitDataBlue();

		for (int i = 0; i < dataSize; i++) {

			redData16[i] = redData16Dup[dataSize - i - 1];
			greenData16[i] = greenData16Dup[dataSize - i - 1];
			blueData16[i] = blueData16Dup[dataSize - i - 1];
		}
	}

	delete tempImage;
}

void Processor::RotateCustom(double angleDegrees) {

	angleDegrees *= -1.0;
	double angleSin = sin(angleDegrees * pi / 180.0);
	double angleCos = cos(angleDegrees * pi / 180.0);
	int pivotX = img->GetWidth() / 2;
	int pivotY = img->GetHeight() / 2;

	int width = img->GetWidth();
	int height = img->GetHeight();
	int dataSize = width * height;
	int x = 0;
	int y = 0;
	int newX = 0;
	int newY = 0;

	// Get pointers to 8 bit data
	uint8_t * redData8 = img->Get8BitDataRed();
	uint8_t * greenData8 = img->Get8BitDataGreen();
	uint8_t * blueData8 = img->Get8BitDataBlue();

	Image * tempImage = new Image(*img);
	uint8_t * redData8Dup = tempImage->Get8BitDataRed();
	uint8_t * greenData8Dup = tempImage->Get8BitDataGreen();
	uint8_t * blueData8Dup = tempImage->Get8BitDataBlue();

	// Set duplicate data to 0
	for (int i = 0; i < dataSize; i++) {
		redData8Dup[i] = 0;
		greenData8Dup[i] = 0;
		blueData8Dup[i] = 0;
	}

	int newI = 0;
	for (int i = 0; i < dataSize; i++) {

		// Get x and y coordinates from current index of data
		x = i % width;
		y = i / width;
		
		// Shift by pivot point
		x -= pivotX;
		y -= pivotY;

		// Rotate point
		newX = (x * angleCos) - (y * angleSin);
		newY = (x * angleSin) + (y * angleCos);

		// Shift back
		newX += pivotX;
		newY += pivotY;

		// Veirfy pixel location is in bounds of original image size
		if(newX > 0 && newX < width && newY > 0 && newY < height){

			// Get new single dimmension array index from new x and y location
			newI = newY * width + newX;

			// Copy pixel from old location to new location
			redData8Dup[i] = redData8[newI];
			greenData8Dup[i] = greenData8[newI];
			blueData8Dup[i] = blueData8[newI];
		}
	}

	for (int i = 0; i < dataSize; i++) {

		// Get red, green and blue temporary pixels
		redData8[i] = redData8Dup[i];
		greenData8[i] = greenData8Dup[i];
		blueData8[i] = blueData8Dup[i];

	}

	// If 16 bit image is enabled
	if (img->GetColorDepth() == 16) {

		// Get pointers to 16 bit data
		uint16_t * redData16 = img->Get16BitDataRed();
		uint16_t * greenData16 = img->Get16BitDataGreen();
		uint16_t * blueData16 = img->Get16BitDataBlue();

		uint16_t * redData16Dup = tempImage->Get16BitDataRed();
		uint16_t * greenData16Dup = tempImage->Get16BitDataGreen();
		uint16_t * blueData16Dup = tempImage->Get16BitDataBlue();

		// Set duplicate data to 0
		for (int i = 0; i < dataSize; i++) {
			redData16Dup[i] = 0;
			greenData16Dup[i] = 0;
			blueData16Dup[i] = 0;
		}

		for (int i = 0; i < dataSize; i++) {

			// Get x and y coordinates from current index of data
			x = i % width;
			y = i / width;

			// Shift by pivot point
			x -= pivotX;
			y -= pivotY;
			
			// rotate point
			newX = (x * angleCos) - (y * angleSin);
			newY = (x * angleSin) + (y * angleCos);

			// Shift back
			newX += pivotX;
			newY += pivotY;

			// Veirfy pixel location is in bounds of original image size
			if (newX > 0 && newX < width && newY > 0 && newY < height) {

				// Get new single dimmension array index from new x and y location
				newI = newY * width + newX;

				// Copy pixel from old location to new location
				redData16Dup[i] = redData16[newI];
				greenData16Dup[i] = greenData16[newI];
				blueData16Dup[i] = blueData16[newI];
			}
		}

		// Copy data from duplicate back to original image
		for (int i = 0; i < dataSize; i++) {

			// Get red, green and blue temporary pixels
			redData16[i] = redData16Dup[i];
			greenData16[i] = greenData16Dup[i];
			blueData16[i] = blueData16Dup[i];
		}
	}

	delete tempImage;
}

void Processor::RotateCustomBilinear(double angleDegrees) {

	double useNearestNeighborDistance = 0.000000001;

	angleDegrees *= -1.0;
	double angleSin = sin(angleDegrees * pi / 180.0);
	double angleCos = cos(angleDegrees * pi / 180.0);
	double pivotX = (double)img->GetWidth() / 2.0;
	double pivotY = (double)img->GetHeight() / 2.0;

	int width = img->GetWidth();
	int height = img->GetHeight();
	int dataSize = width * height;
	int x = 0;
	int y = 0;
	double newX = 0;
	double newY = 0;

	int32_t tempRed;
	int32_t tempGreen;
	int32_t tempBlue;

	// Get pointers to 8 bit data
	uint8_t * redData8 = img->Get8BitDataRed();
	uint8_t * greenData8 = img->Get8BitDataGreen();
	uint8_t * blueData8 = img->Get8BitDataBlue();

	Image * tempImage = new Image(*img);
	uint8_t * redData8Dup = tempImage->Get8BitDataRed();
	uint8_t * greenData8Dup = tempImage->Get8BitDataGreen();
	uint8_t * blueData8Dup = tempImage->Get8BitDataBlue();

	// Set duplicate data to 0
	for (int i = 0; i < dataSize; i++) {
		redData8Dup[i] = 0;
		greenData8Dup[i] = 0;
		blueData8Dup[i] = 0;
	}

	// Neighbors of exact rotation location
	int point1 = 0;
	int point2 = 0;
	int point3 = 0;
	int point4 = 0;

	// Distances from exact point to nearest points to interpolate
	double distance1 = 0.0;
	double distance2 = 0.0;
	double distance3 = 0.0;
	double distance4 = 0.0;

	// Actual index of data of neighbors to interpoalte
	int newI1 = 0;
	int newI2 = 0;
	int newI3 = 0;
	int newI4 = 0;

	// Weights each neighbor will recieve based on distance
	double weight1 = 0.0;
	double weight2 = 0.0;
	double weight3 = 0.0;
	double weight4 = 0.0;
	double weightSum = 1.0;

	for (int i = 0; i < dataSize; i++) {

		// Get x and y coordinates from current index of data
		x = i % width;
		y = i / width;

		// Shift by pivot point
		x -= pivotX;
		y -= pivotY;

		// Rotate point
		newX = ((double)x * angleCos) - ((double)y * angleSin);
		newY = ((double)x * angleSin) + ((double)y * angleCos);

		// Shift back
		newX += pivotX;
		newY += pivotY;

		// Veirfy pixel location is in bounds of original image size
		if (newX > 0 && newX < width && newY > 0 && newY < height) {

			// new X is not on border of image
			if (newX > 0 && newX < width - 1) {
				point1 = wxRound(floor(newX));
				point2 = wxRound(ceil(newX));
			}

			// new X is on border.
			else {
				point1 = wxRound(newX);
				point2 = wxRound(newX);
			}

			// new Y is not on border of image
			if (newY > 0 && newY < height - 1) {
				point3 = wxRound(floor(newY));
				point4 = wxRound(ceil(newY));
			}

			// new Y is on border.
			else {
				point3 = wxRound(newY);
				point4 = wxRound(newY);
			}

			// Get distances between actual point and rounded points
			distance1 = ((newX - point1) * (newX - point1)) + ((newY - point3) * (newY - point3));
			distance2 = ((newX - point2) * (newX - point2)) + ((newY - point3) * (newY - point3));
			distance3 = ((newX - point2) * (newX - point2)) + ((newY - point4) * (newY - point4));
			distance4 = ((newX - point1) * (newX - point1)) + ((newY - point4) * (newY - point4));

			// If distance 1 is close enough to an actual point, use that closest point
			if (distance1 < useNearestNeighborDistance) {

				newI1 = point3 * width + point1;
				redData8Dup[i] = redData8[newI1];
				greenData8Dup[i] = greenData8[newI1];
				blueData8Dup[i] = blueData8[newI1];
			}

			// If distance 2 is close enough to an actual point, use that closest point
			if (distance2 < useNearestNeighborDistance) {

				newI2 = point3 * width + point2;
				redData8Dup[i] = redData8[newI2];
				greenData8Dup[i] = greenData8[newI2];
				blueData8Dup[i] = blueData8[newI2];
			}

			// If distance 3 is close enough to an actual point, use that closest point
			if (distance3 < useNearestNeighborDistance) {

				newI3 = point4 * width + point1;
				redData8Dup[i] = redData8[newI3];
				greenData8Dup[i] = greenData8[newI3];
				blueData8Dup[i] = blueData8[newI3];
			}

			// If distance 4 is close enough to an actual point, use that closest point
			if (distance4 < useNearestNeighborDistance) {

				newI4 = point4 * width + point1;
				redData8Dup[i] = redData8[newI4];
				greenData8Dup[i] = greenData8[newI4];
				blueData8Dup[i] = blueData8[newI4];
			}

			// Calculate weights to scale data 
			weight1 = 1.0 / distance1;
			weight2 = 1.0 / distance2;
			weight3 = 1.0 / distance3;
			weight4 = 1.0 / distance4;
			weightSum = weight1 + weight2 + weight3 + weight4;

			// Get new single dimmension array index from new x and y location
			newI1 = point3 * width + point1;
			newI2 = point3 * width + point2;
			newI3 = point4 * width + point1;
			newI4 = point4 * width + point2;

			// Interpolate the data using weights calculated from distances above
			tempRed = ((weight1 * redData8[newI1]) + (weight2 * redData8[newI2]) + (weight3 * redData8[newI3]) + (weight4 * redData8[newI4])) / weightSum;
			tempGreen = ((weight1 * greenData8[newI1]) + (weight2 * greenData8[newI2]) + (weight3 * greenData8[newI3]) + (weight4 * greenData8[newI4])) / weightSum;
			tempBlue = ((weight1 * blueData8[newI1]) + (weight2 * blueData8[newI2]) + (weight3 * blueData8[newI3]) + (weight4 * blueData8[newI4])) / weightSum;
			
			// handle overflow or underflow
			tempRed = (tempRed > 255) ? 255 : tempRed;
			tempGreen = (tempGreen > 255) ? 255 : tempGreen;
			tempBlue = (tempBlue > 255) ? 255 : tempBlue;

			tempRed = (tempRed < 0) ? 0 : tempRed;
			tempGreen = (tempGreen < 0) ? 0 : tempGreen;
			tempBlue = (tempBlue < 0) ? 0 : tempBlue;

			// Set the new pixel to the 8 bit data
			redData8Dup[i] = (uint8_t)tempRed;
			greenData8Dup[i] = (uint8_t)tempGreen;
			blueData8Dup[i] = (uint8_t)tempBlue;
		}
	}

	// Copy from duplicate data back to original data
	for (int i = 0; i < dataSize; i++) {

		// Get red, green and blue temporary pixels
		redData8[i] = redData8Dup[i];
		greenData8[i] = greenData8Dup[i];
		blueData8[i] = blueData8Dup[i];
	}

	// If 16 bit image is enabled
	if (img->GetColorDepth() == 16) {

		// Get pointers to 16 bit data
		uint16_t * redData16 = img->Get16BitDataRed();
		uint16_t * greenData16 = img->Get16BitDataGreen();
		uint16_t * blueData16 = img->Get16BitDataBlue();

		uint16_t * redData16Dup = tempImage->Get16BitDataRed();
		uint16_t * greenData16Dup = tempImage->Get16BitDataGreen();
		uint16_t * blueData16Dup = tempImage->Get16BitDataBlue();

		// Set duplicate data to 0
		for (int i = 0; i < dataSize; i++) {
			redData16Dup[i] = 0;
			greenData16Dup[i] = 0;
			blueData16Dup[i] = 0;
		}

		for (int i = 0; i < dataSize; i++) {

			// Get x and y coordinates from current index of data
			x = i % width;
			y = i / width;

			// Shift by pivot point
			x -= pivotX;
			y -= pivotY;

			// Rotate point
			newX = ((double)x * angleCos) - ((double)y * angleSin);
			newY = ((double)x * angleSin) + ((double)y * angleCos);

			// Shift back
			newX += pivotX;
			newY += pivotY;

			// Veirfy pixel location is in bounds of original image size
			if (newX > 0 && newX < width && newY > 0 && newY < height) {

				// new X is not on border of image
				if (newX > 0 && newX < width - 1) {
					point1 = wxRound(floor(newX));
					point2 = wxRound(ceil(newX));
				}

				// new X is on border.
				else {
					point1 = wxRound(newX);
					point2 = wxRound(newX);
				}

				// new Y is not on border of image
				if (newY > 0 && newY < height - 1) {
					point3 = wxRound(floor(newY));
					point4 = wxRound(ceil(newY));
				}

				// new Y is on border.
				else {
					point3 = wxRound(newY);
					point4 = wxRound(newY);
				}

				// Get distances between actual point and rounded points
				distance1 = ((newX - point1) * (newX - point1)) + ((newY - point3) * (newY - point3));
				distance2 = ((newX - point2) * (newX - point2)) + ((newY - point3) * (newY - point3));
				distance3 = ((newX - point2) * (newX - point2)) + ((newY - point4) * (newY - point4));
				distance4 = ((newX - point1) * (newX - point1)) + ((newY - point4) * (newY - point4));

				// If distance 1 is close enough to an actual point, use that closest point
				if (distance1 < useNearestNeighborDistance) {

					newI1 = point3 * width + point1;
					redData16Dup[i] = redData16[newI1];
					greenData16Dup[i] = greenData16[newI1];
					blueData16Dup[i] = blueData16[newI1];
				}

				// If distance 2 is close enough to an actual point, use that closest point
				if (distance2 < useNearestNeighborDistance) {

					newI2 = point3 * width + point2;
					redData16Dup[i] = redData16[newI2];
					greenData16Dup[i] = greenData16[newI2];
					blueData16Dup[i] = blueData16[newI2];
				}

				// If distance 3 is close enough to an actual point, use that closest point
				if (distance3 < useNearestNeighborDistance) {

					newI3 = point4 * width + point1;
					redData16Dup[i] = redData16[newI3];
					greenData16Dup[i] = greenData16[newI3];
					blueData16Dup[i] = blueData16[newI3];
				}

				// If distance 4 is close enough to an actual point, use that closest point
				if (distance4 < useNearestNeighborDistance) {

					newI4 = point4 * width + point1;
					redData16Dup[i] = redData16[newI4];
					greenData16Dup[i] = greenData16[newI4];
					blueData16Dup[i] = blueData16[newI4];
				}

				// Calculate weights to scale data 
				weight1 = 1.0 / distance1;
				weight2 = 1.0 / distance2;
				weight3 = 1.0 / distance3;
				weight4 = 1.0 / distance4;
				weightSum = weight1 + weight2 + weight3 + weight4;

				// Get new single dimmension array index from new x and y location
				newI1 = point3 * width + point1;
				newI2 = point3 * width + point2;
				newI3 = point4 * width + point1;
				newI4 = point4 * width + point2;

				// Interpolate the data using weights calculated from distances above
				tempRed = ((weight1 * redData16[newI1]) + (weight2 * redData16[newI2]) + (weight3 * redData16[newI3]) + (weight4 * redData16[newI4])) / weightSum;
				tempGreen = ((weight1 * greenData16[newI1]) + (weight2 * greenData16[newI2]) + (weight3 * greenData16[newI3]) + (weight4 * greenData16[newI4])) / weightSum;
				tempBlue = ((weight1 * blueData16[newI1]) + (weight2 * blueData16[newI2]) + (weight3 * blueData16[newI3]) + (weight4 * blueData16[newI4])) / weightSum;

				// handle overflow or underflow
				tempRed = (tempRed > 65535) ? 65535 : tempRed;
				tempGreen = (tempGreen > 65535) ? 65535 : tempGreen;
				tempBlue = (tempBlue > 65535) ? 65535 : tempBlue;

				tempRed = (tempRed < 0) ? 0 : tempRed;
				tempGreen = (tempGreen < 0) ? 0 : tempGreen;
				tempBlue = (tempBlue < 0) ? 0 : tempBlue;

				// Set the new pixel to the 8 bit data
				redData16Dup[i] = (uint16_t)tempRed;
				greenData16Dup[i] = (uint16_t)tempGreen;
				blueData16Dup[i] = (uint16_t)tempBlue;
			}
		}

		// Copy from duplicate data back to original data
		for (int i = 0; i < dataSize; i++) {

			// Get red, green and blue temporary pixels
			redData16[i] = redData16Dup[i];
			greenData16[i] = greenData16Dup[i];
			blueData16[i] = blueData16Dup[i];
		}
	}

	delete tempImage;
}

void Processor::RotateCustomBicubic(double angleDegrees) {

	double useNearestNeighborDistance = 0.000000001;

	angleDegrees *= -1.0;
	double angleSin = sin(angleDegrees * pi / 180.0);
	double angleCos = cos(angleDegrees * pi / 180.0);
	double pivotX = (double)img->GetWidth() / 2.0;
	double pivotY = (double)img->GetHeight() / 2.0;

	int width = img->GetWidth();
	int height = img->GetHeight();
	int dataSize = width * height;
	int x = 0;
	int y = 0;
	double newX = 0;
	double newY = 0;

	int32_t tempRed;
	int32_t tempGreen;
	int32_t tempBlue;

	// Get pointers to 8 bit data
	uint8_t * redData8 = img->Get8BitDataRed();
	uint8_t * greenData8 = img->Get8BitDataGreen();
	uint8_t * blueData8 = img->Get8BitDataBlue();

	Image * tempImage = new Image(*img);
	uint8_t * redData8Dup = tempImage->Get8BitDataRed();
	uint8_t * greenData8Dup = tempImage->Get8BitDataGreen();
	uint8_t * blueData8Dup = tempImage->Get8BitDataBlue();

	// Set duplicate data to 0
	for (int i = 0; i < dataSize; i++) {
		redData8Dup[i] = 0;
		greenData8Dup[i] = 0;
		blueData8Dup[i] = 0;
	}

	int point1 = 0; int point2 = 0; int point3 = 0; int point4 = 0;
	int point5 = 0; int point6 = 0; int point7 = 0; int point8 = 0;

	double distance1 = 0.0; double distance2 = 0.0; double distance3 = 0.0; double distance4 = 0.0;
	double distance5 = 0.0; double distance6 = 0.0; double distance7 = 0.0; double distance8 = 0.0;
	double distance9 = 0.0; double distance10 = 0.0; double distance11 = 0.0; double distance12 = 0.0;
	double distance13 = 0.0; double distance14 = 0.0; double distance15 = 0.0; double distance16 = 0.0;

	int newI1 = 0; int newI2 = 0; int newI3 = 0; int newI4 = 0;
	int newI5 = 0; int newI6 = 0; int newI7 = 0; int newI8 = 0;
	int newI9 = 0; int newI10 = 0; int newI11 = 0; int newI12 = 0;
	int newI13 = 0; int newI14 = 0; int newI15 = 0; int newI16 = 0;

	double weight1 = 0.0; double weight2 = 0.0; double weight3 = 0.0; double weight4 = 0.0;
	double weight5 = 0.0; double weight6 = 0.0; double weight7 = 0.0; double weight8 = 0.0;
	double weight9 = 0.0; double weight10 = 0.0; double weight11 = 0.0; double weight12 = 0.0;
	double weight13 = 0.0; double weight14 = 0.0; double weight15 = 0.0; double weight16 = 0.0;

	double weightSum = 1.0;

	for (int i = 0; i < dataSize; i++) {

		// Get x and y coordinates from current index of data
		x = i % width;
		y = i / width;

		// Shift by pivot point
		x -= pivotX;
		y -= pivotY;

		// Rotate point
		newX = ((double)x * angleCos) - ((double)y * angleSin);
		newY = ((double)x * angleSin) + ((double)y * angleCos);

		// Shift back
		newX += pivotX;
		newY += pivotY;

		// Veirfy pixel location is in bounds of original image size
		if (newX > 0 && newX < width && newY > 0 && newY < height) {

			// new X is not on border of image
			if (newX > 1 && newX < width - 2) {
				point1 = wxRound(floor(newX)) - 1;
				point2 = wxRound(floor(newX));
				point3 = wxRound(ceil(newX));
				point4 = wxRound(ceil(newX)) + 1;
			}

			// new X is on border.
			else {
				point1 = wxRound(newX);
				point2 = wxRound(newX);
				point3 = wxRound(newX);
				point4 = wxRound(newX);
			}

			// new Y is not on border of image
			if (newY > 1 && newY < height - 2) {
				point5 = wxRound(floor(newY))-1;
				point6 = wxRound(floor(newY));
				point7 = wxRound(ceil(newY));
				point8 = wxRound(ceil(newY));
			}

			// new Y is on border.
			else {
				point5 = wxRound(newY);
				point6 = wxRound(newY);
				point7 = wxRound(newY);
				point8 = wxRound(newY);
			}

			// Get distances between actual point and rounded points
			distance1 = ((newX - point1) * (newX - point1)) + ((newY - point5) * (newY - point5));
			distance2 = ((newX - point2) * (newX - point2)) + ((newY - point5) * (newY - point5));
			distance3 = ((newX - point3) * (newX - point3)) + ((newY - point5) * (newY - point5));
			distance4 = ((newX - point4) * (newX - point4)) + ((newY - point5) * (newY - point5));

			distance5 = ((newX - point1) * (newX - point1)) + ((newY - point6) * (newY - point6));
			distance6 = ((newX - point2) * (newX - point2)) + ((newY - point6) * (newY - point6));
			distance7 = ((newX - point3) * (newX - point3)) + ((newY - point6) * (newY - point6));
			distance8 = ((newX - point4) * (newX - point4)) + ((newY - point6) * (newY - point6));

			distance9 = ((newX - point1) * (newX - point1)) + ((newY - point7) * (newY - point7));
			distance10 = ((newX - point2) * (newX - point2)) + ((newY - point7) * (newY - point7));
			distance11 = ((newX - point3) * (newX - point3)) + ((newY - point7) * (newY - point7));
			distance12 = ((newX - point4) * (newX - point4)) + ((newY - point7) * (newY - point7));

			distance13 = ((newX - point1) * (newX - point1)) + ((newY - point8) * (newY - point8));
			distance14 = ((newX - point2) * (newX - point2)) + ((newY - point8) * (newY - point8));
			distance15 = ((newX - point3) * (newX - point3)) + ((newY - point8) * (newY - point8));
			distance16 = ((newX - point4) * (newX - point4)) + ((newY - point8) * (newY - point8));

			// If distance 1 is close enough to an actual point, use that closest point
			if (distance1 < useNearestNeighborDistance) {

				newI1 = point5 * width + point1;
				redData8Dup[i] = redData8[newI1];
				greenData8Dup[i] = greenData8[newI1];
				blueData8Dup[i] = blueData8[newI1];
			}

			// If distance 2 is close enough to an actual point, use that closest point
			else if (distance2 < useNearestNeighborDistance) {

				newI2 = point5 * width + point2;
				redData8Dup[i] = redData8[newI2];
				greenData8Dup[i] = greenData8[newI2];
				blueData8Dup[i] = blueData8[newI2];
			}

			// If distance 3 is close enough to an actual point, use that closest point
			else if (distance3 < useNearestNeighborDistance) {

				newI3 = point5 * width + point3;
				redData8Dup[i] = redData8[newI3];
				greenData8Dup[i] = greenData8[newI3];
				blueData8Dup[i] = blueData8[newI3];
			}

			// If distance 4 is close enough to an actual point, use that closest point
			else if (distance4 < useNearestNeighborDistance) {

				newI4 = point5 * width + point4;
				redData8Dup[i] = redData8[newI4];
				greenData8Dup[i] = greenData8[newI4];
				blueData8Dup[i] = blueData8[newI4];
			}

			// If distance 5 is close enough to an actual point, use that closest point
			else if (distance5 < useNearestNeighborDistance) {

				newI5 = point6 * width + point1;
				redData8Dup[i] = redData8[newI5];
				greenData8Dup[i] = greenData8[newI5];
				blueData8Dup[i] = blueData8[newI5];
			}

			// If distance 6 is close enough to an actual point, use that closest point
			else if (distance6 < useNearestNeighborDistance) {

				newI6 = point6 * width + point2;
				redData8Dup[i] = redData8[newI6];
				greenData8Dup[i] = greenData8[newI6];
				blueData8Dup[i] = blueData8[newI6];
			}

			// If distance 7 is close enough to an actual point, use that closest point
			else if (distance7 < useNearestNeighborDistance) {

				newI7 = point6 * width + point3;
				redData8Dup[i] = redData8[newI3];
				greenData8Dup[i] = greenData8[newI3];
				blueData8Dup[i] = blueData8[newI3];
			}

			// If distance 8 is close enough to an actual point, use that closest point
			else if (distance8 < useNearestNeighborDistance) {

				newI8 = point6 * width + point4;
				redData8Dup[i] = redData8[newI8];
				greenData8Dup[i] = greenData8[newI8];
				blueData8Dup[i] = blueData8[newI8];
			}
			// If distance 9 is close enough to an actual point, use that closest point
			else if (distance9 < useNearestNeighborDistance) {

				newI9 = point7 * width + point1;
				redData8Dup[i] = redData8[newI9];
				greenData8Dup[i] = greenData8[newI9];
				blueData8Dup[i] = blueData8[newI9];
			}

			// If distance 10 is close enough to an actual point, use that closest point
			else if (distance10 < useNearestNeighborDistance) {

				newI10 = point7 * width + point2;
				redData8Dup[i] = redData8[newI10];
				greenData8Dup[i] = greenData8[newI10];
				blueData8Dup[i] = blueData8[newI10];
			}

			// If distance 11 is close enough to an actual point, use that closest point
			else if (distance11 < useNearestNeighborDistance) {

				newI11 = point7 * width + point3;
				redData8Dup[i] = redData8[newI11];
				greenData8Dup[i] = greenData8[newI11];
				blueData8Dup[i] = blueData8[newI11];
			}

			// If distance 12 is close enough to an actual point, use that closest point
			else if (distance12 < useNearestNeighborDistance) {

				newI12 = point7 * width + point4;
				redData8Dup[i] = redData8[newI12];
				greenData8Dup[i] = greenData8[newI12];
				blueData8Dup[i] = blueData8[newI12];
			}

			// If distance 13 is close enough to an actual point, use that closest point
			else if (distance13 < useNearestNeighborDistance) {

				newI13 = point8 * width + point1;
				redData8Dup[i] = redData8[newI13];
				greenData8Dup[i] = greenData8[newI13];
				blueData8Dup[i] = blueData8[newI13];
			}

			// If distance 14 is close enough to an actual point, use that closest point
			else if (distance14 < useNearestNeighborDistance) {

				newI14 = point8 * width + point2;
				redData8Dup[i] = redData8[newI14];
				greenData8Dup[i] = greenData8[newI14];
				blueData8Dup[i] = blueData8[newI14];
			}

			// If distance 15 is close enough to an actual point, use that closest point
			else if (distance15 < useNearestNeighborDistance) {

				newI15 = point8 * width + point3;
				redData8Dup[i] = redData8[newI15];
				greenData8Dup[i] = greenData8[newI15];
				blueData8Dup[i] = blueData8[newI15];
			}

			// If distance 16 is close enough to an actual point, use that closest point
			else if (distance16 < useNearestNeighborDistance) {

				newI16 = point8 * width + point4;
				redData8Dup[i] = redData8[newI16];
				greenData8Dup[i] = greenData8[newI16];
				blueData8Dup[i] = blueData8[newI16];
			}

			// Calculate weights to scale data 
			weight1 = 1.0 / distance1;  weight2 = 1.0 / distance2;  weight3 = 1.0 / distance3;  weight4 = 1.0 / distance4;
			weight5 = 1.0 / distance5;  weight6 = 1.0 / distance6;  weight7 = 1.0 / distance7;  weight8 = 1.0 / distance8;
			weight9 = 1.0 / distance9;  weight10 = 1.0 / distance10;  weight11 = 1.0 / distance11;  weight12 = 1.0 / distance12;
			weight13 = 1.0 / distance13;  weight14 = 1.0 / distance14;  weight15 = 1.0 / distance15;  weight16 = 1.0 / distance16;

			weightSum =  weight1 + weight2 + weight3 + weight4 + weight5 + weight6 + weight7 + weight8 + 
			             weight9 + weight10 + weight11 + weight12 + weight13 + weight14 + weight15 + weight16;

			// Get new single dimmension array index from new x and y location
			newI1 = point5 * width + point1;  newI2 = point5 * width + point2;  newI3 = point5 * width + point3;  newI4 = point5 * width + point4;  
			newI5 = point6 * width + point1;  newI6 = point6 * width + point2;  newI7 = point6 * width + point3;  newI8 = point6 * width + point4;
			newI9 = point7 * width + point1;  newI10 = point7 * width + point2;  newI11 = point7 * width + point3;  newI12 = point7 * width + point4;
			newI13 = point8 * width + point1;  newI14 = point8 * width + point2;  newI15 = point8 * width + point3;  newI16 = point8 * width + point4;

			// Interpolate the data using weights calculated from distances above
			tempRed = ((weight1 * redData8[newI1]) + (weight2 * redData8[newI2]) + (weight3 * redData8[newI3]) + (weight4 * redData8[newI4]) +
				(weight5 * redData8[newI5]) + (weight6 * redData8[newI6]) + (weight7 * redData8[newI7]) + (weight8 * redData8[newI8]) +
				(weight9 * redData8[newI9]) + (weight10 * redData8[newI10]) + (weight11 * redData8[newI11]) + (weight12 * redData8[newI12]) +
				(weight13 * redData8[newI13]) + (weight14 * redData8[newI14]) + (weight15 * redData8[newI15]) + (weight16 * redData8[newI16])) / weightSum;

			tempGreen = ((weight1 * greenData8[newI1]) + (weight2 * greenData8[newI2]) + (weight3 * greenData8[newI3]) + (weight4 * greenData8[newI4]) +
				(weight5 * greenData8[newI5]) + (weight6 * greenData8[newI6]) + (weight7 * greenData8[newI7]) + (weight8 * greenData8[newI8]) +
				(weight9 * greenData8[newI9]) + (weight10 * greenData8[newI10]) + (weight11 * greenData8[newI11]) + (weight12 * greenData8[newI12]) +
				(weight13 * greenData8[newI13]) + (weight14 * greenData8[newI14]) + (weight15 * greenData8[newI15]) + (weight16 * greenData8[newI16])) / weightSum;

			tempBlue = ((weight1 * blueData8[newI1]) + (weight2 * blueData8[newI2]) + (weight3 * blueData8[newI3]) + (weight4 * blueData8[newI4]) +
				(weight5 * blueData8[newI5]) + (weight6 * blueData8[newI6]) + (weight7 * blueData8[newI7]) + (weight8 * blueData8[newI8]) +
				(weight9 * blueData8[newI9]) + (weight10 * blueData8[newI10]) + (weight11 * blueData8[newI11]) + (weight12 * blueData8[newI12]) +
				(weight13 * blueData8[newI13]) + (weight14 * blueData8[newI14]) + (weight15 * blueData8[newI15]) + (weight16 * blueData8[newI16])) / weightSum;

			// handle overflow or underflow
			tempRed = (tempRed > 255) ? 255 : tempRed;
			tempGreen = (tempGreen > 255) ? 255 : tempGreen;
			tempBlue = (tempBlue > 255) ? 255 : tempBlue;

			tempRed = (tempRed < 0) ? 0 : tempRed;
			tempGreen = (tempGreen < 0) ? 0 : tempGreen;
			tempBlue = (tempBlue < 0) ? 0 : tempBlue;

			// Set the new pixel to the 8 bit data
			redData8Dup[i] = (uint8_t)tempRed;
			greenData8Dup[i] = (uint8_t)tempGreen;
			blueData8Dup[i] = (uint8_t)tempBlue;
		}
	}

	// Copy from duplicate data back to original data
	for (int i = 0; i < dataSize; i++) {

		// Get red, green and blue temporary pixels
		redData8[i] = redData8Dup[i];
		greenData8[i] = greenData8Dup[i];
		blueData8[i] = blueData8Dup[i];
	}

	// If 16 bit image is enabled
	if (img->GetColorDepth() == 16) {

		// Get pointers to 16 bit data
		uint16_t * redData16 = img->Get16BitDataRed();
		uint16_t * greenData16 = img->Get16BitDataGreen();
		uint16_t * blueData16 = img->Get16BitDataBlue();

		uint16_t * redData16Dup = tempImage->Get16BitDataRed();
		uint16_t * greenData16Dup = tempImage->Get16BitDataGreen();
		uint16_t * blueData16Dup = tempImage->Get16BitDataBlue();

		// Set duplicate data to 0
		for (int i = 0; i < dataSize; i++) {
			redData16Dup[i] = 0;
			greenData16Dup[i] = 0;
			blueData16Dup[i] = 0;
		}

		for (int i = 0; i < dataSize; i++) {

			// Get x and y coordinates from current index of data
			x = i % width;
			y = i / width;

			// Shift by pivot point
			x -= pivotX;
			y -= pivotY;

			// Rotate point
			newX = ((double)x * angleCos) - ((double)y * angleSin);
			newY = ((double)x * angleSin) + ((double)y * angleCos);

			// Shift back
			newX += pivotX;
			newY += pivotY;

			// Veirfy pixel location is in bounds of original image size
			if (newX > 0 && newX < width && newY > 0 && newY < height) {

				// new X is not on border of image
				if (newX > 1 && newX < width - 2) {
					point1 = wxRound(floor(newX)) - 1;
					point2 = wxRound(floor(newX));
					point3 = wxRound(ceil(newX));
					point4 = wxRound(ceil(newX)) + 1;
				}

				// new X is on border.
				else {
					point1 = wxRound(newX);
					point2 = wxRound(newX);
					point3 = wxRound(newX);
					point4 = wxRound(newX);
				}

				// new Y is not on border of image
				if (newY > 1 && newY < height - 2) {
					point5 = wxRound(floor(newY)) - 1;
					point6 = wxRound(floor(newY));
					point7 = wxRound(ceil(newY));
					point8 = wxRound(ceil(newY));
				}

				// new Y is on border.
				else {
					point5 = wxRound(newY);
					point6 = wxRound(newY);
					point7 = wxRound(newY);
					point8 = wxRound(newY);
				}

				// Get distances between actual point and rounded points
				distance1 = ((newX - point1) * (newX - point1)) + ((newY - point5) * (newY - point5));
				distance2 = ((newX - point2) * (newX - point2)) + ((newY - point5) * (newY - point5));
				distance3 = ((newX - point3) * (newX - point3)) + ((newY - point5) * (newY - point5));
				distance4 = ((newX - point4) * (newX - point4)) + ((newY - point5) * (newY - point5));

				distance5 = ((newX - point1) * (newX - point1)) + ((newY - point6) * (newY - point6));
				distance6 = ((newX - point2) * (newX - point2)) + ((newY - point6) * (newY - point6));
				distance7 = ((newX - point3) * (newX - point3)) + ((newY - point6) * (newY - point6));
				distance8 = ((newX - point4) * (newX - point4)) + ((newY - point6) * (newY - point6));

				distance9 = ((newX - point1) * (newX - point1)) + ((newY - point7) * (newY - point7));
				distance10 = ((newX - point2) * (newX - point2)) + ((newY - point7) * (newY - point7));
				distance11 = ((newX - point3) * (newX - point3)) + ((newY - point7) * (newY - point7));
				distance12 = ((newX - point4) * (newX - point4)) + ((newY - point7) * (newY - point7));

				distance13 = ((newX - point1) * (newX - point1)) + ((newY - point8) * (newY - point8));
				distance14 = ((newX - point2) * (newX - point2)) + ((newY - point8) * (newY - point8));
				distance15 = ((newX - point3) * (newX - point3)) + ((newY - point8) * (newY - point8));
				distance16 = ((newX - point4) * (newX - point4)) + ((newY - point8) * (newY - point8));

				// If distance 1 is close enough to an actual point, use that closest point
				if (distance1 < useNearestNeighborDistance) {

					newI1 = point5 * width + point1;
					redData16Dup[i] = redData16[newI1];
					greenData16Dup[i] = greenData16[newI1];
					blueData16Dup[i] = blueData16[newI1];
				}

				// If distance 2 is close enough to an actual point, use that closest point
				else if (distance2 < useNearestNeighborDistance) {

					newI2 = point5 * width + point2;
					redData16Dup[i] = redData16[newI2];
					greenData16Dup[i] = greenData16[newI2];
					blueData16Dup[i] = blueData16[newI2];
				}

				// If distance 3 is close enough to an actual point, use that closest point
				else if (distance3 < useNearestNeighborDistance) {

					newI3 = point5 * width + point3;
					redData16Dup[i] = redData16[newI3];
					greenData16Dup[i] = greenData16[newI3];
					blueData16Dup[i] = blueData16[newI3];
				}

				// If distance 4 is close enough to an actual point, use that closest point
				else if (distance4 < useNearestNeighborDistance) {

					newI4 = point5 * width + point4;
					redData16Dup[i] = redData16[newI4];
					greenData16Dup[i] = greenData16[newI4];
					blueData16Dup[i] = blueData16[newI4];
				}

				// If distance 5 is close enough to an actual point, use that closest point
				else if (distance5 < useNearestNeighborDistance) {

					newI5 = point6 * width + point1;
					redData16Dup[i] = redData16[newI5];
					greenData16Dup[i] = greenData16[newI5];
					blueData16Dup[i] = blueData16[newI5];
				}

				// If distance 6 is close enough to an actual point, use that closest point
				else if (distance6 < useNearestNeighborDistance) {

					newI6 = point6 * width + point2;
					redData16Dup[i] = redData16[newI6];
					greenData16Dup[i] = greenData16[newI6];
					blueData16Dup[i] = blueData16[newI6];
				}

				// If distance 7 is close enough to an actual point, use that closest point
				else if (distance7 < useNearestNeighborDistance) {

					newI7 = point6 * width + point3;
					redData16Dup[i] = redData16[newI3];
					greenData16Dup[i] = greenData16[newI3];
					blueData16Dup[i] = blueData16[newI3];
				}

				// If distance 8 is close enough to an actual point, use that closest point
				else if (distance8 < useNearestNeighborDistance) {

					newI8 = point6 * width + point4;
					redData16Dup[i] = redData16[newI8];
					greenData16Dup[i] = greenData16[newI8];
					blueData16Dup[i] = blueData16[newI8];
				}
				// If distance 9 is close enough to an actual point, use that closest point
				else if (distance9 < useNearestNeighborDistance) {

					newI9 = point7 * width + point1;
					redData16Dup[i] = redData16[newI9];
					greenData16Dup[i] = greenData16[newI9];
					blueData16Dup[i] = blueData16[newI9];
				}

				// If distance 10 is close enough to an actual point, use that closest point
				else if (distance10 < useNearestNeighborDistance) {

					newI10 = point7 * width + point2;
					redData16Dup[i] = redData16[newI10];
					greenData16Dup[i] = greenData16[newI10];
					blueData16Dup[i] = blueData16[newI10];
				}

				// If distance 11 is close enough to an actual point, use that closest point
				else if (distance11 < useNearestNeighborDistance) {

					newI11 = point7 * width + point3;
					redData16Dup[i] = redData16[newI11];
					greenData16Dup[i] = greenData16[newI11];
					blueData16Dup[i] = blueData16[newI11];
				}

				// If distance 12 is close enough to an actual point, use that closest point
				else if (distance12 < useNearestNeighborDistance) {

					newI12 = point7 * width + point4;
					redData16Dup[i] = redData16[newI12];
					greenData16Dup[i] = greenData16[newI12];
					blueData16Dup[i] = blueData16[newI12];
				}

				// If distance 13 is close enough to an actual point, use that closest point
				else if (distance13 < useNearestNeighborDistance) {

					newI13 = point8 * width + point1;
					redData16Dup[i] = redData16[newI13];
					greenData16Dup[i] = greenData16[newI13];
					blueData16Dup[i] = blueData16[newI13];
				}

				// If distance 14 is close enough to an actual point, use that closest point
				else if (distance14 < useNearestNeighborDistance) {

					newI14 = point8 * width + point2;
					redData16Dup[i] = redData16[newI14];
					greenData16Dup[i] = greenData16[newI14];
					blueData16Dup[i] = blueData16[newI14];
				}

				// If distance 15 is close enough to an actual point, use that closest point
				else if (distance15 < useNearestNeighborDistance) {

					newI15 = point8 * width + point3;
					redData16Dup[i] = redData16[newI15];
					greenData16Dup[i] = greenData16[newI15];
					blueData16Dup[i] = blueData16[newI15];
				}

				// If distance 16 is close enough to an actual point, use that closest point
				else if (distance16 < useNearestNeighborDistance) {

					newI16 = point8 * width + point4;
					redData16Dup[i] = redData16[newI16];
					greenData16Dup[i] = greenData16[newI16];
					blueData16Dup[i] = blueData16[newI16];
				}

				// Calculate weights to scale data 
				weight1 = 1.0 / distance1;  weight2 = 1.0 / distance2;  weight3 = 1.0 / distance3;  weight4 = 1.0 / distance4;
				weight5 = 1.0 / distance5;  weight6 = 1.0 / distance6;  weight7 = 1.0 / distance7;  weight8 = 1.0 / distance8;
				weight9 = 1.0 / distance9;  weight10 = 1.0 / distance10;  weight11 = 1.0 / distance11;  weight12 = 1.0 / distance12;
				weight13 = 1.0 / distance13;  weight14 = 1.0 / distance14;  weight15 = 1.0 / distance15;  weight16 = 1.0 / distance16;

				weightSum = weight1 + weight2 + weight3 + weight4 + weight5 + weight6 + weight7 + weight8 +
					weight9 + weight10 + weight11 + weight12 + weight13 + weight14 + weight15 + weight16;

				// Get new single dimmension array index from new x and y location
				newI1 = point5 * width + point1;  newI2 = point5 * width + point2;  newI3 = point5 * width + point3;  newI4 = point5 * width + point4;
				newI5 = point6 * width + point1;  newI6 = point6 * width + point2;  newI7 = point6 * width + point3;  newI8 = point6 * width + point4;
				newI9 = point7 * width + point1;  newI10 = point7 * width + point2;  newI11 = point7 * width + point3;  newI12 = point7 * width + point4;
				newI13 = point8 * width + point1;  newI14 = point8 * width + point2;  newI15 = point8 * width + point3;  newI16 = point8 * width + point4;

				// Interpolate the data using weights calculated from distances above
				tempRed = ((weight1 * redData16[newI1]) + (weight2 * redData16[newI2]) + (weight3 * redData16[newI3]) + (weight4 * redData16[newI4]) +
					(weight5 * redData16[newI5]) + (weight6 * redData16[newI6]) + (weight7 * redData16[newI7]) + (weight8 * redData16[newI8]) +
					(weight9 * redData16[newI9]) + (weight10 * redData16[newI10]) + (weight11 * redData16[newI11]) + (weight12 * redData16[newI12]) +
					(weight13 * redData16[newI13]) + (weight14 * redData16[newI14]) + (weight15 * redData16[newI15]) + (weight16 * redData16[newI16])) / weightSum;

				tempGreen = ((weight1 * greenData16[newI1]) + (weight2 * greenData16[newI2]) + (weight3 * greenData16[newI3]) + (weight4 * greenData16[newI4]) +
					(weight5 * greenData16[newI5]) + (weight6 * greenData16[newI6]) + (weight7 * greenData16[newI7]) + (weight8 * greenData16[newI8]) +
					(weight9 * greenData16[newI9]) + (weight10 * greenData16[newI10]) + (weight11 * greenData16[newI11]) + (weight12 * greenData16[newI12]) +
					(weight13 * greenData16[newI13]) + (weight14 * greenData16[newI14]) + (weight15 * greenData16[newI15]) + (weight16 * greenData16[newI16])) / weightSum;

				tempBlue = ((weight1 * blueData16[newI1]) + (weight2 * blueData16[newI2]) + (weight3 * blueData16[newI3]) + (weight4 * blueData16[newI4]) +
					(weight5 * blueData16[newI5]) + (weight6 * blueData16[newI6]) + (weight7 * blueData16[newI7]) + (weight8 * blueData16[newI8]) +
					(weight9 * blueData16[newI9]) + (weight10 * blueData16[newI10]) + (weight11 * blueData16[newI11]) + (weight12 * blueData16[newI12]) +
					(weight13 * blueData16[newI13]) + (weight14 * blueData16[newI14]) + (weight15 * blueData16[newI15]) + (weight16 * blueData16[newI16])) / weightSum;

				// handle overflow or underflow
				tempRed = (tempRed > 65535) ? 65535 : tempRed;
				tempGreen = (tempGreen > 65535) ? 65535 : tempGreen;
				tempBlue = (tempBlue > 65535) ? 65535 : tempBlue;

				tempRed = (tempRed < 0) ? 0 : tempRed;
				tempGreen = (tempGreen < 0) ? 0 : tempGreen;
				tempBlue = (tempBlue < 0) ? 0 : tempBlue;

				// Set the new pixel to the 8 bit data
				redData16Dup[i] = (uint16_t)tempRed;
				greenData16Dup[i] = (uint16_t)tempGreen;
				blueData16Dup[i] = (uint16_t)tempBlue;
			}
		}

		// Copy from duplicate data back to original data
		for (int i = 0; i < dataSize; i++) {

			// Get red, green and blue temporary pixels
			redData16[i] = redData16Dup[i];
			greenData16[i] = greenData16Dup[i];
			blueData16[i] = blueData16Dup[i];
		}
	}
	delete tempImage;
}

Processor::ProcessThread::ProcessThread(Processor * processor, ProcessorEdit * edit) : wxThread(wxTHREAD_DETACHED){
	procParent = processor;
	editVec.push_back(edit);
}

Processor::ProcessThread::ProcessThread(Processor * processor, wxVector<ProcessorEdit*> edits) : wxThread(wxTHREAD_DETACHED) {
	procParent = processor;
	editVec = edits;
}

wxThread::ExitCode Processor::ProcessThread::Entry() {

	while(procParent->GetLocked()) {
		this->Sleep(10);
	}
	procParent->Lock();
	procParent->RevertToOriginalImage();

	size_t numEdits = editVec.size();
	wxString numEditsStr;
	numEditsStr << numEdits;

	// Go through each edit one by one.  Each of these will invoke at least 1 child thread for the edit itself
	for (size_t editIndex = 0; editIndex < numEdits; editIndex++){

		// Get the next edit to take place
		ProcessorEdit * curEdit = editVec.at(editIndex);

		// Get the type of edit to perform
		int editToComplete = curEdit->GetEditType();

		wxString curEditStr;
		curEditStr << (editIndex + 1);

		wxString fullEditNumStr = " (" + curEditStr + "//" + numEditsStr + ")";
		
		switch (editToComplete) {

			// Peform a Shift Brightness edit
			case ProcessorEdit::EditType::SHIFT_BRIGHTNESS: {

				procParent->SendMessageToParent("Processing Shift Brightness Edit" + fullEditNumStr);

				// Get all parameters from the edit
				double allBrightShift = curEdit->GetParam(0);
				double redBrightShift = curEdit->GetParam(1);
				double greenBrightShift = curEdit->GetParam(2);
				double blueBrightShift = curEdit->GetParam(3);

				// Perform an edit on the data through the processor
				procParent->ShiftBrightness(allBrightShift, redBrightShift, greenBrightShift, blueBrightShift);
				procParent->SetUpdated(true);
			}
			break;

			// Peform a Scale Brightness edit
			case ProcessorEdit::EditType::SCALE_BRIGHTNESS: {

				procParent->SendMessageToParent("Processing Scale Brightness Edit" + fullEditNumStr);

				// Get all parameters from the edit
				double allBrightScale = curEdit->GetParam(0);
				double redBrightScale = curEdit->GetParam(1);
				double greenBrightScale = curEdit->GetParam(2);
				double blueBrightScale = curEdit->GetParam(3);

				// Perform an edit on the data through the processor
				procParent->ScaleBrightness(allBrightScale, redBrightScale, greenBrightScale, blueBrightScale);
				procParent->SetUpdated(true);
			}
			break;

			// Peform an Adjust Contrast edit
			case ProcessorEdit::EditType::ADJUST_CONTRAST: {

				procParent->SendMessageToParent("Processing Adjust Contrast Edit" + fullEditNumStr);

				// Get all parameters from the edit
				double allContrast = curEdit->GetParam(0);
				double redContrast = curEdit->GetParam(1);
				double greenContrast = curEdit->GetParam(2);
				double blueContrast = curEdit->GetParam(3);

				// Perform an edit on the data through the processor
				procParent->AdjustContrast(allContrast, redContrast, greenContrast, blueContrast);
				procParent->SetUpdated(true);
			}
			break;		

			// Peform a greyscale conversion, averaging RGB values
			case ProcessorEdit::EditType::CONVERT_GREYSCALE_AVG: {

				procParent->SendMessageToParent("Processing Greyscale (Average) Edit" + fullEditNumStr);

				// Perform an edit on the data through the processor
				procParent->ConvertGreyscale((1.0/3.0), (1.0 / 3.0), (1.0 / 3.0));
				procParent->SetUpdated(true);
			}
			break;

			// Peform a greyscale conversion, using humany eyesight values
			case ProcessorEdit::EditType::CONVERT_GREYSCALE_EYE: {

				procParent->SendMessageToParent("Processing Greyscale (Human Eyesight) Edit" + fullEditNumStr);

				// Perform an edit on the data through the processor
				procParent->ConvertGreyscale(0.2126, 0.7152, 0.0722);
				procParent->SetUpdated(true);
			}
			break;

			// Peform a greyscale conversion, using custom scalars
			case ProcessorEdit::EditType::CONVERT_GREYSCALE_CUSTOM: {

				procParent->SendMessageToParent("Processing Greyscale (Custom) Edit" + fullEditNumStr);

				// Get all parameters from the edit
				double redScale = curEdit->GetParam(0);
				double greenScale = curEdit->GetParam(1);
				double blueScale = curEdit->GetParam(2);

				// Perform an edit on the data through the processor
				procParent->ConvertGreyscale(redScale, greenScale, blueScale);
				procParent->SetUpdated(true);
			}
			break;

			// Peform a greyscale conversion, using custom scalars
			case ProcessorEdit::EditType::CHANNEL_TRANSFORM: {

				procParent->SendMessageToParent("Processing Channel Transform Edit" + fullEditNumStr);

				// Get all parameters from the edit
				double redRedScale = curEdit->GetParam(0);
				double redGreenScale = curEdit->GetParam(1);
				double redBlueScale = curEdit->GetParam(2);
				double greenRedScale = curEdit->GetParam(3);
				double greenGreenScale = curEdit->GetParam(4);
				double greenBlueScale = curEdit->GetParam(5);
				double blueRedScale = curEdit->GetParam(6);
				double blueGreenScale = curEdit->GetParam(7);
				double blueBlueScale = curEdit->GetParam(8);

				// Perform an edit on the data through the processor
				procParent->ChannelScale(redRedScale, redGreenScale, redBlueScale,
					greenRedScale, greenGreenScale, greenBlueScale,
					blueRedScale, blueGreenScale, blueBlueScale);

				procParent->SetUpdated(true);
			}
																	break;

			// Peform a 90 degree clockwise roctation
			case ProcessorEdit::EditType::ROTATE_90_CW: {

				procParent->SendMessageToParent("Processing Rotation Edit" + fullEditNumStr);

				// Perform an edit on the data through the processor
				procParent->Rotate90CW();
				procParent->SetUpdated(true);
			}
			break;

			// Peform a 180 degree clockwise roctation
			case ProcessorEdit::EditType::ROTATE_180: {

				procParent->SendMessageToParent("Processing Rotation Edit" + fullEditNumStr);

				// Perform an edit on the data through the processor
				procParent->Rotate180();
				procParent->SetUpdated(true);
			}
			break;

			// Peform a 270 degree clockwise roctation (90 counter clockwise)
			case ProcessorEdit::EditType::ROTATE_270_CW: {

				procParent->SendMessageToParent("Processing Rotation Edit" + fullEditNumStr);

				// Perform an edit on the data through the processor
				procParent->Rotate180();
				procParent->Rotate90CW();
				procParent->SetUpdated(true);
			}
			break;

			// Peform a custom angle clockwise roctation
			case ProcessorEdit::EditType::ROTATE_CUSTOM_NEAREST: {

				procParent->SendMessageToParent("Processing Rotation Edit" + fullEditNumStr);

				// Perform an edit on the data through the processor
				double angle = curEdit->GetParam(0);
				procParent->RotateCustom(angle);
				procParent->SetUpdated(true);
			}
			break;

			// Peform a custom angle clockwise roctation using bilinear interpolation
			case ProcessorEdit::EditType::ROTATE_CUSTOM_BILINEAR: {

				procParent->SendMessageToParent("Processing Rotation Edit" + fullEditNumStr);

				// Perform an edit on the data through the processor
				double angle = curEdit->GetParam(0);
				procParent->RotateCustomBilinear(angle);
				procParent->SetUpdated(true);
			}
			break;

			// Peform a custom angle clockwise roctation using bicubic interpolation
			case ProcessorEdit::EditType::ROTATE_CUSTOM_BICUBIC: {

				procParent->SendMessageToParent("Processing Rotation Edit" + fullEditNumStr);

				// Perform an edit on the data through the processor
				double angle = curEdit->GetParam(0);
				procParent->RotateCustomBicubic(angle);
				procParent->SetUpdated(true);
			}
			break;
		}
	}

	procParent->SendMessageToParent("");
	procParent->Unlock();
	return (wxThread::ExitCode)0;
}
