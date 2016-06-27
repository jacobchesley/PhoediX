#include "Session.h"

PhoediXSession::PhoediXSession(){
	editList = new PhoediXSessionEditList();
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