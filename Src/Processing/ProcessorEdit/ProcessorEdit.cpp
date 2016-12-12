// Copyright 2016 Jacob Chesley

#include "ProcessorEdit.h"

ProcessorEdit::ProcessorEdit() {

	isDisabled = false;
	this->SetEditType(EditType::UNDEFINED);
	intArrays = wxVector<int*>();
	intArrays.clear();
	doubleArrays = wxVector<double*>();
	doubleArrays.clear();
	params = wxVector<double>();
	params.clear();
	flags = wxVector<int>();
	flags.clear();
}

ProcessorEdit::ProcessorEdit(int editType) {

	this->SetEditType(editType);
	isDisabled = false;
	intArrays = wxVector<int*>();
	intArrays.clear();
	doubleArrays = wxVector<double*>();
	doubleArrays.clear();
	params = wxVector<double>();
	params.clear();
	flags = wxVector<int>();
	flags.clear();
}


ProcessorEdit::ProcessorEdit(ProcessorEdit &edit) {

	isDisabled = false;
	intArrays = wxVector<int*>();
	intArrays.clear();
	doubleArrays = wxVector<double*>();
	doubleArrays.clear();
	params = wxVector<double>();
	params.clear();
	flags = wxVector<int>();
	flags.clear();

	if(edit.GetEditType() < 0 ){
		this->SetEditType(ProcessorEdit::UNDEFINED);
		return;
	}
	this->SetEditType(edit.GetEditType());

	isDisabled = edit.GetDisabled();

	intArrays = wxVector<int*>();
	intArrays.clear();
	doubleArrays = wxVector<double*>();
	doubleArrays.clear();
	params = wxVector<double>();
	params.clear();
	flags = wxVector<int>();
	flags.clear();

	// Copy Params
	for (size_t i = 0; i < edit.GetParamsSize(); i++) {
		this->AddParam(edit.GetParam(i));
	}

	// Copy Flags
	for (size_t i = 0; i < edit.GetFlagsSize(); i++) {
		this->AddFlag(edit.GetFlag(i));
	}

	// Copy Int Arrays
	for(size_t intArrayIdx = 0; intArrayIdx < edit.GetNumIntArrays(); intArrayIdx ++){


		// Allocate new int array
		size_t intArraySize = edit.GetIntArraySize(intArrayIdx);
		int * newIntArray = new int[edit.GetIntArraySize(intArrayIdx)];

		// Copy int array
		for(size_t i = 0; i < intArraySize; i++){
			newIntArray[i] = edit.GetIntArray(intArrayIdx)[i];
		}

		// Add new int array
		this->AddIntArray(newIntArray, intArraySize);
	}

	// Copy Double Arrays
	for(size_t doubleArrayIdx = 0; doubleArrayIdx < edit.GetNumDoubleArrays(); doubleArrayIdx ++){
		
		// Allocate new int array
		size_t doubleArraySize = edit.GetDoubleArraySize(doubleArrayIdx);
		double* newDoubleArray = new double[edit.GetDoubleArraySize(doubleArrayIdx)];

		// Copy int array
		for(size_t i = 0; i < doubleArraySize; i++){
			newDoubleArray[i] = edit.GetDoubleArray(doubleArrayIdx)[i];
		}

		// Add new int array
		this->AddDoubleArray(newDoubleArray, doubleArraySize);
	}

}

ProcessorEdit::~ProcessorEdit() {
	for (size_t i = 0; i < intArrays.size(); i++) {
		delete[] intArrays.at(i);
	}

	for (size_t i = 0; i < doubleArrays.size(); i++) {
		delete[] doubleArrays.at(i);
	}
}

void ProcessorEdit::AddParam(double param) {
	params.push_back(param);
}

void ProcessorEdit::AddIntArray(int * newArray, int arraySize) {
	intArrays.push_back(newArray);
	intArraySizes.push_back(arraySize);
}

void ProcessorEdit::AddDoubleArray(double * newArray, int arraySize) {
	doubleArrays.push_back(newArray);
	doubleArraySizes.push_back(arraySize);
}

void ProcessorEdit::ClearParams() {
	params.clear();
}

void ProcessorEdit::ClearIntArray() {
	for (size_t i = 0; i < intArrays.size(); i++) {
		delete[] intArrays[i];
	}
	
	intArrays.resize(0);
	intArraySizes.resize(0);
}

void ProcessorEdit::ClearDoubleArray() {
	for (size_t i = 0; i < doubleArrays.size(); i++) {
		delete[] doubleArrays[i];
	}
	
	doubleArrays.resize(0);
	doubleArraySizes.resize(0);
}

size_t ProcessorEdit::GetParamsSize() {
	return params.size();
}

size_t ProcessorEdit::GetNumDoubleArrays() {
	return doubleArrays.size();
}

double ProcessorEdit::GetParam(size_t index) {
	if(index >= params.size()){ return 0.0f; }
	return params.at(index);
}

int * ProcessorEdit::GetIntArray(size_t index) {
	if(index >= intArrays.size()){ return NULL; }
	return intArrays.at(index);
}

double * ProcessorEdit::GetDoubleArray(size_t index) {
	if(index >= doubleArrays.size()){ return NULL; }
	return doubleArrays.at(index);
}

size_t ProcessorEdit::GetIntArraySize(size_t index) {
	if (index >= intArraySizes.size()) { return 0; }
	return intArraySizes.at(index);
}

size_t ProcessorEdit::GetDoubleArraySize(size_t index) {
	if(index >= doubleArraySizes.size()) { return 0; }
	return doubleArraySizes.at(index);
}

size_t ProcessorEdit::GetNumIntArrays() {
	return intArraySizes.size();
}

void ProcessorEdit::AddFlag(int flag) {
	flags.push_back(flag);
}

void ProcessorEdit::ClearFlags() {
	flags.clear();
}

size_t ProcessorEdit::GetFlagsSize() {
	return flags.size();
}

int ProcessorEdit::GetFlag(size_t index) {
	if(index >= flags.size()){ return 0; }
	return flags.at(index);
}

int ProcessorEdit::GetEditType() {
	return editInt;
}

void ProcessorEdit::SetDisabled(bool disable) {
	isDisabled = disable;
}

bool ProcessorEdit::GetDisabled() {
	return isDisabled;
}

void ProcessorEdit::SetEditType(int editType) {
	editInt = editType;

	switch (editInt) {

		case EditType::ADJUST_CONTRAST:
			tag = "ADJUST_CONTRAST";
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

		case EditType::SHIFT_RGB:
			tag = "SHIFT_RGB";
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
	else if (inTag == "SHIFT_RGB") { editInt = EditType::SHIFT_RGB; tag = inTag; }
	else if (inTag == "ADJUST_BRIGHTNESS") { editInt = EditType::ADJUST_BRIGHTNESS; tag = inTag; }
	else if (inTag == "SCALE_NEAREST") { editInt = EditType::SCALE_NEAREST; tag = inTag; }
	else if (inTag == "SCALE_BILINEAR") { editInt = EditType::SCALE_BILINEAR; tag = inTag; }
	else if (inTag == "SCALE_BICUBIC") { editInt = EditType::SCALE_BICUBIC; tag = inTag; }
	else if (inTag == "RAW") { editInt = EditType::RAW; tag = inTag; }
	else{ editInt = EditType::UNDEFINED; tag = "UNDEFINED"; }
}

wxString ProcessorEdit::GetEditTag() {
	return tag;
}