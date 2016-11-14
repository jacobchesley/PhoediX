// Copyright 2016 Jacob Chesley

#include "Session.h"
#include <random>

PhoediXSession::PhoediXSession(){
	editList = new PhoediXSessionEditList();
	imgZoom = 1.0f;
	imgScrollX = 0;
	imgScrollY = 0;
	imgWidth = 0;
	imgHeight = 0;
	id = -1;
	name = "";
}

void PhoediXSession::Destroy(){
	delete editList;
	editList = NULL;
	id = -1;
}

void PhoediXSession::LoadSessionFromFile(wxString filePath) {

	// Load the project, return if it fails
	wxXmlDocument session; 
	if (!session.Load(filePath)) { return; }

	// Verify root node
	if (session.GetRoot()->GetName() != "PhoediXProject") { return; }

	wxXmlNode * sessionInfo = session.GetRoot()->GetChildren();

	while (sessionInfo) {

		// Get ImagePath node
		if (sessionInfo->GetName() == "ImagePath") {
			// Get first child (the actual content)
			if(sessionInfo->GetChildren() != NULL){
				imgFile = sessionInfo->GetChildren()[0].GetContent();
			}
		}

		// Get Histogram display selection
		if (sessionInfo->GetName() == "HistogramDisplay") {
			// Get first child (the actual content)
			if(sessionInfo->GetChildren() != NULL){
				histogramDisplaySelect = wxAtoi(sessionInfo->GetChildren()[0].GetContent());
			}
		}

		// Get Image Display Zoom
		if (sessionInfo->GetName() == "DisplayZoom") {
			// Get first child (the actual content)
			if(sessionInfo->GetChildren() != NULL){
				imgZoom = wxAtof(sessionInfo->GetChildren()[0].GetContent());
			}
		}

		// Get Image Display scroll x
		if (sessionInfo->GetName() == "DisplayOffsetX") {
			// Get first child (the actual content)
			if(sessionInfo->GetChildren() != NULL){
				imgScrollX = wxAtoi(sessionInfo->GetChildren()[0].GetContent());
			}
		}

		// Get Image Display scroll y
		if (sessionInfo->GetName() == "DisplayOffsetY") {
			// Get first child (the actual content)
			if(sessionInfo->GetChildren() != NULL){
				imgScrollY = wxAtoi(sessionInfo->GetChildren()[0].GetContent());
			}
		}

		// Get Image Display scroll Width
		if (sessionInfo->GetName() == "ImageWidth") {
			// Get first child (the actual content)
			if(sessionInfo->GetChildren() != NULL){
				imgWidth = wxAtoi(sessionInfo->GetChildren()[0].GetContent());
			}
		}

		// Get Image Display scroll Height
		if (sessionInfo->GetName() == "ImageHeight") {
			// Get first child (the actual content)
			if(sessionInfo->GetChildren() != NULL){
				imgHeight = wxAtoi(sessionInfo->GetChildren()[0].GetContent());
			}
		}

		// Get Perspective node
		if (sessionInfo->GetName() == "Perspective") {
			// Get first child (the actual content)
			if(sessionInfo->GetChildren() != NULL){
				auiPerspective = sessionInfo->GetChildren()[0].GetContent();
			}
		}

		// Get and load edit list into session
		if (sessionInfo->GetName() == "EditList") {
			editList->LoadSessionEditList(sessionInfo);
		}

		// Get and load snapshots into session
		if (sessionInfo->GetName() == "AllSnapshots") {
			if (sessionInfo->GetChildren() != NULL) {

				wxXmlNode * allSnapshots = sessionInfo->GetChildren();

				snapshotsList.clear();

				// Count number of snapshots
				size_t numSnapshots = 0;
				while (allSnapshots) {
					numSnapshots += 1;
					allSnapshots = allSnapshots->GetNext();
				}

				// Resize the parameters vector to account for all parameters
				snapshotsList.resize(numSnapshots);

				for (size_t i = 0; i < numSnapshots; i++) {
					snapshotsList.at(i) = new Snapshot();
				}

				allSnapshots = sessionInfo->GetChildren();
				// Go through all snapshots
				while (allSnapshots) {

					wxXmlNode * snapshotElement = allSnapshots->GetChildren();
					Snapshot * tempSnapshot = new Snapshot();

					// Populate snapshot info
					while (snapshotElement) {

						// Get Snapshot index
						if (snapshotElement->GetName() == "SnapshotIndex") {
							// Get first child (the actual content)
							if (snapshotElement->GetChildren() != NULL) {
								tempSnapshot->snapshotIndex = wxAtoi(snapshotElement->GetChildren()[0].GetContent());
							}
						}

						// Get Snapshot Name
						if (snapshotElement->GetName() == "SnapshotName") {
							// Get first child (the actual content)
							if (snapshotElement->GetChildren() != NULL) {
								tempSnapshot->snapshotName = snapshotElement->GetChildren()[0].GetContent();
							}
						}

						// Get Snapshot AUI Perspective
						if (snapshotElement->GetName() == "SnapshotPerspective") {
							// Get first child (the actual content)
							if (snapshotElement->GetChildren() != NULL) {
								tempSnapshot->auiPerspective = snapshotElement->GetChildren()[0].GetContent();
							}
						}

						// Get Snapshot Edit List
						if (snapshotElement->GetName() == "EditList") {
							PhoediXSessionEditList snapshotEditList = PhoediXSessionEditList();
							snapshotEditList.LoadSessionEditList(snapshotElement);

							// Copy edit list to snapshot
							tempSnapshot->editList.clear();
							for (size_t i = 0; i < snapshotEditList.GetSessionEditList().size(); i++) {
								tempSnapshot->editList.push_back(new ProcessorEdit(*snapshotEditList.GetSessionEditList().at(i)));
							}
						}
						snapshotElement = snapshotElement->GetNext();
					}
					snapshotsList[tempSnapshot->snapshotIndex] = tempSnapshot;
					allSnapshots = allSnapshots->GetNext();
				}
			}
		}
		sessionInfo = sessionInfo->GetNext();
	}
}

void PhoediXSession::SaveSessionToFile(wxString filePath) {

	// Create root XML Doc
	wxXmlDocument session;
	wxXmlNode * sessionInfo = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, "PhoediXProject");
	session.SetRoot(sessionInfo);

	// Write the image file path to XML Doc
	wxXmlNode * imagePath = new wxXmlNode(sessionInfo, wxXML_ELEMENT_NODE, "ImagePath");
	imagePath->AddChild(new wxXmlNode(wxXML_TEXT_NODE, "", imgFile));

	// Write hisotgram display selection to XML Doc
	wxXmlNode * histSelect = new wxXmlNode(sessionInfo, wxXML_ELEMENT_NODE, "HistogramDisplay");
	histSelect->AddChild(new wxXmlNode(wxXML_TEXT_NODE, "", wxString::Format(wxT("%i"), histogramDisplaySelect)));

	// Write display image zoom level to XML Doc
	wxXmlNode * zoom = new wxXmlNode(sessionInfo, wxXML_ELEMENT_NODE, "DisplayZoom");
	zoom->AddChild(new wxXmlNode(wxXML_TEXT_NODE, "", wxString::Format(wxT("%f"), imgZoom)));

	// Write dislay image scroll x
	wxXmlNode * scrollX = new wxXmlNode(sessionInfo, wxXML_ELEMENT_NODE, "DisplayOffsetX");
	scrollX->AddChild(new wxXmlNode(wxXML_TEXT_NODE, "", wxString::Format(wxT("%i"), imgScrollX)));

	// Write display image scroll y
	wxXmlNode * scrollY = new wxXmlNode(sessionInfo, wxXML_ELEMENT_NODE, "DisplayOffsetY");
	scrollY->AddChild(new wxXmlNode(wxXML_TEXT_NODE, "", wxString::Format(wxT("%i"), imgScrollY)));

	// Write dislay image scroll width
	wxXmlNode * imageWidth = new wxXmlNode(sessionInfo, wxXML_ELEMENT_NODE, "ImageWidth");
	imageWidth->AddChild(new wxXmlNode(wxXML_TEXT_NODE, "", wxString::Format(wxT("%i"), imgWidth)));

	// Write display image scroll height
	wxXmlNode * imageHeight = new wxXmlNode(sessionInfo, wxXML_ELEMENT_NODE, "ImageHeight");
	imageHeight->AddChild(new wxXmlNode(wxXML_TEXT_NODE, "", wxString::Format(wxT("%i"), imgHeight)));

	// Write the perspective for AUI to XML Doc
	wxXmlNode * perspective = new wxXmlNode(sessionInfo, wxXML_ELEMENT_NODE, "Perspective");
	perspective->AddChild(new wxXmlNode(wxXML_TEXT_NODE, "", auiPerspective));

	// Write the edit list to XML Doc
	editList->SaveSessionEditList(sessionInfo);

	// Save snapshotsSnapshotPerspective
	wxXmlNode * snapshots = new wxXmlNode(sessionInfo, wxXML_ELEMENT_NODE, "AllSnapshots");
	for (size_t i = 0; i < snapshotsList.size(); i++) {

		wxXmlNode * snapshot = new wxXmlNode(snapshots, wxXML_ELEMENT_NODE, "Snapshot");

		// save snapshots index
		wxXmlNode * snapshotIndex = new wxXmlNode(snapshot, wxXML_ELEMENT_NODE, "SnapshotIndex");
		snapshotIndex->AddChild(new wxXmlNode(wxXML_TEXT_NODE, "", wxString::Format(wxT("%i"), snapshotsList.at(i)->snapshotIndex)));

		// save snapshots name
		wxXmlNode * snapshotName = new wxXmlNode(snapshot, wxXML_ELEMENT_NODE, "SnapshotName");
		snapshotName->AddChild(new wxXmlNode(wxXML_TEXT_NODE, "", snapshotsList.at(i)->snapshotName));

		// save snapshots aui perspective
		wxXmlNode * snapshotAui = new wxXmlNode(snapshot, wxXML_ELEMENT_NODE, "SnapshotPerspective");
		snapshotAui->AddChild(new wxXmlNode(wxXML_TEXT_NODE, "", snapshotsList.at(i)->auiPerspective));

		// Save snapshots edit list
		PhoediXSessionEditList snapshotEditList = PhoediXSessionEditList();
		snapshotEditList.SetSessionEditList(snapshotsList.at(i)->editList);
		snapshotEditList.SaveSessionEditList(snapshot);
	}

	session.Save(filePath);
}

void PhoediXSession::SetImageFilePath(wxString filePath) {
	imgFile = filePath;
}

wxString PhoediXSession::GetImageFilePath() {
	return imgFile;
}

PhoediXSessionEditList * PhoediXSession::GetEditList() {
	return editList;
}

wxString PhoediXSession::GetPerspective() {
	return auiPerspective;
}

void PhoediXSession::SetPerspective(wxString newPerspective) {
	auiPerspective = newPerspective;
}

int PhoediXSession::GetHistogramDisplaySelect() {
	return histogramDisplaySelect;
}

void PhoediXSession::SetHistogramDisplaySelect(int selection) {
	histogramDisplaySelect = selection;
}

float PhoediXSession::GetImageZoomLevel() {
	return imgZoom;
}

void PhoediXSession::SetImageZoomLevel(float zoom) {
	imgZoom = zoom;
}

int PhoediXSession::GetImageScrollX() {
	return imgScrollX;
}

void PhoediXSession::SetImageScrollX(int x) {
	imgScrollX = x;
}

int PhoediXSession::GetImageScrollY() {
	return imgScrollY;
}

void PhoediXSession::SetImageScrollWidth(int width) {
	imgWidth = width;
}

int PhoediXSession::GetImageScrollWidth() {
	return imgWidth;
}

void PhoediXSession::SetImageScrollHeight(int height) {
	imgHeight = height;
}

int PhoediXSession::GetImageScrollHeight() {
	return imgHeight;
}

void PhoediXSession::SetImageScrollY(int y) {
	imgScrollY = y;
}

void PhoediXSession::GenerateID() {

	id = rand() % 20000 + 500;
}

int PhoediXSession::GetID() {
	return id;
}

void PhoediXSession::SetName(wxString newName) {
	name = newName;
}

wxString PhoediXSession::GetName() {
	return name;
}

wxVector<Snapshot*> PhoediXSession::GetSnapshots() {
	return snapshotsList;
}

void PhoediXSession::SetSnapshots(wxVector<Snapshot*> snapshots) {
	
	// Clear current snapshots
	for (size_t i = 0; i < snapshotsList.size(); i++) {

		// Delete all processor edits for snapshot
		for (size_t j = 0; j < snapshotsList.at(i)->editList.size(); j++) {
			delete snapshotsList.at(i)->editList.at(j);
		}

		delete snapshotsList.at(i);
	}

	snapshotsList.clear();
	
	for (size_t i = 0; i < snapshots.size(); i++) {

		// Create new snapshot and copy info
		Snapshot * tempSnapshot = new Snapshot();

		// Copy inedex, name, and perspective
		tempSnapshot->snapshotIndex = snapshots.at(i)->snapshotIndex;
		tempSnapshot->snapshotName = snapshots.at(i)->snapshotName;
		tempSnapshot->auiPerspective = snapshots.at(i)->auiPerspective;

		// Copy edit list
		tempSnapshot->editList.clear();
		for (size_t j = 0; j < snapshots.at(i)->editList.size(); j++) {
			tempSnapshot->editList.push_back(new ProcessorEdit(*snapshots.at(i)->editList.at(j)));
		}

		// push onto list
		snapshotsList.push_back(tempSnapshot);
	}
}