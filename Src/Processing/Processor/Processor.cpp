// Copyright 2016 Jacob Chesley

#include "Processor.h"

wxDEFINE_EVENT(PROCESSOR_MESSAGE_EVENT, wxCommandEvent);
wxDEFINE_EVENT(PROCESSOR_NUM_EVENT, wxCommandEvent);

double Processor::pi = 3.141592653589793238463;

Processor::Processor(wxWindow * parent) {
	img = new Image();
	tempImage = NULL;
	originalImg = NULL;
	didUpdate = false;
	locked = false;
	parWindow = parent;
	rawImage = NULL;
	rawImageGood = false;
	doFitImage = false;
	processThread = NULL;
	forceStop = false;
	lockedRaw = false;
	multiThread = false;
	numThreadsToUse = 1;
	numEdits = 0;
	numStartedThreads = 0;
	isProcessing = false;
	hasEdits = false;
	copiedEdit = NULL;
}

Processor::~Processor() {
	delete img;
	delete originalImg;
	this->DeleteEdits();
	rawPrcoessor.dcraw_clear_mem(rawImage);
	delete rawImage;
	if(copiedEdit != NULL){
		delete copiedEdit;
	}
}

inline double Processor::FastTanH(double x){
	if(x < -3.0) { return -0.99999999; }
	if(x > 3.0){ return 0.99999999; }
	return x * ( 27 + x * x ) / ( 27 + 9 * x * x );
}

Image * Processor::GetImage() {
	return img;
}

Image * Processor::GetTempImage() {
	return tempImage;
}

void Processor::SetOriginalImage(Image *newOriginalImage) {

	if(originalImg != NULL){
		delete originalImg;
	}
	originalImg = NULL;
	originalImg = new Image(*newOriginalImage);
}

Image* Processor::GetOriginalImage() {
	return originalImg;
}

void  Processor::RevertToOriginalImage(bool skipUpdate) {

	if (originalImg == NULL) {
		return;
	}
	delete img;
	img = NULL;
	img = new Image(*originalImg);
	if(!skipUpdate){
		didUpdate = true;
	}
}

void Processor::AddEdit(ProcessorEdit * edit) {
	editListInternal.push_back(edit);
}

void Processor::DeleteEdits() {

	// Delete all edits in the internal vector
	for (size_t i = 0; i < editListInternal.size(); i++) {
		editListInternal.at(i)->ClearIntArray();
		editListInternal.at(i)->ClearDoubleArray();
		delete editListInternal.at(i);
	}

	// Clear the vector
	editListInternal.clear();
}

void Processor::SetHasEdits(bool doesHaveEdits){
	hasEdits = doesHaveEdits;
}

bool Processor::GetHasEdits(){
	return hasEdits;
}
wxVector<ProcessorEdit*> Processor::GetEditVector() {
	return editListInternal;
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

	this->ProcessEdits(editListInternal);
}

void Processor::ProcessEdit(ProcessorEdit * edit) {

	numEdits = 1;

	this->forceStop = true;

	int curTime = 0;
	int timeout = 50;
	while(this->GetLocked()){	
		if(curTime > timeout){break;}
		Sleep(1);
		curTime += 1;
	}

	this->forceStop = false;
	this->didUpdate = false;

	processThread = new ProcessThread(this, edit);
	processThread->Run();

	processThread->SetPriority(95);
	this->didUpdate = true;
}

void Processor::ProcessEdits(wxVector<ProcessorEdit*> editList) {

	numEdits = editList.size();

	this->forceStop = true;
	int curTime = 0;
	int timeout = 50;
	while(this->GetLocked()){	
		if(curTime > timeout){break;}
		Sleep(1);
		curTime += 1;
	}

	this->forceStop = false;
	this->didUpdate = false;

	processThread = new ProcessThread(this, editList);
	processThread->Run();
	processThread->SetPriority(95);

	if(editList.size() < 1){ this->didUpdate = true;}
}

void Processor::KillCurrentProcessing(){
	this->forceStop = true;
	while(this->GetLocked()){}
}

void Processor::ProcessRaw(){
	if (this->GetLockedRaw()) {
		return;
	}
	this->forceStop = true;
	RawProcessThread * rawProcThread = new RawProcessThread(this);
	rawProcThread->Run();
	rawProcThread->SetPriority(95);
	this->forceStop = false;
}

void Processor::UnpackAndProcessRaw() {
	if (this->GetLockedRaw()) {
		return;
	}
	this->forceStop = true;
	RawProcessThread * rawProcThread = new RawProcessThread(this, true);
	rawProcThread->Run();
	rawProcThread->SetPriority(95);
	this->forceStop = false;
}

int Processor::GetLastNumEdits(){
	return numEdits;
}

void Processor::DeleteRawImage(){
	rawImageGood = false;
	rawPrcoessor.dcraw_clear_mem(rawImage);
	rawImage = NULL;
}

bool Processor::RawImageGood(){
	return rawImageGood;
}

int Processor::GetRawError(){
	return rawErrorCode;
}

void Processor::Lock() {

	lockCritical.Enter();
	locked = true;
	
}

void Processor::Unlock() {
	locked = false;
	lockCritical.Leave();
}

bool Processor::GetLocked() {
	return locked;
}

void Processor::LockRaw() {

	lockRawCritical.Enter();
	lockedRaw = true;
}

void Processor::UnlockRaw() {

	lockedRaw = false;
	lockRawCritical.Leave();
}

bool Processor::GetLockedRaw() {
	return lockedRaw;
}

void Processor::SetFilePath(wxString path) {
	filePath = path;
}

wxString Processor::GetFilePath() {
	return filePath;
}

void Processor::SetFileName(wxString name) {
	fileName = name;
}

wxString Processor::GetFileName() {
	return fileName;
}

void Processor::SetUpdated(bool updated) {
	didUpdate = updated;
}

bool Processor::GetUpdated() {
	return didUpdate;
}

void Processor::SetDoFitImage(bool fit) {
	doFitImage = fit;
}

bool Processor::GetDoFitImage() {
	return doFitImage;
}


void Processor::SetMultithread(bool doMultiThread){
	multiThread = doMultiThread;
}

bool Processor::GetMultithread(){
	return multiThread;
}

void Processor::SetNumThreads(int numThreads){
	numThreadsToUse = numThreads;
}

int Processor::GetNumThreads(){
	return numThreadsToUse;
}

void Processor::SendMessageToParent(wxString message) {

	// Send add edit event with edit ID to parent window (the edit panel)
	if (parWindow != NULL) {
		wxCommandEvent evt(PROCESSOR_MESSAGE_EVENT, ID_PROCESSOR_MESSAGE);
		evt.SetString(message);
		wxPostEvent(parWindow, evt);
	}
}

void Processor::SendProcessorEditNumToParent(int editNum){

	// Send add edit event with edit ID to parent window (the edit panel)
	if (parWindow != NULL) {
		wxCommandEvent evt(PROCESSOR_NUM_EVENT, ID_PROCESSOR_NUM);
		evt.SetInt(editNum);
		wxPostEvent(parWindow, evt);
	}
}

void Processor::StoreEditForCopyPaste(ProcessorEdit * edit){
	copiedEdit = edit;
}

ProcessorEdit * Processor::GetEditForCopyPaste(){
	return copiedEdit;
}

void Processor::StoreEditListForCopyPaste(wxVector<ProcessorEdit*> editList){
	copiedEditList.clear();

	for(size_t i = 0; i < editList.size(); i++){
		copiedEditList.push_back(new ProcessorEdit(*editList.at(i)));
	}
}

wxVector<ProcessorEdit*> Processor::GetEditListForCopyPaste(){
	return copiedEditList;
}

void Processor::Get8BitHistrogram(uint32_t * outputHistogramRed, uint32_t * outputHistogramGreen, uint32_t * outputHistogramBlue, uint32_t * outputHistogramGrey) {

	int dataSize = img->GetWidth() * img->GetHeight();

	// Initialize the histrogram to 0
	for (int i = 0; i < 256; i++) {
		outputHistogramRed[i] = 0;
		outputHistogramGreen[i] = 0;
		outputHistogramBlue[i] = 0;
		outputHistogramGrey[i] = 0;
	}

	int grey = 0;

	// Compute the histogram for red, green, blue and grey

	if(img->GetColorDepth() == 8){

		// Get pointers to 8 bit data
		uint8_t * redData8 = img->Get8BitDataRed();
		uint8_t * greenData8 = img->Get8BitDataGreen();
		uint8_t * blueData8 = img->Get8BitDataBlue();

		for (int i = 0; i < dataSize; i++) {

			if (forceStop) { return; }

			outputHistogramRed[redData8[i]] += 1;
			outputHistogramGreen[greenData8[i]] += 1;
			outputHistogramBlue[blueData8[i]] += 1;

			grey = (0.2126 * (redData8[i])) + (0.7152 * (greenData8[i])) + (0.0722 * (blueData8[i]));
			grey = grey > 255 ? 255 : grey;
			grey = grey < 0 ? 0 : grey;
			outputHistogramGrey[grey] += 1;
		}
	}
	else{

		// Get pointers to 16 bit data
		uint16_t * redData16 = img->Get16BitDataRed();
		uint16_t * greenData16 = img->Get16BitDataGreen();
		uint16_t * blueData16 = img->Get16BitDataBlue();
		for (int i = 0; i < dataSize; i++) {

			if (forceStop) { return; }

			outputHistogramRed[(uint16_t)(redData16[i]/256.0)] += 1;
			outputHistogramGreen[(uint16_t)(greenData16[i]/256.0)] += 1;
			outputHistogramBlue[(uint16_t)(blueData16[i]/256.0)] += 1;

			grey = (0.2126 * (uint16_t)(redData16[i]/256.0)) + (0.7152 * (uint16_t)(greenData16[i]/256.0))+ (0.0722 * (uint16_t)(blueData16[i]/256.0));
			grey = grey > 255 ? 255 : grey;
			grey = grey < 0 ? 0 : grey;
			outputHistogramGrey[grey] += 1;
		}
	}
}

void Processor::Get16BitHistrogram(uint32_t * outputHistogramRed, uint32_t * outputHistogramGreen, uint32_t * outputHistogramBlue, uint32_t * outputHistogramGrey) {

	int dataSize = img->GetWidth() * img->GetHeight();

	// Get pointers to 16 bit data
	uint16_t * redData16 = img->Get16BitDataRed();
	uint16_t * greenData16 = img->Get16BitDataGreen();
	uint16_t * blueData16 = img->Get16BitDataBlue();

	// Initialize the histrogram to 0
	for (int i = 0; i < 65536; i++) {
		outputHistogramRed[i] = 0;
		outputHistogramGreen[i] = 0;
		outputHistogramBlue[i] = 0;
	}

	int grey = 0;

	// Compute the histogram for red, green, blue and grey
	for (int i = 0; i < dataSize; i++) {

		if (forceStop) { return; }
		outputHistogramRed[redData16[i]] += 1;
		outputHistogramGreen[greenData16[i]] += 1;
		outputHistogramBlue[blueData16[i]] += 1;

		// Compute grey scale level
		grey = (int)((redData16[i] * 0.2126) + (greenData16[i] * 0.7152) + (blueData16[i] * 0.0722));
		grey = grey > 255 ? 255 : grey;
		grey = grey < 0 ? 0 : grey;
		outputHistogramGrey[grey] += 1;
	}
}


void Processor::ShiftRGB(double all, double red, double green, double blue, int dataStart, int dataEnd) {


	// Get number of pixels for the image
	int dataSize = img->GetWidth() * img->GetHeight();
	if (dataStart < 0 || dataEnd < 0) {
		dataStart = 0;
		dataEnd = dataSize;
	}

	// Create a temporary set of pixels that will allow handling of overflow
	int tempRed;
	int tempGreen;
	int tempBlue;

	// Process 8 bit data
	if(img->GetColorDepth() == 8){

		// Calculate 8 bit shift values.  Need 16 bits because this is now signed
		int16_t all8 = (int16_t)(all * 255);
		int16_t red8 = (int16_t)(red * 255);
		int16_t green8 = (int16_t)(green * 255);
		int16_t blue8 = (int16_t)(blue * 255);

		// Get pointers to 8 bit data
		uint8_t * redData8 = img->Get8BitDataRed();
		uint8_t * greenData8 = img->Get8BitDataGreen();
		uint8_t * blueData8 = img->Get8BitDataBlue();

		for (int i = dataStart; i < dataEnd; i++) {

			if(forceStop) { return; }
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
	}
	// Process 16 bit data
	else {

		// Calculate 16 bit shift values.  Need 32 bits because this is now signed.
		int32_t all16 = (int32_t)(all * 65535.0);
		int32_t red16 = (int32_t)(red * 65535.0);
		int32_t green16 = (int32_t)(green * 65535.0);
		int32_t blue16 = (int32_t)(blue * 65535.0);

		// Get pointers to 16 bit data
		uint16_t * redData16 = img->Get16BitDataRed();
		uint16_t * greenData16 = img->Get16BitDataGreen();
		uint16_t * blueData16 = img->Get16BitDataBlue();

		for (int i = dataStart; i < dataEnd; i++) {

			if(forceStop) { return; }

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


void Processor::AdjustHSL(double hShift, double sScale, double lScale, int dataStart, int dataEnd) {

	if(hShift > 360.0) { hShift = 360.0; }
	if(hShift < 0.0) { hShift = 0.0; }
	// Get number of pixels for the image
	int dataSize = img->GetWidth() * img->GetHeight();
	if (dataStart < 0 || dataEnd < 0) {
		dataStart = 0;
		dataEnd = dataSize;
	}
	
	// Create a temporary set of pixels that will allow handling of overflow
	int tempRed;
	int tempGreen;
	int tempBlue;

	double tempRedF;
	double tempGreenF;
	double tempBlueF;
	double min = 1.0;
	double max = 0.0;
	int maxColor = 0;

	double hue = 0.0;
	double saturation = 0.0;
	double luminace = 0.0;

	double tempRedHSL = 0.0;
	double tempGreenHSL = 0.0;
	double tempBlueHSL = 0.0;

	double temp1 = 0.0;
	double temp2 = 0.0;
	double tempR = 0.0;
	double tempG = 0.0;
	double tempB = 0.0;

	// Process 8 bit data
	if(img->GetColorDepth() == 8){

		// Get pointers to 8 bit data
		uint8_t * redData8 = img->Get8BitDataRed();
		uint8_t * greenData8 = img->Get8BitDataGreen();
		uint8_t * blueData8 = img->Get8BitDataBlue();

		for (int i = dataStart; i < dataEnd; i++) {

			if(forceStop) { return; }

			// Get red, green and blue temporary pixels
			tempRed = redData8[i];
			tempGreen = greenData8[i];
			tempBlue = blueData8[i];

			// Convert to HSL

			// Scale to 0-1
			tempRedF = (double)tempRed/255.0;
			tempGreenF = (double)tempGreen/255.0;
			tempBlueF = (double)tempBlue/255.0;

			min = 1.0;
			max = 0.0;

			// Find min and max
			if(tempRedF < min){ min = tempRedF; }
			if(tempGreenF < min){ min = tempGreenF; }
			if(tempBlueF < min){ min = tempBlueF; }
			if(tempRedF > max){ max = tempRedF; maxColor = 1; }
			if(tempGreenF > max){ max = tempGreenF; maxColor = 2; }
			if(tempBlueF > max){ max = tempBlueF; maxColor = 3;}

			luminace = (min + max) / 2.0;

			// No saturation if all rgb is same
			if(min == max){
				saturation = 0.0;
				hue = 0.0;
			}
			else{
				if(luminace < 0.5){ saturation = (max - min)/(max + min); }
				else{ saturation = (max - min)/(2.0 - max - min); }
		
				// Calculate hue
				//Red is maximum
				if(maxColor == 1){ hue = (tempGreenF - tempBlueF)/(max - min); }

				// Green in maximum
				else if(maxColor == 2){ hue = 2.0 + ((tempBlueF - tempRedF)/(max - min)); }

				// Blue is maximum
				else{ hue = 4.0 + ((tempRedF - tempGreenF)/(max - min));}

				// Convert hue to degrees
				hue *= 60.0;
				if(hue < 0.0){ hue += 360.0;}
			}

			// Shift and scale values
			hue += hShift;
			saturation *= sScale;
			luminace *= lScale;

			// Verify Hue is between 0 and 360
			if(hue > 360.0) { hue -= 360.0; }
			if(hue < 0.0){ hue += 360.0;}

			// Convert hsl back to rgb
			// Saturation is 0, this is greyscale
			if(saturation == 0.0){
				tempRedHSL =  luminace;
				tempGreenHSL = luminace;
				tempBlueHSL = luminace;
			}
			else{
				if(luminace < 0.5){ temp1 = luminace * (1.0 + saturation); }
				else{ temp1 = (luminace + saturation) - (luminace * saturation); }
		
				temp2 = (2 * luminace) - temp1;

				// Convert 0-360 to 0-1
				hue /=360.0;

				// Create temporary RGB from hue
				tempR = hue + (1.0/3.0);
				tempG = hue;
				tempB = hue - (1.0/3.0);

				// Adjust all temp RGB values to be between 0 and 1
				if(tempR < 0.0) {tempR += 1.0; }
				if(tempR > 1.0) {tempR -= 1.0; }
				if(tempG < 0.0) {tempG += 1.0; }
				if(tempG > 1.0) {tempG -= 1.0; }
				if(tempB < 0.0) {tempB += 1.0; }
				if(tempB > 1.0) {tempB -= 1.0; }

				// Calculate RGB Red from HSL
				if((6.0 * tempR) < 1.0){ tempRedHSL = temp2 + ((temp1 - temp2) * 6.0 * tempR); }
				else if((2.0 * tempR) < 1.0){ tempRedHSL = temp1; }
				else if((3.0 * tempR) < 2.0){ tempRedHSL = temp2 + ((temp1 - temp2)*((2.0/3.0) - tempR)*6.0); }
				else{ tempRedHSL = temp2; }

				// Calculate RGB Green from HSL
				if((6.0 * tempG) < 1.0){ tempGreenHSL = temp2 + ((temp1 - temp2) * 6.0 * tempG);}
				else if((2.0 * tempG) < 1.0){ tempGreenHSL = temp1; }
				else if((3.0 * tempG) < 2.0){ tempGreenHSL = temp2 + ((temp1 - temp2)*((2.0/3.0) - tempG)*6.0); }
				else{ tempGreenHSL = temp2; }

				// Calculate RGB Blue from HSL
				if((6.0 * tempB) < 1.0){ tempBlueHSL = temp2 + ((temp1 - temp2) * 6.0 * tempB); }
				else if((2.0 * tempB) < 1.0){ tempBlueHSL = temp1; }
				else if((3.0 * tempB) < 2.0){ tempBlueHSL = temp2 + ((temp1 - temp2)*((2.0/3.0) - tempB)*6.0); }
				else{ tempBlueHSL = temp2; }
			}

			// Scale to 0 - 255
			tempRed = wxRound(tempRedHSL * 255.0);
			tempGreen =  wxRound(tempGreenHSL * 255.0);
			tempBlue =  wxRound(tempBlueHSL * 255.0);

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
	}
	// Process 16 bit data
	else {

		// Get pointers to 16 bit data
		uint16_t * redData16 = img->Get16BitDataRed();
		uint16_t * greenData16 = img->Get16BitDataGreen();
		uint16_t * blueData16 = img->Get16BitDataBlue();

		for (int i = dataStart; i < dataEnd; i++) {

			if(forceStop) { return; }

			// Get red, green and blue temporary pixels
			tempRed = redData16[i];
			tempGreen = greenData16[i];
			tempBlue = blueData16[i];

			// Convert to HSL

			// Scale to 0-1
			tempRedF = (double)tempRed/65535.0;
			tempGreenF = (double)tempGreen/65535.0;
			tempBlueF = (double)tempBlue/65535.0;

			min = 1.0;
			max = 0.0;

			// Find min and max
			if(tempRedF < min){ min = tempRedF; }
			if(tempGreenF < min){ min = tempGreenF; }
			if(tempBlueF < min){ min = tempBlueF; }
			if(tempRedF > max){ max = tempRedF; maxColor = 1; }
			if(tempGreenF > max){ max = tempGreenF; maxColor = 2; }
			if(tempBlueF > max){ max = tempBlueF; maxColor = 3;}

			luminace = (min + max) / 2.0;

			// No saturation if all rgb is same
			if(min == max){
				saturation = 0.0;
				hue = 0.0;
			}
			else{
				if(luminace < 0.5){ saturation = (max - min)/(max + min); }
				else{ saturation = (max - min)/(2.0 - max - min); }
		
				// Calculate hue
				//Red is maximum
				if(maxColor == 1){ hue = (tempGreenF - tempBlueF)/(max - min); }

				// Green in maximum
				else if(maxColor == 2){ hue = 2.0 + ((tempBlueF - tempRedF)/(max - min)); }

				// Blue is maximum
				else{ hue = 4.0 + ((tempRedF - tempGreenF)/(max - min));}

				// Convert hue to degrees
				hue *= 60.0;
				if(hue < 0.0){ hue += 360.0;}
			}

			// Shift and scale values
			hue += hShift;
			saturation *= sScale;
			luminace *= lScale;

			// Verify Hue is between 0 and 360
			if(hue > 360.0) { hue -= 360.0; }
			if(hue < 0.0){ hue += 360.0;}

			// Convert hsl back to rgb
			// Saturation is 0, this is greyscale
			if(saturation == 0.0){
				tempRedHSL =  luminace;
				tempGreenHSL = luminace;
				tempBlueHSL = luminace;
			}
			else{
				if(luminace < 0.5){ temp1 = luminace * (1.0 + saturation); }
				else{ temp1 = (luminace + saturation) - (luminace * saturation); }
		
				temp2 = (2 * luminace) - temp1;

				// Convert 0-360 to 0-1
				hue /=360.0;

				// Create temporary RGB from hue
				tempR = hue + (1.0/3.0);
				tempG = hue;
				tempB = hue - (1.0/3.0);

				// Adjust all temp RGB values to be between 0 and 1
				if(tempR < 0.0) {tempR += 1.0; }
				if(tempR > 1.0) {tempR -= 1.0; }
				if(tempG < 0.0) {tempG += 1.0; }
				if(tempG > 1.0) {tempG -= 1.0; }
				if(tempB < 0.0) {tempB += 1.0; }
				if(tempB > 1.0) {tempB -= 1.0; }

				// Calculate RGB Red from HSL
				if((6.0 * tempR) < 1.0){ tempRedHSL = temp2 + ((temp1 - temp2) * 6.0 * tempR); }
				else if((2.0 * tempR) < 1.0){ tempRedHSL = temp1; }
				else if((3.0 * tempR) < 2.0){ tempRedHSL = temp2 + ((temp1 - temp2)*((2.0/3.0) - tempR)*6.0); }
				else{ tempRedHSL = temp2; }

				// Calculate RGB Green from HSL
				if((6.0 * tempG) < 1.0){ tempGreenHSL = temp2 + ((temp1 - temp2) * 6.0 * tempG);}
				else if((2.0 * tempG) < 1.0){ tempGreenHSL = temp1; }
				else if((3.0 * tempG) < 2.0){ tempGreenHSL = temp2 + ((temp1 - temp2)*((2.0/3.0) - tempG)*6.0); }
				else{ tempGreenHSL = temp2; }

				// Calculate RGB Blue from HSL
				if((6.0 * tempB) < 1.0){ tempBlueHSL = temp2 + ((temp1 - temp2) * 6.0 * tempB); }
				else if((2.0 * tempB) < 1.0){ tempBlueHSL = temp1; }
				else if((3.0 * tempB) < 2.0){ tempBlueHSL = temp2 + ((temp1 - temp2)*((2.0/3.0) - tempB)*6.0); }
				else{ tempBlueHSL = temp2; }
			}

			// Scale to 0 - 65535
			tempRed = wxRound(tempRedHSL *65535.0);
			tempGreen =  wxRound(tempGreenHSL * 65535.0);
			tempBlue =  wxRound(tempBlueHSL * 65535.0);

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

void Processor::AdjustBrightness(double brightness, double detailsPreseve, double toneSetting, int tonePreservation, int dataStart, int dataEnd) {

	if(detailsPreseve < 0.0){ detailsPreseve = 0.0; }
	if(detailsPreseve > 1.0){ detailsPreseve = 1.0; }

	// Get number of pixels for the image
	int dataSize = img->GetWidth() * img->GetHeight();
	if (dataStart < 0 || dataEnd < 0) {
		dataStart = 0;
		dataEnd = dataSize;
	}

	// Create a temporary set of pixels that will allow handling of overflow
	int tempRed;
	int tempGreen;
	int tempBlue;

	double tempRedF;
	double tempGreenF;
	double tempBlueF;
	double min = 1.0;
	double max = 0.0;
	int maxColor = 0;

	double hue = 0.0;
	double saturation = 0.0;
	double luminace = 0.0;

	double tempRedHSL = 0.0;
	double tempGreenHSL = 0.0;
	double tempBlueHSL = 0.0;

	double temp1 = 0.0;
	double temp2 = 0.0;
	double tempR = 0.0;
	double tempG = 0.0;
	double tempB = 0.0;

	// Process 8 bit data
	if(img->GetColorDepth() == 8){

		// Get pointers to 8 bit data
		uint8_t * redData8 = img->Get8BitDataRed();
		uint8_t * greenData8 = img->Get8BitDataGreen();
		uint8_t * blueData8 = img->Get8BitDataBlue();

		for (int i = dataStart; i < dataEnd; i++) {

			if(forceStop) { return; }

			// Get red, green and blue temporary pixels
			tempRed = redData8[i];
			tempGreen = greenData8[i];
			tempBlue = blueData8[i];

			// Convert to HSL

			// Scale to 0-1
			tempRedF = (double)tempRed/255.0;
			tempGreenF = (double)tempGreen/255.0;
			tempBlueF = (double)tempBlue/255.0;

			min = 1.0;
			max = 0.0;

			// Find min and max
			if(tempRedF < min){ min = tempRedF; }
			if(tempGreenF < min){ min = tempGreenF; }
			if(tempBlueF < min){ min = tempBlueF; }
			if(tempRedF > max){ max = tempRedF; maxColor = 1; }
			if(tempGreenF > max){ max = tempGreenF; maxColor = 2; }
			if(tempBlueF > max){ max = tempBlueF; maxColor = 3;}

			luminace = (min + max) / 2.0;

			// No saturation if all rgb is same
			if(min == max){
				saturation = 0.0;
				hue = 0.0;
			}
			else{
				if(luminace < 0.5){ saturation = (max - min)/(max + min); }
				else{ saturation = (max - min)/(2.0 - max - min); }
		
				// Calculate hue
				//Red is maximum
				if(maxColor == 1){ hue = (tempGreenF - tempBlueF)/(max - min); }

				// Green in maximum
				else if(maxColor == 2){ hue = 2.0 + ((tempBlueF - tempRedF)/(max - min)); }

				// Blue is maximum
				else{ hue = 4.0 + ((tempRedF - tempGreenF)/(max - min));}

				// Convert hue to degrees
				hue *= 60.0;
				if(hue < 0.0){ hue += 360.0;}
			}

			// Shift values

			// Function to preserve highlights
			// plot 1.0 - tanh(pi * x) from x = 0 to x = 1
			if(tonePreservation == Processor::BrightnessPreservation::HIGHLIGHTS){
				luminace += (brightness * (1.0 - detailsPreseve)) + (detailsPreseve*(brightness*(1.0 - FastTanH(pi * luminace * (toneSetting * 3.0)))));
			}

			// Function to preserve Shadows (mirror of highlights function)
			// tanh(pi * x) from x = 0 to x = 1
			else if(tonePreservation == Processor::BrightnessPreservation::SHADOWS){
				luminace += (brightness * (1.0 - detailsPreseve)) + (detailsPreseve*(brightness*(FastTanH(pi * luminace * (toneSetting * 3.0)))));
			}
			else{
				luminace += (brightness * (1.0 - detailsPreseve)) + (detailsPreseve * brightness * 0.25 *(pow(4.0, (toneSetting*40.0)) * pow(luminace, ((toneSetting*40.0) - 1.0)) * pow((1.0 - luminace), ((toneSetting*40.0) - 1.0))));
			}
			// Convert hsl back to rgb
			// Saturation is 0, this is greyscale
			if(saturation == 0.0){
				tempRedHSL =  luminace;
				tempGreenHSL = luminace;
				tempBlueHSL = luminace;
			}
			else{
				if(luminace < 0.5){ temp1 = luminace * (1.0 + saturation); }
				else{ temp1 = (luminace + saturation) - (luminace * saturation); }
		
				temp2 = (2 * luminace) - temp1;

				// Convert 0-360 to 0-1
				hue /=360.0;

				// Create temporary RGB from hue
				tempR = hue + (1.0/3.0);
				tempG = hue;
				tempB = hue - (1.0/3.0);

				// Adjust all temp RGB values to be between 0 and 1
				if(tempR < 0.0) {tempR += 1.0; }
				if(tempR > 1.0) {tempR -= 1.0; }
				if(tempG < 0.0) {tempG += 1.0; }
				if(tempG > 1.0) {tempG -= 1.0; }
				if(tempB < 0.0) {tempB += 1.0; }
				if(tempB > 1.0) {tempB -= 1.0; }

				// Calculate RGB Red from HSL
				if((6.0 * tempR) < 1.0){ tempRedHSL = temp2 + ((temp1 - temp2) * 6.0 * tempR); }
				else if((2.0 * tempR) < 1.0){ tempRedHSL = temp1; }
				else if((3.0 * tempR) < 2.0){ tempRedHSL = temp2 + ((temp1 - temp2)*((2.0/3.0) - tempR)*6.0); }
				else{ tempRedHSL = temp2; }

				// Calculate RGB Green from HSL
				if((6.0 * tempG) < 1.0){ tempGreenHSL = temp2 + ((temp1 - temp2) * 6.0 * tempG); }
				else if((2.0 * tempG) < 1.0){ tempGreenHSL = temp1; }
				else if((3.0 * tempG) < 2.0){ tempGreenHSL = temp2 + ((temp1 - temp2)*((2.0/3.0) - tempG)*6.0);	}
				else{ tempGreenHSL = temp2; }

				// Calculate RGB Blue from HSL
				if((6.0 * tempB) < 1.0){ tempBlueHSL = temp2 + ((temp1 - temp2) * 6.0 * tempB); }
				else if((2.0 * tempB) < 1.0){ tempBlueHSL = temp1; }
				else if((3.0 * tempB) < 2.0){ tempBlueHSL = temp2 + ((temp1 - temp2)*((2.0/3.0) - tempB)*6.0); }
				else{ tempBlueHSL = temp2; }
			}

			// Scale to 0 - 255
			tempRed = wxRound(tempRedHSL * 255.0);
			tempGreen =  wxRound(tempGreenHSL * 255.0);
			tempBlue =  wxRound(tempBlueHSL * 255.0);

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
	}
	// Process 16 bit data
	else {

		// Get pointers to 16 bit data
		uint16_t * redData16 = img->Get16BitDataRed();
		uint16_t * greenData16 = img->Get16BitDataGreen();
		uint16_t * blueData16 = img->Get16BitDataBlue();

		for (int i = dataStart; i < dataEnd; i++) {

			if(forceStop) { return; }if(forceStop) { return; }

			// Get red, green and blue temporary pixels
			tempRed = redData16[i];
			tempGreen = greenData16[i];
			tempBlue = blueData16[i];

			// Convert to HSL

			// Scale to 0-1
			tempRedF = (double)tempRed/65535.0;
			tempGreenF = (double)tempGreen/65535.0;
			tempBlueF = (double)tempBlue/65535.0;

			min = 1.0;
			max = 0.0;

			// Find min and max
			if(tempRedF < min){ min = tempRedF; }
			if(tempGreenF < min){ min = tempGreenF; }
			if(tempBlueF < min){ min = tempBlueF; }
			if(tempRedF > max){ max = tempRedF; maxColor = 1; }
			if(tempGreenF > max){ max = tempGreenF; maxColor = 2; }
			if(tempBlueF > max){ max = tempBlueF; maxColor = 3;}

			luminace = (min + max) / 2.0;

			// No saturation if all rgb is same
			if(min == max){
				saturation = 0.0;
				hue = 0.0;
			}
			else{
				if(luminace < 0.5){ saturation = (max - min)/(max + min); }
				else{ saturation = (max - min)/(2.0 - max - min); }
		
				// Calculate hue
				//Red is maximum
				if(maxColor == 1){ hue = (tempGreenF - tempBlueF)/(max - min); }

				// Green in maximum
				else if(maxColor == 2){ hue = 2.0 + ((tempBlueF - tempRedF)/(max - min)); }

				// Blue is maximum
				else{ hue = 4.0 + ((tempRedF - tempGreenF)/(max - min));}

				// Convert hue to degrees
				hue *= 60.0;
				if(hue < 0.0){ hue += 360.0;}
			}

			// Shift values

			// Function to preserve highlights
			// plot 1.0 - tanh(pi * x) from x = 0 to x = 1
			if(tonePreservation == Processor::BrightnessPreservation::HIGHLIGHTS){
				luminace += (brightness * (1.0 - detailsPreseve)) + (detailsPreseve*(brightness*(1.0 - FastTanH(pi * luminace * (toneSetting * 3.0)))));
			}

			// Function to preserve Shadows (mirror of highlights function)
			// tanh(pi * x) from x = 0 to x = 1
			else if(tonePreservation == Processor::BrightnessPreservation::SHADOWS){
				luminace += (brightness * (1.0 - detailsPreseve)) + (detailsPreseve*(brightness*(FastTanH(pi * luminace * (toneSetting * 3.0)))));
			}
			else{
				luminace += (brightness * (1.0 - detailsPreseve)) + (detailsPreseve * brightness * 0.25 *(pow(4.0, (toneSetting*40.0)) * pow(luminace, ((toneSetting*40.0) - 1.0)) * pow((1.0 - luminace), ((toneSetting*40.0) - 1.0))));
			}
			// Convert hsl back to rgb
			// Saturation is 0, this is greyscale
			if(saturation == 0.0){
				tempRedHSL =  luminace;
				tempGreenHSL = luminace;
				tempBlueHSL = luminace;
			}
			else{
				if(luminace < 0.5){ temp1 = luminace * (1.0 + saturation); }
				else{ temp1 = (luminace + saturation) - (luminace * saturation); }
		
				temp2 = (2 * luminace) - temp1;

				// Convert 0-360 to 0-1
				hue /=360.0;

				// Create temporary RGB from hue
				tempR = hue + (1.0/3.0);
				tempG = hue;
				tempB = hue - (1.0/3.0);

				// Adjust all temp RGB values to be between 0 and 1
				if(tempR < 0.0) {tempR += 1.0; }
				if(tempR > 1.0) {tempR -= 1.0; }
				if(tempG < 0.0) {tempG += 1.0; }
				if(tempG > 1.0) {tempG -= 1.0; }
				if(tempB < 0.0) {tempB += 1.0; }
				if(tempB > 1.0) {tempB -= 1.0; }

				// Calculate RGB Red from HSL
				if((6.0 * tempR) < 1.0){ tempRedHSL = temp2 + ((temp1 - temp2) * 6.0 * tempR); }
				else if((2.0 * tempR) < 1.0){ tempRedHSL = temp1; }
				else if((3.0 * tempR) < 2.0){ tempRedHSL = temp2 + ((temp1 - temp2)*((2.0/3.0) - tempR)*6.0); }
				else{ tempRedHSL = temp2; }

				// Calculate RGB Green from HSL
				if((6.0 * tempG) < 1.0){ tempGreenHSL = temp2 + ((temp1 - temp2) * 6.0 * tempG); }
				else if((2.0 * tempG) < 1.0){ tempGreenHSL = temp1; }
				else if((3.0 * tempG) < 2.0){ tempGreenHSL = temp2 + ((temp1 - temp2)*((2.0/3.0) - tempG)*6.0);	}
				else{ tempGreenHSL = temp2; }

				// Calculate RGB Blue from HSL
				if((6.0 * tempB) < 1.0){ tempBlueHSL = temp2 + ((temp1 - temp2) * 6.0 * tempB); }
				else if((2.0 * tempB) < 1.0){ tempBlueHSL = temp1; }
				else if((3.0 * tempB) < 2.0){ tempBlueHSL = temp2 + ((temp1 - temp2)*((2.0/3.0) - tempB)*6.0); }
				else{ tempBlueHSL = temp2; }
			}

			// Scale to 0 - 65535
			tempRed = wxRound(tempRedHSL * 65535.0);
			tempGreen =  wxRound(tempGreenHSL * 65535.0);
			tempBlue =  wxRound(tempBlueHSL * 65535.0);

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

void Processor::AdjustContrast(double allContrast, double redContrast, double greenContrast, double blueContrast, double allCenter, double redCenter, double greenCenter, double blueCenter, int dataStart, int dataEnd) {

	if(allCenter < 0.0) {allCenter = 0.0;}
	if(allCenter > 1.0) {allCenter = 1.0;}
	if(redCenter < 0.0) {redCenter = 0.0;}
	if(redCenter > 1.0) {redCenter = 1.0;}
	if(greenCenter < 0.0) {greenCenter = 0.0;}
	if(greenCenter > 1.0) {greenCenter = 1.0;}
	if(blueCenter < 0.0) {blueCenter = 0.0;}
	if(blueCenter > 1.0) {blueCenter = 1.0;}

	// Get number of pixels for the image
	int dataSize = img->GetWidth() * img->GetHeight();
	if (dataStart < 0 || dataEnd < 0) {
		dataStart = 0;
		dataEnd = dataSize;
	}

	// Create a temporary set of pixels that will allow handling of overflow
	int tempRed;
	int tempGreen;
	int tempBlue;

	// Process 8 bit data
	if(img->GetColorDepth() == 8){

		// Get pointers to 8 bit data
		uint8_t * redData8 = img->Get8BitDataRed();
		uint8_t * greenData8 = img->Get8BitDataGreen();
		uint8_t * blueData8 = img->Get8BitDataBlue();

		for (int i = dataStart; i < dataEnd; i++) {

			if(forceStop) { return; }

			// Perform whole contrast on red, green and blue
			tempRed = (allContrast * (redData8[i] - (255.0 * allCenter))) + (255.0 * allCenter);
			tempGreen = (allContrast * (greenData8[i] - (255.0 * allCenter))) + (255.0 * allCenter);
			tempBlue = (allContrast * (blueData8[i] - (255.0 * allCenter))) + (255.0 * allCenter);

			// Perform individual contrast on red, green and blue
			tempRed = (redContrast * (tempRed - (255.0 * redCenter))) + (255.0 * redCenter);
			tempGreen = (greenContrast * (tempGreen - (255.0 * greenCenter))) + (255.0 * greenCenter);
			tempBlue = (blueContrast * (tempBlue - (255.0 * blueCenter))) + (255.0 * blueCenter);

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
	}

	// Process 16 bit data
	else{

		// Get pointers to 16 bit data
		uint16_t * redData16 = img->Get16BitDataRed();
		uint16_t * greenData16 = img->Get16BitDataGreen();
		uint16_t * blueData16 = img->Get16BitDataBlue();

		for (int i = dataStart; i < dataEnd; i++) {

			if(forceStop) { return; }

			// Perform whole contrast on red, green and blue
			tempRed = (allContrast * (redData16[i] - (65535.0 * allCenter))) + (65535.0 * allCenter);
			tempGreen = (allContrast * (greenData16[i] - (65535.0 * allCenter))) + (65535.0 * allCenter);
			tempBlue = (allContrast * (blueData16[i] - (65535.0 * allCenter))) + (65535.0 * allCenter);

			// Perform individual contrast on red, green and blue
			tempRed = (redContrast * (tempRed - (65535.0 * redCenter))) + (65535.0 * redCenter);
			tempGreen = (greenContrast * (tempGreen - (65535.0 * greenCenter))) + (65535.0 * greenCenter);
			tempBlue = (blueContrast * (tempBlue - (65535.0 * blueCenter))) + (65535.0 * blueCenter);

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

void Processor::AdjustContrastCurve(double allContrast, double redContrast, double greenContrast, double blueContrast, double allCenter, double redCenter, double greenCenter, double blueCenter, int dataStart, int dataEnd) {

	// Get number of pixels for the image
	int dataSize = img->GetWidth() * img->GetHeight();
	if (dataStart < 0 || dataEnd < 0) {
		dataStart = 0;
		dataEnd = dataSize;
	}

	// Create a temporary set of pixels that will allow handling of overflow
	int tempRed;
	int tempGreen;
	int tempBlue;

	// Process 8 bit data
	if (img->GetColorDepth() == 8) {

		// Get pointers to 8 bit data
		uint8_t * redData8 = img->Get8BitDataRed();
		uint8_t * greenData8 = img->Get8BitDataGreen();
		uint8_t * blueData8 = img->Get8BitDataBlue();

		for (int i = dataStart; i < dataEnd; i++) {

			if (forceStop) { return; }

			tempRed = redData8[i];
			tempGreen = greenData8[i];
			tempBlue = blueData8[i];

			// Perform whole contrast on red, green and blue
			tempRed = (127.5*((FastTanH(allContrast * pi * ((tempRed/255.0)-allCenter))) + 1.0));
			tempGreen = (127.5*((FastTanH(allContrast * pi * ((tempGreen/255.0)-allCenter))) + 1.0));
			tempBlue = (127.5*((FastTanH(allContrast * pi * ((tempBlue/255.0)-allCenter))) + 1.0));

			// Perform individual contrast on red, green and blue
			tempRed = (127.5*((FastTanH(redContrast * pi * ((tempRed/255.0)-redCenter))) + 1.0));
			tempGreen = (127.5*((FastTanH(greenContrast * pi * ((tempGreen/255.0)-greenCenter))) + 1.0));
			tempBlue = (127.5*((FastTanH(blueContrast * pi * ((tempBlue/255.0)-blueCenter))) + 1.0));
			
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
	}

	// Process 16 bit data
	else {

		// Get pointers to 16 bit data
		uint16_t * redData16 = img->Get16BitDataRed();
		uint16_t * greenData16 = img->Get16BitDataGreen();
		uint16_t * blueData16 = img->Get16BitDataBlue();

		for (int i = dataStart; i < dataEnd; i++) {

			if (forceStop) { return; }

			tempRed = redData16[i];
			tempGreen = greenData16[i];
			tempBlue = blueData16[i];

			// Perform whole contrast on red, green and blue
			tempRed = (32767.5*((FastTanH(allContrast * pi * ((tempRed/65535.0)-allCenter))) + 1.0));
			tempGreen = (32767.5*((FastTanH(allContrast * pi * ((tempGreen/65535.0)-allCenter))) + 1.0));
			tempBlue = (32767.5*((FastTanH(allContrast * pi * ((tempBlue/65535.0)-allCenter))) + 1.0));

			// Perform individual contrast on red, green and blue
			tempRed = (32767.5*((FastTanH(redContrast * pi * ((tempRed/65535.0)-redCenter))) + 1.0));
			tempGreen = (32767.5*((FastTanH(greenContrast * pi * ((tempGreen/65535.0)-greenCenter))) + 1.0));
			tempBlue = (32767.5*((FastTanH(blueContrast * pi * ((tempBlue/65535.0)-blueCenter))) + 1.0));
			
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

	// Create a temporary set of pixels that will allow handling of overflow
	int tempGrey;

	// Process 8 bit data
	if(img->GetColorDepth() == 8){

		// Get pointers to 8 bit data
		uint8_t * redData8 = img->Get8BitDataRed();
		uint8_t * greenData8 = img->Get8BitDataGreen();
		uint8_t * blueData8 = img->Get8BitDataBlue();

		for (int i = dataStart; i < dataEnd; i++) {

			if(forceStop) { return; }

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
	}

	// Process 16 bit data
	else {

		// Get pointers to 16 bit data
		uint16_t * redData16 = img->Get16BitDataRed();
		uint16_t * greenData16 = img->Get16BitDataGreen();
		uint16_t * blueData16 = img->Get16BitDataBlue();

		for (int i = dataStart; i < dataEnd; i++) {

			if(forceStop) { return; }

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

	int32_t tempRed;
	int32_t tempGreen;
	int32_t tempBlue;

	// Process 8 bit data
	if(img->GetColorDepth() == 8){

		// Get pointers to 8 bit data
		uint8_t * redData8 = img->Get8BitDataRed();
		uint8_t * greenData8 = img->Get8BitDataGreen();
		uint8_t * blueData8 = img->Get8BitDataBlue();

		for (int i = dataStart; i < dataEnd; i++) {

			if(forceStop) { return; }

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
	}

	// Process 16 bit data
	else {

		// Get pointers to 16 bit data
		uint16_t * redData16 = img->Get16BitDataRed();
		uint16_t * greenData16 = img->Get16BitDataGreen();
		uint16_t * blueData16 = img->Get16BitDataBlue();

		for (int i = dataStart; i < dataEnd; i++) {

			if(forceStop) { return; }

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

void Processor::RGBCurves(int * brightCurve8, int * redCurve8, int * greenCurve8, int * blueCurve8,
	int * brightCurve16, int * redCurve16, int * greenCurve16, int * blueCurve16,  int dataStart, int dataEnd) {

	// Need to copy curves incase thread ends and destroys original curve table while we are still in it
	// (but exiting soon)
	int numSteps8 = 256;
	int * brightCurve8Copy = new int[numSteps8];
	int * redCurve8Copy = new int[numSteps8];
	int * greenCurve8Copy = new int[numSteps8];
	int * blueCurve8Copy = new int[numSteps8];
	memcpy(brightCurve8Copy, brightCurve8, numSteps8 * sizeof(int));
	memcpy(redCurve8Copy, redCurve8, numSteps8 * sizeof(int));
	memcpy(greenCurve8Copy, greenCurve8, numSteps8 * sizeof(int));
	memcpy(blueCurve8Copy, blueCurve8, numSteps8 * sizeof(int));

	int numSteps16 = 65535;
	int * brightCurve16Copy = new int[numSteps16];
	int * redCurve16Copy = new int[numSteps16];
	int * greenCurve16Copy = new int[numSteps16];
	int * blueCurve16Copy = new int[numSteps16];
	memcpy(brightCurve16Copy, brightCurve16, numSteps16 * sizeof(int));
	memcpy(redCurve16Copy, redCurve16, numSteps16 * sizeof(int));
	memcpy(greenCurve16Copy, greenCurve16, numSteps16 * sizeof(int));
	memcpy(blueCurve16Copy, blueCurve16, numSteps16 * sizeof(int));

	// Get number of pixels for the image
	int dataSize = img->GetWidth() * img->GetHeight();
	if (dataStart < 0 || dataEnd < 0) {
		dataStart = 0;
		dataEnd = dataSize;
	}

	int32_t tempRed;
	int32_t tempGreen;
	int32_t tempBlue;

	// Process 8 bit data
	if(img->GetColorDepth() == 8){

		// Get pointers to 8 bit data
		uint8_t * redData8 = img->Get8BitDataRed();
		uint8_t * greenData8 = img->Get8BitDataGreen();
		uint8_t * blueData8 = img->Get8BitDataBlue();

		for (int i = dataStart; i < dataEnd; i++) {

			if(forceStop) { return; }

			// Apply red curve and bright curve to red channel
			tempRed = redCurve8Copy[redData8[i]];
			tempRed = (tempRed > 255) ? 255 : tempRed;
			tempRed = (tempRed < 0) ? 0 : tempRed;
			tempRed = brightCurve8Copy[tempRed];

			// Apply green curve and bright curve to green channel
			tempGreen = greenCurve8Copy[greenData8[i]];
			tempGreen = (tempGreen > 255) ? 255 : tempGreen;
			tempGreen = (tempGreen < 0) ? 0 : tempGreen;
			tempGreen = brightCurve8Copy[tempGreen];

			// Apply blue curve and bright curve to blue channel
			tempBlue = blueCurve8Copy[blueData8[i]];
			tempBlue = (tempBlue > 255) ? 255 : tempBlue;
			tempBlue = (tempBlue < 0) ? 0 : tempBlue;
			tempBlue = brightCurve8Copy[tempBlue];

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
	}

	// Process 16 bit data
	else {

		// Get pointers to 16 bit data
		uint16_t * redData16 = img->Get16BitDataRed();
		uint16_t * greenData16 = img->Get16BitDataGreen();
		uint16_t * blueData16 = img->Get16BitDataBlue();

		for (int i = dataStart; i < dataEnd; i++) {

			if(forceStop) { return; }

			// Apply red curve and bright curve to red channel
			tempRed = redCurve16Copy[redData16[i]];
			tempRed = (tempRed > 65535) ? 65535: tempRed;
			tempRed = (tempRed < 0) ? 0 : tempRed;
			tempRed = brightCurve16Copy[tempRed];

			// Apply green curve and bright curve to green channel
			tempGreen = greenCurve16Copy[greenData16[i]];
			tempGreen = (tempGreen > 65535) ? 65535 : tempGreen;
			tempGreen = (tempGreen < 0) ? 0 : tempGreen;
			tempGreen = brightCurve16Copy[tempGreen];

			// Apply blue curve and bright curve to blue channel
			tempBlue = blueCurve16Copy[blueData16[i]];
			tempBlue = (tempBlue > 65535) ? 65535 : tempBlue;
			tempBlue = (tempBlue < 0) ? 0 : tempBlue;
			tempBlue = brightCurve16Copy[tempBlue];

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

	delete[] brightCurve8Copy;
	delete[] redCurve8Copy;
	delete[] greenCurve8Copy;
	delete[] blueCurve8Copy;

	delete[] brightCurve16Copy;
	delete[] redCurve16Copy;
	delete[] greenCurve16Copy;
	delete[] blueCurve16Copy;

}

void Processor::LABCurves(int * lCurve16, int * aCurve16, int * bCurve16, int colorSpace,  int dataStart, int dataEnd){

	// Need to copy curves incase thread ends and destroys original curve table while we are still in it
	// (but exiting soon)
	int numSteps = 65535;
	int * lCurve16Copy = new int[numSteps];
	int * aCurve16Copy = new int[numSteps];
	int * bCurve16Copy = new int[numSteps];
	memcpy(lCurve16Copy, lCurve16, numSteps * sizeof(int));
	memcpy(aCurve16Copy, aCurve16, numSteps * sizeof(int));
	memcpy(bCurve16Copy, bCurve16, numSteps * sizeof(int));

	// Get number of pixels for the image
	int dataSize = img->GetWidth() * img->GetHeight();
	if (dataStart < 0 || dataEnd < 0) {
		dataStart = 0;
		dataEnd = dataSize;
	}

	RGB rgb;
	XYZ xyz;
	LAB lab;

	int lScale;
	int aScale;
	int bScale;

	int newL;
	int newA;
	int newB;

	int32_t tempRed;
	int32_t tempGreen;
	int32_t tempBlue;

	// Process 8 bit data
	if(img->GetColorDepth() == 8){

		// Get pointers to 8 bit data
		uint8_t * redData8 = img->Get8BitDataRed();
		uint8_t * greenData8 = img->Get8BitDataGreen();
		uint8_t * blueData8 = img->Get8BitDataBlue();

		for (int i = dataStart; i < dataEnd; i++) {

			if(forceStop) { return; }

			// Convert RGB to LAB color space
			rgb.R = (float)redData8[i] / 256.0;
			rgb.G = (float)greenData8[i] / 256.0;
			rgb.B = (float)blueData8[i] / 256.0;
			this->RGBtoXYZ(&rgb, &xyz, colorSpace);
			this->XYZtoLAB(&xyz, &lab);

			// Scale LAB to ints, 16 bits of precision
			lScale = (lab.L / 100.0f) * numSteps;
			aScale = ((lab.A + 128.0f)/256.0f)* numSteps;
			bScale = ((lab.B + 128.0f)/256.0f) * numSteps;

			// Apply LAB Curve
			//if(lCurve16 == NULL || aCurve16 == NULL || bCurve16 == NULL){ return; }
			newL = lCurve16Copy[lScale];
			newA = aCurve16Copy[aScale];
			newB = bCurve16Copy[bScale];

			// Scale LAB back
			lab.L = (float)(newL / (float)numSteps);
			lab.A = (float)(newA / (float)numSteps);
			lab.B = (float)(newB / (float)numSteps);

			lab.L *= 100.0;
			lab.A *= 256.0;
			lab.B *= 256.0;

			lab.A -= 128.0f;
			lab.B -= 128.0f;

			// Convert LAB back to RGB color space
			this->LABtoXYZ(&lab, &xyz);
			this->XYZtoRGB(&xyz, &rgb);
			tempRed = (int32_t)(rgb.R * 256.0);
			tempGreen = (int32_t)(rgb.G * 256.0);
			tempBlue = (int32_t)(rgb.B * 256.0);

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
	}
	// Process 16 bit data
	else {

		// Get pointers to 16 bit data
		uint16_t * redData16 = img->Get16BitDataRed();
		uint16_t * greenData16 = img->Get16BitDataGreen();
		uint16_t * blueData16 = img->Get16BitDataBlue();

		for (int i = dataStart; i < dataEnd; i++) {

			if(forceStop) { return; }

			// Convert RGB to LAB color space
			rgb.R = (float)redData16[i] / 65535.0;
			rgb.G = (float)greenData16[i] / 65535.0;
			rgb.B = (float)blueData16[i] / 65535.0;
			this->RGBtoXYZ(&rgb, &xyz, colorSpace);
			this->XYZtoLAB(&xyz, &lab);

			// Scale LAB to ints, 16 bits of precision
			lScale = (lab.L / 100.0f) * numSteps;
			aScale = ((lab.A + 128.0f) / 256.0f)* numSteps;
			bScale = ((lab.B + 128.0f) / 256.0f) * numSteps;

			// Apply LAB Curve
			newL = lCurve16Copy[lScale];
			newA = aCurve16Copy[aScale];
			newB = bCurve16Copy[bScale];

			// Scale LAB back
			lab.L = (float)(newL / (float)numSteps);
			lab.A = (float)(newA / (float)numSteps);
			lab.B = (float)(newB / (float)numSteps);

			lab.L *= 100.0;
			lab.A *= 256.0;
			lab.B *= 256.0;

			lab.A -= 128.0f;
			lab.B -= 128.0f;

			// Convert LAB back to RGB color space
			this->LABtoXYZ(&lab, &xyz);
			this->XYZtoRGB(&xyz, &rgb);
			tempRed = (int32_t)(rgb.R * 65535.0);
			tempGreen = (int32_t)(rgb.G * 65535.0);
			tempBlue = (int32_t)(rgb.B * 65535.0);

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

	delete[] lCurve16Copy;
	delete[] aCurve16Copy;
	delete[] bCurve16Copy;
}

void Processor::HSLCurves(int * hCurve16, int * sCurve16, int * lCurve16, int dataStart, int dataEnd){

	// Need to copy curves incase thread ends and destroys original curve table while we are still in it
	// (but exiting soon)
	int numSteps = 65535;
	int * hCurve16Copy = new int[numSteps + 1];
	int * sCurve16Copy = new int[numSteps + 1];
	int * lCurve16Copy = new int[numSteps + 1];
	memcpy(hCurve16Copy, hCurve16, numSteps * sizeof(int));
	memcpy(sCurve16Copy, sCurve16, numSteps * sizeof(int));
	memcpy(lCurve16Copy, lCurve16, numSteps * sizeof(int));
	hCurve16Copy[numSteps] = hCurve16[numSteps];
	sCurve16Copy[numSteps] = sCurve16[numSteps];
	lCurve16Copy[numSteps] = lCurve16[numSteps];

	// Get number of pixels for the image
	int dataSize = img->GetWidth() * img->GetHeight();
	if (dataStart < 0 || dataEnd < 0) {
		dataStart = 0;
		dataEnd = dataSize;
	}
	
	int hScale;
	int sScale;
	int lScale;

	int newH;
	int newS;
	int newL;

	int32_t tempRed;
	int32_t tempGreen;
	int32_t tempBlue;

	double tempRedF;
	double tempGreenF;
	double tempBlueF;
	double min = 1.0;
	double max = 0.0;
	int maxColor = 0;

	double hue = 0.0;
	double saturation = 0.0;
	double luminace = 0.0;

	double tempRedHSL = 0.0;
	double tempGreenHSL = 0.0;
	double tempBlueHSL = 0.0;

	double temp1 = 0.0;
	double temp2 = 0.0;
	double tempR = 0.0;
	double tempG = 0.0;
	double tempB = 0.0;

	// Process 8 bit data
	if(img->GetColorDepth() == 8){

		// Get pointers to 8 bit data
		uint8_t * redData8 = img->Get8BitDataRed();
		uint8_t * greenData8 = img->Get8BitDataGreen();
		uint8_t * blueData8 = img->Get8BitDataBlue();

		for (int i = dataStart; i < dataEnd; i++) {

			if(forceStop) { return; }

			tempRed = redData8[i];
			tempGreen = greenData8[i];
			tempBlue = blueData8[i];
	
			// Convert to HSL

			// Scale to 0-1
			tempRedF = (double)tempRed/255.0;
			tempGreenF = (double)tempGreen/255.0;
			tempBlueF = (double)tempBlue/255.0;

			min = 1.0;
			max = 0.0;

			// Find min and max
			if(tempRedF < min){ min = tempRedF; }
			if(tempGreenF < min){ min = tempGreenF; }
			if(tempBlueF < min){ min = tempBlueF; }
			if(tempRedF > max){ max = tempRedF; maxColor = 1; }
			if(tempGreenF > max){ max = tempGreenF; maxColor = 2; }
			if(tempBlueF > max){ max = tempBlueF; maxColor = 3;}

			luminace = (min + max) / 2.0;

			// No saturation if all rgb is same
			if(min == max){
				saturation = 0.0;
				hue = 0.0;
			}
			else{
				if(luminace < 0.5){ saturation = (max - min)/(max + min); }
				else{ saturation = (max - min)/(2.0 - max - min); }
		
				// Calculate hue
				//Red is maximum
				if(maxColor == 1){ hue = (tempGreenF - tempBlueF)/(max - min); }

				// Green in maximum
				else if(maxColor == 2){ hue = 2.0 + ((tempBlueF - tempRedF)/(max - min)); }

				// Blue is maximum
				else{ hue = 4.0 + ((tempRedF - tempGreenF)/(max - min));}

				// Convert hue to degrees
				hue *= 60.0;
				if(hue < 0.0){ hue += 360.0;}
			}

			// Shift values

			// Scale HSL to ints, 16 bits of precision
			hScale = (hue/360.0) * numSteps;
			sScale = saturation * numSteps;
			lScale = luminace * numSteps;

			// Apply HSL Curve
			newH = hCurve16Copy[hScale];
			newS = sCurve16Copy[sScale];
			newL = lCurve16Copy[lScale];

			// Scale HSL back
			hue = (float)(newH / (float)numSteps) * 360.0f;
			saturation = (float)(newS / (float)numSteps);
			luminace = (float)(newL / (float)numSteps);
				
			// Convert hsl back to rgb
			// Saturation is 0, this is greyscale
			if(saturation == 0.0){
				tempRedHSL =  luminace;
				tempGreenHSL = luminace;
				tempBlueHSL = luminace;
			}
			else{
				if(luminace < 0.5){ temp1 = luminace * (1.0 + saturation); }
				else{ temp1 = (luminace + saturation) - (luminace * saturation); }
		
				temp2 = (2 * luminace) - temp1;

				// Convert 0-360 to 0-1
				hue /=360.0;

				// Create temporary RGB from hue
				tempR = hue + (1.0/3.0);
				tempG = hue;
				tempB = hue - (1.0/3.0);

				// Adjust all temp RGB values to be between 0 and 1
				if(tempR < 0.0) {tempR += 1.0; }
				if(tempR > 1.0) {tempR -= 1.0; }
				if(tempG < 0.0) {tempG += 1.0; }
				if(tempG > 1.0) {tempG -= 1.0; }
				if(tempB < 0.0) {tempB += 1.0; }
				if(tempB > 1.0) {tempB -= 1.0; }

				// Calculate RGB Red from HSL
				if((6.0 * tempR) < 1.0){ tempRedHSL = temp2 + ((temp1 - temp2) * 6.0 * tempR); }
				else if((2.0 * tempR) < 1.0){ tempRedHSL = temp1; }
				else if((3.0 * tempR) < 2.0){ tempRedHSL = temp2 + ((temp1 - temp2)*((2.0/3.0) - tempR)*6.0); }
				else{ tempRedHSL = temp2; }

				// Calculate RGB Green from HSL
				if((6.0 * tempG) < 1.0){ tempGreenHSL = temp2 + ((temp1 - temp2) * 6.0 * tempG); }
				else if((2.0 * tempG) < 1.0){ tempGreenHSL = temp1; }
				else if((3.0 * tempG) < 2.0){ tempGreenHSL = temp2 + ((temp1 - temp2)*((2.0/3.0) - tempG)*6.0);	}
				else{ tempGreenHSL = temp2; }

				// Calculate RGB Blue from HSL
				if((6.0 * tempB) < 1.0){ tempBlueHSL = temp2 + ((temp1 - temp2) * 6.0 * tempB); }
				else if((2.0 * tempB) < 1.0){ tempBlueHSL = temp1; }
				else if((3.0 * tempB) < 2.0){ tempBlueHSL = temp2 + ((temp1 - temp2)*((2.0/3.0) - tempB)*6.0); }
				else{ tempBlueHSL = temp2; }
			}

			// Scale to 0 - 255
			tempRed = wxRound(tempRedHSL * 255.0);
			tempGreen =  wxRound(tempGreenHSL * 255.0);
			tempBlue =  wxRound(tempBlueHSL * 255.0);

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
	}
	// Process 16 bit data
	else {

		// Get pointers to 16 bit data
		uint16_t * redData16 = img->Get16BitDataRed();
		uint16_t * greenData16 = img->Get16BitDataGreen();
		uint16_t * blueData16 = img->Get16BitDataBlue();

		for (int i = dataStart; i < dataEnd; i++) {

			if(forceStop) { return; }

			// Get red, green and blue temporary pixels
			tempRed = redData16[i];
			tempGreen = greenData16[i];
			tempBlue = blueData16[i];

			// Convert to HSL

			// Scale to 0-1
			tempRedF = (double)tempRed/65535.0;
			tempGreenF = (double)tempGreen/65535.0;
			tempBlueF = (double)tempBlue/65535.0;

			min = 1.0;
			max = 0.0;

			// Find min and max
			if(tempRedF < min){ min = tempRedF; }
			if(tempGreenF < min){ min = tempGreenF; }
			if(tempBlueF < min){ min = tempBlueF; }
			if(tempRedF > max){ max = tempRedF; maxColor = 1; }
			if(tempGreenF > max){ max = tempGreenF; maxColor = 2; }
			if(tempBlueF > max){ max = tempBlueF; maxColor = 3;}

			luminace = (min + max) / 2.0;

			// No saturation if all rgb is same
			if(min == max){
				saturation = 0.0;
				hue = 0.0;
			}
			else{
				if(luminace < 0.5){ saturation = (max - min)/(max + min); }
				else{ saturation = (max - min)/(2.0 - max - min); }
		
				// Calculate hue
				//Red is maximum
				if(maxColor == 1){ hue = (tempGreenF - tempBlueF)/(max - min); }

				// Green in maximum
				else if(maxColor == 2){ hue = 2.0 + ((tempBlueF - tempRedF)/(max - min)); }

				// Blue is maximum
				else{ hue = 4.0 + ((tempRedF - tempGreenF)/(max - min));}

				// Convert hue to degrees
				hue *= 60.0;
				if(hue < 0.0){ hue += 360.0;}
			}

			// Shift values

			// Scale HSL to ints, 16 bits of precision
			hScale = (hue/360.0) * numSteps;
			sScale = saturation * numSteps;
			lScale = luminace * numSteps;

			// Apply HSL Curve
			newH = hCurve16Copy[hScale];
			newS = sCurve16Copy[sScale];
			newL = lCurve16Copy[lScale];

			// Scale HSL back
			hue = (float)(newH / (float)numSteps) * 360.0f;
			saturation = (float)(newS / (float)numSteps);
			luminace = (float)(newL / (float)numSteps);

			// Convert hsl back to rgb
			// Saturation is 0, this is greyscale
			if(saturation == 0.0){
				tempRedHSL =  luminace;
				tempGreenHSL = luminace;
				tempBlueHSL = luminace;
			}
			else{
				if(luminace < 0.5){ temp1 = luminace * (1.0 + saturation); }
				else{ temp1 = (luminace + saturation) - (luminace * saturation); }
		
				temp2 = (2 * luminace) - temp1;

				// Convert 0-360 to 0-1
				hue /=360.0;

				// Create temporary RGB from hue
				tempR = hue + (1.0/3.0);
				tempG = hue;
				tempB = hue - (1.0/3.0);

				// Adjust all temp RGB values to be between 0 and 1
				if(tempR < 0.0) {tempR += 1.0; }
				if(tempR > 1.0) {tempR -= 1.0; }
				if(tempG < 0.0) {tempG += 1.0; }
				if(tempG > 1.0) {tempG -= 1.0; }
				if(tempB < 0.0) {tempB += 1.0; }
				if(tempB > 1.0) {tempB -= 1.0; }

				// Calculate RGB Red from HSL
				if((6.0 * tempR) < 1.0){ tempRedHSL = temp2 + ((temp1 - temp2) * 6.0 * tempR); }
				else if((2.0 * tempR) < 1.0){ tempRedHSL = temp1; }
				else if((3.0 * tempR) < 2.0){ tempRedHSL = temp2 + ((temp1 - temp2)*((2.0/3.0) - tempR)*6.0); }
				else{ tempRedHSL = temp2; }

				// Calculate RGB Green from HSL
				if((6.0 * tempG) < 1.0){ tempGreenHSL = temp2 + ((temp1 - temp2) * 6.0 * tempG); }
				else if((2.0 * tempG) < 1.0){ tempGreenHSL = temp1; }
				else if((3.0 * tempG) < 2.0){ tempGreenHSL = temp2 + ((temp1 - temp2)*((2.0/3.0) - tempG)*6.0);	}
				else{ tempGreenHSL = temp2; }

				// Calculate RGB Blue from HSL
				if((6.0 * tempB) < 1.0){ tempBlueHSL = temp2 + ((temp1 - temp2) * 6.0 * tempB); }
				else if((2.0 * tempB) < 1.0){ tempBlueHSL = temp1; }
				else if((3.0 * tempB) < 2.0){ tempBlueHSL = temp2 + ((temp1 - temp2)*((2.0/3.0) - tempB)*6.0); }
				else{ tempBlueHSL = temp2; }
			}

			// Scale to 0 - 65535
			tempRed = wxRound(tempRedHSL * 65535.0);
			tempGreen =  wxRound(tempGreenHSL * 65535.0);
			tempBlue =  wxRound(tempBlueHSL * 65535.0);

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
	delete[] hCurve16Copy;
	delete[] sCurve16Copy;
	delete[] lCurve16Copy;
}

void Processor::SetupRotation(int editID, double angleDegrees, int crop){
	
	// Copy image data to tmep image
	if(editID == ProcessorEdit::EditType::ROTATE_180){
		tempImage = new Image(*img);
	}

	// Copy image data to tmep image.  Width and height will be swapped in cleanup
	else if (editID == ProcessorEdit::EditType::ROTATE_90_CW ||editID == ProcessorEdit::EditType::ROTATE_270_CW){
		tempImage = new Image(*img);
	}

	// Create a blank temp image that will be filled in from rotation algorithms
	else if(editID ==ProcessorEdit::EditType::ROTATE_CUSTOM_BICUBIC || 
		editID ==ProcessorEdit::EditType::ROTATE_CUSTOM_BILINEAR ||
		editID ==ProcessorEdit::EditType::ROTATE_CUSTOM_NEAREST){

		tempImage = new Image();
		if(img->GetColorDepth() == 16){ 
			tempImage->Enable16Bit();
		}

		// Set width and height to maximum size needed to fit whole image in rotation (with black surrounding borders)
		if (crop == Processor::RotationCropping::EXPAND) {
			tempImage->SetWidth(this->GetExpandedRotationWidth(angleDegrees, img->GetWidth(), img->GetHeight()));
			tempImage->SetHeight(this->GetExpandedRotationHeight(angleDegrees, img->GetWidth(), img->GetHeight()));
		}

		// Set width and height to minumum size needed to fit image with no border
		else if (crop == Processor::RotationCropping::FIT) {
			tempImage->SetWidth(this->GetFittedRotationWidth(angleDegrees, img->GetWidth(), img->GetHeight()));
			tempImage->SetHeight(this->GetFittedRotationHeight(angleDegrees, img->GetWidth(), img->GetHeight()));
		}

		// Rotated image is same size as current image
		else {
			tempImage->SetWidth(img->GetWidth());
			tempImage->SetHeight(img->GetHeight());
		}

		// Create blank image data
		tempImage->InitImage();
	}
}

void Processor::CleanupRotation(int editID){

	// Copy temp image to image if rotate custom was used
	if(editID ==ProcessorEdit::EditType::ROTATE_CUSTOM_BICUBIC || 
		editID ==ProcessorEdit::EditType::ROTATE_CUSTOM_BILINEAR ||
		editID ==ProcessorEdit::EditType::ROTATE_CUSTOM_NEAREST){
		delete img;
		img = new Image(*tempImage);
	}

	// Swap width and height for 90 or 270 rotation, 180 rotation is same width and height
	else if(editID == ProcessorEdit::EditType::ROTATE_270_CW || editID == ProcessorEdit::EditType::ROTATE_90_CW){
		img->SetWidth(tempImage->GetHeight());
		img->SetHeight(tempImage->GetWidth());
	}

	// Cleanup temp image
	if(tempImage != NULL){
		delete tempImage;
		tempImage = NULL;
	}
}
void Processor::Rotate90CW(int dataStart, int dataEnd) {

	int width = img->GetWidth();
	int height = img->GetHeight();

	// Get number of pixels for the image
	int dataSize = img->GetWidth() * img->GetHeight();
	if (dataStart < 0 || dataEnd < 0) {
		dataStart = 0;
		dataEnd = dataSize;
	}

	int x = 0;
	int y = 0;
	int offset = 0;


	// Process 8 bit data
	if(img->GetColorDepth() == 8){

		// Get pointers to 8 bit data
		uint8_t * redData8 = img->Get8BitDataRed();
		uint8_t * greenData8 = img->Get8BitDataGreen();
		uint8_t * blueData8 = img->Get8BitDataBlue();

		uint8_t * redData8Dup = tempImage->Get8BitDataRed();
		uint8_t * greenData8Dup = tempImage->Get8BitDataGreen();
		uint8_t * blueData8Dup = tempImage->Get8BitDataBlue();

		for (int i = dataStart; i < dataEnd; i++) {

			if(forceStop) { return; }

			// Get x and y coordinates from current index of data
			x = i % width;
			y = i / width;
			offset = height * x + y;

			redData8[offset] = redData8Dup[width * (height - 1 - y) + x];
			greenData8[offset] = greenData8Dup[width * (height - 1 - y) + x];
			blueData8[offset] = blueData8Dup[width * (height - 1 - y) + x];
		}
	}

	// Process 16 bit data
	else {

		// Get pointers to 16 bit data
		uint16_t * redData16 = img->Get16BitDataRed();
		uint16_t * greenData16 = img->Get16BitDataGreen();
		uint16_t * blueData16 = img->Get16BitDataBlue();

		uint16_t * redData16Dup = tempImage->Get16BitDataRed();
		uint16_t * greenData16Dup = tempImage->Get16BitDataGreen();
		uint16_t * blueData16Dup = tempImage->Get16BitDataBlue();

		for (int i = dataStart; i < dataEnd; i++) {

			if(forceStop) { return; }

			// Get x and y coordinates from current index of data
			x = i % width;
			y = i / width;
			offset = height * x + y;

			redData16[offset] = redData16Dup[width * (height - 1 - y) + x];
			greenData16[offset] = greenData16Dup[width * (height - 1 - y) + x];
			blueData16[offset] = blueData16Dup[width * (height - 1 - y) + x];
		}
	}
}

void Processor::Rotate180(int dataStart, int dataEnd) {

	// Get number of pixels for the image
	int dataSize = img->GetWidth() * img->GetHeight();
	if (dataStart < 0 || dataEnd < 0) {
		dataStart = 0;
		dataEnd = dataSize;
	}

	// Process 8 bit data
	if(img->GetColorDepth() == 8){

		// Get pointers to 8 bit data
		uint8_t * redData8 = img->Get8BitDataRed();
		uint8_t * greenData8 = img->Get8BitDataGreen();
		uint8_t * blueData8 = img->Get8BitDataBlue();

		uint8_t * redData8Dup = tempImage->Get8BitDataRed();
		uint8_t * greenData8Dup = tempImage->Get8BitDataGreen();
		uint8_t * blueData8Dup = tempImage->Get8BitDataBlue();

		for (int i = dataStart; i < dataEnd; i++) {

			if(forceStop) { return; }

			redData8[i] = redData8Dup[dataSize - i - 1];
			greenData8[i] = greenData8Dup[dataSize - i - 1];
			blueData8[i] = blueData8Dup[dataSize - i - 1];
		}
	}
	
	// Process 16 bit data
	else {

		// Get pointers to 16 bit data
		uint16_t * redData16 = img->Get16BitDataRed();
		uint16_t * greenData16 = img->Get16BitDataGreen();
		uint16_t * blueData16 = img->Get16BitDataBlue();

		uint16_t * redData16Dup = tempImage->Get16BitDataRed();
		uint16_t * greenData16Dup = tempImage->Get16BitDataGreen();
		uint16_t * blueData16Dup = tempImage->Get16BitDataBlue();

		for (int i = dataStart; i < dataEnd; i++) {

			if(forceStop) { return; }

			redData16[i] = redData16Dup[dataSize - i - 1];
			greenData16[i] = greenData16Dup[dataSize - i - 1];
			blueData16[i] = blueData16Dup[dataSize - i - 1];
		}
	}
}

void Processor::RotateCustom(double angleDegrees, int dataStart, int dataEnd) {

	// Get number of pixels for the image
	int newDataSize = tempImage->GetWidth() * tempImage->GetHeight();
	if (dataStart < 0 || dataEnd < 0) {
		dataStart = 0;
		dataEnd = newDataSize;
	}

	angleDegrees *= -1.0;
	double angleSin = sin(angleDegrees * pi / 180.0);
	double angleCos = cos(angleDegrees * pi / 180.0);
	double pivotX = (double)img->GetWidth() / 2.0;
	double pivotY = (double)img->GetHeight() / 2.0;

	int width = img->GetWidth();
	int height = img->GetHeight();
	int x = 0;
	int y = 0;
	double newX = 0;
	double newY = 0;

	int newWidth = 0;
	int newHeight = 0;
	int dWidth = 0;
	int dHeight = 0;

	newWidth = tempImage->GetWidth();
	newHeight = tempImage->GetHeight();
	dWidth = newWidth - width;
	dHeight = newHeight - height;

	int newI = 0;

	// Process 8 bit data
	if(img->GetColorDepth() == 8){

		// Get pointers to 8 bit data
		uint8_t * redData8 = img->Get8BitDataRed();
		uint8_t * greenData8 = img->Get8BitDataGreen();
		uint8_t * blueData8 = img->Get8BitDataBlue();

		uint8_t * redData8Dup = tempImage->Get8BitDataRed();
		uint8_t * greenData8Dup = tempImage->Get8BitDataGreen();
		uint8_t * blueData8Dup = tempImage->Get8BitDataBlue();

		for (int i = dataStart; i < dataEnd; i++) {

			if(forceStop) { return; }

			// Get x and y coordinates from current index of data
			x = i % newWidth;
			y = i / newWidth;

			x -= dWidth / 2.0;
			y -= dHeight / 2.0;

			// Shift by pivot point
			x -= pivotX;
			y -= pivotY;

			// Rotate point
			newX = (x * angleCos) - (y * angleSin);
			newY = (x * angleSin) + (y * angleCos);

			// Shift back
			newX += pivotX;
			newY += pivotY;

			// Round double to int
			newX = wxRound(newX);
			newY = wxRound(newY);

			// Veirfy pixel location is in bounds of original image size
			if (newX > 0 && newX < width && newY > 0 && newY < height) {

				// Get new single dimmension array index from new x and y location
				newI = newY * width + newX;

				// Copy pixel from old location to new location
				redData8Dup[i] = redData8[newI];
				greenData8Dup[i] = greenData8[newI];
				blueData8Dup[i] = blueData8[newI];
			}
		}
	}

	// Process 16 bit data
	else {

		// Get pointers to 16 bit data
		uint16_t * redData16 = img->Get16BitDataRed();
		uint16_t * greenData16 = img->Get16BitDataGreen();
		uint16_t * blueData16 = img->Get16BitDataBlue();

		uint16_t * redData16Dup = tempImage->Get16BitDataRed();
		uint16_t * greenData16Dup = tempImage->Get16BitDataGreen();
		uint16_t * blueData16Dup = tempImage->Get16BitDataBlue();

		for (int i = dataStart; i < dataEnd; i++) {

			if(forceStop) { return; }

			// Get x and y coordinates from current index of data
			x = i % newWidth;
			y = i / newWidth;

			x -= dWidth / 2.0;
			y -= dHeight / 2.0;

			// Shift by pivot point
			x -= pivotX;
			y -= pivotY;

			// rotate point
			newX = (x * angleCos) - (y * angleSin);
			newY = (x * angleSin) + (y * angleCos);

			// Shift back
			newX += pivotX;
			newY += pivotY;

			// Round double to int
			newX = wxRound(newX);
			newY = wxRound(newY);

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
	}
}

void Processor::RotateCustomBilinear(double angleDegrees, int dataStart, int dataEnd) {

	// Get number of pixels for the image
	int newDataSize = tempImage->GetWidth() * tempImage->GetHeight();
	if (dataStart < 0 || dataEnd < 0) {
		dataStart = 0;
		dataEnd = newDataSize;
	}

	if(angleDegrees == 0.0){
		return;
	}

	double useNearestNeighborDistance = 0.0001;

	angleDegrees *= -1.0;
	double angleSin = sin(angleDegrees * pi / 180.0);
	double angleCos = cos(angleDegrees * pi / 180.0);
	double pivotX = (double)img->GetWidth() / 2.0;
	double pivotY = (double)img->GetHeight() / 2.0;

	int width = img->GetWidth();
	int height = img->GetHeight();
	int x = 0;
	int y = 0;
	double newX = 0;
	double newY = 0;

	int32_t tempRed;
	int32_t tempGreen;
	int32_t tempBlue;
	
	int newWidth = 0;
	int newHeight = 0;
	int dWidth = 0;
	int dHeight = 0;
	
	newWidth = tempImage->GetWidth();
	newHeight = tempImage->GetHeight();
	newDataSize = newWidth * newHeight;
	dWidth = newWidth - width;
	dHeight = newHeight - height;

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

	// Process 8 bit data
	if(img->GetColorDepth() == 8){

		// Get pointers to 8 bit data
		uint8_t * redData8 = img->Get8BitDataRed();
		uint8_t * greenData8 = img->Get8BitDataGreen();
		uint8_t * blueData8 = img->Get8BitDataBlue();

		uint8_t * redData8Dup = tempImage->Get8BitDataRed();
		uint8_t * greenData8Dup = tempImage->Get8BitDataGreen();
		uint8_t * blueData8Dup = tempImage->Get8BitDataBlue();

		for (int i = dataStart; i < dataEnd; i++) {

			if(forceStop) { return; }

			// Get x and y coordinates from current index of data
			x = i % newWidth;
			y = i / newWidth;

			x -= dWidth / 2.0;
			y -= dHeight / 2.0;

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
			if (newX > 1 && newX < width - 1 && newY > 1 && newY < height - 1) {

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
					continue;
				}

				// If distance 2 is close enough to an actual point, use that closest point
				if (distance2 < useNearestNeighborDistance) {

					newI2 = point3 * width + point2;
					redData8Dup[i] = redData8[newI2];
					greenData8Dup[i] = greenData8[newI2];
					blueData8Dup[i] = blueData8[newI2];
					continue;
				}

				// If distance 3 is close enough to an actual point, use that closest point
				if (distance3 < useNearestNeighborDistance) {

					newI3 = point4 * width + point1;
					redData8Dup[i] = redData8[newI3];
					greenData8Dup[i] = greenData8[newI3];
					blueData8Dup[i] = blueData8[newI3];
					continue;
				}

				// If distance 4 is close enough to an actual point, use that closest point
				if (distance4 < useNearestNeighborDistance) {

					newI4 = point4 * width + point1;
					redData8Dup[i] = redData8[newI4];
					greenData8Dup[i] = greenData8[newI4];
					blueData8Dup[i] = blueData8[newI4];
					continue;
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

	}
	// Process 16 bit data
	else {

		// Get pointers to 16 bit data
		uint16_t * redData16 = img->Get16BitDataRed();
		uint16_t * greenData16 = img->Get16BitDataGreen();
		uint16_t * blueData16 = img->Get16BitDataBlue();

		uint16_t * redData16Dup = tempImage->Get16BitDataRed();
		uint16_t * greenData16Dup = tempImage->Get16BitDataGreen();
		uint16_t * blueData16Dup = tempImage->Get16BitDataBlue();

		for (int i = dataStart; i < dataEnd; i++) {

			if(forceStop) { return; }

			// Get x and y coordinates from current index of data
			x = i % newWidth;
			y = i / newWidth;

			x -= dWidth / 2.0;
			y -= dHeight / 2.0;

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
			if (newX > 1 && newX < width - 1 && newY > 1 && newY < height - 1) {

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
					continue;
				}

				// If distance 2 is close enough to an actual point, use that closest point
				if (distance2 < useNearestNeighborDistance) {

					newI2 = point3 * width + point2;
					redData16Dup[i] = redData16[newI2];
					greenData16Dup[i] = greenData16[newI2];
					blueData16Dup[i] = blueData16[newI2];
					continue;
				}

				// If distance 3 is close enough to an actual point, use that closest point
				if (distance3 < useNearestNeighborDistance) {

					newI3 = point4 * width + point1;
					redData16Dup[i] = redData16[newI3];
					greenData16Dup[i] = greenData16[newI3];
					blueData16Dup[i] = blueData16[newI3];
					continue;
				}

				// If distance 4 is close enough to an actual point, use that closest point
				if (distance4 < useNearestNeighborDistance) {

					newI4 = point4 * width + point1;
					redData16Dup[i] = redData16[newI4];
					greenData16Dup[i] = greenData16[newI4];
					blueData16Dup[i] = blueData16[newI4];
					continue;
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
	}
}

void Processor::RotateCustomBicubic(double angleDegrees, int dataStart, int dataEnd) {

	// Get number of pixels for the image
	int newDataSize = tempImage->GetWidth() * tempImage->GetHeight();
	if (dataStart < 0 || dataEnd < 0) {
		dataStart = 0;
		dataEnd = newDataSize;
	}

	if(angleDegrees == 0.0){
		return;
	}

	double useNearestNeighborDistance = 0.0001;

	angleDegrees *= -1.0;
	double angleSin = sin(angleDegrees * pi / 180.0);
	double angleCos = cos(angleDegrees * pi / 180.0);
	double pivotX = (double)img->GetWidth() / 2.0;
	double pivotY = (double)img->GetHeight() / 2.0;

	int width = img->GetWidth();
	int height = img->GetHeight();
	int x = 0;
	int y = 0;
	double newX = 0;
	double newY = 0;

	int32_t tempRed;
	int32_t tempGreen;
	int32_t tempBlue;

	int newWidth = 0;
	int newHeight = 0;
	int dWidth = 0;
	int dHeight = 0;

	newWidth = tempImage->GetWidth();
	newHeight = tempImage->GetHeight();
	newDataSize = newWidth * newHeight;
	dWidth = newWidth - width;
	dHeight = newHeight - height;

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

	// Process 8 bit data
	if(img->GetColorDepth() == 8){

		// Get pointers to 8 bit data
		uint8_t * redData8 = img->Get8BitDataRed();
		uint8_t * greenData8 = img->Get8BitDataGreen();
		uint8_t * blueData8 = img->Get8BitDataBlue();

		uint8_t * redData8Dup = tempImage->Get8BitDataRed();
		uint8_t * greenData8Dup = tempImage->Get8BitDataGreen();
		uint8_t * blueData8Dup = tempImage->Get8BitDataBlue();

		for (int i = dataStart; i < dataEnd; i++) {

			if(forceStop) { return; }

			// Get x and y coordinates from current index of data
			x = i % newWidth;
			y = i / newWidth;

			x -= dWidth / 2.0;
			y -= dHeight / 2.0;

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
			if (newX > 2 && newX < width - 2 && newY > 2 && newY < height - 2) {

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
					redData8Dup[i] = redData8[newI1];
					greenData8Dup[i] = greenData8[newI1];
					blueData8Dup[i] = blueData8[newI1];
					continue;
				}

				// If distance 2 is close enough to an actual point, use that closest point
				else if (distance2 < useNearestNeighborDistance) {

					newI2 = point5 * width + point2;
					redData8Dup[i] = redData8[newI2];
					greenData8Dup[i] = greenData8[newI2];
					blueData8Dup[i] = blueData8[newI2];
					continue;
				}

				// If distance 3 is close enough to an actual point, use that closest point
				else if (distance3 < useNearestNeighborDistance) {

					newI3 = point5 * width + point3;
					redData8Dup[i] = redData8[newI3];
					greenData8Dup[i] = greenData8[newI3];
					blueData8Dup[i] = blueData8[newI3];
					continue;
				}

				// If distance 4 is close enough to an actual point, use that closest point
				else if (distance4 < useNearestNeighborDistance) {

					newI4 = point5 * width + point4;
					redData8Dup[i] = redData8[newI4];
					greenData8Dup[i] = greenData8[newI4];
					blueData8Dup[i] = blueData8[newI4];
					continue;
				}

				// If distance 5 is close enough to an actual point, use that closest point
				else if (distance5 < useNearestNeighborDistance) {

					newI5 = point6 * width + point1;
					redData8Dup[i] = redData8[newI5];
					greenData8Dup[i] = greenData8[newI5];
					blueData8Dup[i] = blueData8[newI5];
					continue;
				}

				// If distance 6 is close enough to an actual point, use that closest point
				else if (distance6 < useNearestNeighborDistance) {

					newI6 = point6 * width + point2;
					redData8Dup[i] = redData8[newI6];
					greenData8Dup[i] = greenData8[newI6];
					blueData8Dup[i] = blueData8[newI6];
					continue;
				}

				// If distance 7 is close enough to an actual point, use that closest point
				else if (distance7 < useNearestNeighborDistance) {

					newI7 = point6 * width + point3;
					redData8Dup[i] = redData8[newI7];
					greenData8Dup[i] = greenData8[newI7];
					blueData8Dup[i] = blueData8[newI7];
					continue;
				}

				// If distance 8 is close enough to an actual point, use that closest point
				else if (distance8 < useNearestNeighborDistance) {

					newI8 = point6 * width + point4;
					redData8Dup[i] = redData8[newI8];
					greenData8Dup[i] = greenData8[newI8];
					blueData8Dup[i] = blueData8[newI8];
					continue;
				}
				// If distance 9 is close enough to an actual point, use that closest point
				else if (distance9 < useNearestNeighborDistance) {

					newI9 = point7 * width + point1;
					redData8Dup[i] = redData8[newI9];
					greenData8Dup[i] = greenData8[newI9];
					blueData8Dup[i] = blueData8[newI9];
					continue;
				}

				// If distance 10 is close enough to an actual point, use that closest point
				else if (distance10 < useNearestNeighborDistance) {

					newI10 = point7 * width + point2;
					redData8Dup[i] = redData8[newI10];
					greenData8Dup[i] = greenData8[newI10];
					blueData8Dup[i] = blueData8[newI10];
					continue;
				}

				// If distance 11 is close enough to an actual point, use that closest point
				else if (distance11 < useNearestNeighborDistance) {

					newI11 = point7 * width + point3;
					redData8Dup[i] = redData8[newI11];
					greenData8Dup[i] = greenData8[newI11];
					blueData8Dup[i] = blueData8[newI11];
					continue;
				}

				// If distance 12 is close enough to an actual point, use that closest point
				else if (distance12 < useNearestNeighborDistance) {

					newI12 = point7 * width + point4;
					redData8Dup[i] = redData8[newI12];
					greenData8Dup[i] = greenData8[newI12];
					blueData8Dup[i] = blueData8[newI12];
					continue;
				}

				// If distance 13 is close enough to an actual point, use that closest point
				else if (distance13 < useNearestNeighborDistance) {

					newI13 = point8 * width + point1;
					redData8Dup[i] = redData8[newI13];
					greenData8Dup[i] = greenData8[newI13];
					blueData8Dup[i] = blueData8[newI13];
					continue;
				}

				// If distance 14 is close enough to an actual point, use that closest point
				else if (distance14 < useNearestNeighborDistance) {

					newI14 = point8 * width + point2;
					redData8Dup[i] = redData8[newI14];
					greenData8Dup[i] = greenData8[newI14];
					blueData8Dup[i] = blueData8[newI14];
					continue;
				}

				// If distance 15 is close enough to an actual point, use that closest point
				else if (distance15 < useNearestNeighborDistance) {

					newI15 = point8 * width + point3;
					redData8Dup[i] = redData8[newI15];
					greenData8Dup[i] = greenData8[newI15];
					blueData8Dup[i] = blueData8[newI15];
					continue;
				}

				// If distance 16 is close enough to an actual point, use that closest point
				else if (distance16 < useNearestNeighborDistance) {

					newI16 = point8 * width + point4;
					redData8Dup[i] = redData8[newI16];
					greenData8Dup[i] = greenData8[newI16];
					blueData8Dup[i] = blueData8[newI16];
					continue;
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
	}

	// Process 16 bit data
	else {

		// Get pointers to 16 bit data
		uint16_t * redData16 = img->Get16BitDataRed();
		uint16_t * greenData16 = img->Get16BitDataGreen();
		uint16_t * blueData16 = img->Get16BitDataBlue();

		uint16_t * redData16Dup = tempImage->Get16BitDataRed();
		uint16_t * greenData16Dup = tempImage->Get16BitDataGreen();
		uint16_t * blueData16Dup = tempImage->Get16BitDataBlue();

		for (int i = dataStart; i < dataEnd; i++) {

			if(forceStop) { return; }

			// Get x and y coordinates from current index of data
			x = i % newWidth;
			y = i / newWidth;

			x -= dWidth / 2.0;
			y -= dHeight / 2.0;

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
			if (newX > 2 && newX < width - 2 && newY > 2 && newY < height - 2) {

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
					continue;
				}

				// If distance 2 is close enough to an actual point, use that closest point
				else if (distance2 < useNearestNeighborDistance) {

					newI2 = point5 * width + point2;
					redData16Dup[i] = redData16[newI2];
					greenData16Dup[i] = greenData16[newI2];
					blueData16Dup[i] = blueData16[newI2];
					continue;
				}

				// If distance 3 is close enough to an actual point, use that closest point
				else if (distance3 < useNearestNeighborDistance) {

					newI3 = point5 * width + point3;
					redData16Dup[i] = redData16[newI3];
					greenData16Dup[i] = greenData16[newI3];
					blueData16Dup[i] = blueData16[newI3];
					continue;
				}

				// If distance 4 is close enough to an actual point, use that closest point
				else if (distance4 < useNearestNeighborDistance) {

					newI4 = point5 * width + point4;
					redData16Dup[i] = redData16[newI4];
					greenData16Dup[i] = greenData16[newI4];
					blueData16Dup[i] = blueData16[newI4];
					continue;
				}

				// If distance 5 is close enough to an actual point, use that closest point
				else if (distance5 < useNearestNeighborDistance) {

					newI5 = point6 * width + point1;
					redData16Dup[i] = redData16[newI5];
					greenData16Dup[i] = greenData16[newI5];
					blueData16Dup[i] = blueData16[newI5];
					continue;
				}

				// If distance 6 is close enough to an actual point, use that closest point
				else if (distance6 < useNearestNeighborDistance) {

					newI6 = point6 * width + point2;
					redData16Dup[i] = redData16[newI6];
					greenData16Dup[i] = greenData16[newI6];
					blueData16Dup[i] = blueData16[newI6];
					continue;
				}

				// If distance 7 is close enough to an actual point, use that closest point
				else if (distance7 < useNearestNeighborDistance) {

					newI7 = point6 * width + point3;
					redData16Dup[i] = redData16[newI7];
					greenData16Dup[i] = greenData16[newI7];
					blueData16Dup[i] = blueData16[newI7];
					continue;
				}

				// If distance 8 is close enough to an actual point, use that closest point
				else if (distance8 < useNearestNeighborDistance) {

					newI8 = point6 * width + point4;
					redData16Dup[i] = redData16[newI8];
					greenData16Dup[i] = greenData16[newI8];
					blueData16Dup[i] = blueData16[newI8];
					continue;
				}

				// If distance 9 is close enough to an actual point, use that closest point
				else if (distance9 < useNearestNeighborDistance) {

					newI9 = point7 * width + point1;
					redData16Dup[i] = redData16[newI9];
					greenData16Dup[i] = greenData16[newI9];
					blueData16Dup[i] = blueData16[newI9];
					continue;
				}

				// If distance 10 is close enough to an actual point, use that closest point
				else if (distance10 < useNearestNeighborDistance) {

					newI10 = point7 * width + point2;
					redData16Dup[i] = redData16[newI10];
					greenData16Dup[i] = greenData16[newI10];
					blueData16Dup[i] = blueData16[newI10];
					continue;
				}

				// If distance 11 is close enough to an actual point, use that closest point
				else if (distance11 < useNearestNeighborDistance) {

					newI11 = point7 * width + point3;
					redData16Dup[i] = redData16[newI11];
					greenData16Dup[i] = greenData16[newI11];
					blueData16Dup[i] = blueData16[newI11];
					continue;
				}

				// If distance 12 is close enough to an actual point, use that closest point
				else if (distance12 < useNearestNeighborDistance) {

					newI12 = point7 * width + point4;
					redData16Dup[i] = redData16[newI12];
					greenData16Dup[i] = greenData16[newI12];
					blueData16Dup[i] = blueData16[newI12];
					continue;
				}

				// If distance 13 is close enough to an actual point, use that closest point
				else if (distance13 < useNearestNeighborDistance) {

					newI13 = point8 * width + point1;
					redData16Dup[i] = redData16[newI13];
					greenData16Dup[i] = greenData16[newI13];
					blueData16Dup[i] = blueData16[newI13];
					continue;
				}

				// If distance 14 is close enough to an actual point, use that closest point
				else if (distance14 < useNearestNeighborDistance) {

					newI14 = point8 * width + point2;
					redData16Dup[i] = redData16[newI14];
					greenData16Dup[i] = greenData16[newI14];
					blueData16Dup[i] = blueData16[newI14];
					continue;
				}

				// If distance 15 is close enough to an actual point, use that closest point
				else if (distance15 < useNearestNeighborDistance) {

					newI15 = point8 * width + point3;
					redData16Dup[i] = redData16[newI15];
					greenData16Dup[i] = greenData16[newI15];
					blueData16Dup[i] = blueData16[newI15];
					continue;
				}

				// If distance 16 is close enough to an actual point, use that closest point
				else if (distance16 < useNearestNeighborDistance) {

					newI16 = point8 * width + point4;
					redData16Dup[i] = redData16[newI16];
					greenData16Dup[i] = greenData16[newI16];
					blueData16Dup[i] = blueData16[newI16];
					continue;
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
	}
}

int Processor::GetExpandedRotationWidth(double angleDegrees, int originalWidth, int originalHeight) {

	if (angleDegrees < 0.0) {
		angleDegrees += 180.0;
	}
	if ((angleDegrees < 90.0 && angleDegrees > 0.0) || (angleDegrees > -90.0 && angleDegrees < 0.0)) {
		return (int)(((double)originalWidth * cos(angleDegrees * pi / 180.0)) + ((double)originalHeight * sin(angleDegrees * pi / 180.0)));
	}
	else if ((angleDegrees > 90.0 && angleDegrees < 180.0) || (angleDegrees < -90.0 && angleDegrees > -180.0)) {
		angleDegrees -= 90.0;
		return (int)(((double)originalHeight * cos(angleDegrees * pi / 180.0)) + ((double)originalWidth * sin(angleDegrees * pi / 180.0)));
	}
	else if (angleDegrees == 90 || angleDegrees == -90) {
		return originalHeight;
	}
	else {
		return originalWidth;
	}
}

int Processor::GetExpandedRotationHeight(double angleDegrees, int originalWidth, int originalHeight) {

	if (angleDegrees < 0.0) {
		angleDegrees *= -1.0;
	}
	if ((angleDegrees < 90.0 && angleDegrees > 0.0) || (angleDegrees > -90.0 && angleDegrees < 0.0)) {
		return (int)(((double)originalWidth * sin(angleDegrees * pi / 180.0)) + ((double)originalHeight * cos(angleDegrees * pi / 180.0)));
	}
	else if ((angleDegrees > 90.0 && angleDegrees < 180.0) || (angleDegrees < -90.0 && angleDegrees > -180.0)) {
		angleDegrees -= 90.0;
		return (int)(((double)originalHeight * sin(angleDegrees * pi / 180.0)) + ((double)originalWidth * cos(angleDegrees * pi / 180.0)));
	}
	else if (angleDegrees == 90 || angleDegrees == -90) {
		return originalWidth;
	}
	else {
		return originalHeight;
	}
}

int Processor::GetFittedRotationWidth(double angleDegrees, int originalWidth, int originalHeight) {

	// Height stays the same if 180 degree rotation
	if (angleDegrees == 0.0 || angleDegrees == 180.0 || angleDegrees == -180.0) {
		return originalWidth;
	}

	// Height becomes width if 90 degree rotation
	else if (angleDegrees == 90.0 || angleDegrees == -90.0) {
		return originalHeight;
	}

	// Get absolute value of sin and cos of angle
	double absoluteSin = sin(angleDegrees * pi / 180.0);
	double absoluteCos = cos(angleDegrees * pi / 180.0);
	if (absoluteSin < 0.0) { absoluteSin *= -1.0; }
	if (absoluteCos < 0.0) { absoluteCos *= -1.0; }

	// Find length of shorter and longer sides
	int shorterSide = 0;
	int longerSide = 0;

	bool doSwap = false;
	if (originalWidth > originalHeight) {
		longerSide = originalWidth;
		shorterSide = originalHeight;
	}
	else {
		longerSide = originalHeight;
		shorterSide = originalWidth;
		doSwap = true;
	}

	if (shorterSide < 2.0 * absoluteSin * absoluteCos * longerSide) {
		if (!doSwap) {
			return (int)((0.5 * shorterSide) / absoluteSin);
		}
		else {
			return (int)((0.5 * shorterSide) / absoluteCos);
		}
	}
	else {

		double absoulteCos2 = absoluteCos * absoluteCos;
		double absoulteSin2 = absoluteSin * absoluteSin;
		double absoulteCos2MinAbsolulteSin2 = absoulteCos2 - absoulteSin2;

		return (int)(((originalWidth * absoluteCos) - (originalHeight * absoluteSin)) / absoulteCos2MinAbsolulteSin2);
	}
}

int Processor::GetFittedRotationHeight(double angleDegrees, int originalWidth, int originalHeight) {

	// Height stays the same if 180 degree rotation
	if (angleDegrees == 0.0 || angleDegrees == 180.0 || angleDegrees == -180.0) {
		return originalHeight;
	}

	// Height becomes width if 90 degree rotation
	else if (angleDegrees == 90.0 || angleDegrees == -90.0) {
		return originalWidth;
	}

	// Get absolute value of sin and cos of angle
	double absoluteSin = sin(angleDegrees * pi / 180.0);
	double absoluteCos = cos(angleDegrees * pi / 180.0);
	if (absoluteSin < 0.0) { absoluteSin *= -1.0; }
	if (absoluteCos < 0.0) { absoluteCos *= -1.0; }

	// Find length of shorter and longer sides
	int shorterSide = 0;
	int longerSide = 0;

	bool doSwap = false;
	if (originalWidth > originalHeight) {
		longerSide = originalWidth;
		shorterSide = originalHeight;
	}
	else {
		longerSide = originalHeight;
		shorterSide = originalWidth;
		doSwap = true;
	}

	if (shorterSide < 2.0 * absoluteSin * absoluteCos * longerSide) {
		if (!doSwap) {
			return (int)((0.5 * shorterSide) / absoluteCos);
		}
		else {
			return (int)((0.5 * shorterSide) / absoluteSin);
		}
	}
	else {

		double absoulteCos2 = absoluteCos * absoluteCos;
		double absoulteSin2 = absoluteSin * absoluteSin;
		double absoulteCos2MinAbsolulteSin2 = absoulteCos2 - absoulteSin2;

		return (int)(((originalHeight * absoluteCos) - (originalWidth * absoluteSin)) / absoulteCos2MinAbsolulteSin2);
	}
}

void Processor::SetupScale(int newWidth, int newHeight){

	// Create temp image with new scale size
	tempImage = new Image();
	tempImage->SetWidth(newWidth);
	tempImage->SetHeight(newHeight);
	if (this->GetImage()->GetColorDepth() == 16) { tempImage->Enable16Bit(); }
	tempImage->InitImage();
}

void Processor::CleanupScale(){

	// Copy temp image to image
	delete img;
	img = new Image(*tempImage);

	// Cleanup temp image
	if(tempImage != NULL){
		delete tempImage;
		tempImage = NULL;
	}
}

void Processor::ScaleNearest(int dataStart, int dataEnd){

	// Get number of pixels for the image
	int newDataSize = tempImage->GetWidth() * tempImage->GetHeight();
	if (dataStart < 0 || dataEnd < 0) {
		dataStart = 0;
		dataEnd = newDataSize;
	}

	int width = img->GetWidth();
	int height = img->GetHeight();

	int newWidth = tempImage->GetWidth();
	int newHeight = tempImage->GetHeight();

	double xRatio = (double)width / (double)newWidth;
	double yRatio = (double)height / (double)newHeight;

	int x = 0;
	int y = 0;
	double newX = 0;
	double newY = 0;

	int newI = 0;

	// Process 8 bit data
	if(img->GetColorDepth() == 8){

		// Get pointers to 8 bit data
		uint8_t * redData8 = img->Get8BitDataRed();
		uint8_t * greenData8 = img->Get8BitDataGreen();
		uint8_t * blueData8 = img->Get8BitDataBlue();

		uint8_t * redData8Dup = tempImage->Get8BitDataRed();
		uint8_t * greenData8Dup = tempImage->Get8BitDataGreen();
		uint8_t * blueData8Dup = tempImage->Get8BitDataBlue();

		for (int i = dataStart; i < dataEnd; i++) {

			if(forceStop) { return; }

			// Get x and y coordinates from current index of data
			x = i % newWidth;
			y = i / newWidth;
			
			// Scale point
			newX = x * xRatio;
			newY = y * yRatio;

			// Round double to int
			newX = wxRound(newX);
			newY = wxRound(newY);

			// Veirfy pixel location is in bounds of original image size
			if (newX > 0 && newX < width && newY > 0 && newY < height) {

				// Get new single dimmension array index from new x and y location
				newI = newY * width + newX;

				// Copy pixel from old location to new location
				redData8Dup[i] = redData8[newI];
				greenData8Dup[i] = greenData8[newI];
				blueData8Dup[i] = blueData8[newI];
			}	
		}
	}

	// Process 16 bit data
	else {

		// Get pointers to 16 bit data
		uint16_t * redData16 = img->Get16BitDataRed();
		uint16_t * greenData16 = img->Get16BitDataGreen();
		uint16_t * blueData16 = img->Get16BitDataBlue();

		uint16_t * redData16Dup = tempImage->Get16BitDataRed();
		uint16_t * greenData16Dup = tempImage->Get16BitDataGreen();
		uint16_t * blueData16Dup = tempImage->Get16BitDataBlue();

		for (int i = dataStart; i < dataEnd; i++) {

			if(forceStop) { return; }

			// Get x and y coordinates from current index of data
			x = i % newWidth;
			y = i / newWidth;
			
			// Scale point
			newX = x * xRatio;
			newY = y * yRatio;

			// Round double to int
			newX = wxRound(newX);
			newY = wxRound(newY);

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
	}
}

void Processor::ScaleBilinear(int dataStart, int dataEnd) {

	// Get number of pixels for the image
	int newDataSize = tempImage->GetWidth() * tempImage->GetHeight();
	if (dataStart < 0 || dataEnd < 0) {
		dataStart = 0;
		dataEnd = newDataSize;
	}

	double useNearestNeighborDistance = 0.0001;

	int width = img->GetWidth();
	int height = img->GetHeight();

	int newWidth = tempImage->GetWidth();
	int newHeight = tempImage->GetHeight();

	double xRatio = (double)width / (double)newWidth;
	double yRatio = (double)height / (double)newHeight;

	int x = 0;
	int y = 0;
	double newX = 0;
	double newY = 0;

	int32_t tempRed;
	int32_t tempGreen;
	int32_t tempBlue;

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

	// Process 8 bit data
	if(img->GetColorDepth() == 8){

		// Get pointers to 8 bit data
		uint8_t * redData8 = img->Get8BitDataRed();
		uint8_t * greenData8 = img->Get8BitDataGreen();
		uint8_t * blueData8 = img->Get8BitDataBlue();

		uint8_t * redData8Dup = tempImage->Get8BitDataRed();
		uint8_t * greenData8Dup = tempImage->Get8BitDataGreen();
		uint8_t * blueData8Dup = tempImage->Get8BitDataBlue();

		for (int i = dataStart; i < dataEnd; i++) {

			if(forceStop) { return; }

			// Get x and y coordinates from current index of data
			x = i % newWidth;
			y = i / newWidth;

			// Scale point
			newX = x * xRatio;
			newY = y * yRatio;
		
			// Veirfy pixel location is in bounds of original image size
			if (newX > 1 && newX < width - 1 && newY > 1 && newY < height - 1) {

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
					continue;
				}

				// If distance 2 is close enough to an actual point, use that closest point
				if (distance2 < useNearestNeighborDistance) {

					newI2 = point3 * width + point2;
					redData8Dup[i] = redData8[newI2];
					greenData8Dup[i] = greenData8[newI2];
					blueData8Dup[i] = blueData8[newI2];
					continue;
				}

				// If distance 3 is close enough to an actual point, use that closest point
				if (distance3 < useNearestNeighborDistance) {

					newI3 = point4 * width + point1;
					redData8Dup[i] = redData8[newI3];
					greenData8Dup[i] = greenData8[newI3];
					blueData8Dup[i] = blueData8[newI3];
					continue;
				}

				// If distance 4 is close enough to an actual point, use that closest point
				if (distance4 < useNearestNeighborDistance) {

					newI4 = point4 * width + point1;
					redData8Dup[i] = redData8[newI4];
					greenData8Dup[i] = greenData8[newI4];
					blueData8Dup[i] = blueData8[newI4];
					continue;
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

	}
	// Process 16 bit data
	else {

		// Get pointers to 16 bit data
		uint16_t * redData16 = img->Get16BitDataRed();
		uint16_t * greenData16 = img->Get16BitDataGreen();
		uint16_t * blueData16 = img->Get16BitDataBlue();

		uint16_t * redData16Dup = tempImage->Get16BitDataRed();
		uint16_t * greenData16Dup = tempImage->Get16BitDataGreen();
		uint16_t * blueData16Dup = tempImage->Get16BitDataBlue();

		for (int i = dataStart; i < dataEnd; i++) {

			if(forceStop) { return; }

			// Get x and y coordinates from current index of data
			x = i % newWidth;
			y = i / newWidth;

			// Scale point
			newX = x * xRatio;
			newY = y * yRatio;

			// Veirfy pixel location is in bounds of original image size
			if (newX > 1 && newX < width - 1 && newY > 1 && newY < height - 1) {

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
					continue;
				}

				// If distance 2 is close enough to an actual point, use that closest point
				if (distance2 < useNearestNeighborDistance) {

					newI2 = point3 * width + point2;
					redData16Dup[i] = redData16[newI2];
					greenData16Dup[i] = greenData16[newI2];
					blueData16Dup[i] = blueData16[newI2];
					continue;
				}

				// If distance 3 is close enough to an actual point, use that closest point
				if (distance3 < useNearestNeighborDistance) {

					newI3 = point4 * width + point1;
					redData16Dup[i] = redData16[newI3];
					greenData16Dup[i] = greenData16[newI3];
					blueData16Dup[i] = blueData16[newI3];
					continue;
				}

				// If distance 4 is close enough to an actual point, use that closest point
				if (distance4 < useNearestNeighborDistance) {

					newI4 = point4 * width + point1;
					redData16Dup[i] = redData16[newI4];
					greenData16Dup[i] = greenData16[newI4];
					blueData16Dup[i] = blueData16[newI4];
					continue;
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
	}
}

void Processor::ScaleBicubic(int dataStart, int dataEnd) {

	// Get number of pixels for the image
	int newDataSize = tempImage->GetWidth() * tempImage->GetHeight();
	if (dataStart < 0 || dataEnd < 0) {
		dataStart = 0;
		dataEnd = newDataSize;
	}

	double useNearestNeighborDistance = 0.0001;

	int width = img->GetWidth();
	int height = img->GetHeight();

	int newWidth = tempImage->GetWidth();
	int newHeight = tempImage->GetHeight();

	double xRatio = (double)width / (double)newWidth;
	double yRatio = (double)height / (double)newHeight;

	int x = 0;
	int y = 0;
	double newX = 0;
	double newY = 0;

	int32_t tempRed;
	int32_t tempGreen;
	int32_t tempBlue;
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

	// Process 8 bit data
	if(img->GetColorDepth() == 8){

		// Get pointers to 8 bit data
		uint8_t * redData8 = img->Get8BitDataRed();
		uint8_t * greenData8 = img->Get8BitDataGreen();
		uint8_t * blueData8 = img->Get8BitDataBlue();

		uint8_t * redData8Dup = tempImage->Get8BitDataRed();
		uint8_t * greenData8Dup = tempImage->Get8BitDataGreen();
		uint8_t * blueData8Dup = tempImage->Get8BitDataBlue();

		for (int i = dataStart; i < dataEnd; i++) {

			if(forceStop) { return; }

			// Get x and y coordinates from current index of data
			x = i % newWidth;
			y = i / newWidth;

			// Scale point
			newX = x * xRatio;
			newY = y * yRatio;

			// Veirfy pixel location is in bounds of original image size
			if (newX > 2 && newX < width - 2 && newY > 2 && newY < height - 2) {

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
					redData8Dup[i] = redData8[newI1];
					greenData8Dup[i] = greenData8[newI1];
					blueData8Dup[i] = blueData8[newI1];
					continue;
				}

				// If distance 2 is close enough to an actual point, use that closest point
				else if (distance2 < useNearestNeighborDistance) {

					newI2 = point5 * width + point2;
					redData8Dup[i] = redData8[newI2];
					greenData8Dup[i] = greenData8[newI2];
					blueData8Dup[i] = blueData8[newI2];
					continue;
				}

				// If distance 3 is close enough to an actual point, use that closest point
				else if (distance3 < useNearestNeighborDistance) {

					newI3 = point5 * width + point3;
					redData8Dup[i] = redData8[newI3];
					greenData8Dup[i] = greenData8[newI3];
					blueData8Dup[i] = blueData8[newI3];
					continue;
				}

				// If distance 4 is close enough to an actual point, use that closest point
				else if (distance4 < useNearestNeighborDistance) {

					newI4 = point5 * width + point4;
					redData8Dup[i] = redData8[newI4];
					greenData8Dup[i] = greenData8[newI4];
					blueData8Dup[i] = blueData8[newI4];
					continue;
				}

				// If distance 5 is close enough to an actual point, use that closest point
				else if (distance5 < useNearestNeighborDistance) {

					newI5 = point6 * width + point1;
					redData8Dup[i] = redData8[newI5];
					greenData8Dup[i] = greenData8[newI5];
					blueData8Dup[i] = blueData8[newI5];
					continue;
				}

				// If distance 6 is close enough to an actual point, use that closest point
				else if (distance6 < useNearestNeighborDistance) {

					newI6 = point6 * width + point2;
					redData8Dup[i] = redData8[newI6];
					greenData8Dup[i] = greenData8[newI6];
					blueData8Dup[i] = blueData8[newI6];
					continue;
				}

				// If distance 7 is close enough to an actual point, use that closest point
				else if (distance7 < useNearestNeighborDistance) {

					newI7 = point6 * width + point3;
					redData8Dup[i] = redData8[newI7];
					greenData8Dup[i] = greenData8[newI7];
					blueData8Dup[i] = blueData8[newI7];
					continue;
				}

				// If distance 8 is close enough to an actual point, use that closest point
				else if (distance8 < useNearestNeighborDistance) {

					newI8 = point6 * width + point4;
					redData8Dup[i] = redData8[newI8];
					greenData8Dup[i] = greenData8[newI8];
					blueData8Dup[i] = blueData8[newI8];
					continue;
				}
				// If distance 9 is close enough to an actual point, use that closest point
				else if (distance9 < useNearestNeighborDistance) {

					newI9 = point7 * width + point1;
					redData8Dup[i] = redData8[newI9];
					greenData8Dup[i] = greenData8[newI9];
					blueData8Dup[i] = blueData8[newI9];
					continue;
				}

				// If distance 10 is close enough to an actual point, use that closest point
				else if (distance10 < useNearestNeighborDistance) {

					newI10 = point7 * width + point2;
					redData8Dup[i] = redData8[newI10];
					greenData8Dup[i] = greenData8[newI10];
					blueData8Dup[i] = blueData8[newI10];
					continue;
				}

				// If distance 11 is close enough to an actual point, use that closest point
				else if (distance11 < useNearestNeighborDistance) {

					newI11 = point7 * width + point3;
					redData8Dup[i] = redData8[newI11];
					greenData8Dup[i] = greenData8[newI11];
					blueData8Dup[i] = blueData8[newI11];
					continue;
				}

				// If distance 12 is close enough to an actual point, use that closest point
				else if (distance12 < useNearestNeighborDistance) {

					newI12 = point7 * width + point4;
					redData8Dup[i] = redData8[newI12];
					greenData8Dup[i] = greenData8[newI12];
					blueData8Dup[i] = blueData8[newI12];
					continue;
				}

				// If distance 13 is close enough to an actual point, use that closest point
				else if (distance13 < useNearestNeighborDistance) {

					newI13 = point8 * width + point1;
					redData8Dup[i] = redData8[newI13];
					greenData8Dup[i] = greenData8[newI13];
					blueData8Dup[i] = blueData8[newI13];
					continue;
				}

				// If distance 14 is close enough to an actual point, use that closest point
				else if (distance14 < useNearestNeighborDistance) {

					newI14 = point8 * width + point2;
					redData8Dup[i] = redData8[newI14];
					greenData8Dup[i] = greenData8[newI14];
					blueData8Dup[i] = blueData8[newI14];
					continue;
				}

				// If distance 15 is close enough to an actual point, use that closest point
				else if (distance15 < useNearestNeighborDistance) {

					newI15 = point8 * width + point3;
					redData8Dup[i] = redData8[newI15];
					greenData8Dup[i] = greenData8[newI15];
					blueData8Dup[i] = blueData8[newI15];
					continue;
				}

				// If distance 16 is close enough to an actual point, use that closest point
				else if (distance16 < useNearestNeighborDistance) {

					newI16 = point8 * width + point4;
					redData8Dup[i] = redData8[newI16];
					greenData8Dup[i] = greenData8[newI16];
					blueData8Dup[i] = blueData8[newI16];
					continue;
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
	}

	// Process 16 bit data
	else {

		// Get pointers to 16 bit data
		uint16_t * redData16 = img->Get16BitDataRed();
		uint16_t * greenData16 = img->Get16BitDataGreen();
		uint16_t * blueData16 = img->Get16BitDataBlue();

		uint16_t * redData16Dup = tempImage->Get16BitDataRed();
		uint16_t * greenData16Dup = tempImage->Get16BitDataGreen();
		uint16_t * blueData16Dup = tempImage->Get16BitDataBlue();

		for (int i = dataStart; i < dataEnd; i++) {

			if(forceStop) { return; }

			// Get x and y coordinates from current index of data
			x = i % newWidth;
			y = i / newWidth;

			// Scale point
			newX = x * xRatio;
			newY = y * yRatio;

			// Veirfy pixel location is in bounds of original image size
			if (newX > 2 && newX < width - 2 && newY > 2 && newY < height - 2) {

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
					continue;
				}

				// If distance 2 is close enough to an actual point, use that closest point
				else if (distance2 < useNearestNeighborDistance) {

					newI2 = point5 * width + point2;
					redData16Dup[i] = redData16[newI2];
					greenData16Dup[i] = greenData16[newI2];
					blueData16Dup[i] = blueData16[newI2];
					continue;
				}

				// If distance 3 is close enough to an actual point, use that closest point
				else if (distance3 < useNearestNeighborDistance) {

					newI3 = point5 * width + point3;
					redData16Dup[i] = redData16[newI3];
					greenData16Dup[i] = greenData16[newI3];
					blueData16Dup[i] = blueData16[newI3];
					continue;
				}

				// If distance 4 is close enough to an actual point, use that closest point
				else if (distance4 < useNearestNeighborDistance) {

					newI4 = point5 * width + point4;
					redData16Dup[i] = redData16[newI4];
					greenData16Dup[i] = greenData16[newI4];
					blueData16Dup[i] = blueData16[newI4];
					continue;
				}

				// If distance 5 is close enough to an actual point, use that closest point
				else if (distance5 < useNearestNeighborDistance) {

					newI5 = point6 * width + point1;
					redData16Dup[i] = redData16[newI5];
					greenData16Dup[i] = greenData16[newI5];
					blueData16Dup[i] = blueData16[newI5];
					continue;
				}

				// If distance 6 is close enough to an actual point, use that closest point
				else if (distance6 < useNearestNeighborDistance) {

					newI6 = point6 * width + point2;
					redData16Dup[i] = redData16[newI6];
					greenData16Dup[i] = greenData16[newI6];
					blueData16Dup[i] = blueData16[newI6];
					continue;
				}

				// If distance 7 is close enough to an actual point, use that closest point
				else if (distance7 < useNearestNeighborDistance) {

					newI7 = point6 * width + point3;
					redData16Dup[i] = redData16[newI7];
					greenData16Dup[i] = greenData16[newI7];
					blueData16Dup[i] = blueData16[newI7];
					continue;
				}

				// If distance 8 is close enough to an actual point, use that closest point
				else if (distance8 < useNearestNeighborDistance) {

					newI8 = point6 * width + point4;
					redData16Dup[i] = redData16[newI8];
					greenData16Dup[i] = greenData16[newI8];
					blueData16Dup[i] = blueData16[newI8];
					continue;
				}
				// If distance 9 is close enough to an actual point, use that closest point
				else if (distance9 < useNearestNeighborDistance) {

					newI9 = point7 * width + point1;
					redData16Dup[i] = redData16[newI9];
					greenData16Dup[i] = greenData16[newI9];
					blueData16Dup[i] = blueData16[newI9];
					continue;
				}

				// If distance 10 is close enough to an actual point, use that closest point
				else if (distance10 < useNearestNeighborDistance) {

					newI10 = point7 * width + point2;
					redData16Dup[i] = redData16[newI10];
					greenData16Dup[i] = greenData16[newI10];
					blueData16Dup[i] = blueData16[newI10];
					continue;
				}

				// If distance 11 is close enough to an actual point, use that closest point
				else if (distance11 < useNearestNeighborDistance) {

					newI11 = point7 * width + point3;
					redData16Dup[i] = redData16[newI11];
					greenData16Dup[i] = greenData16[newI11];
					blueData16Dup[i] = blueData16[newI11];
					continue;
				}

				// If distance 12 is close enough to an actual point, use that closest point
				else if (distance12 < useNearestNeighborDistance) {

					newI12 = point7 * width + point4;
					redData16Dup[i] = redData16[newI12];
					greenData16Dup[i] = greenData16[newI12];
					blueData16Dup[i] = blueData16[newI12];
					continue;
				}

				// If distance 13 is close enough to an actual point, use that closest point
				else if (distance13 < useNearestNeighborDistance) {

					newI13 = point8 * width + point1;
					redData16Dup[i] = redData16[newI13];
					greenData16Dup[i] = greenData16[newI13];
					blueData16Dup[i] = blueData16[newI13];
					continue;
				}

				// If distance 14 is close enough to an actual point, use that closest point
				else if (distance14 < useNearestNeighborDistance) {

					newI14 = point8 * width + point2;
					redData16Dup[i] = redData16[newI14];
					greenData16Dup[i] = greenData16[newI14];
					blueData16Dup[i] = blueData16[newI14];
					continue;
				}

				// If distance 15 is close enough to an actual point, use that closest point
				else if (distance15 < useNearestNeighborDistance) {

					newI15 = point8 * width + point3;
					redData16Dup[i] = redData16[newI15];
					greenData16Dup[i] = greenData16[newI15];
					blueData16Dup[i] = blueData16[newI15];
					continue;
				}

				// If distance 16 is close enough to an actual point, use that closest point
				else if (distance16 < useNearestNeighborDistance) {

					newI16 = point8 * width + point4;
					redData16Dup[i] = redData16[newI16];
					greenData16Dup[i] = greenData16[newI16];
					blueData16Dup[i] = blueData16[newI16];
					continue;
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
	}
}

void Processor::SetupCrop(int newWidth, int newHeight){

	// Create temp image with new scale size
	tempImage = new Image();
	tempImage->SetWidth(newWidth);
	tempImage->SetHeight(newHeight);
	if (this->GetImage()->GetColorDepth() == 16) { tempImage->Enable16Bit(); }
	tempImage->InitImage();
}

void Processor::CleanupCrop(){

	// Copy temp image to image
	delete img;
	img = new Image(*tempImage);

	// Cleanup temp image
	if(tempImage != NULL){
		delete tempImage;
		tempImage = NULL;
	}
}

void Processor::Crop(int startPointX, int startPointY, int dataStart, int dataEnd){

	// Get number of pixels for the image
	int newDataSize = tempImage->GetWidth() * tempImage->GetHeight();
	if (dataStart < 0 || dataEnd < 0) {
		dataStart = 0;
		dataEnd = newDataSize;
	}

	int width = img->GetWidth();

	int newWidth = tempImage->GetWidth();
	int newHeight = tempImage->GetHeight();

	int x = 0;
	int y = 0;

	int newI = 0;

	// Process 8 bit data
	if(img->GetColorDepth() == 8){

		// Get pointers to 8 bit data
		uint8_t * redData8 = img->Get8BitDataRed();
		uint8_t * greenData8 = img->Get8BitDataGreen();
		uint8_t * blueData8 = img->Get8BitDataBlue();

		uint8_t * redData8Dup = tempImage->Get8BitDataRed();
		uint8_t * greenData8Dup = tempImage->Get8BitDataGreen();
		uint8_t * blueData8Dup = tempImage->Get8BitDataBlue();

		for (int i = dataStart; i < dataEnd; i++) {

			if(forceStop) { return; }

			// Get x and y coordinates from current index of data
			x = i % width;
			y = i / width;
			
			// Veirfy pixel location is in bounds of new image size
			if (x >= startPointX && x < (newWidth + startPointX) && y >= startPointY && y < (newHeight + startPointY)) {

				// Get new single dimmension array index from new x and y location
				newI = ((y - startPointY)* (newWidth - startPointX)) + (x - startPointX);

				// Copy pixel from old location to new location
				redData8Dup[newI] = redData8[i];
				greenData8Dup[newI] = greenData8[i];
				blueData8Dup[newI] = blueData8[i];
			}	
		}
	}

	// Process 16 bit data
	else {

		// Get pointers to 16 bit data
		uint16_t * redData16 = img->Get16BitDataRed();
		uint16_t * greenData16 = img->Get16BitDataGreen();
		uint16_t * blueData16 = img->Get16BitDataBlue();

		uint16_t * redData16Dup = tempImage->Get16BitDataRed();
		uint16_t * greenData16Dup = tempImage->Get16BitDataGreen();
		uint16_t * blueData16Dup = tempImage->Get16BitDataBlue();

		for (int i = dataStart; i < dataEnd; i++) {

			if(forceStop) { return; }

			// Get x and y coordinates from current index of data
			x = i % width;
			y = i / width;
			
			// Veirfy pixel location is in bounds of new image size
			if (x >= startPointX && x < (newWidth + startPointX) && y >= startPointY && y < (newHeight + startPointY)) {

				// Get new single dimmension array index from new x and y location
				newI = (y - startPointY) * newWidth + (x-startPointX);

				// Copy pixel from old location to new location
				redData16Dup[newI] = redData16[i];
				greenData16Dup[newI] = greenData16[i];
				blueData16Dup[newI] = blueData16[i];
			}		
		}
	}
}

void Processor::MirrorHorizontal(int dataStart, int dataEnd) {

	int width = img->GetWidth();

	// Get number of pixels for the image
	int dataSize = img->GetWidth() * img->GetHeight();
	if (dataStart < 0 || dataEnd < 0) {
		dataStart = 0;
		dataEnd = dataSize;
	}

	int32_t tempRed;
	int32_t tempGreen;
	int32_t tempBlue;

	int x = 0;
	int y = 0;

	int widthOver2 = width / 2;

	int newI = 0;

	// Process 8 bit data
	if(img->GetColorDepth() == 8){

		// Get pointers to 8 bit data
		uint8_t * redData8 = img->Get8BitDataRed();
		uint8_t * greenData8 = img->Get8BitDataGreen();
		uint8_t * blueData8 = img->Get8BitDataBlue();

		for (int i = dataStart; i < dataEnd; i++) {

			if(forceStop) { return; }

			x = i % width;
			y = i / width;
			newI = (y * width) + (width - x);

			if (x < widthOver2 && newI < dataSize) {

				tempRed = redData8[i];
				redData8[i] = redData8[newI];
				redData8[newI] = tempRed;

				tempGreen = greenData8[i];
				greenData8[i] = greenData8[newI];
				greenData8[newI] = tempGreen;

				tempBlue = blueData8[i];
				blueData8[i] = blueData8[newI];
				blueData8[newI] = tempBlue;
			}
		}
	}

	// Process 16 bit data
	else {

		// Get pointers to 16 bit data
		uint16_t * redData16 = img->Get16BitDataRed();
		uint16_t * greenData16 = img->Get16BitDataGreen();
		uint16_t * blueData16 = img->Get16BitDataBlue();

		for (int i = dataStart; i < dataEnd; i++) {

			if(forceStop) { return; }

			x = i % width;
			y = i / width;
			newI = (y * width) + (width - x);

			if (x < widthOver2  && newI < dataSize) {

				tempRed = redData16[i];
				redData16[i] = redData16[newI];
				redData16[newI] = tempRed;

				tempGreen = greenData16[i];
				greenData16[i] = greenData16[newI];
				greenData16[newI] = tempGreen;

				tempBlue = blueData16[i];
				blueData16[i] = blueData16[newI];
				blueData16[newI] = tempBlue;
			}
		}
	}
}

void Processor::MirrorVertical(int dataStart, int dataEnd) {

	int width = img->GetWidth();
	int height = img->GetHeight();

	// Get number of pixels for the image
	int dataSize = img->GetWidth() * img->GetHeight();
	if (dataStart < 0 || dataEnd < 0) {
		dataStart = 0;
		dataEnd = dataSize;
	}

	int32_t tempRed;
	int32_t tempGreen;
	int32_t tempBlue;

	int x = 0;
	int y = 0;

	int heightOver2 = height / 2;

	int newI = 0;

	// Process 8 bit data
	if(img->GetColorDepth() == 8){

		// Get pointers to 8 bit data
		uint8_t * redData8 = img->Get8BitDataRed();
		uint8_t * greenData8 = img->Get8BitDataGreen();
		uint8_t * blueData8 = img->Get8BitDataBlue();

		for (int i = dataStart; i < dataEnd; i++) {

			if(forceStop) { return; }

			x = i % width;
			y = i / width;
			newI = (width *(height - y)) + x;

			if (y < heightOver2  && newI < dataSize) {

				tempRed = redData8[i];
				redData8[i] = redData8[newI];
				redData8[newI] = tempRed;

				tempGreen = greenData8[i];
				greenData8[i] = greenData8[newI];
				greenData8[newI] = tempGreen;

				tempBlue = blueData8[i];
				blueData8[i] = blueData8[newI];
				blueData8[newI] = tempBlue;
			}
		}
	}

	// Process 16 bit data
	else {

		// Get pointers to 16 bit data
		uint16_t * redData16 = img->Get16BitDataRed();
		uint16_t * greenData16 = img->Get16BitDataGreen();
		uint16_t * blueData16 = img->Get16BitDataBlue();

		for (int i = dataStart; i < dataEnd; i++) {

			if(forceStop) { return; }

			x = i % width;
			y = i / width;
			newI = (width *(height - y)) + x;

			if (y < heightOver2 && newI < dataSize) {

				tempRed = redData16[i];
				redData16[i] = redData16[newI];
				redData16[newI] = tempRed;

				tempGreen = greenData16[i];
				greenData16[i] = greenData16[newI];
				greenData16[newI] = tempGreen;

				tempBlue = blueData16[i];
				blueData16[i] = blueData16[newI];
				blueData16[newI] = tempBlue;
			}
		}
	}
}

void Processor::RGBtoXYZ(RGB * rgb, XYZ * xyz, int colorSpace) {

	float tempR = rgb->R;
	float tempG = rgb->G;
	float tempB = rgb->B;

	if (tempR > 0.04045f) {
		tempR = pow(((tempR + 0.055f) / 1.055f), 2.4f);
	}
	else {
		tempR = tempR / 12.92f;
	}
	if (tempG > 0.04045f) {
		tempG = pow(((tempG + 0.055f) / 1.055f), 2.4f);
	}
	else {
		tempG = tempG / 12.92f;
	}
	if (tempB > 0.04045f) {
		tempB = pow(((tempB + 0.055f) / 1.055f), 2.4f);
	}
	else {
		tempB = tempB / 12.92f;
	}

	tempR *= 100.0f;
	tempG *= 100.0f;
	tempB *= 100.0f;

	switch (colorSpace) {

	case ADOBE_RGB:
		xyz->X = (tempR * 0.5767309f) + (tempG * 0.1855540f) + (tempB * 0.1881852f);
		xyz->Y = (tempR * 0.2973769f) + (tempG * 0.6273491f) + (tempB * 0.0752741f);
		xyz->Z = (tempR * 0.0270343f) + (tempG * 0.0706872f) + (tempB * 0.9911085f);
		break;

	case PROPHOTO_RGB:
		xyz->X = (tempR * 0.7976749f) + (tempG * 0.1351917f) + (tempB * 0.0313534f);
		xyz->Y = (tempR * 0.2880402f) + (tempG * 0.7118741f) + (tempB * 0.0000857f);
		xyz->Z = (tempR * 0.0000000f) + (tempG * 0.0000000f) + (tempB * 0.8252100f);
		break;

	case sRGB:
		xyz->X = (tempR * 0.4124564f) + (tempG * 0.3575761f) + (tempB * 0.1804375f);
		xyz->Y = (tempR * 0.2126729f) + (tempG * 0.7151522f) + (tempB * 0.0721750f);
		xyz->Z = (tempR * 0.0193339f) + (tempG * 0.1191920f) + (tempB * 0.9503041f);
		break;

	case WIDE_GAMUT_RGB:
		xyz->X = (tempR * 0.7161046f) + (tempG * 0.1009296f) + (tempB * 0.1471858f);
		xyz->Y = (tempR * 0.2581874f) + (tempG * 0.7249378f) + (tempB * 0.0168748f);
		xyz->Z = (tempR * 0.0000000f) + (tempG * 0.0517813f) + (tempB * 0.7734287f);
		break;
	}
}

void Processor::XYZtoRGB(XYZ * xyz, RGB * rgb, int colorSpace) {

	float tempX = (float)xyz->X / 100.0f;
	float tempY = (float)xyz->Y / 100.0f;
	float tempZ = (float)xyz->Z / 100.0f;

	float tempR = 0.0f;
	float tempG = 0.0f;
	float tempB = 0.0f;

	switch (colorSpace) {

	case ADOBE_RGB:
		tempR = (tempX *  2.0413690f) + (tempY * -0.5649464f) + (tempZ * -0.3446944f);
		tempG = (tempX * -0.9692660f) + (tempY *  1.8760108f) + (tempZ *  0.0415560f);
		tempB = (tempX *  0.0134474f) + (tempY * -0.1183897f) + (tempZ *  1.0154096f);
		break;

	case PROPHOTO_RGB:
		tempR = (tempX *  1.3459433f) + (tempY * -0.2556075f) + (tempZ * -0.0511118f);
		tempG = (tempX * -0.5445989f) + (tempY *  1.5081673f) + (tempZ *  0.0205351f);
		tempB = (tempX *  0.0000000f) + (tempY *  0.0000000f) + (tempZ *  1.2118128f);
		break;

	case sRGB:
		tempR = (tempX *  3.2404542f) + (tempY * -1.5371385f) + (tempZ * -0.4985314f);
		tempG = (tempX * -0.9692660f) + (tempY *  1.8760108f) + (tempZ *  0.0415560f);
		tempB = (tempX *  0.0556434f) + (tempY * -0.2040259f) + (tempZ *  1.0572252f);
		break;

	case WIDE_GAMUT_RGB:
		tempR = (tempX *  1.4628067f) + (tempY * -0.1840623f) + (tempZ * -0.2743606f);
		tempG = (tempX * -0.5217933f) + (tempY *  1.4472381f) + (tempZ *  0.0677227f);
		tempB = (tempX *  0.0349342f) + (tempY * -0.0968930f) + (tempZ *  1.2884099f);
		break;
	}

	if (tempR > 0.0031308f) {
		tempR = 1.055f * pow(tempR, 1.0f / 2.4f) - 0.055f;
	}
	else {
		tempR = 12.92f * tempR;
	}
	if (tempG > 0.0031308f) {
		tempG = 1.055f * pow(tempG, 1.0f / 2.4f) - 0.055f;
	}
	else {
		tempG = 12.92f * tempG;
	}
	if (tempB > 0.0031308f) {
		tempB = 1.055f * pow(tempB, 1.0f / 2.4f) - 0.055f;
	}
	else {
		tempB = 12.92f * tempB;
	}

	rgb->R = tempR;
	rgb->G = tempG;
	rgb->B = tempB;
}

void Processor::XYZtoLAB(XYZ * xyz, LAB * lab) {

	float tempX = xyz->X / 95.047f;
	float tempY = xyz->Y / 100.000f;
	float tempZ = xyz->Z / 108.883f;

	if (tempX > 0.008856f) {
		tempX = pow(tempX, 1.0f / 3.0f);
	}
	else {
		tempX = (7.787f * tempX) + (16.0f / 116.0f);
	}
	if (tempY  > 0.008856f) {
		tempY = pow(tempY, 1.0f / 3.0f);
	}
	else {
		tempY = (7.787f * tempY) + (16.0f / 116.0f);
	}
	if (tempZ > 0.008856f) {
		tempZ = pow(tempZ, 1.0f / 3.0f);
	}
	else {
		tempZ = (7.787f * tempZ) + (16.0f / 116.0f);
	}

	lab->L = (116.0f * tempY) - 16.0f;
	lab->A = 500.0f  * (tempX - tempY);
	lab->B = 200.0f  * (tempY - tempZ);

}

void Processor::LABtoXYZ(LAB * lab, XYZ * xyz) {

	float tempY = (lab->L + 16.0f) / 116.0f;
	float tempX = (lab->A / 500.0f) + tempY;
	float tempZ = tempY - (lab->B / 200.0F);

	if (tempY*tempY*tempY > 0.008856) {
		tempY = tempY*tempY*tempY;
	}
	else {
		tempY = (tempY - (16.0f / 116.0f)) / 7.787f;
	}
	if (tempX*tempX*tempX > 0.008856) {
		tempX = tempX*tempX*tempX;
	}
	else {
		tempX = (tempX - (16.0f / 116.0f)) / 7.787f;
	}
	if (tempZ*tempZ*tempZ > 0.008856) {
		tempZ = tempZ*tempZ*tempZ;
	}
	else {
		tempZ = (tempZ - (16.0f / 116.0f)) / 7.787f;
	}

	xyz->X = tempX * 95.047f;
	xyz->Y = tempY * 100.0f;
	xyz->Z = tempZ * 108.883f;
}

Processor::ProcessThread::ProcessThread(Processor * processor, ProcessorEdit * edit) : wxThread(wxTHREAD_DETACHED) {
	procParent = processor;
	editVec.push_back(edit);
	terminated = false;
}

Processor::ProcessThread::ProcessThread(Processor * processor, wxVector<ProcessorEdit*> edits) : wxThread(wxTHREAD_DETACHED) {
	procParent = processor;
	editVec = edits;
	terminated = false;
}

void Processor::ProcessThread::Terminate(){
	terminated = true;
}

wxThread::ExitCode Processor::ProcessThread::Entry() {

	// Immediatly return if there is no vaild image
	if(procParent->GetImage() == NULL){
		return 0;
	}
	if(procParent->GetImage()->GetWidth() < 1 || procParent->GetImage()->GetHeight() < 1){
		return 0;
	}

	// Wait for last thread to finish
	while (procParent->GetLocked() && !terminated) {
		this->Sleep(20);
	}

	// Wait for raw image to process (must process raw before image data)
	while (procParent->GetLockedRaw()) {
		this->Sleep(20);
	}

	// Lock and revert to original image, so we have a clean slate
	procParent->Lock();
	procParent->RevertToOriginalImage(true);

	// Get number of edits and set to a string to display in UI
	size_t numberOfEdits = editVec.size();
	wxString numEditsStr;
	numEditsStr << numberOfEdits;

	// Go through each edit one by one.  Each of these will invoke at least 1 child thread for the edit itself
	for (size_t editIndex = 0; editIndex < numberOfEdits; editIndex++) {

		if(procParent->forceStop){
			procParent->SendMessageToParent("");
			procParent->Unlock();
			return 0;
		}

		// Get the next edit to take place
		ProcessorEdit * curEdit = editVec.at(editIndex);

		// Get the type of edit to perform
		int editToComplete = curEdit->GetEditType();

		// Skip disabled edits
		if (curEdit->GetDisabled()) {
			procParent->SetUpdated(true);
			continue;
		}

		wxString curEditStr;
		curEditStr << (editIndex + 1);

		wxString fullEditNumStr = " (" + curEditStr + "/" + numEditsStr + ")";

		switch (editToComplete) {

		case ProcessorEdit::EditType::RAW:
			procParent->SetUpdated(true);
			continue;

			// Peform a Brightness Adjustment edit
			case ProcessorEdit::EditType::ADJUST_BRIGHTNESS: {

				procParent->SendMessageToParent("Processing Adjust Brightness Edit" + fullEditNumStr);

				// Get all parameters from the edit
				double brighnessAmount = curEdit->GetParam(0);
				double detailsPreservation = curEdit->GetParam(1);
				double toneSetting = curEdit->GetParam(2);

				int toneFlag = curEdit->GetFlag(0);

				// Multithread if needed
				if(procParent->GetMultithread()){
				
					int numThreads = procParent->GetNumThreads();
					int dataSize = procParent->GetImage()->GetWidth() * procParent->GetImage()->GetHeight();
					int chunkSize = dataSize / numThreads;

					omp_set_dynamic(0);     // Explicitly disable dynamic teams
					omp_set_num_threads(numThreads); // Use 4 threads for all consecutive parallel regions

					#pragma omp parallel for 
					for(int thread = 0; thread < numThreads; thread++){

						// Process current chunk of data
						if(thread != numThreads - 1){
							procParent->AdjustBrightness(brighnessAmount, detailsPreservation, toneSetting, toneFlag, chunkSize * thread, chunkSize * (thread + 1));
						}

						// Go all the way to end of data (incase of rounding error)
						else{
							procParent->AdjustBrightness(brighnessAmount, detailsPreservation, toneSetting, toneFlag, chunkSize * thread, dataSize);
						}
					}
				}

				// Single thread
				else{
					// Perform an edit on the data through the processor
					procParent->AdjustBrightness(brighnessAmount, detailsPreservation, toneSetting, toneFlag);
				}
				procParent->SetUpdated(true);
			}
			break;

			// Peform a Shift RGB edit
			case ProcessorEdit::EditType::SHIFT_RGB: {

				procParent->SendMessageToParent("Processing Shift RGB Edit" + fullEditNumStr);

				// Get all parameters from the edit
				double allBrightShift = curEdit->GetParam(0);
				double redBrightShift = curEdit->GetParam(1);
				double greenBrightShift = curEdit->GetParam(2);
				double blueBrightShift = curEdit->GetParam(3);

				// Multithread if needed
				if(procParent->GetMultithread()){
				
					int numThreads = procParent->GetNumThreads();
					int dataSize = procParent->GetImage()->GetWidth() * procParent->GetImage()->GetHeight();
					int chunkSize = dataSize / numThreads;

					omp_set_dynamic(0);     // Explicitly disable dynamic teams
					omp_set_num_threads(numThreads); // Use 4 threads for all consecutive parallel regions

					#pragma omp parallel for 
					for(int thread = 0; thread < numThreads; thread++){

						// Process current chunk of data
						if(thread != numThreads - 1){
							procParent->ShiftRGB(allBrightShift, redBrightShift, greenBrightShift, blueBrightShift, chunkSize * thread, chunkSize * (thread + 1));
						}

						// Go all the way to end of data (incase of rounding error)
						else{
							procParent->ShiftRGB(allBrightShift, redBrightShift, greenBrightShift, blueBrightShift, chunkSize * thread,  dataSize);
						}
					}
				}

				// Single thread
				else{
					// Perform an edit on the data through the processor
					procParent->ShiftRGB(allBrightShift, redBrightShift, greenBrightShift, blueBrightShift);
				}
				procParent->SetUpdated(true);
			}
			break;

			// Peform an HSL adjustment
			case ProcessorEdit::EditType::ADJUST_HSL: {

				procParent->SendMessageToParent("Processing Adjust HSL Edit" + fullEditNumStr);

				// Get all parameters from the edit
				double hueShift = curEdit->GetParam(0);
				double saturationScale = curEdit->GetParam(1);
				double luminaceScale = curEdit->GetParam(2);

				// Multithread if needed
				if(procParent->GetMultithread()){
				
					int numThreads = procParent->GetNumThreads();
					int dataSize = procParent->GetImage()->GetWidth() * procParent->GetImage()->GetHeight();
					int chunkSize = dataSize / numThreads;

					omp_set_dynamic(0);     // Explicitly disable dynamic teams
					omp_set_num_threads(numThreads); // Use 4 threads for all consecutive parallel regions

					#pragma omp parallel for 
					for(int thread = 0; thread < numThreads; thread++){

						// Process current chunk of data
						if(thread != numThreads - 1){
							procParent->AdjustHSL(hueShift, saturationScale, luminaceScale, chunkSize * thread, chunkSize * (thread + 1));
						}

						// Go all the way to end of data (incase of rounding error)
						else{
							procParent->AdjustHSL(hueShift, saturationScale, luminaceScale, chunkSize * thread, dataSize);
						}
					}
				}

				// Single thread
				else{
					// Perform an edit on the data through the processor
					procParent->AdjustHSL(hueShift, saturationScale, luminaceScale);
				}
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
				double allCenter = curEdit->GetParam(4);
				double redCenter = curEdit->GetParam(5);
				double greenCenter = curEdit->GetParam(6);
				double blueCenter = curEdit->GetParam(7);

				// Multithread if needed
				if(procParent->GetMultithread()){
				
					int numThreads = procParent->GetNumThreads();
					int dataSize = procParent->GetImage()->GetWidth() * procParent->GetImage()->GetHeight();
					int chunkSize = dataSize / numThreads;

					omp_set_dynamic(0);     // Explicitly disable dynamic teams
					omp_set_num_threads(numThreads); // Use 4 threads for all consecutive parallel regions

					#pragma omp parallel for 
					for(int thread = 0; thread < numThreads; thread++){

						// Process current chunk of data
						if(thread != numThreads - 1){
							procParent->AdjustContrast(allContrast, redContrast, greenContrast, blueContrast, 
								allCenter, redCenter, greenCenter, blueCenter, chunkSize * thread, chunkSize * (thread + 1));
						}

						// Go all the way to end of data (incase of rounding error)
						else{
							procParent->AdjustContrast(allContrast, redContrast, greenContrast, blueContrast, 
								allCenter, redCenter, greenCenter, blueCenter, chunkSize * thread, dataSize);
						}
					}
				}

				// Single thread
				else{
					// Perform an edit on the data through the processor
					procParent->AdjustContrast(allContrast, redContrast, greenContrast, blueContrast, allCenter, redCenter, greenCenter, blueCenter);
				}
				procParent->SetUpdated(true);
			}
			break;

			// Peform an Adjust Contrast edit
			case ProcessorEdit::EditType::ADJUST_CONTRAST_CURVE: {

				procParent->SendMessageToParent("Processing Adjust Contrast Edit" + fullEditNumStr);

				// Get all parameters from the edit
				double allContrast = curEdit->GetParam(0);
				double redContrast = curEdit->GetParam(1);
				double greenContrast = curEdit->GetParam(2);
				double blueContrast = curEdit->GetParam(3);
				double allCenter = curEdit->GetParam(4);
				double redCenter = curEdit->GetParam(5);
				double greenCenter = curEdit->GetParam(6);
				double blueCenter = curEdit->GetParam(7);

				// Multithread if needed
				if (procParent->GetMultithread()) {

					int numThreads = procParent->GetNumThreads();
					int dataSize = procParent->GetImage()->GetWidth() * procParent->GetImage()->GetHeight();
					int chunkSize = dataSize / numThreads;

					omp_set_dynamic(0);     // Explicitly disable dynamic teams
					omp_set_num_threads(numThreads); // Use 4 threads for all consecutive parallel regions

					#pragma omp parallel for 
					for (int thread = 0; thread < numThreads; thread++) {

						// Process current chunk of data
						if (thread != numThreads - 1) {
							procParent->AdjustContrastCurve(allContrast, redContrast, greenContrast, blueContrast, 
								allCenter, redCenter, greenCenter, blueCenter, chunkSize * thread, chunkSize * (thread + 1));
						}

						// Go all the way to end of data (incase of rounding error)
						else {
							procParent->AdjustContrastCurve(allContrast, redContrast, greenContrast, blueContrast, 
								allCenter, redCenter, greenCenter, blueCenter, chunkSize * thread, dataSize);
						}
					}
				}

				// Single thread
				else {
					// Perform an edit on the data through the processor
					procParent->AdjustContrastCurve(allContrast, redContrast, greenContrast, blueContrast, 
						allCenter, redCenter, greenCenter, blueCenter);
				}
				procParent->SetUpdated(true);
			}
			break;

			// Peform a greyscale conversion, averaging RGB values
			case ProcessorEdit::EditType::CONVERT_GREYSCALE_AVG: {

				procParent->SendMessageToParent("Processing Greyscale (Average) Edit" + fullEditNumStr);

				// Multithread if needed
				if(procParent->GetMultithread()){
				
					int numThreads = procParent->GetNumThreads();
					int dataSize = procParent->GetImage()->GetWidth() * procParent->GetImage()->GetHeight();
					int chunkSize = dataSize / numThreads;

					omp_set_dynamic(0);     // Explicitly disable dynamic teams
					omp_set_num_threads(numThreads); // Use 4 threads for all consecutive parallel regions

					#pragma omp parallel for 
					for(int thread = 0; thread < numThreads; thread++){

						// Process current chunk of data
						if(thread != numThreads - 1){
							procParent->ConvertGreyscale((1.0 / 3.0), (1.0 / 3.0), (1.0 / 3.0), chunkSize * thread, chunkSize * (thread + 1));
						}

						// Go all the way to end of data (incase of rounding error)
						else{
							procParent->ConvertGreyscale((1.0 / 3.0), (1.0 / 3.0), (1.0 / 3.0), chunkSize * thread, dataSize);
						}
					}
				}

				// Single thread
				else{
					// Perform an edit on the data through the processor
					procParent->ConvertGreyscale((1.0 / 3.0), (1.0 / 3.0), (1.0 / 3.0));
				}
				procParent->SetUpdated(true);
			}
			break;

			// Peform a greyscale conversion, using humany eyesight values
			case ProcessorEdit::EditType::CONVERT_GREYSCALE_EYE: {

				procParent->SendMessageToParent("Processing Greyscale (Human Eyesight) Edit" + fullEditNumStr);

				// Multithread if needed
				if(procParent->GetMultithread()){
				
					int numThreads = procParent->GetNumThreads();
					int dataSize = procParent->GetImage()->GetWidth() * procParent->GetImage()->GetHeight();
					int chunkSize = dataSize / numThreads;

					omp_set_dynamic(0);     // Explicitly disable dynamic teams
					omp_set_num_threads(numThreads); // Use 4 threads for all consecutive parallel regions

					#pragma omp parallel for 
					for(int thread = 0; thread < numThreads; thread++){

						// Process current chunk of data
						if(thread != numThreads - 1){
							procParent->ConvertGreyscale(0.2126, 0.7152, 0.0722, chunkSize * thread, chunkSize * (thread + 1));
						}

						// Go all the way to end of data (incase of rounding error)
						else{
							procParent->ConvertGreyscale(0.2126, 0.7152, 0.0722, chunkSize * thread, dataSize);
						}
					}
				}

				// Single thread
				else{
					// Perform an edit on the data through the processor
					procParent->ConvertGreyscale(0.2126, 0.7152, 0.0722);
				}
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

				// Multithread if needed
				if(procParent->GetMultithread()){
				
					int numThreads = procParent->GetNumThreads();
					int dataSize = procParent->GetImage()->GetWidth() * procParent->GetImage()->GetHeight();
					int chunkSize = dataSize / numThreads;

					omp_set_dynamic(0);     // Explicitly disable dynamic teams
					omp_set_num_threads(numThreads); // Use 4 threads for all consecutive parallel regions

					#pragma omp parallel for 
					for(int thread = 0; thread < numThreads; thread++){

						// Process current chunk of data
						if(thread != numThreads - 1){
							procParent->ConvertGreyscale(redScale, greenScale, blueScale, chunkSize * thread, chunkSize * (thread + 1));
						}

						// Go all the way to end of data (incase of rounding error)
						else{
							procParent->ConvertGreyscale(redScale, greenScale, blueScale, chunkSize * thread, dataSize);
						}
					}
				}

				// Single thread
				else{
					// Perform an edit on the data through the processor
					procParent->ConvertGreyscale(redScale, greenScale, blueScale);
				}
				procParent->SetUpdated(true);
			}
			break;

			// Peform a greyscale conversion, using custom scalars
			case ProcessorEdit::EditType::CHANNEL_MIXER: {

				procParent->SendMessageToParent("Processing Channel Mixer Edit" + fullEditNumStr);

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

				// Multithread if needed
				if(procParent->GetMultithread()){
				
					int numThreads = procParent->GetNumThreads();
					int dataSize = procParent->GetImage()->GetWidth() * procParent->GetImage()->GetHeight();
					int chunkSize = dataSize / numThreads;

					omp_set_dynamic(0);     // Explicitly disable dynamic teams
					omp_set_num_threads(numThreads); // Use 4 threads for all consecutive parallel regions

					#pragma omp parallel for 
					for(int thread = 0; thread < numThreads; thread++){

						// Process current chunk of data
						if(thread != numThreads - 1){
							procParent->ChannelScale(redRedScale, redGreenScale, redBlueScale,
								greenRedScale, greenGreenScale, greenBlueScale,
								blueRedScale, blueGreenScale, blueBlueScale, chunkSize * thread, chunkSize * (thread + 1));
						}

						// Go all the way to end of data (incase of rounding error)
						else{
							procParent->ChannelScale(redRedScale, redGreenScale, redBlueScale,
								greenRedScale, greenGreenScale, greenBlueScale,
								blueRedScale, blueGreenScale, blueBlueScale, chunkSize * thread, dataSize);
						}
					}
				}

				// Single thread
				else{
					// Perform an edit on the data through the processor
					procParent->ChannelScale(redRedScale, redGreenScale, redBlueScale,
					greenRedScale, greenGreenScale, greenBlueScale,
					blueRedScale, blueGreenScale, blueBlueScale);
				}
				procParent->SetUpdated(true);
			}
			break;

			// Peform a 90 degree clockwise roctation
			case ProcessorEdit::EditType::ROTATE_90_CW: {

				procParent->SendMessageToParent("Processing Rotation Edit" + fullEditNumStr);
			
				// Multithread if needed
				if(procParent->GetMultithread()){
				
					procParent->SetupRotation(editToComplete, 90.0, 0);

					int numThreads = procParent->GetNumThreads();
					int dataSize = procParent->GetTempImage()->GetWidth() * procParent->GetTempImage()->GetHeight();
					int chunkSize = dataSize / numThreads;

					if(dataSize < 1){ 
						procParent->SendMessageToParent("Image rotation failure: " + procParent->GetTempImage()->GetErrorStr());
						return 0;
					}

					omp_set_dynamic(0);
					omp_set_num_threads(numThreads);

					#pragma omp parallel for 
					for(int thread = 0; thread < numThreads; thread++){

						// Process current chunk of data
						if(thread != numThreads - 1){
							procParent->Rotate90CW(chunkSize * thread, chunkSize * (thread + 1));
						}

						// Go all the way to end of data (incase of rounding error)
						else{
							procParent->Rotate90CW(chunkSize * thread,  dataSize);
						}
					}
					procParent->CleanupRotation(editToComplete);
				}

				// Single thread
				else{
					// Perform an edit on the data through the processor
					procParent->SetupRotation(editToComplete, 90.0, 0);

					if(procParent->GetTempImage()->GetWidth() < 1 || procParent->GetTempImage()->GetHeight() < 1 ){ 
						procParent->SendMessageToParent("Image rotation failure: " + procParent->GetTempImage()->GetErrorStr());
						return 0;
					}

					procParent->Rotate90CW();
					procParent->CleanupRotation(editToComplete);
				}
				procParent->SetUpdated(true);
			}
			break;


			// Peform a 180 degree clockwise roctation
			case ProcessorEdit::EditType::ROTATE_180: {

				procParent->SendMessageToParent("Processing Rotation Edit" + fullEditNumStr);

				// Perform an edit on the data through the processor
				// No multithreading for performance reasons.
				procParent->SetupRotation(editToComplete, 180.0, 0);
				procParent->Rotate180();
				procParent->CleanupRotation(editToComplete);
				procParent->SetUpdated(true);
			}
			break;

			// Peform a 270 degree clockwise roctation (90 counter clockwise)
			case ProcessorEdit::EditType::ROTATE_270_CW: {

				procParent->SendMessageToParent("Processing Rotation Edit" + fullEditNumStr);

				// Perform an edit on the data through the processor
				// No multithreading for performance reasons.
				procParent->SetupRotation(ProcessorEdit::EditType::ROTATE_180, 180.0, 0);
				procParent->Rotate180();
				procParent->CleanupRotation(ProcessorEdit::EditType::ROTATE_180);

				procParent->SetupRotation(ProcessorEdit::EditType::ROTATE_90_CW, 90.0, 0);
				procParent->Rotate90CW();
				procParent->CleanupRotation(ProcessorEdit::EditType::ROTATE_90_CW);

				procParent->SetUpdated(true);
			}
			break;

			// Peform a custom angle clockwise roctation
			case ProcessorEdit::EditType::ROTATE_CUSTOM_NEAREST: {

				procParent->SendMessageToParent("Processing Rotation Edit" + fullEditNumStr);

				// Perform an edit on the data through the processor
				double angle = curEdit->GetParam(0);
				int cropFlag = curEdit->GetFlag(2);

				if(angle == 0.0){ break; }

				// Multithread if needed
				if(procParent->GetMultithread()){
				
					procParent->SetupRotation(editToComplete, angle, cropFlag);

					int numThreads = procParent->GetNumThreads();
					int dataSize = procParent->GetTempImage()->GetWidth() * procParent->GetTempImage()->GetHeight();
					int chunkSize = dataSize / numThreads;

					omp_set_dynamic(0);
					omp_set_num_threads(numThreads);


					#pragma omp parallel for 
					for(int thread = 0; thread < numThreads; thread++){

						// Process current chunk of data
						if(thread != numThreads - 1){
							procParent->RotateCustom(angle, chunkSize * thread, chunkSize * (thread + 1));
						}

						// Go all the way to end of data (incase of rounding error)
						else{
							procParent->RotateCustom(angle, chunkSize * thread,  dataSize);
						}
					}
					procParent->CleanupRotation(editToComplete);
				}

				// Single thread
				else{
					// Perform an edit on the data through the processor
					procParent->SetupRotation(editToComplete, angle, cropFlag);
					procParent->RotateCustom(angle);
					procParent->CleanupRotation(editToComplete);
				}
				procParent->SetUpdated(true);
			}
			break;

			// Peform a custom angle clockwise roctation using bilinear interpolation
			case ProcessorEdit::EditType::ROTATE_CUSTOM_BILINEAR: {

				procParent->SendMessageToParent("Processing Rotation Edit" + fullEditNumStr);

				// Perform an edit on the data through the processor
				double angle = curEdit->GetParam(0);
				int cropFlag = curEdit->GetFlag(2);

				if(angle == 0.0){ break; }

				// Multithread if needed
				if(procParent->GetMultithread()){
				
					procParent->SetupRotation(editToComplete, angle, cropFlag);

					int numThreads = procParent->GetNumThreads();
					int dataSize = procParent->GetTempImage()->GetWidth() * procParent->GetTempImage()->GetHeight();
					int chunkSize = dataSize / numThreads;

					omp_set_dynamic(0);
					omp_set_num_threads(numThreads);

					#pragma omp parallel for 
					for(int thread = 0; thread < numThreads; thread++){

						// Process current chunk of data
						if(thread != numThreads - 1){
							procParent->RotateCustomBilinear(angle, chunkSize * thread, chunkSize * (thread + 1));
						}

						// Go all the way to end of data (incase of rounding error)
						else{
							procParent->RotateCustomBilinear(angle, chunkSize * thread,  dataSize);
						}
					}
					procParent->CleanupRotation(editToComplete);
				}

				// Single thread
				else{
					// Perform an edit on the data through the processor
					procParent->SetupRotation(editToComplete, angle, cropFlag);
					procParent->RotateCustomBilinear(angle);
					procParent->CleanupRotation(editToComplete);
				}
				procParent->SetUpdated(true);
			}
			break;

			// Peform a custom angle clockwise roctation using bicubic interpolation
			case ProcessorEdit::EditType::ROTATE_CUSTOM_BICUBIC: {

				procParent->SendMessageToParent("Processing Rotation Edit" + fullEditNumStr);

				// Perform an edit on the data through the processor
				double angle = curEdit->GetParam(0);
				int cropFlag = curEdit->GetFlag(2);

				if(angle == 0.0){ break; }

				// Multithread if needed
				if(procParent->GetMultithread()){
				
					procParent->SetupRotation(editToComplete, angle, cropFlag);

					int numThreads = procParent->GetNumThreads();
					int dataSize = procParent->GetTempImage()->GetWidth() * procParent->GetTempImage()->GetHeight();
					int chunkSize = dataSize / numThreads;

					omp_set_dynamic(0);
					omp_set_num_threads(numThreads);

					#pragma omp parallel for 
					for(int thread = 0; thread < numThreads; thread++){

						// Process current chunk of data
						if(thread != numThreads - 1){
							procParent->RotateCustomBicubic(angle, chunkSize * thread, chunkSize * (thread + 1));
						}

						// Go all the way to end of data (incase of rounding error)
						else{
							procParent->RotateCustomBicubic(angle, chunkSize * thread,  dataSize);
						}
					}
					procParent->CleanupRotation(editToComplete);
				}

				// Single thread
				else{
					// Perform an edit on the data through the processor
					procParent->SetupRotation(editToComplete, angle, cropFlag);
					procParent->RotateCustomBicubic(angle);
					procParent->CleanupRotation(editToComplete);
				}
				procParent->SetUpdated(true);
			}
			break;
	
			// Peform a sclaing of the image using nearest neighbor
			case ProcessorEdit::EditType::SCALE_NEAREST: {

				procParent->SendMessageToParent("Processing Scale Edit" + fullEditNumStr);

				// Perform an edit on the data through the processor
				int width = (int) curEdit->GetParam(0);
				int height = (int) curEdit->GetParam(1);
				
				if(width < 0 || height < 0.0){ break; }

				// Multithread if needed
				if(procParent->GetMultithread()){
				
					procParent->SetupScale(width, height);

					int numThreads = procParent->GetNumThreads();
					int dataSize = procParent->GetTempImage()->GetWidth() * procParent->GetTempImage()->GetHeight();
					int chunkSize = dataSize / numThreads;

					omp_set_dynamic(0);
					omp_set_num_threads(numThreads);

					#pragma omp parallel for 
					for(int thread = 0; thread < numThreads; thread++){

						// Process current chunk of data
						if(thread != numThreads - 1){
							procParent->ScaleNearest(chunkSize * thread, chunkSize * (thread + 1));
						}

						// Go all the way to end of data (incase of rounding error)
						else{
							procParent->ScaleNearest(chunkSize * thread, chunkSize * (thread + 1));
						}
					}
					procParent->CleanupScale();
				}

				// Single thread
				else{
					// Perform an edit on the data through the processor
					procParent->SetupScale(width, height);
					procParent->ScaleNearest();
					procParent->CleanupScale();
				}
				procParent->SetUpdated(true);
			}
			break;

			// Peform a sclaing of the image using bilinear interpolation
			case ProcessorEdit::EditType::SCALE_BILINEAR: {

				procParent->SendMessageToParent("Processing Scale Edit" + fullEditNumStr);

				// Perform an edit on the data through the processor
				int width = (int) curEdit->GetParam(0);
				int height = (int) curEdit->GetParam(1);
				
				if(width < 0 || height < 0.0){ break; }

				// Multithread if needed
				if(procParent->GetMultithread()){
				
					procParent->SetupScale(width, height);

					int numThreads = procParent->GetNumThreads();
					int dataSize = procParent->GetTempImage()->GetWidth() * procParent->GetTempImage()->GetHeight();
					int chunkSize = dataSize / numThreads;

					omp_set_dynamic(0);
					omp_set_num_threads(numThreads);

					#pragma omp parallel for 
					for(int thread = 0; thread < numThreads; thread++){

						// Process current chunk of data
						if(thread != numThreads - 1){
							procParent->ScaleBilinear(chunkSize * thread, chunkSize * (thread + 1));
						}

						// Go all the way to end of data (incase of rounding error)
						else{
							procParent->ScaleBilinear(chunkSize * thread, chunkSize * (thread + 1));
						}
					}
					procParent->CleanupScale();
				}

				// Single thread
				else{
					// Perform an edit on the data through the processor
					procParent->SetupScale(width, height);
					procParent->ScaleBilinear();
					procParent->CleanupScale();
				}
				procParent->SetUpdated(true);
			}
			break;

			// Peform a sclaing of the image using bicubic interpolation
			case ProcessorEdit::EditType::SCALE_BICUBIC: {

				procParent->SendMessageToParent("Processing Scale Edit" + fullEditNumStr);

				// Perform an edit on the data through the processor
				int width = (int) curEdit->GetParam(0);
				int height = (int) curEdit->GetParam(1);
				
				if(width < 0 || height < 0.0){ break; }

				// Multithread if needed
				if(procParent->GetMultithread()){
				
					procParent->SetupScale(width, height);

					int numThreads = procParent->GetNumThreads();
					int dataSize = procParent->GetTempImage()->GetWidth() * procParent->GetTempImage()->GetHeight();
					int chunkSize = dataSize / numThreads;

					omp_set_dynamic(0);
					omp_set_num_threads(numThreads);

					#pragma omp parallel for 
					for(int thread = 0; thread < numThreads; thread++){

						// Process current chunk of data
						if(thread != numThreads - 1){
							procParent->ScaleBicubic(chunkSize * thread, chunkSize * (thread + 1));
						}

						// Go all the way to end of data (incase of rounding error)
						else{
							procParent->ScaleBicubic(chunkSize * thread, chunkSize * (thread + 1));
						}
					}
					procParent->CleanupScale();
				}

				// Single thread
				else{
					// Perform an edit on the data through the processor
					procParent->SetupScale(width, height);
					procParent->ScaleBicubic();
					procParent->CleanupScale();
				}
				procParent->SetUpdated(true);
			}
			break;

			// Peform a horizontal image flip
			case ProcessorEdit::EditType::MIRROR_HORIZONTAL: {

				procParent->SendMessageToParent("Processing Mirror Edit" + fullEditNumStr);

				// Multithread if needed
				if(procParent->GetMultithread()){
				
					int numThreads = procParent->GetNumThreads();
					int dataSize = procParent->GetImage()->GetWidth() * procParent->GetImage()->GetHeight();
					int chunkSize = dataSize / numThreads;

					omp_set_dynamic(0);     // Explicitly disable dynamic teams
					omp_set_num_threads(numThreads); // Use 4 threads for all consecutive parallel regions

					#pragma omp parallel for 
					for(int thread = 0; thread < numThreads; thread++){

						// Process current chunk of data
						if(thread != numThreads - 1){
							procParent->MirrorHorizontal(chunkSize * thread, chunkSize * (thread + 1));
						}

						// Go all the way to end of data (incase of rounding error)
						else{
							procParent->MirrorHorizontal(chunkSize * thread, dataSize);
						}
					}
				}

				// Single thread
				else{
					// Perform an edit on the data through the processor
					procParent->MirrorHorizontal();
				}
				procParent->SetUpdated(true);
			}
			break;

			 // Peform a vertical image flip
			case ProcessorEdit::EditType::MIRROR_VERTICAL: {

				procParent->SendMessageToParent("Processing Mirror Edit" + fullEditNumStr);

				// Multithread if needed
				if(procParent->GetMultithread()){
				
					int numThreads = procParent->GetNumThreads();
					int dataSize = procParent->GetImage()->GetWidth() * procParent->GetImage()->GetHeight();
					int chunkSize = dataSize / numThreads;

					omp_set_dynamic(0);     // Explicitly disable dynamic teams
					omp_set_num_threads(numThreads); // Use 4 threads for all consecutive parallel regions

					#pragma omp parallel for 
					for(int thread = 0; thread < numThreads; thread++){

						// Process current chunk of data
						if(thread != numThreads - 1){
							procParent->MirrorVertical(chunkSize * thread, chunkSize * (thread + 1));
						}

						// Go all the way to end of data (incase of rounding error)
						else{
							procParent->MirrorVertical(chunkSize * thread, dataSize);
						}
					}
				}

				// Single thread
				else{
					// Perform an edit on the data through the processor
					procParent->MirrorVertical();
				}
				procParent->SetUpdated(true);
			}
			break;

			// Peform RGB Curves edit
			case ProcessorEdit::EditType::RGB_CURVES: {

				if (curEdit->GetNumIntArrays() == 8) {
					procParent->SendMessageToParent("Processing RGB Curves Edit" + fullEditNumStr);

					// Get 8 bit curve data
					int * brightCurve8 = curEdit->GetIntArray(0);
					int * redCurve8 = curEdit->GetIntArray(1);
					int * greenCurve8 = curEdit->GetIntArray(2);
					int * blueCurve8 = curEdit->GetIntArray(3);

					// Get 16 bit curve data
					int * brightCurve16 = curEdit->GetIntArray(4);
					int * redCurve16 = curEdit->GetIntArray(5);
					int * greenCurve16 = curEdit->GetIntArray(6);
					int * blueCurve16 = curEdit->GetIntArray(7);

					// Multithread if needed
					if(procParent->GetMultithread()){
				
						int numThreads = procParent->GetNumThreads();
						int dataSize = procParent->GetImage()->GetWidth() * procParent->GetImage()->GetHeight();
						int chunkSize = dataSize / numThreads;

						omp_set_dynamic(0);     // Explicitly disable dynamic teams
						omp_set_num_threads(numThreads); // Use 4 threads for all consecutive parallel regions

						#pragma omp parallel for 
						for(int thread = 0; thread < numThreads; thread++){

							// Process current chunk of data
							if(thread != numThreads - 1){
								procParent->RGBCurves(brightCurve8, redCurve8, greenCurve8, blueCurve8,
									brightCurve16, redCurve16, greenCurve16, blueCurve16, chunkSize * thread, chunkSize * (thread + 1));
							}

							// Go all the way to end of data (incase of rounding error)
							else{
								procParent->RGBCurves(brightCurve8, redCurve8, greenCurve8, blueCurve8,
									brightCurve16, redCurve16, greenCurve16, blueCurve16, chunkSize * thread, dataSize);
							}
						}
					}

					// Single thread
					else{
						// Perform an edit on the data through the processor
						procParent->RGBCurves(brightCurve8, redCurve8, greenCurve8, blueCurve8,
						brightCurve16, redCurve16, greenCurve16, blueCurve16);
					}
					procParent->SetUpdated(true);
				}
			}
			break;

			// Peform LAB Curves edit
			case ProcessorEdit::EditType::LAB_CURVES: {
			
				if (curEdit->GetNumIntArrays() == 3) {
					procParent->SendMessageToParent("Processing LAB Curves Edit" + fullEditNumStr);

					// Get LAB curve data
					int * lCurve16 = curEdit->GetIntArray(0);
					int * aCurve16 = curEdit->GetIntArray(1);
					int * bCurve16 = curEdit->GetIntArray(2);

					int colorSpace = curEdit->GetParam(0);

					// Multithread if needed
					if(procParent->GetMultithread()){
				
						int numThreads = procParent->GetNumThreads();
						int dataSize = procParent->GetImage()->GetWidth() * procParent->GetImage()->GetHeight();
						int chunkSize = dataSize / numThreads;

						omp_set_dynamic(0);     // Explicitly disable dynamic teams
						omp_set_num_threads(numThreads); // Use 4 threads for all consecutive parallel regions

						#pragma omp parallel for 
						for(int thread = 0; thread < numThreads; thread++){

							if(procParent->forceStop){
								continue;
							}
							// Process current chunk of data
							if(thread != numThreads - 1){
								procParent->LABCurves(lCurve16, aCurve16, bCurve16, colorSpace, chunkSize * thread, chunkSize * (thread + 1));
							}

							// Go all the way to end of data (incase of rounding error)
							else{
								procParent->LABCurves(lCurve16, aCurve16, bCurve16, colorSpace, chunkSize * thread, dataSize);
							}
						}
					}

					// Single thread
					else{
						// Perform an edit on the data through the processor
						procParent->LABCurves(lCurve16, aCurve16, bCurve16, colorSpace);
					}
					procParent->SetUpdated(true);
			
				}
			}
			break;

			// Peform HSL Curves edit
			case ProcessorEdit::EditType::HSL_CURVES: {
			
				if (curEdit->GetNumIntArrays() == 3) {
					procParent->SendMessageToParent("Processing HSL Curves Edit" + fullEditNumStr);

					// Get LAB curve data
					int * hCurve16 = curEdit->GetIntArray(0);
					int * sCurve16 = curEdit->GetIntArray(1);
					int * lCurve16 = curEdit->GetIntArray(2);

					// Multithread if needed
					if(procParent->GetMultithread()){
				
						int numThreads = procParent->GetNumThreads();
						int dataSize = procParent->GetImage()->GetWidth() * procParent->GetImage()->GetHeight();
						int chunkSize = dataSize / numThreads;

						omp_set_dynamic(0);     // Explicitly disable dynamic teams
						omp_set_num_threads(numThreads); // Use 4 threads for all consecutive parallel regions

						#pragma omp parallel for 
						for(int thread = 0; thread < numThreads; thread++){

							// Process current chunk of data
							if(thread != numThreads - 1){
								procParent->HSLCurves(hCurve16, sCurve16, lCurve16, chunkSize * thread, chunkSize * (thread + 1));
							}

							// Go all the way to end of data (incase of rounding error)
							else{
								procParent->HSLCurves(hCurve16, sCurve16, lCurve16, chunkSize * thread, chunkSize * (thread + 1));
							}
						}
					}

					// Single thread
					else{
						// Perform an edit on the data through the processor
						procParent->HSLCurves(hCurve16, sCurve16, lCurve16);
					}
					procParent->SetUpdated(true);
			
				}
			}
			break;

			// Peform Crop edit
			case ProcessorEdit::EditType::CROP: {
			
				procParent->SendMessageToParent("Processing CROP" + fullEditNumStr);

				// Get crop dimmensions
				int startX = curEdit->GetParam(0);
				int startY = curEdit->GetParam(1);
				int newWidth = curEdit->GetParam(2);
				int newHeight = curEdit->GetParam(3);
	
				// Multithread if needed
				if(procParent->GetMultithread()){
						
					procParent->SetupCrop(newWidth, newHeight);

					int numThreads = procParent->GetNumThreads();
					int dataSize = procParent->GetImage()->GetWidth() * procParent->GetImage()->GetHeight();
					int chunkSize = dataSize / numThreads;

					omp_set_dynamic(0);     // Explicitly disable dynamic teams
					omp_set_num_threads(numThreads); // Use 4 threads for all consecutive parallel regions

					#pragma omp parallel for 
					for(int thread = 0; thread < numThreads; thread++){

						// Process current chunk of data
						if(thread != numThreads - 1){
							procParent->Crop(startX, startY, chunkSize * thread, chunkSize * (thread + 1));
						}

						// Go all the way to end of data (incase of rounding error)
						else{
							procParent->Crop(startX, startY, chunkSize * thread, dataSize);
						}
					}
					procParent->CleanupCrop();
				}

				// Single thread
				else{
					// Perform an edit on the data through the processor
					procParent->SetupCrop(newWidth, newHeight);
					procParent->Crop(startX, startY);
					procParent->CleanupCrop();
				}
				procParent->SetUpdated(true);
			}
			break;
		}

		procParent->SendProcessorEditNumToParent(editIndex + 1);
	}

	procParent->SendMessageToParent("");
	procParent->Unlock();
	return (wxThread::ExitCode)0;
}


Processor::RawProcessThread::RawProcessThread(Processor * processorPar, bool unpackAndProcess) : wxThread(wxTHREAD_DETACHED) {
	processor = processorPar;
	unpackProcess = unpackAndProcess;
}

wxThread::ExitCode Processor::RawProcessThread::Entry() {

	if (unpackProcess) {
		while (processor->GetLockedRaw()) {
			this->Sleep(20);
		}
		processor->LockRaw();
		processor->SendMessageToParent("Unpacking Raw File");
		processor->rawPrcoessor.unpack();
		processor->UnlockRaw();
		processor->ProcessRaw();
		return 0;
	}

	if (processor->GetLockedRaw()) {
		return 0;
	}
	processor->LockRaw();
	// process the raw image
	processor->SendMessageToParent("RAW Image Processing");
	processor->rawErrorCode = processor->rawPrcoessor.dcraw_process();
	processor->SendMessageToParent("RAW Image Done Processing");

	// Exit method before creating raw image from bad data
	if(processor->rawErrorCode != LIBRAW_SUCCESS){

		wxString errorName;
		errorName << processor->rawErrorCode;

		processor->SendMessageToParent("RAW Image processing failed: " + errorName);
		processor->UnlockRaw();
		return 0;
	}

	// Create the raw image
	processor->SendMessageToParent("Creating RAW Image for display");
	processor->rawImage = processor->rawPrcoessor.dcraw_make_mem_image(&processor->rawErrorCode);

	// Set rawImageGood on processor if success
	if(processor->rawErrorCode == LIBRAW_SUCCESS){

		processor->SendMessageToParent("Updating Display");
		processor->rawImageGood = true;
		
		while(processor->GetLocked()){
			this->Sleep(20);
		}
		processor->Lock();
		ImageHandler::CopyImageFromRaw(processor->rawImage, processor->GetImage());
		if(processor->GetOriginalImage() != NULL){
			processor->GetOriginalImage()->Destroy();
		}
		processor->SetOriginalImage(processor->GetImage());

		// If there are no edits after raw procesing, set updated.  We don't want to show the raw image without
		// following edits, if those edits exist
		if(!processor->GetHasEdits()){
			processor->SetUpdated(true);
		}
		processor->Unlock();
		processor->DeleteRawImage();
		processor->SendMessageToParent("");
		
	}
	else{
		
	}
	processor->UnlockRaw();
	return (wxThread::ExitCode)0;
}