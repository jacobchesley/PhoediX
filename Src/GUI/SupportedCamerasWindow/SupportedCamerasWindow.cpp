// Copyright 2018 Jacob Chesley.
// See LICENSE.TXT in root of project for license information.

#include "SupportedCamerasWindow.h"
#include "libraw.h"

SupportedCamerasWindow::SupportedCamerasWindow(wxWindow * parent) : wxFrame(parent, -1, "Supported Cameras (RAW)") {

	this->SetBackgroundColour(parent->GetBackgroundColour());

	Icons icons;
	wxIcon theIcon;
	theIcon.CopyFromBitmap(wxBitmap(icons.pxIcon));
	this->SetIcon(theIcon);

	mainSizer = new wxBoxSizer(wxVERTICAL);
	this->SetSizer(mainSizer);

	librawVersionTitle = new wxStaticText(this, -1, "Supported Cameras in LibRaw " + wxString::FromUTF8(LibRaw::version()));
	librawVersionTitle->SetForegroundColour(Colors::TextLightGrey);

	supportedCameras = new wxTreeCtrl(this);
	supportedCameras->SetBackgroundColour(this->GetBackgroundColour());
	supportedCameras->SetForegroundColour(Colors::TextLightGrey);
	rootId = supportedCameras->AddRoot("Supported Cameras (RAW)");

	this->PopulateCameras();
	supportedCameras->Expand(rootId);

	okayButton = new PhoediXButton(this, ID_OKAY, "OK");
	okayButton->SetForegroundColour(Colors::TextLightGrey);
	okayButton->SetBackgroundColour(Colors::BackGrey);
	okayButton->SetFont(wxFont(11, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL));

	this->GetSizer()->Add(librawVersionTitle);
	this->GetSizer()->AddSpacer(10);
	this->GetSizer()->Add(supportedCameras, 1, wxEXPAND);
	this->GetSizer()->AddSpacer(20);
	this->GetSizer()->Add(okayButton, 0, wxALIGN_RIGHT);

	this->Bind(wxEVT_CLOSE_WINDOW, (wxObjectEventFunction)&SupportedCamerasWindow::OnClose, this);
	this->Bind(wxEVT_BUTTON, (wxObjectEventFunction)&SupportedCamerasWindow::OnOkay, this, ID_OKAY);

	this->Fit();
	this->SetSize(this->GetSize().GetWidth(), wxSystemSettings::GetMetric(wxSYS_SCREEN_Y) / 2.0);
	this->CentreOnParent();
}

void SupportedCamerasWindow::PopulateCameras() {

	const char ** cameras = LibRaw::cameraList();
	const char **camera = cameras;
	while (*camera)
	{
		wxString cameraStr = wxString::FromUTF8(*camera);

		// Camera make and model.  Make will be a root, and models will be children of root
		wxString cameraMake = "";
		wxString cameraModel = "";

		// Split camera string where spaces occur
		wxStringTokenizer tokenizer(cameraStr, " ");
		int curToken = 0;

		// Go through camera words
		while (tokenizer.HasMoreTokens()){

			// Get current word
			wxString token = tokenizer.GetNextToken();

			// First word is usually make (unless special case)
			if (curToken == 0) { 
				cameraMake = token; 
				
				// Handle camera makes with more than 1 word
				// Digital Bolex
				if (cameraMake.CmpNoCase("Digital") == 0) {
					wxString bolex = tokenizer.GetNextToken();
					cameraMake += " " + bolex;
					curToken += 1;
				}

				// Adobe Digital Negative (DNG)
				if (cameraMake.CmpNoCase("Adobe") == 0) {
					wxString digital = tokenizer.GetNextToken();
					wxString negative = tokenizer.GetNextToken();
					wxString dng = tokenizer.GetNextToken();
					cameraMake += " " + digital + " " + negative + " " + dng;
					curToken += 3;

					// Do not add camera model, DNG will just be listed as camera make
					break;
				}
			}

			// Append all other words to create camera model
			else { cameraModel = cameraModel + " " + token; }
			curToken += 1;
		}

		// If camera make is not in list, add camera make and then the model as a child
		if (!this->CheckCameraMakeInList(cameraMake)) {

			// Create new camera make root and add it to list
			CameraMakeRoot tempRoot;
			tempRoot.cameraMakeID = supportedCameras->AppendItem(rootId, cameraMake);
			tempRoot.cameraMake = cameraMake;
			cameraMakeList.push_back(tempRoot);

			// Add camera model if it is not empty
			if (cameraModel != "") {
				supportedCameras->AppendItem(tempRoot.cameraMakeID, cameraModel);
			}
		}

		// Camera make already exists
		else {

			// Search for root ID based on camera make string
			for (size_t i = 0; i < cameraMakeList.size(); i++) {
				if (cameraMakeList.at(i).cameraMake.CmpNoCase(cameraMake) == 0) { 

					// Add camera model if it is not empty
					if (cameraModel != "") {
						supportedCameras->AppendItem(cameraMakeList.at(i).cameraMakeID, cameraModel);
					}
					break;
				}
			}
		}
		
		// Get the next camera string
		camera++;
	}
}

bool SupportedCamerasWindow::CheckCameraMakeInList(wxString cameraMake) {
	for (size_t i = 0; i < cameraMakeList.size(); i++) {
		if (cameraMakeList.at(i).cameraMake.CmpNoCase(cameraMake) == 0) { return true; }
	}
	return false;
}

void SupportedCamerasWindow::OnClose(wxCloseEvent& WXUNUSED(evt)) {
	this->Hide();
}

void SupportedCamerasWindow::OnOkay(wxCommandEvent& WXUNUSED(evt)) {
	this->Hide();
}