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
	editVector.push_back(Edit(AvailableEditIDS::EDIT_ID_SHIFT_BRIGHTNESS, "Shift Brightness", "Shift the images brightness with an addative value."));
	editVector.push_back(Edit(AvailableEditIDS::EDIT_ID_SCALE_BRIGHTNESS, "Scale Brightness", "Scale the images brightness with a multiplicative value."));
	editVector.push_back(Edit(AvailableEditIDS::EDIT_ID_CONTRAST, "Adjust Contrast", "Adjust the images contrast, to give a more vivid or flat look."));
	editVector.push_back(Edit(AvailableEditIDS::EDIT_ID_GREYSCALE, "Convert to Greyscale", "Convert the image to greyscale, choosing one of three defined methods of doing so."));
	editVector.push_back(Edit(AvailableEditIDS::EDIT_ID_CHANNEL_TRANSFORM, "Channel Transform", "Transform RGB Channels, to create new tones such as sepia and more."));
	editVector.push_back(Edit(AvailableEditIDS::EDIT_ID_ROTATE, "Rotate Image", "Rotate image 90, 180, 270, or custom amount of degrees"));
	editVector.push_back(Edit(AvailableEditIDS::EDIT_ID_MIRROR, "Mirror Image", "Mirror image horizontally or vertically"));
}

wxVector<Edit> AvailableEdits::GetAvailableEdits() {
	return editVector;
}

wxString AvailableEdits::GetNameFromID(int id) {

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