// Copyright 2016 Jacob Chesley

#include "EditListPanel.h"

EditListPanel::EditListPanel(wxWindow * parent, Processor * processor) : wxPanel(parent) {

	mainSizer = new wxBoxSizer(wxVERTICAL);
	this->SetSizer(mainSizer);
	this->SetBackgroundColour(Colors::BackDarkGrey);
	
	titleText = new wxStaticText(this, -1, "List of Edits");
	titleText->SetForegroundColour(Colors::TextLightGrey);
	titleText->SetFont(wxFont(14, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD));

	scroller = new EditListScroll(this);
	scroller->Connect(wxEVT_RIGHT_DOWN, wxMouseEventHandler(EditListPanel::OnRightClick), NULL, this);

	Icons icons;
	addEditButton = new wxButton(this, EditListPanel::Buttons::ADD_EDIT_BUTTON, "Add Edit", wxDefaultPosition, wxDefaultSize, wxBORDER_NONE);
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
	this->GetSizer()->Add(titleText, 0, wxALIGN_CENTER, 1);
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
	proc = processor;
	hasRaw = false;

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
}

void EditListPanel::OnRightClick(wxMouseEvent& WXUNUSED(event)){

	// Display a popup menu of options
	wxMenu popupMenu;
	popupMenu.Append(EditListPanel::PopupMenuActions::COPY_EDIT_LIST, "Copy Edit List");

	if(proc->GetEditListForCopyPaste().size() > 0){
		popupMenu.Append(EditListPanel::PopupMenuActions::PASTE_EDIT_LIST, "Paste Edit List");
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

		case EditListPanel::PopupMenuActions::PASTE_EDIT_LIST:{
			this->AddEditWindows(proc->GetEditListForCopyPaste());
		}
	}
}

void EditListPanel::ReprocessImageEvt(wxCommandEvent& WXUNUSED(event)) {
	this->ReprocessImage();
}

void EditListPanel::ReprocessImageRawEvt(wxCommandEvent& WXUNUSED(event)) {
	this->ReprocessImageRaw();
}

void EditListPanel::ReprocessUnpackImageRawEvt(wxCommandEvent& WXUNUSED(event)) {
	this->ReprocessUnpackImageRaw();
}

void EditListPanel::AddEditsToProcessor() {

	// Delete the current list of internally stored edits in the processor
	proc->DeleteEdits();

	// Create a vector of Edit list Items that are displayed on the panel
	wxVector<EditListItem*> editList = scroller->GetEditList();
	for (size_t i = 0; i < editList.size(); i++) {

		// Add each edit from the edit window, to the processor
		if (editList.at(i)->GetEditWindow() != NULL) {

			editList.at(i)->GetEditWindow()->SetDisabled(editList.at(i)->GetDisabled());
			editList.at(i)->GetEditWindow()->AddEditToProcessor();
		}
	}
}

void EditListPanel::ReprocessImage() {
	
	// Delete the current list of internally stored edits in the processor
	proc->DeleteEdits();
	
	// Add all edits to processor
	this->AddEditsToProcessor();

	// Process the edits
	proc->ProcessEdits();
}

void EditListPanel::ReprocessImageRaw() {

	// Process Raw if needed
	if (hasRaw) {

		// If there are edits, tell raw processor to not set processor to updated 
		// (still need to perform edits after raw processing).
		if(scroller->GetEditList().size() > 0){
			proc->SetHasEdits(true);
		}
		else{
			proc->SetHasEdits(false);
		}
		proc->ProcessRaw();
	}	

	this->ReprocessImage();
}

void EditListPanel::ReprocessUnpackImageRaw() {

	// Process Raw if needed
	if (hasRaw) {
		proc->UnpackAndProcessRaw();
	}

	this->ReprocessImage();
}

void EditListPanel::OnAddEdit(wxCommandEvent& WXUNUSED(event)) {
	PhoedixAUIManager::GetPhoedixAUIManager()->GetPane(editSelection).Show();
	PhoedixAUIManager::GetPhoedixAUIManager()->Update();
}


void EditListPanel::AddEditToPanel(wxCommandEvent& addEvt) {

	// Get the edit ID to add to the panel
	int editID = addEvt.GetInt();

	// Create new edit window and add it to panel
	EditWindow * newEditWindow = AvailableEditWindows::GetEditWindow(editID, this, proc);
	this->AddEditWindowToPanel(newEditWindow, editID, false, true);

	// Reprocess the image with the new edit added
	this->ReprocessImage();
}

void EditListPanel::AddEditWindows(wxVector<ProcessorEdit*> inEdits) {
	
	this->RemoveAllWindows();
	
	for (size_t i = 0; i < inEdits.size(); i++) {
		if (inEdits.at(i) != NULL) {

			// Add a raw window properly
			if(inEdits.at(i)->GetEditType() == ProcessorEdit::EditType::RAW){
				this->AddRawWindow(inEdits.at(i));
			}

			// Add the correct type of window, and set parameter for window based on edit
			else{
				EditWindow * newEditWindow = AvailableEditWindows::GetEditWindow(inEdits.at(i), this, proc);
				this->AddEditWindowToPanel(newEditWindow, AvailableEditWindows::GetEditIDFromEdit(inEdits.at(i)), inEdits.at(i)->GetDisabled(), false);

				if (newEditWindow != NULL) {
					newEditWindow->SetDisabled(inEdits.at(i)->GetDisabled());
					newEditWindow->SetParamsAndFlags(inEdits.at(i));
				}
			}
		}
	}

	for(size_t i = 0; i < scroller->GetEditList().size(); i++){
		scroller->GetEditList().at(i)->GetEditWindow()->Activate();
	}
	
	this->ReprocessImageRaw();
}

void EditListPanel::RemoveAllWindows() {
	this->RemoveRawWindow();
	scroller->DeleteAllEdits();
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
		editWindowInfo.BestSize(window->GetClientSize());
		editWindowInfo.Name(window->GetName() + idStr);

		// Add new edit window to AUI manager
		PhoedixAUIManager::GetPhoedixAUIManager()->AddPane(window, editWindowInfo);
		PhoedixAUIManager::GetPhoedixAUIManager()->Update();
		
		// Add a new Edit List Item to the Edit List scroll panel
		// Add special panel that will disable buttons if it is raw.  Keep raw panel at top
		if(editID == AvailableEditIDS::EDIT_ID_RAW){

			EditListItem * newEditListItem = new EditListItem(scroller, available.GetNameFromID(editID), scroller->GetNextID(), window, true);
			scroller->AddEdit(newEditListItem);
			if(autoActivate){ newEditListItem->GetEditWindow()->Activate(); }
		}
		else{
			EditListItem * newEditListItem = new EditListItem(scroller, available.GetNameFromID(editID), scroller->GetNextID(), window, false);
			newEditListItem->SetDisabled(disable);
			scroller->AddEdit(newEditListItem);
			if(autoActivate){ newEditListItem->GetEditWindow()->Activate(); }
		}
	}
}

void EditListPanel::MoveEditUp(wxCommandEvent& upEvt) {

	// Get edit number that is requesting to be moved up
	int editNum = upEvt.GetInt();
	scroller->MoveEditUp(editNum);
	this->ReprocessImage();
}

void EditListPanel::MoveEditDown(wxCommandEvent& downEvt) {

	// Get edit number that is requesting to be moved down
	int editNum = downEvt.GetInt();
	scroller->MoveEditDown(editNum);
	this->ReprocessImage();
}

void EditListPanel::DeleteEdit(wxCommandEvent& deleteEvt) {

	// Get edit number that is requesting to be deleted
	int editNum = deleteEvt.GetInt();
	scroller->DeleteEdit(editNum);
	this->ReprocessImage();
}

void EditListPanel::CopyEdit(wxCommandEvent& copyEvt) {

	// Get edit number that is requesting to be copied
	int editNum = copyEvt.GetInt();

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
	int editNum = pasteEvt.GetInt();

	// Cant paste edit that does not exist in list
	if(editNum > scroller->GetEditList().size() - 1 || editNum < 0){
		return;
	}

	// Grab the params and flags of the edit in the processor and paste in to the edit window
	ProcessorEdit * copiedEdit = proc->GetEditForCopyPaste();
	scroller->GetEditList()[editNum]->GetEditWindow()->SetParamsAndFlags(copiedEdit);
}

void EditListPanel::DisableEdit(wxCommandEvent& WXUNUSED(event)) {
	this->ReprocessImage();
}

void EditListPanel::AddRawWindow(){

	// Get number of edits already in list
	size_t numEdits = scroller->GetNextID();

	// Add new raw processor edit panel to list
	EditWindow * newEditWindow = AvailableEditWindows::GetEditWindow(AvailableEditIDS::EDIT_ID_RAW, this, proc);
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

}

void EditListPanel::AddRawWindow(ProcessorEdit * editForParams){

	// Get number of edits already in list
	size_t numEdits = scroller->GetNextID();

	// Add new raw processor edit panel to list
	EditWindow * newEditWindow = AvailableEditWindows::GetEditWindow(AvailableEditIDS::EDIT_ID_RAW, this, proc);
	this->AddEditWindowToPanel(newEditWindow, AvailableEditIDS::EDIT_ID_RAW, false, true);
	newEditWindow->SetParamsAndFlags(editForParams);

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
	}
}


EditListPanel::EditListScroll::EditListScroll(wxWindow * parent) : wxScrolledWindow(parent) {

	parWindow = parent;
	sizer = new wxBoxSizer(wxVERTICAL);
	this->SetSizer(sizer);
	this->FitInside();
	this->SetScrollRate(5, 5);
	topEdits = 0;
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
	this->GetSizer()->Add(edit, 0, wxALL| wxEXPAND);
	this->GetSizer()->AddSpacer(12);

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
	PhoedixAUIManager::GetPhoedixAUIManager()->Update();

	// Make minimum size of parent the original min size.  This panel is still the size that it does not 
	// show scroll bars, but can get smaller now
	PhoedixAUIManager::GetPhoedixAUIManager()->GetPane(parWindow).MinSize(originalParSize);
	PhoedixAUIManager::GetPhoedixAUIManager()->Update();
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
	editList.at(index)->DestroyItem();
	editList.erase(editList.begin() + index);
	PhoedixAUIManager::GetPhoedixAUIManager()->Update();

	this->RedrawEdits();
}

void EditListPanel::EditListScroll::DeleteAllEdits() {

	// Remove all edit items from the sizer, but do not destroy them
	this->GetSizer()->Clear();

	// Remove all edit windows from list
	for (size_t i = 0; i < editList.size(); i++){

		// Remove the edit window from AUI Manager if it exists
		if (editList.at(i)->GetEditWindow() != NULL) {
			PhoedixAUIManager::GetPhoedixAUIManager()->DetachPane(editList.at(i)->GetEditWindow());
		}

		//Destroy and erase the one edit item we no longer want
		editList.at(i)->DestroyItem();
		PhoedixAUIManager::GetPhoedixAUIManager()->Update();
	}

	editList.clear();
	this->RedrawEdits();
}

void EditListPanel::EditListScroll::RedrawEdits() {

	// Iterate and reset the sequence of all edit items, and add them back to the sizer with a spacer
	for (size_t i = 0; i < editList.size(); i++) {
		editList.at(i)->SetSequence(i);
		this->GetSizer()->Add(editList.at(i), 0, wxALL | wxEXPAND);
		this->GetSizer()->AddSpacer(12);
	}

	// Fit inside the scroll bars
	this->FitInside();
}

int EditListPanel::EditListScroll::GetNextID() {
	return editList.size();
}

wxVector<EditListItem*> EditListPanel::EditListScroll::GetEditList() {
	return editList;
}