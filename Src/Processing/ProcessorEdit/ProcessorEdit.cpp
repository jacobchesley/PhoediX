// Copyright 2018 Jacob Chesley.
// See LICENSE.TXT in root of project for license information.

#include "ProcessorEdit.h"

ProcessorEdit::ProcessorEdit() {

	isDisabled = false;
	this->SetEditType(EditType::UNDEFINED);
}

ProcessorEdit::ProcessorEdit(int editType) {

	this->SetEditType(editType);
	isDisabled = false;
}

ProcessorEdit::ProcessorEdit(ProcessorEdit &edit) {

	isDisabled = false;

	if(edit.GetEditType() < 0 ){
		this->SetEditType(ProcessorEdit::UNDEFINED);
		return;
	}
	this->SetEditType(edit.GetEditType());
	this->SetDisabled(edit.GetDisabled());

	// Copy Params
	for (std::map<wxString, double>::const_iterator it = edit.paramMap.begin(); it != edit.paramMap.end(); it++) {
		paramMap[it->first] = it->second;
	}

	// Copy Flags
	for (std::map<wxString, int>::const_iterator it = edit.flagMap.begin(); it != edit.flagMap.end(); it++) {
		flagMap[it->first] = it->second;
	}

	// Copy Int Arrays
	for (std::map<wxString, int*>::const_iterator it = edit.intArrayMap.begin(); it != edit.intArrayMap.end(); it++) {
		
		// Allocate new int array
		int intArraySize = edit.intArraySizeMap[it->first];
		int * newIntArray = new int[edit.GetIntArraySize(it->first)];
		int * oldIntArray = edit.GetIntArray(it->first);

		// Copy int array
		for (size_t i = 0; i < intArraySize; i++) {
			newIntArray[i] = oldIntArray[i];
		}

		// Add new int array
		this->AddIntArray(it->first, newIntArray, intArraySize);
	}



	// Copy Double Arrays
	for (std::map<wxString, double*>::const_iterator it = edit.doubleArrayMap.begin(); it != edit.doubleArrayMap.end(); it++) {

		// Allocate new int array
		int doubleArraySize = edit.doubleArraySizeMap[it->first];
		double * newDoubleArray = new double[edit.GetDoubleArraySize(it->first)];
		double * oldDoubleArray = edit.GetDoubleArray(it->first);

		// Copy int array
		for (size_t i = 0; i < doubleArraySize; i++) {
			newDoubleArray[i] = oldDoubleArray[i];
		}

		// Add new int array
		this->AddDoubleArray(it->first, newDoubleArray, doubleArraySize);
	}
}

ProcessorEdit::~ProcessorEdit() {
	
	this->ClearIntArray();
	this->ClearDoubleArray();
	this->ClearParams();
	this->ClearFlags();
}

void ProcessorEdit::AddParam(wxString parameterName, double parameterValue) {
	paramMap[parameterName] = parameterValue;
}

void ProcessorEdit::AddFlag(wxString flagName, int flag) {
	flagMap[flagName] = flag;
}

void ProcessorEdit::AddIntArray(wxString arrayName, int * newArray, int arraySize) {

	intArrayMap[arrayName] = newArray;
	intArraySizeMap[arrayName] = arraySize;
}

void ProcessorEdit::AddDoubleArray(wxString arrayName, double * newArray, int arraySize) {

	doubleArrayMap[arrayName] = newArray;
	doubleArraySizeMap[arrayName] = arraySize;
}

void ProcessorEdit::ClearParams() {
	paramMap.clear();
}

void ProcessorEdit::ClearFlags() {
	flagMap.clear();
}

void ProcessorEdit::ClearIntArray() {

	for (std::map<wxString, int*>::const_iterator it = intArrayMap.begin(); it != intArrayMap.end(); it++) {
		delete[] it->second;
	}
	
	intArrayMap.clear();
	intArraySizeMap.clear();
}

void ProcessorEdit::ClearDoubleArray() {

	for (std::map<wxString, double*>::const_iterator it = doubleArrayMap.begin(); it != doubleArrayMap.end(); it++) {
		delete[] it->second;
	}
	
	doubleArrayMap.clear();
	doubleArraySizeMap.clear();
}

size_t ProcessorEdit::GetParamsSize() {
	return paramMap.size();
}

size_t ProcessorEdit::GetNumDoubleArrays() {
	return doubleArrayMap.size();
}

double ProcessorEdit::GetParam(wxString parameterName) {

	if(paramMap.count(parameterName) > 0){ return paramMap[parameterName]; }
	else { return 0.0; }
}

int ProcessorEdit::GetFlag(wxString flagName) {

	if (flagMap.count(flagName) > 0) { return flagMap[flagName]; }
	else { return 0; }
}

int * ProcessorEdit::GetIntArray(wxString arrayName) {
	
	if (intArrayMap.count(arrayName) > 0) { return intArrayMap[arrayName]; }
	else { return NULL; }
}

double * ProcessorEdit::GetDoubleArray(wxString arrayName) {

	if (doubleArrayMap.count(arrayName) > 0) { return doubleArrayMap[arrayName]; }
	else { return NULL; }
}

size_t ProcessorEdit::GetIntArraySize(wxString arrayName) {

	if (intArraySizeMap.count(arrayName) > 0) { return intArraySizeMap[arrayName]; }
	else { return 0; }
}

size_t ProcessorEdit::GetDoubleArraySize(wxString arrayName) {

	if (doubleArraySizeMap.count(arrayName) > 0) { return doubleArraySizeMap[arrayName]; }
	else { return 0; }

}

bool ProcessorEdit::CheckForParameter(wxString parameterName){

	if (paramMap.count(parameterName) > 0) { return true; }
	else { return false; }
}

bool ProcessorEdit::CheckForFlag(wxString flagName) {

	if (flagMap.count(flagName) > 0) { return true; }
	else { return false; }
}


bool ProcessorEdit::CheckForIntArray(wxString arrayName) {

	if (intArrayMap.count(arrayName) > 0) { return true; }
	else { return false; }
}

bool ProcessorEdit::CheckForDoubleArray(wxString arrayName) {

	if (doubleArrayMap.count(arrayName) > 0) { return true; }
	else { return false; }
}

wxVector<wxString> ProcessorEdit::GetParamterNames() {

	wxVector<wxString> paramNames;

	for (std::map<wxString, double>::const_iterator it = paramMap.begin(); it != paramMap.end(); it++) {
		paramNames.push_back(it->first);
	}
	return paramNames;
}

wxVector<wxString> ProcessorEdit::GetFlagNames() {

	wxVector<wxString> flagNames;

	for (std::map<wxString, int>::const_iterator it = flagMap.begin(); it != flagMap.end(); it++) {
		flagNames.push_back(it->first);
	}
	return flagNames;
}

wxVector<wxString> ProcessorEdit::GetIntArrayNames() {

	wxVector<wxString> arrayNames;

	for (std::map<wxString, int*>::const_iterator it = intArrayMap.begin(); it != intArrayMap.end(); it++) {
		arrayNames.push_back(it->first);
	}
	return arrayNames;
}

wxVector<wxString> ProcessorEdit::GetDoubleArrayNames() {

	wxVector<wxString> arrayNames;

	for (std::map<wxString, double*>::const_iterator it = doubleArrayMap.begin(); it != doubleArrayMap.end(); it++) {
		arrayNames.push_back(it->first);
	}
	return arrayNames;
}

size_t ProcessorEdit::GetNumIntArrays() {
	return intArrayMap.size();
}

size_t ProcessorEdit::GetFlagsSize() {
	return flagMap.size();
}

int ProcessorEdit::GetEditType() {
	return editInt;
}

wxString ProcessorEdit::GetEditTag() {
	return tag;
}

void ProcessorEdit::SetDisabled(bool disable) {
	isDisabled = disable;
}

bool ProcessorEdit::GetDisabled() {
	return isDisabled;
}

bool ProcessorEdit::CompareProcessorEdits(ProcessorEdit * editOne, ProcessorEdit * editTwo) {

	// Compare edit types and disabled param
	if (editOne->GetEditType() != editTwo->GetEditType()) { return false; }
	if (editOne->GetDisabled() != editTwo->GetDisabled()) { return false; }

	// Compare params
	if (editOne->GetParamsSize() != editTwo->GetParamsSize()) { return false; }
	for (std::map<wxString, double>::const_iterator it = editOne->paramMap.begin(); it != editOne->paramMap.end(); it++) {
		if (editOne->GetParam(it->first) != editTwo->GetParam(it->first)) { return false; }
	}

	// Compare Flags
	if (editOne->GetFlagsSize() != editTwo->GetFlagsSize()) { return false; }
	for (std::map<wxString, int>::const_iterator it = editOne->flagMap.begin(); it != editOne->flagMap.end(); it++) {
		if (editOne->GetFlag(it->first) != editTwo->GetFlag(it->first)) { return false; }
	}

	// Compare Int Arrays
	if (editOne->GetNumIntArrays() != editTwo->GetNumIntArrays()) { return false; }

	for (std::map<wxString, int*>::const_iterator it = editOne->intArrayMap.begin(); it != editOne->intArrayMap.end(); it++) {

		int * intArrayOne = editOne->GetIntArray(it->first);
		int * intArrayTwo = editTwo->GetIntArray(it->first);

		if (intArrayOne != NULL && intArrayTwo == NULL) { return false; }
		if (editOne->GetIntArraySize(it->first) != editTwo->GetIntArraySize(it->first)) { return false; }

		for (size_t i = 0; i < editOne->GetIntArraySize(it->first); i++) {

			if (editOne->GetIntArray(it->first)[i] != editTwo->GetIntArray(it->first)[i]) { return false; }
		}
	}

	// Compare Double Arrays
	if (editOne->GetNumDoubleArrays() != editTwo->GetNumDoubleArrays()) { return false; }

	for (std::map<wxString, double*>::const_iterator it = editOne->doubleArrayMap.begin(); it != editOne->doubleArrayMap.end(); it++) {

		double * intArrayOne = editOne->GetDoubleArray(it->first);
		double * intArrayTwo = editTwo->GetDoubleArray(it->first);

		if (intArrayOne != NULL && intArrayTwo == NULL) { return false; }
		if (editOne->GetDoubleArraySize(it->first) != editTwo->GetDoubleArraySize(it->first)) { return false; }

		for (size_t i = 0; i < editOne->GetDoubleArraySize(it->first); i++) {

			if (editOne->GetDoubleArray(it->first)[i] != editTwo->GetDoubleArray(it->first)[i]) { return false; }
		}
	}
	return true;
}

void ProcessorEdit::SetEditType(int editType) {
	editInt = editType;

	switch (editInt) {

		case EditType::ADJUST_CONTRAST:
			tag = "ADJUST_CONTRAST";
			break;

		case EditType::ADJUST_CONTRAST_CURVE:
			tag = "ADJUST_CONTRAST_CURVE";
			break;

		case EditType::ADJUST_BRIGHTNESS:
			tag = "ADJUST_BRIGHTNESS";
			break;

		case EditType::CHANNEL_MIXER:
			tag = "CHANNEL_MIXER";
			break;

		case EditType::CONVERT_GREYSCALE_AVG:
			tag = "CONVERT_GREYSCALE_AVG";
			break;

		case EditType::CONVERT_GREYSCALE_CUSTOM:
			tag = "CONVERT_GREYSCALE_CUSTOM";
			break;

		case EditType::CONVERT_GREYSCALE_EYE:
			tag = "CONVERT_GREYSCALE_EYE";
			break;

		case EditType::HSL_CURVES:
			tag = "HSL_CURVES";
			break;

		case EditType::LAB_CURVES:
			tag = "LAB_CURVES";
			break;

		case EditType::MIRROR_NONE:
			tag = "MIRROR_NONE";
			break;

		case EditType::MIRROR_HORIZONTAL:
			tag = "MIRROR_HORIZONTAL";
			break;

		case EditType::MIRROR_VERTICAL:
			tag = "MIRROR_VERTICAL";
			break;

		case EditType::RGB_CURVES:
			tag = "RGB_CURVES";
			break;
	
		case EditType::ROTATE_NONE:
			tag = "ROTATE_NONE";
			break;

		case EditType::ROTATE_180:
			tag = "ROTATE_180";
			break;

		case EditType::ROTATE_270_CW:
			tag = "ROTATE_270_CW";
			break;

		case EditType::ROTATE_90_CW:
			tag = "ROTATE_90_CW";
			break;

		case EditType::ROTATE_CUSTOM_BICUBIC:
			tag = "ROTATE_CUSTOM_BICUBIC";
			break;

		case EditType::ROTATE_CUSTOM_BILINEAR:
			tag = "ROTATE_CUSTOM_BILINEAR";
			break;

		case EditType::ROTATE_CUSTOM_NEAREST:
			tag = "ROTATE_CUSTOM_NEAREST";
			break;

		case EditType::ADJUST_HSL:
			tag = "ADJUST_HSL";
			break;

		case EditType::ADJUST_LAB:
			tag = "ADJUST_LAB";
			break;

		case EditType::ADJUST_RGB:
			tag = "ADJUST_RGB";
			break;

		case EditType::SCALE_NEAREST:
			tag = "SCALE_NEAREST";
			break;

		case EditType::SCALE_BILINEAR:
			tag = "SCALE_BILINEAR";
			break;

		case EditType::SCALE_BICUBIC:
			tag = "SCALE_BICUBIC";
			break;

		case EditType::CROP:
			tag = "CROP";
			break;

		case EditType::BLUR:
			tag = "BLUR";
			break;

		case EditType::HORIZONTAL_BLUR:
			tag = "HORIZONTAL_BLUR";
			break;

		case EditType::VERTICAL_BLUR:
			tag = "VERTICAL_BLUR";
			break;

		case EditType::RAW:
			tag = "RAW";
			break;

		case EditType::UNDEFINED:
			tag = "UNDEFINED";
			break;

		default:
			editInt = ProcessorEdit::UNDEFINED;
			tag = "UNDEFINED";
			break;
	}
}

void ProcessorEdit::SetEditTypeFromTag(wxString inTag) {

	     if (inTag == "ADJUST_CONTRAST") { editInt = EditType::ADJUST_CONTRAST; tag = inTag; }
	else if (inTag == "ADJUST_CONTRAST_CURVE") { editInt = EditType::ADJUST_CONTRAST_CURVE; tag = inTag; }
	else if (inTag == "CHANNEL_MIXER") { editInt = EditType::CHANNEL_MIXER; tag = inTag; }
	else if (inTag == "CONVERT_GREYSCALE_AVG") { editInt = EditType::CONVERT_GREYSCALE_AVG; tag = inTag; }
	else if (inTag == "CONVERT_GREYSCALE_CUSTOM") { editInt = EditType::CONVERT_GREYSCALE_CUSTOM; tag = inTag; }
	else if (inTag == "CONVERT_GREYSCALE_EYE") { editInt = EditType::CONVERT_GREYSCALE_EYE; tag = inTag; }
	else if (inTag == "HSL_CURVES") { editInt = EditType::HSL_CURVES; tag = inTag; }
	else if (inTag == "LAB_CURVES") { editInt = EditType::LAB_CURVES; tag = inTag; }
	else if (inTag == "MIRROR_NONE") { editInt = EditType::MIRROR_NONE; tag = inTag; }
	else if (inTag == "MIRROR_HORIZONTAL") { editInt = EditType::MIRROR_HORIZONTAL; tag = inTag; }
	else if (inTag == "MIRROR_VERTICAL") { editInt = EditType::MIRROR_VERTICAL; tag = inTag; }
	else if (inTag == "RGB_CURVES") { editInt = EditType::RGB_CURVES; tag = inTag; }
	else if (inTag == "ROTATE_NONE") { editInt = EditType::ROTATE_NONE; tag = inTag; }
	else if (inTag == "ROTATE_180") { editInt = EditType::ROTATE_180; tag = inTag; }
	else if (inTag == "ROTATE_270_CW") { editInt = EditType::ROTATE_270_CW; tag = inTag; }
	else if (inTag == "ROTATE_90_CW") { editInt = EditType::ROTATE_90_CW; tag = inTag; }
	else if (inTag == "ROTATE_CUSTOM_BICUBIC") { editInt = EditType::ROTATE_CUSTOM_BICUBIC; tag = inTag; }
	else if (inTag == "ROTATE_CUSTOM_BILINEAR") { editInt = EditType::ROTATE_CUSTOM_BILINEAR; tag = inTag; }
	else if (inTag == "ROTATE_CUSTOM_NEAREST") { editInt = EditType::ROTATE_CUSTOM_NEAREST; tag = inTag; }
	else if (inTag == "ADJUST_HSL") { editInt = EditType::ADJUST_HSL; tag = inTag; }
	else if (inTag == "ADJUST_LAB") { editInt = EditType::ADJUST_LAB; tag = inTag; }
	else if (inTag == "ADJUST_RGB") { editInt = EditType::ADJUST_RGB; tag = inTag; }
	else if (inTag == "ADJUST_BRIGHTNESS") { editInt = EditType::ADJUST_BRIGHTNESS; tag = inTag; }
	else if (inTag == "SCALE_NEAREST") { editInt = EditType::SCALE_NEAREST; tag = inTag; }
	else if (inTag == "SCALE_BILINEAR") { editInt = EditType::SCALE_BILINEAR; tag = inTag; }
	else if (inTag == "SCALE_BICUBIC") { editInt = EditType::SCALE_BICUBIC; tag = inTag; }
	else if (inTag == "CROP") { editInt = EditType::CROP; tag = inTag; }
	else if (inTag == "BLUR") { editInt = EditType::BLUR; tag = inTag; }
	else if (inTag == "HORIZONTAL_BLUR") { editInt = EditType::HORIZONTAL_BLUR; tag = inTag; }
	else if (inTag == "VERTICAL_BLUR") { editInt = EditType::VERTICAL_BLUR; tag = inTag; }
	else if (inTag == "RAW") { editInt = EditType::RAW; tag = inTag; }
	else{ editInt = EditType::UNDEFINED; tag = "UNDEFINED"; }
}

const wxString ProcessorEdit::ParametersFlags::PHOEDIX_PARAMETER_RED_SCALE = "RedScale";
const wxString ProcessorEdit::ParametersFlags::PHOEDIX_PARAMETER_GREEN_SCALE = "GreenScale";
const wxString ProcessorEdit::ParametersFlags::PHOEDIX_PARAMETER_BLUE_SCALE = "BlueScale";
const wxString ProcessorEdit::ParametersFlags::PHOEDIX_PARAMETER_LUMINACE = "Luminace";
const wxString ProcessorEdit::ParametersFlags::PHOEDIX_FLAG_INTERPOLATION = "Interpolation";
const wxString ProcessorEdit::ParametersFlags::PHOEDIX_PARAMETER_R_CURVE = "RCurve";
const wxString ProcessorEdit::ParametersFlags::PHOEDIX_PARAMETER_G_CURVE = "GCurve";
const wxString ProcessorEdit::ParametersFlags::PHOEDIX_PARAMETER_B_CURVE = "BCurve";
const wxString ProcessorEdit::ParametersFlags::PHOEDIX_PARAMETER_L_CURVE = "LCurve";
const wxString ProcessorEdit::ParametersFlags::PHOEDIX_PARAMETER_A_CURVE = "ACurve";
const wxString ProcessorEdit::ParametersFlags::PHOEDIX_PARAMETER_H_CURVE = "HCurve";
const wxString ProcessorEdit::ParametersFlags::PHOEDIX_PARAMETER_S_CURVE = "SCurve";

// Adjust Brightness Parameter/Flag/Array names
const wxString ProcessorEdit::ParametersFlags::PHOEDIX_PARAMETER_BRIGHTNESS = "Brightness";
const wxString ProcessorEdit::ParametersFlags::PHOEDIX_PARAMETER_PRESERVATION = "Preservation";
const wxString ProcessorEdit::ParametersFlags::PHOEDIX_PARAMETER_TONE = "Tone";
const wxString ProcessorEdit::ParametersFlags::PHOEDIX_FLAG_PRESERVATION_TYPE = "PreservationType";

// Adjust HSL Parameter/Flag/Array names
const wxString ProcessorEdit::ParametersFlags::PHOEDIX_PARAMETER_HUE = "Hue";
const wxString ProcessorEdit::ParametersFlags::PHOEDIX_PARAMETER_SATURATION = "Saturation";

// Adjust LAB Parameter/Flag/Array names
const wxString ProcessorEdit::ParametersFlags::PHOEDIX_PARAMETER_A = "A";
const wxString ProcessorEdit::ParametersFlags::PHOEDIX_PARAMETER_B = "B";

// Adjust RGB Parameter/Flag/Array names
const wxString ProcessorEdit::ParametersFlags::PHOEDIX_PARAMETER_ALL = "All";
const wxString ProcessorEdit::ParametersFlags::PHOEDIX_PARAMETER_RED = "Red";
const wxString ProcessorEdit::ParametersFlags::PHOEDIX_PARAMETER_GREEN = "Green";
const wxString ProcessorEdit::ParametersFlags::PHOEDIX_PARAMETER_BLUE = "Blue";

// Blur Parameter/Flag/Array names
const wxString ProcessorEdit::ParametersFlags::PHOEDIX_PARAMETER_BLURSIZE = "BlurSize";
const wxString ProcessorEdit::ParametersFlags::PHOEDIX_PARAMETER_NUM_PASSES = "NumberPasses";

// Channel Mixer Parameter/Flag/Array names
const wxString ProcessorEdit::ParametersFlags::PHOEDIX_FLAG_PRESET = "Preset";
const wxString ProcessorEdit::ParametersFlags::PHOEDIX_PARAMETER_RED_RED = "RedRed";
const wxString ProcessorEdit::ParametersFlags::PHOEDIX_PARAMETER_RED_GREEN = "RedGreen";
const wxString ProcessorEdit::ParametersFlags::PHOEDIX_PARAMETER_RED_BLUE = "RedBlue";
const wxString ProcessorEdit::ParametersFlags::PHOEDIX_PARAMETER_GREEN_RED = "GreenRed";
const wxString ProcessorEdit::ParametersFlags::PHOEDIX_PARAMETER_GREEN_GREEN = "GreenGreen";
const wxString ProcessorEdit::ParametersFlags::PHOEDIX_PARAMETER_GREEN_BLUE = "GreenBlue";
const wxString ProcessorEdit::ParametersFlags::PHOEDIX_PARAMETER_BLUE_RED = "BlueRed";
const wxString ProcessorEdit::ParametersFlags::PHOEDIX_PARAMETER_BLUE_GREEN = "BlueGreen";
const wxString ProcessorEdit::ParametersFlags::PHOEDIX_PARAMETER_BLUE_BLUE = "BlueBlue";

// Contrast Parameter/Flag/Array names
const wxString ProcessorEdit::ParametersFlags::PHOEDIX_PARAMETER_ALL_CONTRAST = "AllContrast";
const wxString ProcessorEdit::ParametersFlags::PHOEDIX_PARAMETER_RED_CONTRAST = "RedContrast";
const wxString ProcessorEdit::ParametersFlags::PHOEDIX_PARAMETER_GREEN_CONTRAST = "greenContrast";
const wxString ProcessorEdit::ParametersFlags::PHOEDIX_PARAMETER_BLUE_CONTRAST = "BlueContrast";
const wxString ProcessorEdit::ParametersFlags::PHOEDIX_PARAMETER_ALL_CONTRAST_CENTER = "AllContrastCenter";
const wxString ProcessorEdit::ParametersFlags::PHOEDIX_PARAMETER_RED_CONTRAST_CENTER = "RedContrastCenter";
const wxString ProcessorEdit::ParametersFlags::PHOEDIX_PARAMETER_GREEN_CONTRAST_CENTER = "greenContrastCenter";
const wxString ProcessorEdit::ParametersFlags::PHOEDIX_PARAMETER_BLUE_CONTRAST_CENTER = "BlueContrastCenter";

// Crop Parameter/Flag/Array names
const wxString ProcessorEdit::ParametersFlags::PHOEDIX_PARAMETER_STARTX = "StartX";
const wxString ProcessorEdit::ParametersFlags::PHOEDIX_PARAMETER_STARTY = "StartY";
const wxString ProcessorEdit::ParametersFlags::PHOEDIX_PARAMETER_CROP_WIDTH = "CropWidth";
const wxString ProcessorEdit::ParametersFlags::PHOEDIX_PARAMETER_CROP_HEIGHT = "CropHeight";
const wxString ProcessorEdit::ParametersFlags::PHOEDIX_PARAMETER_NUMERATOR = "Numerator";
const wxString ProcessorEdit::ParametersFlags::PHOEDIX_PARAMETER_DENOMINATOR = "Denominator";
const wxString ProcessorEdit::ParametersFlags::PHOEDIX_FLAG_ASPECT_SELECTION = "AspectSelection";
const wxString ProcessorEdit::ParametersFlags::PHOEDIX_FLAG_CROP_ENABLED = "CropEnabled";
const wxString ProcessorEdit::ParametersFlags::PHOEDIX_FLAG_FLIP_ASPECT = "FlipAspect";
const wxString ProcessorEdit::ParametersFlags::PHOEDIX_FLAG_BOX_ENABLED = "BoxEnabled";

// Greyscale Parameter/Flag/Array names
const wxString ProcessorEdit::ParametersFlags::PHOEDIX_FLAG_GREYTYPE = "GreyType";

// Mirror Parameter/Flag/Array names
const wxString ProcessorEdit::ParametersFlags::PHOEDIX_PARAMETER_MIRROR_SELECT = "MirrorSelection";

// RGB Curve Parameter/Flag/Array names
const wxString ProcessorEdit::ParametersFlags::PHOEDIX_PARAMETER_BRIGHT_CURVE = "BrightCurve";

// RGB 16 Curve Parameter/Flag/Array names
const wxString ProcessorEdit::ParametersFlags::PHOEDIX_PARAMETER_BRIGHT_CURVE_16 = "BrightCurve16";
const wxString ProcessorEdit::ParametersFlags::PHOEDIX_PARAMETER_R_CURVE_16 = "RCurve16";
const wxString ProcessorEdit::ParametersFlags::PHOEDIX_PARAMETER_G_CURVE_16 = "GCurve16";
const wxString ProcessorEdit::ParametersFlags::PHOEDIX_PARAMETER_B_CURVE_16 = "BCurve16";

// Rotation Parameter/Flag/Array names
const wxString ProcessorEdit::ParametersFlags::PHOEDIX_PARAMETER_ROTATE_ANGLE = "RotationAngle";
const wxString ProcessorEdit::ParametersFlags::PHOEDIX_FLAG_ROTATE_TYPE = "RotationType";
const wxString ProcessorEdit::ParametersFlags::PHOEDIX_FLAG_ROTATE_CROP = "RotationCrop";

// Scale Parameter/Flag/Array names
const wxString ProcessorEdit::ParametersFlags::PHOEDIX_PARAMETER_SCALE_WIDTH = "ScaleWidth";
const wxString ProcessorEdit::ParametersFlags::PHOEDIX_PARAMETER_SCALE_HEIGHT = "ScaleHeight";
const wxString ProcessorEdit::ParametersFlags::PHOEDIX_FLAG_SCALE_TYPE = "ScaleType";
const wxString ProcessorEdit::ParametersFlags::PHOEDIX_FLAG_SCALE_KEEP_ASPECT = "ScaleKeepAspect";

// Raw Parameter/Flag/Array names
const wxString ProcessorEdit::ParametersFlags::PHOEDIX_PARAMETER_RAW_BRIGHTNESS = "Brightness";
const wxString ProcessorEdit::ParametersFlags::PHOEDIX_PARAMETER_RAW_HIGHLIGHTS = "Highlights";
const wxString ProcessorEdit::ParametersFlags::PHOEDIX_PARAMETER_RAW_SATURATION = "Saturation";
const wxString ProcessorEdit::ParametersFlags::PHOEDIX_PARAMETER_RAW_AUTOBRIGHT = "AutoBright";
const wxString ProcessorEdit::ParametersFlags::PHOEDIX_PARAMETER_RAW_AUTOBRIGHT_THRESH = "AutoBrightThreshold";
const wxString ProcessorEdit::ParametersFlags::PHOEDIX_PARAMETER_RAW_EXPOSURE = "Exposure";
const wxString ProcessorEdit::ParametersFlags::PHOEDIX_PARAMETER_RAW_EXPOSURE_PRESERVE = "ExposurePreserve";
const wxString ProcessorEdit::ParametersFlags::PHOEDIX_PARAMETER_RAW_MAXBRIGHT_THRESH = "MaxThreshold";
const wxString ProcessorEdit::ParametersFlags::PHOEDIX_PARAMETER_RAW_GAMMA_LEVEL = "GammaLevel";
const wxString ProcessorEdit::ParametersFlags::PHOEDIX_PARAMETER_RAW_GAMMA_SLOPE = "GammaSlope";
const wxString ProcessorEdit::ParametersFlags::PHOEDIX_PARAMETER_RAW_INTERPOLATION = "Interpolation";
const wxString ProcessorEdit::ParametersFlags::PHOEDIX_PARAMETER_RAW_GREEN_MATCH = "GreenMatch";
const wxString ProcessorEdit::ParametersFlags::PHOEDIX_PARAMETER_RAW_WB_PRESET = "WhiteBalancePreset";
const wxString ProcessorEdit::ParametersFlags::PHOEDIX_PARAMETER_RAW_WB_RED = "WhiteBalanceRed";
const wxString ProcessorEdit::ParametersFlags::PHOEDIX_PARAMETER_RAW_WB_GREEN = "WhiteBalanceGreen";
const wxString ProcessorEdit::ParametersFlags::PHOEDIX_PARAMETER_RAW_WB_BLUE = "WhiteBalanceBlue";
const wxString ProcessorEdit::ParametersFlags::PHOEDIX_PARAMETER_RAW_WAVELET_NOISE_RD = "WaveletNoiseRD";
const wxString ProcessorEdit::ParametersFlags::PHOEDIX_PARAMETER_FLIP = "Flip";
const wxString ProcessorEdit::ParametersFlags::PHOEDIX_FLAG_RAW_TAB_OPEN_SETTINGS = "SettingsTabOpen";
const wxString ProcessorEdit::ParametersFlags::PHOEDIX_FLAG_RAW_TAB_OPEN_EXPOSURE = "ExposureTabOpen";
const wxString ProcessorEdit::ParametersFlags::PHOEDIX_FLAG_RAW_TAB_OPEN_COLOR = "ColorTabOpen";
const wxString ProcessorEdit::ParametersFlags::PHOEDIX_FLAG_RAW_TAB_OPEN_NOISE = "NoiseTabOpen";
const wxString ProcessorEdit::ParametersFlags::PHOEDIX_FLAG_RAW_TAB_OPEN_INFO = "InfoTabOpen";