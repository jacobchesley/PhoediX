// Copyright 2018 Jacob Chesley.
// See LICENSE.TXT in root of project for license information.

#include "EditListItem.h"

wxDEFINE_EVENT(EDIT_UP_EVENT, wxCommandEvent);
wxDEFINE_EVENT(EDIT_DOWN_EVENT, wxCommandEvent);
wxDEFINE_EVENT(EDIT_DELETE_EVENT, wxCommandEvent);
wxDEFINE_EVENT(EDIT_DISABLE_EVENT, wxCommandEvent);
wxDEFINE_EVENT(EDIT_COPY_EVENT, wxCommandEvent);
wxDEFINE_EVENT(EDIT_PASTE_EVENT, wxCommandEvent);
wxDEFINE_EVENT(EDIT_DRAG_EVENT, wxCommandEvent);
wxDEFINE_EVENT(EDIT_DRAG_COMPLETE_EVENT, wxCommandEvent);

EditListItem::EditListItem(wxWindow * parent, wxString title, int Sequence, EditWindow * editWindow, bool disableButtons) : wxPanel(parent) {

	parWindow = parent;

	sizer = new wxBoxSizer(wxHORIZONTAL);
	this->SetSizer(sizer);
	this->SetBackgroundColour(Colors::BackGrey);

	textSizer = new wxBoxSizer(wxVERTICAL);

	// Add title text
	titleText = new PhoediXButton(this, EditListItem::Buttons::OPEN_EDIT_BUTTON, title);
	titleText->SetBackgroundColour(this->GetBackgroundColour());
	titleText->SetForegroundColour(Colors::TextWhite);
	titleText->SetFont(wxFont(12, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL));
	titleText->Connect(wxEVT_RIGHT_DOWN, wxMouseEventHandler(EditListItem::OnRightClick), NULL, this);
	titleText->HighlightWhenEnabled(false);
	textSizer->AddSpacer(5);
	textSizer->Add(titleText, 0, wxALIGN_CENTER, 1);
	textSizer->AddSpacer(5);

	fixedItem = disableButtons;
			
	upDownDisableDefined = false;

	// Add control buttons when there is an edit that must remain in the exact place (RAW edit must be at top)
	if(!disableButtons){

		upDownDisableDefined = true;

		// Add size to keep up and down button in
		upDownButtonSizer = new wxBoxSizer(wxVERTICAL);

		// Layout is:
		//                                  [Up Button]
		// [disable button]  [TitleText]                 [Delete Button]
		//                                 [Down Button]
		upButton = new wxButton(this, EditListItem::Buttons::UP_BUTTON, "", wxDefaultPosition, wxDefaultSize, wxBORDER_NONE);
		downButton = new wxButton(this, EditListItem::Buttons::DOWN_BUTTON, "", wxDefaultPosition, wxDefaultSize, wxBORDER_NONE);
		deleteButton = new wxButton(this, EditListItem::Buttons::DELETE_BUTTON, "", wxDefaultPosition, wxDefaultSize, wxBORDER_NONE);
		disableButton = new wxButton(this, EditListItem::Buttons::DISABLE_BUTTON, "", wxDefaultPosition, wxDefaultSize, wxBORDER_NONE);

		// Background of buttons
		upButton->SetBackgroundColour(this->GetBackgroundColour());
		downButton->SetBackgroundColour(this->GetBackgroundColour());
		deleteButton->SetBackgroundColour(this->GetBackgroundColour());
		disableButton->SetBackgroundColour(this->GetBackgroundColour());

		// Add icons to buttons
		Icons icons;
		upButton->SetBitmap(icons.UpButton.Rescale(17, 17, wxIMAGE_QUALITY_HIGH));
		downButton->SetBitmap(icons.DownButton.Rescale(17, 17, wxIMAGE_QUALITY_HIGH));
		deleteButton->SetBitmap(icons.DeleteButton.Rescale(22, 22, wxIMAGE_QUALITY_HIGH));
		disableButton->SetBitmap(icons.DisableButton.Rescale(22, 22, wxIMAGE_QUALITY_HIGH));

		// Size buttons
		upButton->SetMinSize(wxSize(20, 20));
		downButton->SetMinSize(wxSize(20, 20));
		deleteButton->SetMinSize(wxSize(25, 25));
		disableButton->SetMinSize(wxSize(35, 25));

		// Add up and down button to its own sizer
		upDownButtonSizer->Add(upButton);
		upDownButtonSizer->Add(downButton);
	}

	// Add disable button if buttons are allowed
	if (!disableButtons) {
		this->GetSizer()->Add(disableButton, 0, wxEXPAND);
	}

	// Always add title text
	this->GetSizer()->Add(textSizer, 1, wxEXPAND);

	// Add up and down buttons, and delete button, is buttons are allowed
	if(!disableButtons){
		this->GetSizer()->Add(upDownButtonSizer, 0, wxEXPAND);
		this->GetSizer()->AddSpacer(12);
		this->GetSizer()->Add(deleteButton, 0, wxEXPAND);
	}

	// Add connected edit window
	editWin = NULL;
	if (editWindow != NULL) {
		editWin = editWindow;
	}

	// Bind all events needed
	this->Bind(wxEVT_BUTTON, (wxObjectEventFunction)&EditListItem::OnOpenEdit, this, EditListItem::Buttons::OPEN_EDIT_BUTTON);
	this->Bind(wxEVT_BUTTON, (wxObjectEventFunction)&EditListItem::OnUp, this, EditListItem::Buttons::UP_BUTTON);
	this->Bind(wxEVT_BUTTON, (wxObjectEventFunction)&EditListItem::OnDown, this, EditListItem::Buttons::DOWN_BUTTON);
	this->Bind(wxEVT_BUTTON, (wxObjectEventFunction)&EditListItem::OnDelete, this, EditListItem::Buttons::DELETE_BUTTON);
	this->Bind(wxEVT_BUTTON, (wxObjectEventFunction)&EditListItem::OnDisable, this, EditListItem::Buttons::DISABLE_BUTTON);
	this->Bind(wxEVT_RIGHT_DOWN, (wxMouseEventFunction)&EditListItem::OnRightClick, this);
	this->Bind(wxEVT_LEFT_DOWN, (wxObjectEventFunction)&EditListItem::OnDragStart, this);
	this->Bind(wxEVT_LEFT_DOWN, (wxObjectEventFunction)&EditListItem::OnDragStart, this, EditListItem::Buttons::OPEN_EDIT_BUTTON);
	this->Bind(wxEVT_TIMER, (wxObjectEventFunction)&EditListItem::OnTimerFire, this, ID_HIGHLIGHT_TIMER);
	this->Bind(wxEVT_TIMER, (wxObjectEventFunction)& EditListItem::OnMouseCheck, this, ID_MOUSE_TIMER);

	this->Bind(wxEVT_ENTER_WINDOW, (wxMouseEventFunction)& EditListItem::OnMouseEnter, this);
	this->Bind(wxEVT_LEAVE_WINDOW, (wxMouseEventFunction)& EditListItem::OnMouseLeave, this);

	wxWindowListNode * child = this->GetChildren().GetFirst();
	while (child){
		wxWindow* childWin = child->GetData();
		childWin->Connect(wxEVT_LEAVE_WINDOW, wxMouseEventHandler(EditListItem::OnMouseLeave), NULL, this);
		childWin->Connect(wxEVT_ENTER_WINDOW, wxMouseEventHandler(EditListItem::OnMouseEnter), NULL, this);
		child = child->GetNext();
	}

	seq = Sequence;
	isDisabled = false;

	isHighlighted = false;
	highlightTimerFired = false;

	mouseTimer.SetOwner(this, ID_MOUSE_TIMER);
	mouseTimer.Start(50, false);
	mouseHighlightActive = false;
}

// Destroy the edit window
EditListItem::~EditListItem() {
	if (editWin != NULL) {
		editWin->DestroyEditWindow();
		editWin->Destroy();
		editWin = NULL;
	}
}

// Destroy this and edit window
void EditListItem::DestroyItem() {
	if (editWin != NULL) {
		editWin->DestroyEditWindow();
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

	// Darken the title text to show this is disabled
	if (disabled) {
		titleText->SetForegroundColour(Colors::TextGrey);
		isDisabled = true;
	}

	// Lighten the title text to show this is enabled
	else {
		titleText->SetForegroundColour(Colors::TextWhite);
		isDisabled = false;
	}
}

void EditListItem::OnOpenEdit(wxCommandEvent& WXUNUSED(event)) {

	// Item was dragged, not meant to have edit window opened
	if(this->CheckDrag()){
		return;
	}

	// Show the edit window
	if (editWin != NULL) {
		PhoedixAUIManager::GetPhoedixAUIManager()->GetPane(editWin).Show();
		PhoedixAUIManager::GetPhoedixAUIManager()->GetPane(editWin).Left();
		PhoedixAUIManager::GetPhoedixAUIManager()->GetPane(editWin).Dock();
		PhoedixAUIManager::GetPhoedixAUIManager()->GetPane(editWin).Position(seq);
		PhoedixAUIManager::GetPhoedixAUIManager()->Update();
	}
}

void EditListItem::OnRightClick(wxMouseEvent& WXUNUSED(event)) {

	// Display a popup menu of options
	wxMenu popupMenu;
	popupMenu.Append(EditListItem::PopupMenuActions::COPY_EDIT_PARAMS, "Copy Edit Parameters");

	if(this->GetEditWindow()->CheckCopiedParamsAndFlags()){
		popupMenu.Append(EditListItem::PopupMenuActions::PASTE_EDIT_PARAMS, "Paste Edit Parameters");
	}
	popupMenu.Connect(wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(EditListItem::OnPopupMenuClick), NULL, this);
	this->PopupMenu(&popupMenu);
}

void EditListItem::OnPopupMenuClick(wxCommandEvent& inEvt) {

	int id = inEvt.GetId();

	switch(id){

		// Copy the edit parameters
		case EditListItem::PopupMenuActions::COPY_EDIT_PARAMS:{
			// Tell the Edit Layer Panel to copy parameters of this edit
			wxCommandEvent evt(EDIT_COPY_EVENT, ID_EDIT_COPY);
			evt.SetInt(seq);
			wxPostEvent(parWindow, evt);
			break;
		}

		case EditListItem::PopupMenuActions::PASTE_EDIT_PARAMS:{
			// Tell the Edit Layer Panel to copy parameters of this edit
			wxCommandEvent evt(EDIT_PASTE_EVENT, ID_EDIT_PASTE);
			evt.SetInt(seq);
			wxPostEvent(parWindow, evt);
			break;
		}
	}
}

void EditListItem::OnUp(wxCommandEvent& WXUNUSED(event)) {

	// Tell the Edit Layer Panel to move this edit up
	wxCommandEvent evt(EDIT_UP_EVENT, ID_EDIT_UP);
	evt.SetInt(seq);
	wxPostEvent(parWindow, evt);
}

void EditListItem::OnDown(wxCommandEvent& WXUNUSED(event)) {

	// Tell the Edit Layer Panel to move this edit down
	wxCommandEvent evt(EDIT_DOWN_EVENT, ID_EDIT_DOWN);
	evt.SetInt(seq);
	wxPostEvent(parWindow, evt);
}

void EditListItem::OnDelete(wxCommandEvent& WXUNUSED(event)) {

	// Tell the Edit Layer Panel to delete this edit
	wxCommandEvent evt(EDIT_DELETE_EVENT, ID_EDIT_DELETE);
	evt.SetInt(seq);
	wxPostEvent(parWindow, evt);
}

void EditListItem::OnDisable(wxCommandEvent& WXUNUSED(event)) {

	// Enable if this is already disabled
	if (isDisabled) {
		this->SetDisabled(false);
	}

	// Disable if this is enabled
	else {
		this->SetDisabled(true);
	}

	// Tell parent window if this is disabled or enabled
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

void EditListItem::SetHighlighted(bool highlight, bool dimHighlight){

	// Can't drag item that is fixed to top (RAW Processor)
	if (fixedItem) { return; }

	if(highlight == isHighlighted){return;}

	isHighlighted = highlight;

	wxColor backgroundColor = Colors::BackGrey;
	int red = backgroundColor.Red();
	int green = backgroundColor.Green();
	int blue =  backgroundColor.Blue();

	if(highlight){
		backgroundColor.Set(red + 40, green + 40, blue + 40);
	}
	
	this->SetBackgroundColour(backgroundColor);

	upButton->SetBackgroundColour(this->GetBackgroundColour());
	downButton->SetBackgroundColour(this->GetBackgroundColour());
	deleteButton->SetBackgroundColour(this->GetBackgroundColour());
	disableButton->SetBackgroundColour(this->GetBackgroundColour());
	titleText->SetBackgroundColour(this->GetBackgroundColour());
	this->Refresh();
	this->Update();
}

bool EditListItem::GetHighlighted(){
	return isHighlighted;
}

void EditListItem::OnDragStart(wxMouseEvent & WXUNUSED(evt)){

	this->CheckDrag();
}

bool EditListItem::CheckDrag(){

	// Can't drag item that is fixed to top (RAW Processor)
	if (fixedItem) { return false; }

	bool didDrag = false;

	// Set half second timer to highlight and consider drag
	wxTimer timer;
	timer.SetOwner(this, ID_HIGHLIGHT_TIMER);
	timer.Start(500);
	bool mouseLeftWindow = false;

	// While mouse is left button down
	wxMouseState mouse(wxGetMouseState());
	while(mouse.LeftIsDown()){
		
		// Mouse has left window
		if(!this->GetScreenRect().Contains(mouse.GetX(), mouse.GetY())){
			mouseLeftWindow  = true;
		}

		// Edit in list was held down for a while, or mouse left window.  This is a drag event now
		if(highlightTimerFired || mouseLeftWindow){
			this->SetHighlighted(true);
			didDrag = true;
			
			wxCommandEvent evt(EDIT_DRAG_EVENT, ID_EDIT_DRAG);
			evt.SetInt(this->GetId());
			wxPostEvent(this->GetParent(), evt);
		}
		
		// Get current mouse state
		mouse.SetState(wxGetMouseState());
		wxSafeYield();
	}

	// Reset drag / highlight
	highlightTimerFired = false;
	mouseLeftWindow = false;
	this->SetHighlighted(false);
	wxCommandEvent evt(EDIT_DRAG_COMPLETE_EVENT, ID_EDIT_DRAG_COMPLETE);
	wxPostEvent(this->GetParent(), evt);

	return didDrag;
}

void EditListItem::OnTimerFire(wxTimerEvent& WXUNUSED(evt)){
	highlightTimerFired = true;
}

void EditListItem::OnMouseEnter(wxMouseEvent& WXUNUSED(mouseEvent)) {
	this->SetHighlightIfNeeded();
}

void EditListItem::OnMouseLeave(wxMouseEvent& WXUNUSED(mouseEvent)) {
	this->SetHighlightIfNeeded();
}

void EditListItem::OnMouseCheck(wxTimerEvent& WXUNUSED(event)) {
	this->SetHighlightIfNeeded();
}

void EditListItem::SetHighlightIfNeeded() {

	// Do not change highlight if drag and drop is in progress
	if (isHighlighted) { return; }

	wxColor backgroundColor = Colors::BackGrey;
	int red = backgroundColor.Red();
	int green = backgroundColor.Green();
	int blue = backgroundColor.Blue();

	bool oldMouseHighlight = mouseHighlightActive;
	// highlight if mouse is inside
	if (this->GetClientRect().Contains(this->ScreenToClient(wxGetMousePosition()))) {
		backgroundColor.Set(red + 20, green + 20, blue + 20);
		mouseHighlightActive = true;
	}
	else {
		mouseHighlightActive = false;
	}

	// Only update if needed to avoid flicker
	if (mouseHighlightActive != oldMouseHighlight) {
		this->SetBackgroundColour(backgroundColor);

		if (upDownDisableDefined) {
			upButton->SetBackgroundColour(this->GetBackgroundColour());
			downButton->SetBackgroundColour(this->GetBackgroundColour());
			deleteButton->SetBackgroundColour(this->GetBackgroundColour());
			disableButton->SetBackgroundColour(this->GetBackgroundColour());
		}
		titleText->SetBackgroundColour(this->GetBackgroundColour());
		this->Refresh();
		this->Update();
	}
}
