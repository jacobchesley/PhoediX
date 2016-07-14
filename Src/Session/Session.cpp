#include "Session.h"

PhoediXSession::PhoediXSession(){
	editList = new PhoediXSessionEditList();
	imgZoom = 1.0f;
	imgScrollX = 0;
	imgScrollY = 0;
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
			imgFile = sessionInfo->GetChildren()[0].GetContent();
		}

		// Get Histogram display selection
		if (sessionInfo->GetName() == "HistogramDisplay") {
			// Get first child (the actual content)
			histogramDisplaySelect = wxAtoi(sessionInfo->GetChildren()[0].GetContent());
		}

		// Get Image Display Zoom
		if (sessionInfo->GetName() == "DisplayZoom") {
			// Get first child (the actual content)
			imgZoom = wxAtof(sessionInfo->GetChildren()[0].GetContent());
		}

		// Get Image Display scroll x
		if (sessionInfo->GetName() == "DisplayOffsetX") {
			// Get first child (the actual content)
			imgScrollX = wxAtoi(sessionInfo->GetChildren()[0].GetContent());
		}

		// Get Image Display scroll y
		if (sessionInfo->GetName() == "DisplayOffsetY") {
			// Get first child (the actual content)
			imgScrollY = wxAtoi(sessionInfo->GetChildren()[0].GetContent());
		}

		// Get Perspective node
		if (sessionInfo->GetName() == "Perspective") {
			// Get first child (the actual content)
			auiPerspective = sessionInfo->GetChildren()[0].GetContent();
		}

		// Get and load edit list into session
		if (sessionInfo->GetName() == "EditList") {
			editList->LoadSessionEditList(sessionInfo);
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

	// Write the perspective for AUI to XML Doc
	wxXmlNode * perspective = new wxXmlNode(sessionInfo, wxXML_ELEMENT_NODE, "Perspective");
	perspective->AddChild(new wxXmlNode(wxXML_TEXT_NODE, "", auiPerspective));

	// Write the edit list to XML Doc
	editList->SaveSessionEditList(sessionInfo);

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

void PhoediXSession::SetImageScrollY(int y) {
	imgScrollY = y;
}