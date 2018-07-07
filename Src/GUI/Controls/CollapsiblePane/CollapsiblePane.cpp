// Copyright 2016 Jacob Chesley

#include "CollapsiblePane.h"
#include "GUI/Colors/Colors.h"

CollapsiblePane::CollapsiblePane(wxWindow * parent, wxString name) : wxPanel(parent) {

	parentWindow = parent;
	this->SetBackgroundColour(parent->GetBackgroundColour());

	mainSizer = new wxBoxSizer(wxVERTICAL);
	buttonSizer = new wxBoxSizer(wxHORIZONTAL);
	indentAndWindowSizer = new wxBoxSizer(wxHORIZONTAL);

	this->SetSizer(mainSizer);

	collapseButton = new PhoediXButton(this, CollapsiblePane::Button::COLLAPSE, name);
	arrow = new CollapseArrow(this);
	arrow->SetBackgroundColour(this->GetBackgroundColour());
	arrow->SetArrowSize(10);
	arrow->SetOffset(0, 12);

	buttonSizer->Add(collapseButton);
	buttonSizer->AddSpacer(8);
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

	// Size button to fit text exactly
	wxStaticText * tempStaticText = new wxStaticText(this, -1, collapseButton->GetLabel());
	tempStaticText->SetFont(font);
	wxSize size = tempStaticText->GetTextExtent(collapseButton->GetLabel());
	collapseButton->SetMinSize(wxSize(size.x + 16, size.y + 30));
	tempStaticText->Destroy();
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

	this->SetSize(wxSize(arrowSize * 2, arrowSize * 2));
	this->SetMinSize(wxSize(arrowSize * 2, arrowSize * 2));
	this->SetMaxSize(wxSize(arrowSize * 2, arrowSize * 2));
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

	this->SetSize(wxSize(arrowSize * 2.5, arrowSize * 2.5));
	this->SetMinSize(wxSize(arrowSize * 2.5, arrowSize * 2.5));
	this->SetMaxSize(wxSize(arrowSize * 2.5, arrowSize * 2.5));
}

void CollapsiblePane::CollapseArrow::SetOffset(int x, int y){
	xOffset = x;
	yOffset = y;
}
void CollapsiblePane::CollapseArrow::Render(wxDC& dc){

	wxPointList trianglePoints;
	wxPen outline;
	wxBrush fill;
	
	// Draw hollow triangle pointing to the right
	if(isCollapsed){
		wxPoint p1(0,0);
		wxPoint p2(0,arrowSize);
		wxPoint p3(arrowSize, arrowSize/2);
		wxPoint p4(0,0);
		trianglePoints.Append(&p1); 
		trianglePoints.Append(&p2); 
		trianglePoints.Append(&p3);
		trianglePoints.Append(&p4);
		outline.SetColour(Colors::TextLightGrey);
		fill.SetColour(this->GetBackgroundColour());
	}

	// Draw solid triangle pointing down
	else{
		wxPoint p1(0,0);
		wxPoint p2(arrowSize,0);
		wxPoint p3(arrowSize/2, arrowSize);
		wxPoint p4(0,0);
		trianglePoints.Append(&p1); 
		trianglePoints.Append(&p2); 
		trianglePoints.Append(&p3);
		trianglePoints.Append(&p4);
		outline.SetColour(Colors::TextLightGrey);
		fill.SetColour(Colors::TextLightGrey);
	}

	// Clear DC, set brushes and draw triangle
	dc.Clear();
	dc.SetBackground(wxBrush(this->GetBackgroundColour()));
	dc.SetPen(outline);
	dc.SetBrush(fill);
	dc.DrawPolygon(&trianglePoints, xOffset, yOffset);
}