// Copyright 2018 Jacob Chesley.
// See LICENSE.TXT in root of project for license information.

#include "CollapsiblePane.h"
#include "GUI/Colors/Colors.h"

wxDEFINE_EVENT(COLLAPSE_PANE_OPEN_CLOSE, wxCommandEvent);

CollapsiblePane::CollapsiblePane(wxWindow * parent, wxString name) : wxPanel(parent) {

	parentWindow = parent;
	this->SetBackgroundColour(parent->GetBackgroundColour());

	mainSizer = new wxBoxSizer(wxVERTICAL);
	buttonSizer = new wxBoxSizer(wxHORIZONTAL);
	indentAndWindowSizer = new wxBoxSizer(wxHORIZONTAL);

	this->SetSizer(mainSizer);

	collapseButton = new PhoediXButton(this, CollapsiblePane::Button::COLLAPSE, name);
	collapseButton->SetBorder(10, 2);
	arrow = new CollapseArrow(this);
	arrow->SetBackgroundColour(this->GetBackgroundColour());
	arrow->SetArrowSize(10);
	arrow->SetOffset(0, 4);

	buttonSizer->Add(collapseButton);
	buttonSizer->AddSpacer(4);
	buttonSizer->Add(arrow, 1, wxEXPAND);

	this->GetSizer()->Add(buttonSizer, 0, wxALIGN_LEFT);
	this->GetSizer()->AddSpacer(4);
	this->GetSizer()->Add(indentAndWindowSizer, 1, wxEXPAND);

	this->Bind(wxEVT_BUTTON, (wxObjectEventFunction)&CollapsiblePane::OnCollapse, this, CollapsiblePane::Button::COLLAPSE);
	attachedWindow = NULL;

	isDisplayed = parent->IsShown();
}

void CollapsiblePane::OnCollapse(wxCommandEvent& WXUNUSED(event)) {

	// Hide window if it is already displayed
	if (isDisplayed) {
		this->Collapse();
	}

	// Show window if it is hidden
	else {
		this->Open();
	}
}

void CollapsiblePane::Collapse() {

	// Hide window if it is already displayed
	if (isDisplayed) {
		isDisplayed = false;
		if (attachedWindow != NULL) {
			attachedWindow->Hide();
		}
	}
	this->Layout();
	this->Fit();

	parentWindow->Layout();
	parentWindow->FitInside();
	arrow->Collapse();

	wxCommandEvent evt(COLLAPSE_PANE_OPEN_CLOSE, ID_COLLAPSE_PANE_OPEN_CLOSE);
	wxPostEvent(parentWindow, evt);
}

void CollapsiblePane::Open() {

	// Show window if it is hidden
	if(!isDisplayed){
		isDisplayed = true;
		if (attachedWindow != NULL) {
			attachedWindow->Show();
		}
	}
	this->Layout();
	this->Fit();

	parentWindow->Layout();
	parentWindow->FitInside();
	arrow->Expand();

	wxCommandEvent evt(COLLAPSE_PANE_OPEN_CLOSE, ID_COLLAPSE_PANE_OPEN_CLOSE);
	wxPostEvent(parentWindow, evt);
}

bool CollapsiblePane::IsOpen() {
	return isDisplayed;
}

void CollapsiblePane::SetTextBackgroundColour(wxColour color) {
	collapseButton->SetBackgroundColour(color);
}

void CollapsiblePane::SetTextForegroundColour(wxColour color) {
	collapseButton->SetForegroundColour(color);
}

void CollapsiblePane::SetTextFont(wxFont font) {

	// Set button font
	collapseButton->SetFont(font);
}

void CollapsiblePane::AttachWindow(wxWindow * attach) {
	indentAndWindowSizer->AddSpacer(20);
	indentAndWindowSizer->Add(attach, 1, wxEXPAND);
	attachedWindow = attach;
}

CollapsiblePane::CollapseArrow::CollapseArrow(wxWindow * parent) : wxPanel(parent){
	isCollapsed = true;
	this->Bind(wxEVT_PAINT, (wxObjectEventFunction)&CollapsiblePane::CollapseArrow::OnPaint, this);
	arrowSize = 10;
	xOffset = 0;
	yOffset = 0;

	this->SetSize(wxSize(arrowSize * 1.5, arrowSize * 1.5));
	this->SetMinSize(wxSize(arrowSize * 1.5, arrowSize * 1.5));
	this->SetMaxSize(wxSize(arrowSize * 1.5, arrowSize * 1.5));
}

void CollapsiblePane::CollapseArrow::Expand(){
	isCollapsed = false;
	this->PaintNow();
}

void CollapsiblePane::CollapseArrow::Collapse(){
	isCollapsed = true;
	this->PaintNow();
}

void CollapsiblePane::CollapseArrow::OnPaint(wxPaintEvent& WXUNUSED(evt)){
	wxPaintDC dc(this);
    this->Render(dc);
}

void CollapsiblePane::CollapseArrow::PaintNow(){

	wxClientDC dc(this);
	this->Render(dc);
}

void CollapsiblePane::CollapseArrow::SetArrowSize(int size){
	arrowSize = size;

	this->SetSize(wxSize(arrowSize * 1.5, arrowSize * 1.5));
	this->SetMinSize(wxSize(arrowSize * 1.5, arrowSize * 1.5));
	this->SetMaxSize(wxSize(arrowSize * 1.5, arrowSize * 1.5));
}

void CollapsiblePane::CollapseArrow::SetOffset(int x, int y){
	xOffset = x;
	yOffset = y;
}
void CollapsiblePane::CollapseArrow::Render(wxDC& dc){

	wxPointList trianglePoints;
	wxPen outline;
	wxBrush fill;
	
	wxPoint p0;
	wxPoint p1;
	wxPoint p2;
	wxPoint p3;
	
	// Draw hollow triangle pointing to the right
	if(isCollapsed){
		p0 = wxPoint(0,0);
		p1 = wxPoint(0,arrowSize);
		p2 = wxPoint(arrowSize, arrowSize/2);
		p3 = wxPoint(0,0);

		outline.SetColour(Colors::TextLightGrey);
		fill.SetColour(this->GetBackgroundColour());
	}

	// Draw solid triangle pointing down
	else{
		p0 = wxPoint(0,0);
		p1 = wxPoint(arrowSize,0);
		p2 = wxPoint(arrowSize/2, arrowSize);
		p3 = wxPoint(0,0);
		
		outline.SetColour(Colors::TextLightGrey);
		fill.SetColour(Colors::TextLightGrey);
	}

	// Create list of points to draw polygon
	trianglePoints.Append(&p0);
	trianglePoints.Append(&p1);
	trianglePoints.Append(&p2);
	trianglePoints.Append(&p3);

	// Clear DC, set brushes and draw triangle
	dc.Clear();
	dc.SetBackground(wxBrush(this->GetBackgroundColour()));
	dc.SetPen(outline);
	dc.SetBrush(fill);
	dc.DrawPolygon(&trianglePoints, xOffset, yOffset);
}