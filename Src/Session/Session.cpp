// Copyright 2018 Jacob Chesley.
// See LICENSE.TXT in root of project for license information.

#include "Session.h"
#include "App/PhoediX.h"
#include "Debugging/Logger/Logger.h"

#if defined(__WXMSW__) || defined(__WXGTK__)
#include <random>
#endif

PhoediXSession::PhoediXSession(){
	editList = new PhoediXSessionEditList();
	imgZoom = 1.0f;
	imgScrollX = 0;
	imgScrollY = 0;
	imgWidth = 0;
	imgHeight = 0;
	id = -1;
	name = "";
	fitImage = false;
	processFast = false;
	histogramDisplaySelect = 0;
	exportFolder = "";
	exportName = "";
	exportImageType = 0;
	exportJPEGQuality = 90;
	guideColor = wxColor(128, 128, 128);
	gridColor1 = wxColor(0, 0, 0);
	gridColor2 = wxColor(255, 255, 255);
	guideType = 1;
	showGuidelines = false;
}

PhoediXSession::~PhoediXSession() {
	this->Destroy();
}

void PhoediXSession::Destroy(){
    
    Logger::Log("PhoediX PhoediXSession::Destroy - Called destroy", Logger::LogLevel::LOG_VERBOSE);
    
	if (editList != NULL) {
        
        Logger::Log("PhoediX PhoediXSession::Destroy - deleting edit list", Logger::LogLevel::LOG_VERBOSE);
		delete editList;
		editList = NULL;
	}
    
    Logger::Log("PhoediX PhoediXSession::Destroy - deleting snapshots...", Logger::LogLevel::LOG_VERBOSE);
	// Delete all snapshot edit lists
	for (size_t i = 0; i < snapshotsList.size(); i++) {
		for (size_t j = 0; j < snapshotsList.at(i)->editList.size(); j++) {
             Logger::Log("     PhoediX PhoediXSession::Destroy - deleting snapshot edit list", Logger::LogLevel::LOG_VERBOSE);
			delete snapshotsList.at(i)->editList.at(j);
		}
        
        Logger::Log("     PhoediX PhoediXSession::Destroy - deleting snapshot", Logger::LogLevel::LOG_VERBOSE);
		delete snapshotsList.at(i);
	}
    Logger::Log("PhoediX PhoediXSession::Destroy - returning", Logger::LogLevel::LOG_VERBOSE);
	id = -1;
}

void PhoediXSession::LoadSessionFromFile(wxString filePath) {

    Logger::Log("PhoediX PhoediXSession::LoadSessionFromFile - Loading file: " + filePath, Logger::LogLevel::LOG_VERBOSE);
	// Load the project, return if it fails
	wxXmlDocument session; 
	if (!session.Load(filePath)) {
        
        Logger::Log("PhoediX PhoediXSession::LoadSessionFromFile - Failed to load, checking for backup", Logger::LogLevel::LOG_WARNING);
        // Try loading backup
        if(wxFileExists(filePath + ".bak")){
            wxCopyFile(filePath + ".bak", filePath);
            if (!session.Load(filePath)) {
                Logger::Log("PhoediX PhoediXSession::LoadSessionFromFile - Backup file failed to load, returning", Logger::LogLevel::LOG_ERROR);
                return;
            }
            else{
                Logger::Log("PhoediX PhoediXSession::LoadSessionFromFile - Successfully loaded backup file", Logger::LogLevel::LOG_VERBOSE);
            }
        }
        else{
            Logger::Log("PhoediX PhoediXSession::LoadSessionFromFile - no backup file, returning", Logger::LogLevel::LOG_ERROR);
            return;
        }
    }

	// Verify root node
	if (session.GetRoot()->GetName() != "PhoediXProject") {
        Logger::Log("PhoediX PhoediXSession::LoadSessionFromFile - Not a PhoediX Project, returning", Logger::LogLevel::LOG_ERROR);
        return;
    }

	wxXmlNode * sessionInfo = session.GetRoot()->GetChildren();

    Logger::Log("PhoediX PhoediXSession::LoadSessionFromFile - Loading session...", Logger::LogLevel::LOG_VERBOSE);
	while (sessionInfo) {

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

		// Get Fit Image
		if (sessionInfo->GetName() == "FitImage") {
			// Get first child (the actual content)
			if(sessionInfo->GetChildren() != NULL){
				int fitImageInt = wxAtoi(sessionInfo->GetChildren()[0].GetContent());
				if(fitImageInt == 0){ fitImage = false;}
				else{ fitImage = true; }
			}
		}

		// Get Fast Process
		if (sessionInfo->GetName() == "FastProcess") {
			// Get first child (the actual content)
			if (sessionInfo->GetChildren() != NULL) {
				int processFastInt = wxAtoi(sessionInfo->GetChildren()[0].GetContent());
				if (processFastInt == 0) { processFast = false; }
				else { processFast = true; }
			}
		}

		// Get Perspective node
		if (sessionInfo->GetName() == "Perspective") {
			// Get first child (the actual content)
			if(sessionInfo->GetChildren() != NULL){
				auiPerspective = sessionInfo->GetChildren()[0].GetContent();
			}
		}

		// Get Export Folder node
		if (sessionInfo->GetName() == "ExportFolder") {
			// Get first child (the actual content)
			if (sessionInfo->GetChildren() != NULL) {
				exportFolder = sessionInfo->GetChildren()[0].GetContent();
			}
		}

		// Get Export Name node
		if (sessionInfo->GetName() == "ExportName") {
			// Get first child (the actual content)
			if (sessionInfo->GetChildren() != NULL) {
				exportName = sessionInfo->GetChildren()[0].GetContent();
			}
		}

		// Get Export Image Type
		if (sessionInfo->GetName() == "ExportImageType") {
			// Get first child (the actual content)
			if (sessionInfo->GetChildren() != NULL) {
				exportImageType = wxAtoi(sessionInfo->GetChildren()[0].GetContent());
			}
		}

		// Get Export Image JPEG Quality
		if (sessionInfo->GetName() == "ExportJPEGQuality") {
			// Get first child (the actual content)
			if (sessionInfo->GetChildren() != NULL) {
				exportJPEGQuality = wxAtoi(sessionInfo->GetChildren()[0].GetContent());
			}
		}

		// Get Guideline Type
		if (sessionInfo->GetName() == "GuidelineType") {
			// Get first child (the actual content)
			if (sessionInfo->GetChildren() != NULL) {
				guideType = wxAtoi(sessionInfo->GetChildren()[0].GetContent());
			}
		}

		// Get Guideline Color
		if (sessionInfo->GetName() == "GuidelineColor") {
			// Get first child (the actual content)
			if (sessionInfo->GetChildren() != NULL) {
				guideColor.Set(sessionInfo->GetChildren()[0].GetContent());
			}
		}

		// Get Crop Grid Color 1
		if (sessionInfo->GetName() == "CropGridColor1") {
			// Get first child (the actual content)
			if (sessionInfo->GetChildren() != NULL) {
				gridColor1.Set(sessionInfo->GetChildren()[0].GetContent());
			}
		}

		// Get Crop Grid Color 2
		if (sessionInfo->GetName() == "CropGridColor2") {
			// Get first child (the actual content)
			if (sessionInfo->GetChildren() != NULL) {
				gridColor2.Set(sessionInfo->GetChildren()[0].GetContent());
			}
		}

		// Get Show Guideline
		if (sessionInfo->GetName() == "ShowGuidelines") {
			// Get first child (the actual content)
			if (sessionInfo->GetChildren() != NULL) {
				int showGuidelineInt = wxAtoi(sessionInfo->GetChildren()[0].GetContent());
				if (showGuidelineInt == 0) { showGuidelines = false; }
				else { showGuidelines = true; }
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
    
    Logger::Log("PhoediX PhoediXSession::LoadSessionFromFile - Session loaded, returning", Logger::LogLevel::LOG_VERBOSE);
}

void PhoediXSession::SaveSessionToFile(wxString filePath) {

    Logger::Log("PhoediX PhoediXSession::SaveSessionToFile - Saving session: " + filePath, Logger::LogLevel::LOG_VERBOSE);
    // Backup the file before saving
    if(wxFileExists(filePath)){
        Logger::Log("PhoediX PhoediXSession::SaveSessionToFile - Backing up session file", Logger::LogLevel::LOG_VERBOSE);
        wxCopyFile(filePath, filePath +".bak");
    }
    
	// Create root XML Doc
	wxXmlDocument session;
	wxXmlNode * sessionInfo = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, "PhoediXProject");
	sessionInfo->AddAttribute("PhoediX_Version", PHOEDIX_VERSION_STRING);
	session.SetRoot(sessionInfo);

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

	// Write fit image
	wxXmlNode * imageFit = new wxXmlNode(sessionInfo, wxXML_ELEMENT_NODE, "FitImage");
	imageFit->AddChild(new wxXmlNode(wxXML_TEXT_NODE, "", wxString::Format(wxT("%i"), (int)fitImage)));

	// Write fast process
	wxXmlNode * fastProc = new wxXmlNode(sessionInfo, wxXML_ELEMENT_NODE, "FastProcess");
	fastProc->AddChild(new wxXmlNode(wxXML_TEXT_NODE, "", wxString::Format(wxT("%i"), (int)processFast)));

	// Write the perspective for AUI to XML Doc
	wxXmlNode * perspective = new wxXmlNode(sessionInfo, wxXML_ELEMENT_NODE, "Perspective");
	perspective->AddChild(new wxXmlNode(wxXML_TEXT_NODE, "", auiPerspective));

	// Write the export folder
	wxXmlNode * exFolder = new wxXmlNode(sessionInfo, wxXML_ELEMENT_NODE, "ExportFolder");
	exFolder->AddChild(new wxXmlNode(wxXML_TEXT_NODE, "", exportFolder));

	// Write the export name
	wxXmlNode * exName = new wxXmlNode(sessionInfo, wxXML_ELEMENT_NODE, "ExportName");
	exName->AddChild(new wxXmlNode(wxXML_TEXT_NODE, "", exportName));

	// Write export image type
	wxXmlNode * exType = new wxXmlNode(sessionInfo, wxXML_ELEMENT_NODE, "ExportImageType");
	exType->AddChild(new wxXmlNode(wxXML_TEXT_NODE, "", wxString::Format(wxT("%i"), (int)exportImageType)));

	// Write export jpeg quality
	wxXmlNode * exJpegQual = new wxXmlNode(sessionInfo, wxXML_ELEMENT_NODE, "ExportJPEGQuality");
	exJpegQual->AddChild(new wxXmlNode(wxXML_TEXT_NODE, "", wxString::Format(wxT("%i"), (int)exportJPEGQuality)));

	// Write guideline type
	wxXmlNode * exGuideType = new wxXmlNode(sessionInfo, wxXML_ELEMENT_NODE, "GuidelineType");
	exGuideType->AddChild(new wxXmlNode(wxXML_TEXT_NODE, "", wxString::Format(wxT("%i"), (int)guideType)));

	// Write show guidelines
	wxXmlNode * exGuideShow = new wxXmlNode(sessionInfo, wxXML_ELEMENT_NODE, "ShowGuidelines");
	exGuideShow->AddChild(new wxXmlNode(wxXML_TEXT_NODE, "", wxString::Format(wxT("%i"), (int)showGuidelines)));

	// Write guideline color
	wxXmlNode * exGuideColor = new wxXmlNode(sessionInfo, wxXML_ELEMENT_NODE, "GuidelineColor");
	exGuideColor->AddChild(new wxXmlNode(wxXML_TEXT_NODE, "", guideColor.GetAsString()));

	// Write grid color 1
	wxXmlNode * exGridColor1 = new wxXmlNode(sessionInfo, wxXML_ELEMENT_NODE, "CropGridColor1");
	exGridColor1->AddChild(new wxXmlNode(wxXML_TEXT_NODE, "", gridColor1.GetAsString()));

	// Write grid color 2
	wxXmlNode * exGridColor2 = new wxXmlNode(sessionInfo, wxXML_ELEMENT_NODE, "CropGridColor2");
	exGridColor2->AddChild(new wxXmlNode(wxXML_TEXT_NODE, "", gridColor2.GetAsString()));

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

	wxFileName sessionFile(filePath);
	wxFileName sessionDir(sessionFile.GetPath());

	if(!sessionDir.IsDir()){
        Logger::Log("PhoediX PhoediXSession::SaveSessionToFile - Creating subdirectory for file", Logger::LogLevel::LOG_VERBOSE);
		wxMkDir(sessionFile.GetPath(), wxS_DIR_DEFAULT);
	}
    Logger::Log("PhoediX PhoediXSession::SaveSessionToFile - Saving session...", Logger::LogLevel::LOG_VERBOSE);
	session.Save(filePath);
    Logger::Log("PhoediX PhoediXSession::SaveSessionToFile - Session saved, returning", Logger::LogLevel::LOG_VERBOSE);
    
}

void PhoediXSession::SetProjectPath(wxString path) {
	projectPath = path;
}

wxString PhoediXSession::GetProjectPath() {
	return projectPath;
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

void PhoediXSession::SetFitImage(bool imageFit) {
	fitImage = imageFit;
}

bool PhoediXSession::GetFitImage(){
	return fitImage;
}

void PhoediXSession::SetFastProcess(bool fastProcess) {
	processFast = fastProcess;
}

bool PhoediXSession::GetFastProcess() {
	return processFast;
}

wxString PhoediXSession::GetExportFolder() {
	return exportFolder;
}

void PhoediXSession::SetExportFolder(wxString exFolder) {
	exportFolder = exFolder;
}

wxString PhoediXSession::GetExportName() {
	return exportName;
}

void PhoediXSession::SetExportName(wxString exName) {
	exportName = exName;
}

int PhoediXSession::GetExportImageType() {
	return exportImageType;
}

void PhoediXSession::SetExportImageType(int type) {
	exportImageType = type;
}

int PhoediXSession::GetExportJPEGQuality() {
	return exportJPEGQuality;
}

void PhoediXSession::SetExportJPEGQuality(int quality) {
	exportJPEGQuality = quality;
}

wxColour PhoediXSession::GetGuidelineColor() {
	return guideColor;
}

void PhoediXSession::SetGuidelineColor(wxColour color) {
	guideColor = color;
}

wxColour PhoediXSession::GetCropgridColor1() {
	return gridColor1;
}

void PhoediXSession::SetCropgridColor1(wxColour color) {
	gridColor1 = color;
}

wxColour PhoediXSession::GetCropgridColor2() {
	return gridColor2;
}

void PhoediXSession::SetCropgridColor2(wxColour color) {
	gridColor2 = color;
}

int PhoediXSession::GetGuidelineType() {
	return guideType;
}

void PhoediXSession::SetGuidelineType(int type) {
	guideType = type;
}

bool PhoediXSession::GetGuidelinesShown() {
	return showGuidelines;
}
void PhoediXSession::SetGuidelinesShown(bool guidelinesShown) {
	showGuidelines = guidelinesShown;
}

void PhoediXSession::GenerateID() {
	#if defined(__WXMSW__) || defined(__WXGTK__)
		id = rand() % 20000 + 500;
	#endif
    
	#if defined(__WXMAC__)
		FILE * fp = fopen("/dev/random", "r");
		if(!fp){ return;}
		for(int i = 0; i < sizeof(id); i++){ id <<= 8; id |= fgetc(fp); }
		fclose(fp);

		if(id < 1) { id *= -1;}
		id = id % 20000 + 500;
	#endif
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

bool PhoediXSession::CompareSessions(PhoediXSession * sessionOne, PhoediXSession * sessionTwo) {

	if (sessionOne->GetName() != sessionTwo->GetName()) { return false; }

	wxVector<ProcessorEdit*> editListOne = sessionOne->GetEditList()->GetSessionEditList();
	wxVector<ProcessorEdit*> editListTwo = sessionTwo->GetEditList()->GetSessionEditList();

	if (editListOne.size() != editListTwo.size()) { return false; }

	// Compare edit lists
	for (size_t i = 0; i < editListOne.size(); i++) {
		ProcessorEdit * editOne = editListOne.at(i);
		ProcessorEdit * editTwo = editListTwo.at(i);
		
		if (ProcessorEdit::CompareProcessorEdits(editOne, editTwo) == false) { return false; }
	}

	// Compare Snapshots
	if (sessionOne->GetSnapshots().size() != sessionTwo->GetSnapshots().size()) { return false; }

	wxVector<Snapshot*> snapListOne = sessionOne->GetSnapshots();
	wxVector<Snapshot*> snapListTwo = sessionTwo->GetSnapshots();

	for (size_t i = 0; i < snapListOne.size(); i++) {

		// Snapshot attributes
		if (snapListOne.at(i)->snapshotIndex != snapListTwo.at(i)->snapshotIndex) { return false; }
		if (snapListOne.at(i)->snapshotName != snapListTwo.at(i)->snapshotName) { return false; }

		// Compare snapshot edit list
		wxVector<ProcessorEdit*> snapEditListOne = snapListOne.at(i)->editList;
		wxVector<ProcessorEdit*> snapEditListTwo = snapListTwo.at(i)->editList;
		if (snapEditListOne.size() != snapEditListTwo.size()) { return false; }
		
		// Compare edit lists
		for (size_t j = 0; j < snapEditListOne.size(); j++) {
			ProcessorEdit * editOne = snapEditListOne.at(j);
			ProcessorEdit * editTwo = snapEditListTwo.at(j);

			if (ProcessorEdit::CompareProcessorEdits(editOne, editTwo) == false) { return false; }
		}
	}
	return true;
}

bool PhoediXSession::CheckIfSession(wxString filePath) {

	// Load the project, return if it fails
	wxXmlDocument session;
	{
		// Disable error dialog
		wxLogNull logNo;
		if (!session.Load(filePath)) { return false; }
	}
	
	// Verify root node
	if (session.GetRoot()->GetName() != "PhoediXProject") { return false; }

	return true;
}

wxString PhoediXSession::GetImagePathFromProjectPath(wxString projectFile) {
	// Get project name and path
	wxString projectName = wxFileName(projectFile).GetName();
	wxString projectPath = wxFileName(projectFile).GetPath();

	// Get image path from project path and name (image is one directory above)
	wxString imagePath = projectPath + wxFileName::GetPathSeparator() + "..";
	wxString fullImagePath = imagePath + wxFileName::GetPathSeparator() + projectName;
	return fullImagePath;
}