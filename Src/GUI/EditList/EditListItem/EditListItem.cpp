#include "EditListItem.h"

wxDEFINE_EVENT(EDIT_UP_EVENT, wxCommandEvent);
wxDEFINE_EVENT(EDIT_DOWN_EVENT, wxCommandEvent);
wxDEFINE_EVENT(EDIT_DELETE_EVENT, wxCommandEvent);
wxDEFINE_EVENT(EDIT_DISABLE_EVENT, wxCommandEvent);

EditListItem::EditListItem(wxWindow * parent, wxString title, int Sequence, EditWindow * editWindow, bool disableButtons) : wxPanel(parent) {

	parWindow = parent;

	sizer = new wxBoxSizer(wxHORIZONTAL);
	this->SetSizer(sizer);
	this->SetBackgroundColour(Colors::BackGrey);

	textSizer = new wxBoxSizer(wxVERTICAL);
	titleText = new wxButton(this, EditListItem::Buttons::OPEN_EDIT_BUTTON, title, wxDefaultPosition, wxDefaultSize, wxBORDER_NONE);
	titleText->SetBackgroundColour(this->GetBackgroundColour());
	titleText->SetForegroundColour(Colors::TextWhite);
	titleText->SetFont(wxFont(12, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL));
	textSizer->AddSpacer(5);
	textSizer->Add(titleText, 0, wxALIGN_CENTER, 1);
	textSizer->AddSpacer(5);
	disableButton = new wxButton(this, EditListItem::Buttons::DISABLE_BUTTON, "", wxDefaultPosition, wxDefaultSize, wxBORDER_NONE);
	
	upDownButtonSizer = new wxBoxSizer(wxVERTICAL);
	upButton = new wxButton(this, EditListItem::Buttons::UP_BUTTON, "", wxDefaultPosition, wxDefaultSize, wxBORDER_NONE);
	downButton = new wxButton(this, EditListItem::Buttons::DOWN_BUTTON, "", wxDefaultPosition, wxDefaultSize, wxBORDER_NONE);
	deleteButton = new wxButton(this, EditListItem::Buttons::DELETE_BUTTON, "", wxDefaultPosition, wxDefaultSize, wxBORDER_NONE);


	upButton->SetBackgroundColour(this->GetBackgroundColour());
	downButton->SetBackgroundColour(this->GetBackgroundColour());
	deleteButton->SetBackgroundColour(this->GetBackgroundColour());
	disableButton->SetBackgroundColour(this->GetBackgroundColour());

	if(!disableButtons){
		Icons icons;
		upButton->SetBitmap(icons.UpButton.Rescale(17, 17, wxIMAGE_QUALITY_HIGH));
		downButton->SetBitmap(icons.DownButton.Rescale(17, 17, wxIMAGE_QUALITY_HIGH));
		deleteButton->SetBitmap(icons.DeleteButton.Rescale(22, 22, wxIMAGE_QUALITY_HIGH));
		disableButton->SetBitmap(icons.DisableButton.Rescale(22, 22, wxIMAGE_QUALITY_HIGH));

		upButton->SetMinSize(wxSize(17, 17));
		downButton->SetMinSize(wxSize(17, 17));
		deleteButton->SetMinSize(wxSize(22, 22));
		disableButton->SetMinSize(wxSize(22, 22));

		upDownButtonSizer->Add(upButton);
		upDownButtonSizer->Add(downButton);
	}

	if (!disableButtons) {
		this->GetSizer()->Add(disableButton, 0, wxEXPAND);
	}

	this->GetSizer()->Add(textSizer, 1, wxEXPAND);

	if(!disableButtons){
		this->GetSizer()->Add(upDownButtonSizer, 0, wxEXPAND);
		this->GetSizer()->AddSpacer(12);
		this->GetSizer()->Add(deleteButton, 0, wxEXPAND);
	}

	editWin = NULL;
	if (editWindow != NULL) {
		editWin = editWindow;
	}

	this->Bind(wxEVT_BUTTON, (wxObjectEventFunction)&EditListItem::OnOpenEdit, this, EditListItem::Buttons::OPEN_EDIT_BUTTON);
	this->Bind(wxEVT_BUTTON, (wxObjectEventFunction)&EditListItem::OnUp, this, EditListItem::Buttons::UP_BUTTON);
	this->Bind(wxEVT_BUTTON, (wxObjectEventFunction)&EditListItem::OnDown, this, EditListItem::Buttons::DOWN_BUTTON);
	this->Bind(wxEVT_BUTTON, (wxObjectEventFunction)&EditListItem::OnDelete, this, EditListItem::Buttons::DELETE_BUTTON);
	this->Bind(wxEVT_BUTTON, (wxObjectEventFunction)&EditListItem::OnDisable, this, EditListItem::Buttons::DISABLE_BUTTON);

	seq = Sequence;
	isDisabled = false;
}

EditListItem::~EditListItem() {
	if (editWin != NULL) {
		editWin->StopWatchdog();
		editWin->Destroy();
	}
}

void EditListItem::DestroyItem() {
	if (editWin != NULL) {
		editWin->StopWatchdog();
		editWin->Destroy();
		editWin = NULL;
	}

	this->Destroy();
}

EditWindow* EditListItem::GetEditWindow() {
	return editWin;
}

bool EditListItem::GetDisabled() {
	return isDisabled;
}

void EditListItem::SetDisabled(bool disabled){

	if (disabled) {
		titleText->SetForegroundColour(Colors::TextGrey);
		isDisabled = true;
	}
	else {
		titleText->SetForegroundColour(Colors::TextWhite);
		isDisabled = false;
	}
}

void EditListItem::OnOpenEdit(wxCommandEvent& WXUNUSED(event)) {
	if (editWin != NULL) {
		PhoedixAUIManager::GetPhoedixAUIManager()->GetPane(editWin).Show();
		PhoedixAUIManager::GetPhoedixAUIManager()->Update();
	}
}

void EditListItem::OnUp(wxCommandEvent& WXUNUSED(event)) {
	wxCommandEvent evt(EDIT_UP_EVENT, ID_EDIT_UP);
	evt.SetInt(seq);
	wxPostEvent(parWindow, evt);
}

void EditListItem::OnDown(wxCommandEvent& WXUNUSED(event)) {
	wxCommandEvent evt(EDIT_DOWN_EVENT, ID_EDIT_DOWN);
	evt.SetInt(seq);
	wxPostEvent(parWindow, evt);
}

void EditListItem::OnDelete(wxCommandEvent& WXUNUSED(event)) {
	wxCommandEvent evt(EDIT_DELETE_EVENT, ID_EDIT_DELETE);
	evt.SetInt(seq);
	wxPostEvent(parWindow, evt);
}

void EditListItem::OnDisable(wxCommandEvent& WXUNUSED(event)) {

	if (isDisabled) {
		this->SetDisabled(false);
	}
	else {
		this->SetDisabled(true);
	}

	wxCommandEvent evt(EDIT_DISABLE_EVENT, ID_EDIT_DISABLE);
	wxPostEvent(parWindow, evt);
}

wxString EditListItem::GetTitle() {
	return titleText->GetLabel();
}

void EditListItem::SetTitle(wxString title) {
	return titleText->SetLabel(title);
}

int EditListItem::GetSequence() {
	return seq;
}

void EditListItem::SetSequence(size_t sequence) {
	seq = sequence;
}