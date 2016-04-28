#include "EditListPanel.h"

EditListPanel::EditListPanel(wxWindow * parent) : wxPanel(parent) {

	mainSizer = new wxBoxSizer(wxVERTICAL);
	this->SetSizer(mainSizer);
	this->SetBackgroundColour(Colors::BackDarkGrey);
	
	titleText = new wxStaticText(this, -1, "List of Edits");
	titleText->SetForegroundColour(Colors::TextLightGrey);
	titleText->SetFont(wxFont(14, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD));

	scroller = new EditListScroll(this);

	Icons icons;
	addEditButton = new wxButton(this, EditListPanel::Buttons::ADD_EDIT_BUTTON, "Add Edit", wxDefaultPosition, wxDefaultSize, wxBORDER_NONE);
	addEditButton->SetForegroundColour(Colors::TextLightGrey);
	addEditButton->SetBackgroundColour(Colors::BackGrey);
	addEditButton->SetFont(wxFont(11, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL));

	editSelection = new EditSelection(this);

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
	this->Bind(EDIT_ADD_EVENT, (wxObjectEventFunction)&EditListPanel::AddEditToPanel, this, ID_EDIT_ADD);

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

void EditListPanel::OnAddEdit(wxCommandEvent& WXUNUSED(event)) {
	editSelection->Show();
}


void EditListPanel::AddEditToPanel(wxCommandEvent& addEvt) {

	int editID = addEvt.GetInt();
	AvailableEdits available;

	scroller->AddEdit(new EditListItem(scroller, available.GetNameFromID(editID), scroller->GetNextID()));
}

void EditListPanel::MoveEditUp(wxCommandEvent& upEvt) {

	// Get edit number that is requesting to be moved up
	int editNum = upEvt.GetInt();
	scroller->MoveEditUp(editNum);
}

void EditListPanel::MoveEditDown(wxCommandEvent& downEvt) {

	// Get edit number that is requesting to be moved down
	int editNum = downEvt.GetInt();
	scroller->MoveEditDown(editNum);
}

void EditListPanel::DeleteEdit(wxCommandEvent& deleteEvt) {

	// Get edit number that is requesting to be deleted
	int editNum = deleteEvt.GetInt();
	scroller->DeleteEdit(editNum);
}

EditListPanel::EditListScroll::EditListScroll(wxWindow * parent) : wxScrolledWindow(parent) {

	sizer = new wxBoxSizer(wxVERTICAL);
	this->SetSizer(sizer);
	this->FitInside();
	this->SetScrollRate(5, 5);
}

void EditListPanel::EditListScroll::AddEdit(EditListItem * edit) {

	// Add the edit to the sizer, and push onto the editList
	editList.push_back(edit);
	this->GetSizer()->Add(edit, 0, wxALL| wxEXPAND);
	this->GetSizer()->AddSpacer(12);

	// Fit inside the scroll bars
	this->FitInside();
}

void EditListPanel::EditListScroll::MoveEditUp(size_t index) {

	// We cant move an item further up if it is at the top
	if (index <= 0) {
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

	// Destroy and erase the one edit item we no longer want
	editList.at(index)->Destroy();
	editList.erase(editList.begin() + index);

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