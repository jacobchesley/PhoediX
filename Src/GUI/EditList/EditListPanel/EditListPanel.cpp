// Copyright 2018 Jacob Chesley.
// See LICENSE.TXT in root of project for license information.

#include "EditListPanel.h"
#include "Debugging/Logger/Logger.h"

wxDEFINE_EVENT(START_EDITS_COMPLETE, wxThreadEvent);

EditListPanel::EditListPanel(wxWindow * parent, Processor * processor, ZoomImagePanel * imgPanel) : wxPanel(parent) {

	par = parent;
	currentSession = NULL;
	mainSizer = new wxBoxSizer(wxVERTICAL);
	this->SetSizer(mainSizer);
	this->SetBackgroundColour(Colors::BackDarkGrey);
	
	titleText = new wxStaticText(this, -1, "Edit Pipeline", wxDefaultPosition, wxDefaultSize, wxALIGN_CENTER_HORIZONTAL);
	titleText->SetForegroundColour(Colors::TextLightGrey);
	titleText->SetFont(wxFont(14, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD));
	titleText->SetMinSize(titleText->GetTextExtent(titleText->GetLabel()) + wxSize(0, 5));

	scroller = new EditListScroll(this);
	scroller->Connect(wxEVT_RIGHT_DOWN, wxMouseEventHandler(EditListPanel::OnRightClick), NULL, this);

	Icons icons;
	addEditButton = new PhoediXButton(this, EditListPanel::Buttons::ADD_EDIT_BUTTON, "Add Edit");
	addEditButton->SetForegroundColour(Colors::TextLightGrey);
	addEditButton->SetBackgroundColour(Colors::BackGrey);
	addEditButton->SetFont(wxFont(11, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL));

	editSelection = new EditSelection(this);
	wxAuiPaneInfo editSelectionInfo = wxAuiPaneInfo();
	editSelectionInfo.Float();
	editSelectionInfo.Caption("Edit Selection");
	editSelectionInfo.Name("EditSelection");
	editSelectionInfo.CloseButton(true);
	editSelectionInfo.PinButton(true);
	editSelectionInfo.DestroyOnClose(false);
	editSelectionInfo.Hide();
	editSelectionInfo.BestSize(editSelection->GetVirtualSize());

	PhoedixAUIManager::GetPhoedixAUIManager()->AddPane(editSelection, editSelectionInfo);
	PhoedixAUIManager::GetPhoedixAUIManager()->Update();

	this->GetSizer()->AddSpacer(5);
	this->GetSizer()->Add(titleText, 0, wxEXPAND);
	this->GetSizer()->AddSpacer(20);
	this->GetSizer()->Add(scroller, 1, wxEXPAND);
	this->GetSizer()->Add(addEditButton, 0, wxALIGN_CENTER);
	this->GetSizer()->AddSpacer(5);
	this->Fit();

	this->Bind(wxEVT_BUTTON, (wxObjectEventFunction)&EditListPanel::OnAddEdit, this, EditListPanel::Buttons::ADD_EDIT_BUTTON);
	this->Bind(EDIT_UP_EVENT, (wxObjectEventFunction)&EditListPanel::MoveEditUp, this, ID_EDIT_UP);
	this->Bind(EDIT_DOWN_EVENT, (wxObjectEventFunction)&EditListPanel::MoveEditDown, this, ID_EDIT_DOWN);
	this->Bind(EDIT_DELETE_EVENT, (wxObjectEventFunction)&EditListPanel::DeleteEdit, this, ID_EDIT_DELETE);
	this->Bind(EDIT_DISABLE_EVENT, (wxObjectEventFunction)&EditListPanel::DisableEdit, this, ID_EDIT_DISABLE);
	this->Bind(EDIT_ADD_EVENT, (wxObjectEventFunction)&EditListPanel::AddEditToPanel, this, ID_EDIT_ADD);
	this->Bind(EDIT_COPY_EVENT, (wxObjectEventFunction)&EditListPanel::CopyEdit, this, ID_EDIT_COPY);
	this->Bind(EDIT_PASTE_EVENT, (wxObjectEventFunction)&EditListPanel::PasteEdit, this, ID_EDIT_PASTE);
	this->Bind(REPROCESS_IMAGE_EVENT, (wxObjectEventFunction)&EditListPanel::ReprocessImageEvt, this, ID_REPROCESS_IMAGE);
	this->Bind(REPROCESS_IMAGE_RAW_EVENT, (wxObjectEventFunction)&EditListPanel::ReprocessImageRawEvt, this, ID_REPROCESS_IMAGE_RAW);
	this->Bind(REPROCESS_IMAGE_RAW_EVENT, (wxObjectEventFunction)&EditListPanel::ReprocessUnpackImageRawEvt, this, ID_REPROCESS_UNPACK_IMAGE_RAW);
	this->Bind(wxEVT_RIGHT_DOWN, (wxMouseEventFunction)&EditListPanel::OnRightClick, this);
	this->Bind(START_EDITS_COMPLETE, (wxObjectEventFunction)&EditListPanel::StartEditsComplete, this, ID_START_EDITS_COMPLETE);
	this->Bind(SAVE_NO_REPROCESS_EVENT, (wxObjectEventFunction)&EditListPanel::OnSaveNoReprocess, this, ID_SAVE_NO_REPROCESS);
	this->Bind(EDIT_DRAG_EVENT, (wxObjectEventFunction)& EditListPanel::OnEditDrag, this, ID_EDIT_DRAG);
	this->Bind(EDIT_DRAG_COMPLETE_EVENT, (wxObjectEventFunction)& EditListPanel::OnEditDragComplete, this, ID_EDIT_DRAG_COMPLETE);

	proc = processor;
	hasRaw = false;
	startEdits = NULL;
	editsStarted = false;
	imagePanel = imgPanel;
	preventProcessing = false;

	this->InitializeEdits();
}

void EditListPanel::InitializeEdits() {
	
	AvailableEdits available;
	wxVector<Edit> allEdits = available.GetAvailableEdits();

	for (size_t i = 0; i < allEdits.size(); i++) {

		wxString editName = allEdits.at(i).GetName();
		wxString editDescription = allEdits.at(i).GetDescription();
		editSelection->AddEditSelection(editName, editDescription);
	}
	editSelection->FitEdits();
}

void EditListPanel::OnRightClick(wxMouseEvent& WXUNUSED(event)){

	// Display a popup menu of options
	wxMenu popupMenu;
	popupMenu.Append(EditListPanel::PopupMenuActions::COPY_EDIT_LIST, "Copy Edit List");
	popupMenu.Append(EditListPanel::PopupMenuActions::PASTE_EDIT_LIST, "Paste Edit List");
	popupMenu.AppendSeparator();
	popupMenu.Append(EditListPanel::PopupMenuActions::ENABLE_ALL_EDITS, "Enable All Edits");
	popupMenu.Append(EditListPanel::PopupMenuActions::DISABLE_ALL_EDITS, "Disable All Edits");
	popupMenu.AppendSeparator();
	popupMenu.Append(EditListPanel::PopupMenuActions::DELETE_ALL_EDITS, "Delete All Edits");

	// No copied edit list, disable paste action
	if(proc->GetEditListForCopyPaste().size() <= 0){ popupMenu.GetMenuItems()[1]->Enable(false); }
	
	// No edits that can be enabled/disabled, disable enable/disable all action
	if((scroller->GetEditList().size() - scroller->GetNumTopEdits()) <= 0){
		popupMenu.GetMenuItems()[3]->Enable(false);
		popupMenu.GetMenuItems()[4]->Enable(false);
	}

	popupMenu.Connect(wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(EditListPanel::OnPopupMenuClick), NULL, this);
	this->PopupMenu(&popupMenu);
}

void EditListPanel::OnPopupMenuClick(wxCommandEvent& inEvt) {

	int id = inEvt.GetId();

	switch(id){

		// Copy the edit parameters
		case EditListPanel::PopupMenuActions::COPY_EDIT_LIST:{
			this->AddEditsToProcessor();
			proc->StoreEditListForCopyPaste(proc->GetEditVector());
			break;
		}

		// Paste stored edit parameters														
		case EditListPanel::PopupMenuActions::PASTE_EDIT_LIST:{
			this->AddEditWindows(proc->GetEditListForCopyPaste());
			break;
		}

		// Enable all edits
		case EditListPanel::PopupMenuActions::ENABLE_ALL_EDITS:{
			this->EnableAllEdits();
			break;
		}

		case EditListPanel::PopupMenuActions::DISABLE_ALL_EDITS:{
			this->DisableAllEdits();
			break;
		}
		case EditListPanel::PopupMenuActions::DELETE_ALL_EDITS: {
			this->RemoveAllWindows(false);
			this->ReprocessImage();
			break;
		}
	}
}

void EditListPanel::ReprocessImageEvt(wxCommandEvent& WXUNUSED(event)) {
	if (preventProcessing) { return; }
	this->ReprocessImage();
	this->InformParentReprocess();
}

void EditListPanel::ReprocessImageRawEvt(wxCommandEvent& WXUNUSED(event)) {
	if (preventProcessing) { return; }
	// Reprocess raw with no unpacking
	this->ReprocessImageRaw(false);
	this->InformParentReprocess();
}

void EditListPanel::ReprocessUnpackImageRawEvt(wxCommandEvent& WXUNUSED(event)) {
	if (preventProcessing) { return; }
	// Reprocess raw with unpack
	this->ReprocessImageRaw(true);
	this->InformParentReprocess();
}

void EditListPanel::SetSession(PhoediXSession * newSession) {
	currentSession = newSession;
}

void EditListPanel::InformParentReprocess(){
	if (preventProcessing) { return; }
	wxCommandEvent evt(REPROCESS_IMAGE_EVENT, ID_REPROCESS_IMAGE);
	wxPostEvent(PhoedixAUIManager::GetMainWindow(), evt);
}

void EditListPanel::OnSaveNoReprocess(wxMouseEvent& WXUNUSED(event)) {
	if (preventProcessing) { return; }
	this->SaveNoReprocess();
}

void EditListPanel::SaveNoReprocess() {
	if (preventProcessing) { return; }
	if (currentSession != NULL) {
		this->AddEditsToProcessor();
		currentSession->SetPerspective(PhoedixAUIManager::GetPhoedixAUIManager()->SavePerspective());
		currentSession->GetEditList()->SetSessionEditList(proc->GetEditVector());
		currentSession->SaveSessionToFile(currentSession->GetProjectPath());
	}
}

void EditListPanel::SaveNoReprocessWithSnapshots(wxVector<Snapshot*> snapshots) {
	if (preventProcessing) { return; }
	if (currentSession != NULL) {
		this->AddEditsToProcessor();
		currentSession->SetPerspective(PhoedixAUIManager::GetPhoedixAUIManager()->SavePerspective());
		currentSession->GetEditList()->SetSessionEditList(proc->GetEditVector());
		currentSession->SetSnapshots(snapshots);
		currentSession->SaveSessionToFile(currentSession->GetProjectPath());
	}
}

void EditListPanel::AddEditsToProcessor() {
	if (preventProcessing) { return; }
	// Delete the current list of internally stored edits in the processor
	proc->DeleteEdits();

	// Create a vector of Edit list Items that are displayed on the panel
	wxVector<EditListItem*> editList = scroller->GetEditList();
	int idx = 0;

	for (size_t i = 0; i < editList.size(); i++) {

		// Add each edit from the edit window, to the processor
		if (editList.at(i)->GetEditWindow() != NULL) {

			int editType = editList.at(i)->GetEditWindow()->GetEditType();
			if (editType != ProcessorEdit::EditType::RAW) {
				editList.at(i)->GetEditWindow()->SetIndex(idx);
				idx += 1;
			}
			editList.at(i)->GetEditWindow()->SetDisabled(editList.at(i)->GetDisabled());
			editList.at(i)->GetEditWindow()->AddEditToProcessor();
		}
	}
}

void EditListPanel::ReprocessImage() {
	if (preventProcessing) { return; }
	// Delete the current list of internally stored edits in the processor
	proc->DeleteEdits();

	if (editsStarted) {
		startEdits->Stop();
	}

	// Add all edits to processor
	this->AddEditsToProcessor();

	proc->KillCurrentProcessing();
	
	if (!editsStarted) {
		// Start edit thread (with raw disabled)
		startEdits = new StartEditsThread(this, proc, false);
		startEdits->Run();
	}
	this->InformParentReprocess();
}

void EditListPanel::ReprocessImageRaw(bool unpack) {
	if (preventProcessing) { return; }
	// Delete the current list of internally stored edits in the processor
	proc->DeleteEdits();

	// Process Raw if needed
	if (hasRaw) {

		// Stop current edits taking place
		if (editsStarted) {
			startEdits->Stop();
		}

		// If there are edits, tell raw processor to not set processor to updated 
		// (still need to perform edits after raw processing).
		if(scroller->GetEditList().size() > 1){
			proc->SetHasEdits(true);
		}
		else{
			proc->SetHasEdits(false);
		}

		proc->KillRawProcessing();
		proc->KillCurrentProcessing();

		if (!editsStarted) {
			// Start edit thread (with raw enabled)
			startEdits = new StartEditsThread(this, proc, true, unpack);
			startEdits->Run();
		}
	}
	else {
		this->ReprocessImage();
	}
	this->InformParentReprocess();
}

void EditListPanel::StartEditsComplete() {
	editsStarted = false;
}

void EditListPanel::OnAddEdit(wxCommandEvent& WXUNUSED(event)) {
	PhoedixAUIManager::GetPhoedixAUIManager()->GetPane(editSelection).Show();
	PhoedixAUIManager::GetPhoedixAUIManager()->Update();
}

void EditListPanel::HideEditSelectionWindow() {
	PhoedixAUIManager::GetPhoedixAUIManager()->GetPane(editSelection).Hide();
	PhoedixAUIManager::GetPhoedixAUIManager()->Update();
}

void EditListPanel::AddEditToPanel(wxCommandEvent& addEvt) {

	// Get the edit ID to add to the panel
	int editID = addEvt.GetInt();

	// Create new edit window and add it to panel
	EditWindow * newEditWindow = AvailableEditWindows::GetEditWindow(editID, this, proc, imagePanel);
	this->AddEditWindowToPanel(newEditWindow, editID, false, true);

	// Reprocess the image with the new edit added
	this->ReprocessImage();
}

void EditListPanel::AddEditWindows(wxVector<ProcessorEdit*> inEdits) {
	
	preventProcessing = true;
    Logger::Log("PhoediX EditListPanel::AddEditWindows - Called", Logger::LogLevel::LOG_VERBOSE);
	this->Freeze();
    
    Logger::Log("PhoediX EditListPanel::AddEditWindows - Removing all windows", Logger::LogLevel::LOG_VERBOSE);
	this->RemoveAllWindows();
	proc->DeleteEdits();
	Logger::Log("PhoediX EditListPanel::AddEditWindows - Windows removed", Logger::LogLevel::LOG_VERBOSE);
	for (size_t i = 0; i < inEdits.size(); i++) {
        
		if (inEdits.at(i) != NULL) {

			// Add a raw window properly
			if(inEdits.at(i)->GetEditType() == ProcessorEdit::EditType::RAW){
                
                Logger::Log("PhoediX EditListPanel::AddEditWindows - Adding RAW Window", Logger::LogLevel::LOG_VERBOSE);
				this->AddRawWindow(inEdits.at(i));
                Logger::Log("PhoediX EditListPanel::AddEditWindows - RAW window added", Logger::LogLevel::LOG_VERBOSE);
			}

			// Add the correct type of window, and set parameter for window based on edit
			else{
                
                Logger::Log("PhoediX EditListPanel::AddEditWindows - Adding edit window", Logger::LogLevel::LOG_VERBOSE);
				EditWindow * newEditWindow = AvailableEditWindows::GetEditWindow(inEdits.at(i), this, proc, imagePanel);
				if (newEditWindow != NULL) {
                    
                    Logger::Log("PhoediX EditListPanel::AddEditWindows - Edit window exists, adding edit item to panel", Logger::LogLevel::LOG_VERBOSE);
					this->AddEditWindowToPanel(newEditWindow, AvailableEditWindows::GetEditIDFromEdit(inEdits.at(i)), inEdits.at(i)->GetDisabled(), false);
                    
                    Logger::Log("PhoediX EditListPanel::AddEditWindows - Setting edit windows disabled and params/flags", Logger::LogLevel::LOG_VERBOSE);
					newEditWindow->SetDisabled(inEdits.at(i)->GetDisabled());
					newEditWindow->SetParamsAndFlags(inEdits.at(i));
					this->Refresh();
					this->Update();
					wxYield();
				}
			}
		}
        Logger::Log("PhoediX EditListPanel::AddEditWindows - returning", Logger::LogLevel::LOG_VERBOSE);
	}

	for(size_t i = 0; i < scroller->GetEditList().size(); i++){
		scroller->GetEditList().at(i)->GetEditWindow()->Activate();
	}
	
	PhoedixAUIManager::GetPhoedixAUIManager()->Update();

	preventProcessing = false;
	this->Thaw();
}

void EditListPanel::RemoveAllWindows(bool removeRawWindow) {

    Logger::Log("PhoediX EditListPanel::RemoveAllWindows - Stopping processing", Logger::LogLevel::LOG_VERBOSE);
	proc->KillCurrentProcessing();
    
    Logger::Log("PhoediX EditListPanel::RemoveAllWindows - Removing RAW window (if it exists)", Logger::LogLevel::LOG_VERBOSE);

	if (removeRawWindow) {
		this->RemoveRawWindow();
		scroller->DeleteAllEdits();
		hasRaw = false;
	}
	else {
		scroller->DeleteNonTopEdits();
	}
    
    Logger::Log("PhoediX EditListPanel::RemoveAllWindows - Removing edits from edit list", Logger::LogLevel::LOG_VERBOSE);
    
    Logger::Log("PhoediX EditListPanel::RemoveAllWindows - Removing edits from processor", Logger::LogLevel::LOG_VERBOSE);
	proc->DeleteEdits();
}

void EditListPanel::AddEditWindowToPanel(EditWindow * window, int editID, bool disable, bool autoActivate) {

	// If a new edit window was created
	AvailableEdits available;

	if (window != NULL) {

		int id = scroller->GetNextID();
		wxString idStr;
		idStr << id;

		// Create the info to style the AUI Pane
		wxAuiPaneInfo editWindowInfo = wxAuiPaneInfo();
		editWindowInfo.DestroyOnClose(false);
		editWindowInfo.CloseButton(true);
		editWindowInfo.PinButton(true);
		editWindowInfo.Caption(window->GetName());
		editWindowInfo.Float();
		editWindowInfo.Show();
		editWindowInfo.Hide();
		editWindowInfo.BestSize(window->GetClientSize());
		editWindowInfo.Name(window->GetName() + idStr);

		// Add new edit window to AUI manager
		PhoedixAUIManager::GetPhoedixAUIManager()->AddPane(window, editWindowInfo);
		PhoedixAUIManager::GetPhoedixAUIManager()->GetPane(window).Show();	
		PhoedixAUIManager::GetPhoedixAUIManager()->Update();

		// Add a new Edit List Item to the Edit List scroll panel
		// Add special panel that will disable buttons if it is raw.  Keep raw panel at top
		if(editID == AvailableEditIDS::EDIT_ID_RAW){

			EditListItem * newEditListItem = new EditListItem(scroller, available.GetNameFromID(editID), id, window, true);
			scroller->AddEdit(newEditListItem);
			if(autoActivate){ newEditListItem->GetEditWindow()->Activate(); }
		}
		else{
			EditListItem * newEditListItem = new EditListItem(scroller, available.GetNameFromID(editID), id, window, false);
			newEditListItem->SetDisabled(disable);
			scroller->AddEdit(newEditListItem);
			if(autoActivate){ newEditListItem->GetEditWindow()->Activate(); }
		}
	}
}

void EditListPanel::MoveEditUp(wxCommandEvent& upEvt) {

	// Get edit number that is requesting to be moved up
	int editNum = upEvt.GetInt();
	scroller->Freeze();
	scroller->MoveEditUp(editNum);
	scroller->Thaw();
	this->ReprocessImage();
}

void EditListPanel::MoveEditDown(wxCommandEvent& downEvt) {

	// Get edit number that is requesting to be moved down
	int editNum = downEvt.GetInt();
	scroller->Freeze();
	scroller->MoveEditDown(editNum);
	scroller->Thaw();
	this->ReprocessImage();
}

void EditListPanel::DeleteEdit(wxCommandEvent& deleteEvt) {

	// Get edit number that is requesting to be deleted
	int editNum = deleteEvt.GetInt();
	scroller->Freeze();
	scroller->DeleteEdit(editNum);
	scroller->Thaw();
	this->ReprocessImage();
}

void EditListPanel::CopyEdit(wxCommandEvent& copyEvt) {

	// Get edit number that is requesting to be copied
	size_t editNum = (size_t) copyEvt.GetInt();

	// Cant copy edit that does not exist in list
	if(editNum > scroller->GetEditList().size() - 1 || editNum < 0){
		return;
	}

	// Grab the params and flags of the edit and store it in the clipboard of the processor
	ProcessorEdit * editToCopy = scroller->GetEditList()[editNum]->GetEditWindow()->GetParamsAndFlags();
	proc->StoreEditForCopyPaste(editToCopy);
}

void EditListPanel::PasteEdit(wxCommandEvent& pasteEvt) {

	// Get edit number that is requesting to be pasted
	size_t editNum = (size_t)pasteEvt.GetInt();

	// Cant paste edit that does not exist in list
	if(editNum > scroller->GetEditList().size() - 1 || editNum < 0){
		return;
	}

	// Grab the params and flags of the edit in the processor and paste in to the edit window
	ProcessorEdit * copiedEdit = proc->GetEditForCopyPaste();
	scroller->GetEditList()[editNum]->GetEditWindow()->SetParamsAndFlags(copiedEdit);
	this->ReprocessImage();
}

void EditListPanel::EnableAllEdits(){

	for(int i = 0; i < scroller->GetEditList().size(); i++){
		scroller->GetEditList().at(i)->SetDisabled(false);
	}
	this->ReprocessImage();
}

void EditListPanel::DisableAllEdits(){

	for(int i = 0; i < scroller->GetEditList().size(); i++){

		if(!(scroller->GetEditList().at(i)->GetEditWindow()->GetParamsAndFlags()->GetEditType() == ProcessorEdit::EditType::RAW)){
			scroller->GetEditList().at(i)->SetDisabled(true);
		}
	}
	this->ReprocessImage();
}

void EditListPanel::DisableEdit(wxCommandEvent& WXUNUSED(event)) {
	this->ReprocessImage();
}

void EditListPanel::AddRawWindow(){

	if (!proc->GetLockedRaw()) {
		#ifdef __WXMSW__
			proc->rawProcessor->open_file(proc->GetFilePath().wc_str());
		#else
			proc->rawProcessor->open_file(proc->GetFilePath().c_str());
		#endif
		proc->rawProcessor->unpack();
	}
	// Get number of edits already in list
	size_t numEdits = scroller->GetNextID();

	// Add new raw processor edit panel to list
	EditWindow * newEditWindow = AvailableEditWindows::GetEditWindow(AvailableEditIDS::EDIT_ID_RAW, this, proc, imagePanel);
	this->AddEditWindowToPanel(newEditWindow, AvailableEditIDS::EDIT_ID_RAW, false, true);

	//Move raw processor edit to top
	int rawEditIdx = numEdits;
	for(size_t i = 0; i < numEdits; i++){
		scroller->MoveEditUp(rawEditIdx);
		rawEditIdx -= 1;
	}

	// Mark raw processor top edits as permenant.  Cannot move any edits above this one.
	scroller->SetNumTopEdits(1);
	hasRaw = true;

	// Unpack and reprocess raw
	this->ReprocessImageRaw(true);
}

void EditListPanel::AddRawWindow(ProcessorEdit * editForParams){

	// RAW edit already exists.  Do not add again
	if (scroller->GetNumTopEdits() > 0) {return;}

	if (!proc->GetLockedRaw()) {
		#ifdef __WXMSW__
			proc->rawProcessor->open_file(proc->GetFilePath().wc_str());
		#else
			proc->rawProcessor->open_file(proc->GetFilePath().c_str());
		#endif
		proc->rawProcessor->unpack();
	}
	// Get number of edits already in list
	size_t numEdits = scroller->GetNextID();

	// Add new raw processor edit panel to list
	EditWindow * newEditWindow = AvailableEditWindows::GetEditWindow(AvailableEditIDS::EDIT_ID_RAW, this, proc, imagePanel);
	this->AddEditWindowToPanel(newEditWindow, AvailableEditIDS::EDIT_ID_RAW, false, true);
	newEditWindow->SetParamsAndFlags(editForParams, false);

	//Move raw processor edit to top
	int rawEditIdx = numEdits;
	for(size_t i = 0; i < numEdits; i++){
		scroller->MoveEditUp(rawEditIdx);
		rawEditIdx -= 1;
	}

	// Mark raw processor top edits as permenant.  Cannot move any edits above this one.
	scroller->SetNumTopEdits(1);
	hasRaw = true;
}

void EditListPanel::RemoveRawWindow() {

	if (scroller->GetNumTopEdits() == 1) {
		scroller->DeleteEdit(0);
		scroller->SetNumTopEdits(0);
		hasRaw = false;
	}
}

bool EditListPanel::GetRawWindowOpen(){
	return hasRaw;
}

void EditListPanel::OnEditDrag(wxCommandEvent& evt) {

	wxMouseState mouse(wxGetMouseState());

	for (int i = 0; i < scroller->GetEditList().size(); i++) {
		
		EditListItem* editItem = scroller->GetEditList().at(i);


		if (evt.GetInt() == editItem->GetId()) {
			highlightedItem = editItem;
		}
		
		wxRect editListItemRect = editItem->GetScreenRect();
		wxRect editListItemRectTop = editListItemRect;
		editListItemRectTop.SetHeight(editListItemRect.GetHeight() / 2);

		// Mouse is in this edit list item
		if (editListItemRect.Contains(mouse.GetX(), mouse.GetY())) {

			// Mouse is in same edit that is highlighted, hide cursor
			if (evt.GetInt() == editItem->GetId()) {
				scroller->SetEditDragCursor(-1);
				lastDragPos = -1;
			}

			// Handle case where top edit exists
			else if (scroller->GetNumTopEdits() > 0) {
				if (editItem->GetSequence() < scroller->GetNumTopEdits()) {
					continue;
				}
				else {
					// Mouse is in the top half, draw cursor above
					if (editListItemRectTop.Contains(mouse.GetX(), mouse.GetY())) {
						scroller->SetEditDragCursor(i);
						lastDragPos = i;
					}

					// Mouse is in the bottom half, draw cursor below
					else {
						scroller->SetEditDragCursor(i + 1);
						lastDragPos = i + 1;
					}
				}
			}
			else {
				// Mouse is in the top half, draw cursor above
				if (editListItemRectTop.Contains(mouse.GetX(), mouse.GetY())) {
					scroller->SetEditDragCursor(i);
					lastDragPos = i;
				}

				// Mouse is in the bottom half, draw cursor below
				else {
					scroller->SetEditDragCursor(i + 1);
					lastDragPos = i + 1;
				}
			}
		}
	}
}

void EditListPanel::OnEditDragComplete(wxCommandEvent& WXUNUSED(evt)) {
	if (lastDragPos > -1) {
		scroller->CompleteDrag(highlightedItem, lastDragPos);
	}
	lastDragPos = -1;
	this->ReprocessImage();
}

EditListPanel::EditListScroll::EditListScroll(wxWindow * parent) : wxScrolledWindow(parent) {

	parWindow = parent;
	sizer = new wxBoxSizer(wxVERTICAL);
	this->SetSizer(sizer);
	this->FitInside();
	this->SetScrollRate(5, 5);
	topEdits = 0;

	// Setup cursor for edit item dragging position
	dragCursor = new wxPanel(this);
	dragCursor->SetMinClientSize(wxSize(2, 2));
	this->GetSizer()->Add(dragCursor, 0, wxRIGHT | wxEXPAND);
	wxColor cursorColor = Colors::BackGrey;
	int red = cursorColor.Red();
	int green = cursorColor.Green();
	int blue = cursorColor.Blue();
	cursorColor.Set(red + 40, green + 40, blue + 40);
	dragCursor->SetBackgroundColour(cursorColor);
	dragCursor->Hide();
    dragCursorPos = -1;
}

void EditListPanel::EditListScroll::SetNumTopEdits(size_t numTop){
	topEdits = numTop;
}

size_t EditListPanel::EditListScroll::GetNumTopEdits(){
	return topEdits;
}

void EditListPanel::EditListScroll::AddEdit(EditListItem * edit) {

	// Add the edit to the sizer, and push onto the editList
	editList.push_back(edit);
	this->GetSizer()->AddSpacer(6);
	this->GetSizer()->Add(edit, 0, wxALL| wxEXPAND);
	this->GetSizer()->AddSpacer(6);

	// Fit inside the scroll bars
	this->FitInside();

	// Get the parents original size
	wxSize originalParSize = parWindow->GetClientSize();

	// Set the client size of parent and this to min size needed to not show scrollbars
	this->SetClientSize(this->GetVirtualSize());
	parWindow->SetClientSize(this->GetVirtualSize());

	// Set best size and min size of parent to min size needed to not show scrollbars
	PhoedixAUIManager::GetPhoedixAUIManager()->GetPane(parWindow).BestSize(this->GetVirtualSize());
	PhoedixAUIManager::GetPhoedixAUIManager()->GetPane(parWindow).MinSize(this->GetVirtualSize());

	// Make minimum size of parent the original min size.  This panel is still the size that it does not 
	// show scroll bars, but can get smaller now
	PhoedixAUIManager::GetPhoedixAUIManager()->GetPane(parWindow).MinSize(originalParSize);
	PhoedixAUIManager::GetPhoedixAUIManager()->Update();
	
	this->Refresh();
	this->Update();
}

void EditListPanel::EditListScroll::MoveEditUp(size_t index) {

	// We cant move an item further up if it is at the top
	if (index <= topEdits) {
		return;
	}

	// Remove all edit items from the sizer, but do not destroy them
	this->GetSizer()->Clear();

	// Get items needed to be moved
	EditListItem * toMoveUp = editList.at(index);
	EditListItem * toMoveDown = editList.at(index - 1);
	
	// Erase the items from the list
	editList.erase(editList.begin() + index);
	editList.erase(editList.begin() + index - 1);

	// Inset in new order
	editList.insert(editList.begin() + index - 1, toMoveUp);
	editList.insert(editList.begin() + index, toMoveDown);

	this->RedrawEdits();
}

void EditListPanel::EditListScroll::MoveEditDown(size_t index) {

	// We cant move an item further down if it is at the bottom
	if (index >= editList.size() - 1) {
		return;
	}

	// Remove all edit items from the sizer, but do not destroy them
	this->GetSizer()->Clear();

	// Get items needed to be moved
	EditListItem * toMoveUp = editList.at(index + 1);
	EditListItem * toMoveDown = editList.at(index);

	// Erase the items from the list
	editList.erase(editList.begin() + index + 1);
	editList.erase(editList.begin() + index);

	// Inset in new order
	editList.insert(editList.begin() + index, toMoveDown);
	editList.insert(editList.begin() + index, toMoveUp);

	this->RedrawEdits();
}

void EditListPanel::EditListScroll::DeleteEdit(size_t index) {

	// Remove all edit items from the sizer, but do not destroy them
	this->GetSizer()->Clear();

	// Remove the edit window from AUI Manager if it exists
	if (editList.at(index)->GetEditWindow() != NULL) {
		PhoedixAUIManager::GetPhoedixAUIManager()->DetachPane(editList.at(index)->GetEditWindow());
	}

	//Destroy and erase the one edit item we no longer want
	editList.at(index)->GetEditWindow()->DestroyEditWindow();
	editList.at(index)->DestroyItem();
	editList.erase(editList.begin() + index);
	PhoedixAUIManager::GetPhoedixAUIManager()->Update();

	this->RedrawEdits();
}

void EditListPanel::EditListScroll::DeleteAllEdits() {

	// Remove all edit items from the sizer, but do not destroy them
    
    Logger::Log("PhoediX EditListPanel::EditListScroll::DeleteAllEdits - Called", Logger::LogLevel::LOG_VERBOSE);
	this->GetSizer()->Clear();

	// Remove all edit windows from list
	for (size_t i = 0; i < editList.size(); i++){

        Logger::Log("PhoediX EditListPanel::EditListScroll::DeleteAllEdits - Removing edit", Logger::LogLevel::LOG_VERBOSE);
		// Remove the edit window from AUI Manager if it exists
		if (editList.at(i)->GetEditWindow() != NULL) {
            
            Logger::Log("PhoediX EditListPanel::EditListScroll::DeleteAllEdits - Detaching edit from AUI manager", Logger::LogLevel::LOG_VERBOSE);
			PhoedixAUIManager::GetPhoedixAUIManager()->DetachPane(editList.at(i)->GetEditWindow());
		}

        Logger::Log("PhoediX EditListPanel::EditListScroll::DeleteAllEdits - Destroying edit", Logger::LogLevel::LOG_VERBOSE);
		//Destroy and erase the one edit item we no longer want
		editList.at(i)->DestroyItem();
        
        Logger::Log("PhoediX EditListPanel::EditListScroll::DeleteAllEdits - Updating AUI Manager", Logger::LogLevel::LOG_VERBOSE);
		PhoedixAUIManager::GetPhoedixAUIManager()->Update();
	}

    Logger::Log("PhoediX EditListPanel::EditListScroll::DeleteAllEdits - Clearing edit list vector", Logger::LogLevel::LOG_VERBOSE);
	editList.clear();
    
    Logger::Log("PhoediX EditListPanel::EditListScroll::DeleteAllEdits - Redrawing edits", Logger::LogLevel::LOG_VERBOSE);
	this->RedrawEdits();
    Logger::Log("PhoediX EditListPanel::EditListScroll::DeleteAllEdits - Returning", Logger::LogLevel::LOG_VERBOSE);
}

void EditListPanel::EditListScroll::DeleteNonTopEdits() {

	if (editList.size() == this->GetNumTopEdits()) { return; }

	// Remove all edit items from the sizer, but do not destroy them
	Logger::Log("PhoediX EditListPanel::EditListScroll::DeleteNonTopEdits - Called", Logger::LogLevel::LOG_VERBOSE);
	this->GetSizer()->Clear();

	// Remove all edit windows from list
	for (size_t i = this->GetNumTopEdits(); i < editList.size(); i++) {

		Logger::Log("PhoediX EditListPanel::EditListScroll::DeleteNonTopEdits - Removing edit", Logger::LogLevel::LOG_VERBOSE);
		// Remove the edit window from AUI Manager if it exists
		if (editList.at(i)->GetEditWindow() != NULL) {

			Logger::Log("PhoediX EditListPanel::EditListScroll::DeleteNonTopEdits - Detaching edit from AUI manager", Logger::LogLevel::LOG_VERBOSE);
			PhoedixAUIManager::GetPhoedixAUIManager()->DetachPane(editList.at(i)->GetEditWindow());
		}

		Logger::Log("PhoediX EditListPanel::EditListScroll::DeleteNonTopEdits - Destroying edit", Logger::LogLevel::LOG_VERBOSE);
		//Destroy and erase the one edit item we no longer want
		editList.at(i)->DestroyItem();

		Logger::Log("PhoediX EditListPanel::EditListScroll::DeleteNonTopEdits - Updating AUI Manager", Logger::LogLevel::LOG_VERBOSE);
		PhoedixAUIManager::GetPhoedixAUIManager()->Update();
	}

	Logger::Log("PhoediX EditListPanel::EditListScroll::DeleteNonTopEdits - Clearing edit list vector", Logger::LogLevel::LOG_VERBOSE);
	editList.erase(editList.begin() + this->GetNumTopEdits(), editList.end());

	Logger::Log("PhoediX EditListPanel::EditListScroll::DeleteNonTopEdits - Redrawing edits", Logger::LogLevel::LOG_VERBOSE);
	this->RedrawEdits();
	Logger::Log("PhoediX EditListPanel::EditListScroll::DeleteNonTopEdits - Returning", Logger::LogLevel::LOG_VERBOSE);
}

void EditListPanel::EditListScroll::RedrawEdits() {

    Logger::Log("PhoediX EditListPanel::EditListScroll::RedrawEdits - Called", Logger::LogLevel::LOG_VERBOSE);
	int seq = 0;
	int plusDragCursor = dragCursorPos > -1 ? 1 : 0;

	// Iterate and reset the sequence of all edit items, and add them back to the sizer with a spacer
	for (size_t i = 0; i < editList.size() + plusDragCursor; i++) {

		// Draw cursor
		if (i == dragCursorPos) {
            
            Logger::Log("PhoediX EditListPanel::EditListScroll::RedrawEdits - Drawing cursor (dragging edit)", Logger::LogLevel::LOG_VERBOSE);
			this->GetSizer()->Add(dragCursor, 0, wxALL | wxEXPAND);
            Logger::Log("PhoediX EditListPanel::EditListScroll::RedrawEdits - Cursor drawn", Logger::LogLevel::LOG_VERBOSE);
		}

		// Draw edit list item (before and after cursor)
		else if(i == dragCursorPos - 1 || i == dragCursorPos + 1){
            
            Logger::Log("PhoediX EditListPanel::EditListScroll::RedrawEdits - Drawing edit item", Logger::LogLevel::LOG_VERBOSE);
			editList.at(seq)->SetSequence(seq);
			this->GetSizer()->AddSpacer(5);
			this->GetSizer()->Add(editList.at(seq), 0, wxALL | wxEXPAND);
			this->GetSizer()->AddSpacer(5);
			seq += 1;
            Logger::Log("PhoediX EditListPanel::EditListScroll::RedrawEdits - Edit item drawn", Logger::LogLevel::LOG_VERBOSE);
		}
		else {
			Logger::Log("PhoediX EditListPanel::EditListScroll::RedrawEdits - Drawing edit item", Logger::LogLevel::LOG_VERBOSE);
			editList.at(seq)->SetSequence(seq);
			this->GetSizer()->AddSpacer(6);
			this->GetSizer()->Add(editList.at(seq), 0, wxALL | wxEXPAND);
			this->GetSizer()->AddSpacer(6);
			seq += 1;
			Logger::Log("PhoediX EditListPanel::EditListScroll::RedrawEdits - Edit item drawn", Logger::LogLevel::LOG_VERBOSE);
		}
	}
	
	// Fit inside the scroll bars
    Logger::Log("PhoediX EditListPanel::EditListScroll::RedrawEdits - Fitting sizer", Logger::LogLevel::LOG_VERBOSE);
	this->FitInside();
    Logger::Log("PhoediX EditListPanel::EditListScroll::RedrawEdits - Returning", Logger::LogLevel::LOG_VERBOSE);
}

int EditListPanel::EditListScroll::GetNextID() {
	return editList.size();
}

wxVector<EditListItem*> EditListPanel::EditListScroll::GetEditList() {
	return editList;
}

void EditListPanel::EditListScroll::OnCropActivate(wxCommandEvent& evt) {
	wxPostEvent(parWindow, evt);
}
void EditListPanel::EditListScroll::OnCropDeactivate(wxCommandEvent& evt) {
	wxPostEvent(parWindow, evt);
}

void EditListPanel::EditListScroll::SetEditDragCursor(int index) {
	if (index < 0) {
		dragCursor->Hide();
	}
	else {
		dragCursor->Show();
	}
	dragCursorPos = index;

	this->GetSizer()->Clear();
	this->RedrawEdits();
}

void EditListPanel::EditListScroll::CompleteDrag(EditListItem* item, int newIndex) {

	// Hide drag cursor
	dragCursor->Hide();

	// Get index of highlighted item
	int highlightIndex = -1;
	for (int i = 0; i < editList.size(); i++) {
		if (editList.at(i) == item) {
			highlightIndex = i;
			break;
		}
	}

	// If highlight item is indexed properly
	if (highlightIndex > -1) {

		// Get offset incase moving item below
		int offset = 0;
		if (newIndex > highlightIndex) {
			offset = -1;
		}

		// Erase highlighted item, and reinsert it in new position
		editList.erase(editList.begin() + highlightIndex);
		editList.insert(editList.begin() + newIndex + offset, item);
	}

	// Redraw edit list
	dragCursorPos = -1;
	this->GetSizer()->Clear();
	this->RedrawEdits();
}

EditListPanel::StartEditsThread::StartEditsThread(EditListPanel * parent, Processor * processor, bool processRaw, bool unpack) : wxThread(wxTHREAD_DETACHED) {
	par = parent;
	proc = processor;
	stop = false;
	raw = processRaw;
	unpck = unpack;
}

void EditListPanel::StartEditsThread::Stop() {
	stop = true;
}

wxThread::ExitCode EditListPanel::StartEditsThread::Entry() {

	if (raw) {
		
		// Unpack and process raw
		if (unpck) {
			while (proc->UnpackAndProcessRaw() < 0) {

				// Another edit was made while waiting to prcoess current one.  Stop and let
				// edit list restart accounting for new edit
				if (stop) {
					break;
				}
				Sleep(1);
			}
		}

		// Just process raw, no unpacking necessary
		else {
			while (proc->ProcessRaw() < 0) {

				// Another edit was made while waiting to prcoess current one.  Stop and let
				// edit list restart accounting for new edit
				if (stop) {
					break;
				}
				Sleep(1);
			}
		}
		par->StartEditsComplete();
		wxCommandEvent evt(REPROCESS_IMAGE_EVENT, ID_REPROCESS_IMAGE);
		wxPostEvent(par, evt);

		return 0;
	}

	// Process a normal non raw image
	else {

		// Maximum wait for force stop to complete, 0.5 seconds.
		// After 0.05 seconds, process anyway
		int count = 0;
		int maxCount = 25;

		while (proc->ProcessEdits() < 0) {

			// Another edit was made while waiting to prcoess current one.  Stop and let
			// edit list restart accounting for new edit
			if (stop) {
				break;
			}

			// Timeout.  Set force stop to false and restart prcocessing
			if (count > maxCount) {
				proc->ResetForceStop();
				proc->ProcessEdits();
				break;
			}
			Sleep(1);
			count += 1;
		}

		// Let parent know process is complete
		par->StartEditsComplete();
		return 0;
	}
}
