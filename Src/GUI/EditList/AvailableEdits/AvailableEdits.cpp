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
	editVector.push_back(Edit(AvailableEditIDS::EDIT_ID_ADJUST_BRIGHTNESS, "Adjust Brightness", "Adjust Image Brightness"));
	editVector.push_back(Edit(AvailableEditIDS::EDIT_ID_ADJUST_HSL, "Adjust HSL", "Scale and shift HSL image channels."));
	editVector.push_back(Edit(AvailableEditIDS::EDIT_ID_SHIFT_RGB, "Shift RGB", "Shift the images RGB channels with an addative value."));
	editVector.push_back(Edit(AvailableEditIDS::EDIT_ID_CONTRAST, "Adjust Contrast", "Adjust the images contrast, to give a more vivid or flat look."));
	editVector.push_back(Edit(AvailableEditIDS::EDIT_ID_GREYSCALE, "Convert to Greyscale", "Convert the image to greyscale, choosing one of three defined methods of doing so."));
	editVector.push_back(Edit(AvailableEditIDS::EDIT_ID_CHANNEL_MIXER, "Channel Mixer", "RGB Channel Mixer"));
	editVector.push_back(Edit(AvailableEditIDS::EDIT_ID_RGB_CURVES, "RGB Curves", "Curves tool in RGB Color Space"));
	editVector.push_back(Edit(AvailableEditIDS::EDIT_ID_LAB_CURVES, "LAB Curves", "Curves tool in LAB Color Space"));
	editVector.push_back(Edit(AvailableEditIDS::EDIT_ID_HSL_CURVES, "HSL Curves", "Curves tool in HSL Color Space"));
	editVector.push_back(Edit(AvailableEditIDS::EDIT_ID_ROTATE, "Rotate Image", "Rotate image 90, 180, 270, or custom amount of degrees"));
	editVector.push_back(Edit(AvailableEditIDS::EDIT_ID_MIRROR, "Mirror Image", "Mirror image horizontally or vertically"));
	editVector.push_back(Edit(AvailableEditIDS::EDIT_ID_SCALE, "Scale Image", "Resize image to any size"));
	editVector.push_back(Edit(AvailableEditIDS::EDIT_ID_CROP, "Crop Image", "Crop image with a starting point, along with width and height"));
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