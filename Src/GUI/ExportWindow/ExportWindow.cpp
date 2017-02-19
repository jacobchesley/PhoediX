// Copyright 2016 Jacob Chesley

#include "ExportWindow.h"

wxDEFINE_EVENT(UPDATE_EXPORT_PROGRESS_NUM_EVENT, wxCommandEvent);
wxDEFINE_EVENT(UPDATE_EXPORT_PROGRESS_MSG_EVENT, wxCommandEvent);
wxDEFINE_EVENT(UPDATE_EXPORT_PROGRESS_CLOSE, wxCommandEvent);

ExportWindow::ExportWindow(wxWindow * parent, Processor * processor, EditListPanel * panel) : wxScrolledWindow(parent){

	editList = panel;
	proc = processor;

	this->SetBackgroundColour(parent->GetBackgroundColour());

	mainSizer = new wxFlexGridSizer(3, 15, 15);
	this->SetSizer(mainSizer);

	fileTypeLabel = new wxStaticText(this, -1, "Image Type");
	fileTypeLabel->SetForegroundColour(Colors::TextLightGrey);
	fileTypeControl = new wxComboBox(this, -1);
	fileTypeControl->AppendString("JPEG");
	fileTypeControl->AppendString("PNG");
	fileTypeControl->AppendString("TIFF 8 Bit");
	fileTypeControl->AppendString("TIFF 16 Bit");
	fileTypeControl->AppendString("BMP");
	fileTypeControl->SetSelection(0);

	fileTypeControl->SetBackgroundColour(Colors::BackDarkDarkGrey);
	fileTypeControl->SetForegroundColour(Colors::TextLightGrey);

	this->GetSizer()->Add(fileTypeLabel);
	this->GetSizer()->Add(fileTypeControl);
	this->GetSizer()->Add(new wxPanel(this));

	jpegQualityLabel = new wxStaticText(this, -1, "JPEG Quality");
	jpegQualityLabel->SetForegroundColour(Colors::TextLightGrey);
	jpegQualitySlide = new DoubleSlider(this, 90.0, 0.0, 100.0, 100, 0);
	jpegQualitySlide->SetValuePosition(DoubleSlider::VALUE_INLINE_RIGHT);

	jpegQualitySlide->SetBackgroundColour(this->GetBackgroundColour());
	jpegQualitySlide->SetForegroundColour(Colors::TextLightGrey);
	this->GetSizer()->Add(jpegQualityLabel);
	this->GetSizer()->Add(jpegQualitySlide);
	this->GetSizer()->Add(new wxPanel(this));

	outputFolderLabel = new wxStaticText(this, -1, "Export Folder");
	outputFolderLabel->SetForegroundColour(Colors::TextLightGrey);
	outputFolderText = new wxTextCtrl(this, -1);
	outputFolderText->SetBackgroundColour(this->GetBackgroundColour());
	outputFolderText->SetForegroundColour(Colors::TextLightGrey);

	outputFolderButton = new wxButton(this, ExportWindow::ExportActions::ID_BROWSE, "Browse", wxDefaultPosition, wxDefaultSize, wxBORDER_NONE);
	outputFolderButton->SetForegroundColour(Colors::TextLightGrey);
	outputFolderButton->SetBackgroundColour(Colors::BackGrey);
	this->GetSizer()->Add(outputFolderLabel);
	this->GetSizer()->Add(outputFolderText,0, wxEXPAND);
	this->GetSizer()->Add(outputFolderButton);

	outputNameLabel = new wxStaticText(this, -1, "Export Image Name");
	outputNameLabel->SetForegroundColour(Colors::TextLightGrey);
	outputNameText = new wxTextCtrl(this, -1);
	outputNameText->SetBackgroundColour(this->GetBackgroundColour());
	outputNameText->SetForegroundColour(Colors::TextLightGrey);
	this->GetSizer()->Add(outputNameLabel);
	this->GetSizer()->Add(outputNameText,0, wxEXPAND);
	this->GetSizer()->Add(new wxPanel(this));

	exportButton = new wxButton(this, ExportWindow::ExportActions::ID_EXPORT, "Export", wxDefaultPosition, wxDefaultSize, wxBORDER_NONE);
	exportButton->SetForegroundColour(Colors::TextLightGrey);
	exportButton->SetBackgroundColour(Colors::BackGrey);
	exportButton->SetFont(wxFont(10, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL));

	this->GetSizer()->Add(exportButton);
	this->GetSizer()->Add(new wxPanel(this));
	this->GetSizer()->Add(new wxPanel(this));

	this->FitInside();
	this->SetScrollRate(5, 5);
	this->SetClientSize(this->GetVirtualSize());

	this->Bind(wxEVT_BUTTON, (wxObjectEventFunction)&ExportWindow::OnBrowse, this, ExportWindow::ExportActions::ID_BROWSE);
	this->Bind(wxEVT_BUTTON, (wxObjectEventFunction)&ExportWindow::OnExport, this, ExportWindow::ExportActions::ID_EXPORT);
	this->Bind(wxEVT_COMBOBOX, (wxObjectEventFunction)&ExportWindow::OnCombo, this);
	this->Bind(UPDATE_EXPORT_PROGRESS_MSG_EVENT, (wxObjectEventFunction)&ExportWindow::SetProgressEditMsg, this, ID_UPDATE_EXPORT_PROGRESS_MSG_EVENT);
	this->Bind(UPDATE_EXPORT_PROGRESS_NUM_EVENT, (wxObjectEventFunction)&ExportWindow::SetProgressEditNum, this, ID_UPDATE_EXPORT_PROGRESS_NUM_EVENT);
	this->Bind(UPDATE_EXPORT_PROGRESS_CLOSE, (wxObjectEventFunction)&ExportWindow::CloseProgress, this, ID_UPDATE_EXPORT_PROGRESS_CLOSE);

	progress = NULL;
	exportStarted = false;
	rawImageLoaded = false;
	progressSize = 0;

}

void ExportWindow::RawImageLoaded(bool isRawImageLoaded){
	rawImageLoaded = isRawImageLoaded;
}

void ExportWindow::OnCombo(wxCommandEvent & WXUNUSED(event)){

	if(fileTypeControl->GetSelection() == 0){
		jpegQualityLabel->Show();
		jpegQualitySlide->Show();
	}
	else{
		jpegQualityLabel->Hide();
		jpegQualitySlide->Hide();
	}
	this->Layout();
}

void ExportWindow::OnBrowse(wxCommandEvent & WXUNUSED(event)){

	// Browse for directory to store export in
	wxDirDialog openDirDialog(this, "Export Folder");
	if (openDirDialog.ShowModal() == wxID_CANCEL) {
		return;
	}
	outputFolderText->SetValue(openDirDialog.GetPath());
}

void ExportWindow::OnExport(wxCommandEvent & WXUNUSED(event)){

	exportStarted = true;

	// Process both raw image (if it has one) and image itself
	editList->ReprocessImageRaw();
	progressSize = proc->GetLastNumEdits() + 1;
	progress = new wxGenericProgressDialog("Export Image Progress", "", progressSize, this, wxPD_SMOOTH | wxPD_APP_MODAL);
	Icons icons;
	wxIcon theIcon;
	theIcon.CopyFromBitmap(wxBitmap(icons.pxIcon));
	progress->SetIcon(theIcon);
}

void ExportWindow::ProcessingComplete(){

	if(exportStarted){
		
		// Make sure progress bar is including saving image step
		//this->SetEditNum(proc->GetLastNumEdits() + 1);
		this->SetMessage("Saving Image");
		wxString extension = "";
		int fileType = ImageHandler::SaveType::JPEG;
		if(fileTypeControl->GetSelection() == 0){ fileType = ImageHandler::SaveType::JPEG; extension = ".jpg"; }
		else if(fileTypeControl->GetSelection() == 1){ fileType = ImageHandler::SaveType::PNG; extension = ".png"; }
		else if(fileTypeControl->GetSelection() == 2){ fileType = ImageHandler::SaveType::TIFF8; extension = ".tif"; }
		else if(fileTypeControl->GetSelection() == 3){ fileType = ImageHandler::SaveType::TIFF16; extension = ".tif"; }
		else if(fileTypeControl->GetSelection() == 4){ fileType = ImageHandler::SaveType::BMP; extension = ".bmp"; }
		else{}
		int jpegQual = (int)jpegQualitySlide->GetValue();

		wxString fileName = outputFolderText->GetValue() + wxFileName::GetPathSeparator() + outputNameText->GetValue() + extension;

		// Save the image
		ImageHandler::SaveImageToFile(fileName, proc->GetImage(), fileType, jpegQual);

		// Destrory progress dialog
		wxCommandEvent closeEvt(UPDATE_EXPORT_PROGRESS_CLOSE, ID_UPDATE_EXPORT_PROGRESS_CLOSE);
		wxPostEvent(this, closeEvt);
	}
}

void ExportWindow::SetProgressEditMsg(wxCommandEvent & evt){

	// Not right event type, return
	if(evt.GetEventType() != UPDATE_EXPORT_PROGRESS_MSG_EVENT){
		return;
	}

	// Update if we can
	if(exportStarted && progress != NULL){
		progress->Update(progress->GetValue(), evt.GetString());
	}
}

void ExportWindow::SetProgressEditNum(wxCommandEvent & evt){
	
	// Not right event type, return
	if(evt.GetEventType() != UPDATE_EXPORT_PROGRESS_NUM_EVENT){
		return;
	}

	// If we cant enter, we are in the middle of closing the progress bar
	if (!locker.TryEnter()) {
		return;
	}

	if(exportStarted && progress != NULL){
		progress->Update(evt.GetInt(), progress->GetMessage());
	}
}

void ExportWindow::CloseProgress(wxCommandEvent& WXUNUSED(evt)){

	// Enter locker to prevent other methods from access our amost destoryed progress dialog
	locker.Enter();
	
	// Destroy
	if(exportStarted && progress != NULL){
		exportStarted = false;
		progress->Destroy();
		progress->Close();
		wxSafeYield();
		progress = NULL;
	}

	// Leave locker
	locker.Leave();
}

void ExportWindow::SetEditNum(int editNum){

	// If we cant enter, we are in the middle of closing the progress bar
	if(!locker.TryEnter()){
		return;
	}

	// Fire event with int to update to
	wxCommandEvent evt(UPDATE_EXPORT_PROGRESS_NUM_EVENT, ID_UPDATE_EXPORT_PROGRESS_NUM_EVENT);
	evt.SetInt(editNum);
	wxPostEvent(this, evt);
}

void ExportWindow::SetMessage(wxString message){

	// If we cant enter, we are in the middle of closing the progress bar
	if(!locker.TryEnter()){
		return;
	}

	// Fire event with string to update message to
	wxCommandEvent evt(UPDATE_EXPORT_PROGRESS_MSG_EVENT, ID_UPDATE_EXPORT_PROGRESS_MSG_EVENT);
	evt.SetString(message);
	wxPostEvent(this, evt);
}