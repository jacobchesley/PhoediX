#include "ProcessorEdit.h"

ProcessorEdit::ProcessorEdit() {
	doMultithread = false;
	numThread = 0;
}

ProcessorEdit::ProcessorEdit(int editType) {
	this->SetEditType(editType);
}

void ProcessorEdit::AddParam(double param) {
	params.push_back(param);
}

void ProcessorEdit::AddIntArray(int * newArray, int arraySize) {
	intArrays.push_back(newArray);
	intArraySizes.push_back(arraySize);
}

void ProcessorEdit::ClearParams() {
	params.clear();
}

void ProcessorEdit::ClearIntArray() {
	for (size_t i = 0; i < intArrays.size(); i++) {
		delete[] intArrays[i];
	}
	intArrays.clear();
	intArraySizes.clear();
}

int ProcessorEdit::GetParamsSize() {
	return params.size();
}

int ProcessorEdit::GetIntArraySize() {
	return intArrays.size();
}

double ProcessorEdit::GetParam(size_t index) {
	return params.at(index);
}

int * ProcessorEdit::GetIntArray(size_t index) {
	return intArrays.at(index);
}

int ProcessorEdit::GetIntArraySize(size_t index) {
	return intArraySizes.at(index);
}

int ProcessorEdit::GetNumIntArrays() {
	return intArraySizes.size();
}

void ProcessorEdit::AddFlag(int flag) {
	flags.push_back(flag);
}

void ProcessorEdit::ClearFlags() {
	flags.clear();
}

int ProcessorEdit::GetFlagsSize() {
	return flags.size();
}

double ProcessorEdit::GetFlag(size_t index) {
	return flags.at(index);
}

int ProcessorEdit::GetEditType() {
	return edit;
}

void ProcessorEdit::SetEditType(int editType) {
	edit = editType;

	switch (edit) {

		case EditType::ADJUST_CONTRAST:
			tag = "ADJUST_CONTRAST";
			break;

		case EditType::CHANNEL_TRANSFORM:
			tag = "CHANNEL_TRANSFORM";
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

		case EditType::SCALE_BRIGHTNESS:
			tag = "SCALE_BRIGHTNESS";
			break;

		case EditType::SHIFT_BRIGHTNESS:
			tag = "SHIFT_BRIGHTNESS";
			break;
	}
}

void ProcessorEdit::SetEditTypeFromTag(wxString inTag) {

	if (inTag == "ADJUST_CONTRAST") { edit = EditType::ADJUST_CONTRAST; tag = inTag; }
	else if (inTag == "CHANNEL_TRANSFORM") { edit = EditType::CHANNEL_TRANSFORM; tag = inTag; }
	else if (inTag == "CONVERT_GREYSCALE_AVG") { edit = EditType::CONVERT_GREYSCALE_AVG; tag = inTag; }
	else if (inTag == "CONVERT_GREYSCALE_CUSTOM") { edit = EditType::CONVERT_GREYSCALE_CUSTOM; tag = inTag; }
	else if (inTag == "CONVERT_GREYSCALE_EYE") { edit = EditType::CONVERT_GREYSCALE_EYE; tag = inTag; }
	else if (inTag == "LAB_CURVES") { edit = EditType::LAB_CURVES; tag = inTag; }
	else if (inTag == "MIRROR_NONE") { edit = EditType::MIRROR_NONE; tag = inTag; }
	else if (inTag == "MIRROR_HORIZONTAL") { edit = EditType::MIRROR_HORIZONTAL; tag = inTag; }
	else if (inTag == "MIRROR_VERTICAL") { edit = EditType::MIRROR_VERTICAL; tag = inTag; }
	else if (inTag == "RGB_CURVES") { edit = EditType::RGB_CURVES; tag = inTag; }
	else if (inTag == "ROTATE_NONE") { edit = EditType::ROTATE_NONE; tag = inTag; }
	else if (inTag == "ROTATE_180") { edit = EditType::ROTATE_180; tag = inTag; }
	else if (inTag == "ROTATE_270_CW") { edit = EditType::ROTATE_270_CW; tag = inTag; }
	else if (inTag == "ROTATE_90_CW") { edit = EditType::ROTATE_90_CW; tag = inTag; }
	else if (inTag == "ROTATE_CUSTOM_BICUBIC") { edit = EditType::ROTATE_CUSTOM_BICUBIC; tag = inTag; }
	else if (inTag == "ROTATE_CUSTOM_BILINEAR") { edit = EditType::ROTATE_CUSTOM_BILINEAR; tag = inTag; }
	else if (inTag == "ROTATE_CUSTOM_NEAREST") { edit = EditType::ROTATE_CUSTOM_NEAREST; tag = inTag; }
	else if (inTag == "SCALE_BRIGHTNESS") { edit = EditType::SCALE_BRIGHTNESS; tag = inTag; }
	else if (inTag == "SHIFT_BRIGHTNESS") { edit = EditType::SHIFT_BRIGHTNESS; tag = inTag; }
}

wxString ProcessorEdit::GetEditTag() {
	return tag;
}