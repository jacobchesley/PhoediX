// Copyright 2018 Jacob Chesley.
// See LICENSE.TXT in root of project for license information.

#include "AvailableEdits.h"

Edit::Edit(int editID, wxString editName, wxString editDescription) {

	id = editID;
	name = editName;
	description = editDescription;
}

int Edit::GetID() {
	return id;
}

wxString Edit::GetName() {
	return name;
}

wxString Edit::GetDescription() {
	return description;
}


AvailableEdits::AvailableEdits() {
	editVector.push_back(Edit(AvailableEditIDS::EDIT_ID_ADJUST_BRIGHTNESS, "Adjust Brightness", "Adjust image brightness."));
	editVector.push_back(Edit(AvailableEditIDS::EDIT_ID_CONTRAST, "Adjust Contrast", "Adjust image contrast."));
	editVector.push_back(Edit(AvailableEditIDS::EDIT_ID_ADJUST_HSL, "Adjust HSL", "Scale and shift HSL image channels."));
	editVector.push_back(Edit(AvailableEditIDS::EDIT_ID_ADJUST_LAB, "Adjust LAB", "Scale and shift LAB image channels."));
	editVector.push_back(Edit(AvailableEditIDS::EDIT_ID_ADJUST_RGB, "Adjust RGB", "Shift RGB image channels."));
	editVector.push_back(Edit(AvailableEditIDS::EDIT_ID_BLUR, "Blur", "Blur image horiztontally, vertically, or all directions."));
	editVector.push_back(Edit(AvailableEditIDS::EDIT_ID_CHANNEL_MIXER, "Channel Mixer", "RGB channel mixer."));
	editVector.push_back(Edit(AvailableEditIDS::EDIT_ID_CROP, "Crop", "Crop image."));
	editVector.push_back(Edit(AvailableEditIDS::EDIT_ID_HSL_CURVES, "Curves - HSL", "Curves tool in HSL Color Space."));
	editVector.push_back(Edit(AvailableEditIDS::EDIT_ID_LAB_CURVES, "Curves - LAB", "Curves tool in LAB Color Space."));
	editVector.push_back(Edit(AvailableEditIDS::EDIT_ID_RGB_CURVES, "Curves - RGB", "Curves tool in RGB Color Space."));
	editVector.push_back(Edit(AvailableEditIDS::EDIT_ID_GREYSCALE, "Greyscale", "Convert image to greyscale, choosing one of three defined methods of doing so."));
	editVector.push_back(Edit(AvailableEditIDS::EDIT_ID_MIRROR, "Mirror", "Mirror image horizontally or vertically."));
	editVector.push_back(Edit(AvailableEditIDS::EDIT_ID_ROTATE, "Rotate", "Rotate image 90, 180, 270, or custom degree angle."));
	editVector.push_back(Edit(AvailableEditIDS::EDIT_ID_SCALE, "Scale", "Resize image to any size."));
}

wxVector<Edit> AvailableEdits::GetAvailableEdits() {
	return editVector;
}

wxString AvailableEdits::GetNameFromID(int id) {

	if(id == AvailableEditIDS::EDIT_ID_RAW){
		return "Raw Processor";
	}

	for (size_t i = 0; i < editVector.size(); i++) {
		if (editVector.at(i).GetID() == id) {
			return editVector.at(i).GetName();
		}
	}
	return "";
}

wxString AvailableEdits::GetDescriptionFromID(int id) {

	for (size_t i = 0; i < editVector.size(); i++) {
		if (editVector.at(i).GetID() == id) {
			return editVector.at(i).GetDescription();
		}
	}
	return "";
}

int AvailableEdits::GetIDFromName(wxString name) {

	for (size_t i = 0; i < editVector.size(); i++) {
		if (editVector.at(i).GetName() == name) {	
			return editVector.at(i).GetID();
		}
	}
	return -1;
}

int AvailableEdits::GetIDFromDescription(wxString description) {

	for (size_t i = 0; i < editVector.size(); i++) {
		if (editVector.at(i).GetDescription() == description) {
			return editVector.at(i).GetID();
		}
	}
	return -1;
}