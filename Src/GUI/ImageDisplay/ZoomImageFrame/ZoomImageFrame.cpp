// Copyright 2016 Jacob Chesley

#include "ZoomImageFrame.h"

ZoomImageFrame::ZoomImageFrame(wxWindow * parent, wxString title) : wxFrame(parent, -1, title){
	this->SetBackgroundColour(parent->GetBackgroundColour());
	mainSizer = new wxBoxSizer(wxVERTICAL);
	this->SetSizer(mainSizer);

	imageDisplay = new ZoomImagePanel(this);
	this->GetSizer()->Add(imageDisplay, 0, wxEXPAND);
	this->GetSizer()->Layout();

	imageDisplay->FitImage();
}

ZoomImageFrame::ZoomImageFrame(wxWindow * parent, wxString title, wxImage * image) : wxFrame(parent, -1, title){
	this->SetBackgroundColour(parent->GetBackgroundColour());
	mainSizer = new wxBoxSizer(wxVERTICAL);
	this->SetSizer(mainSizer);

	imageDisplay = new ZoomImagePanel(this, image);
	this->GetSizer()->Add(imageDisplay, 1, wxEXPAND);
	this->GetSizer()->Layout();

	imageDisplay->FitImage();
}

ZoomImageFrame::ZoomImageFrame(wxWindow * parent, wxString title, Image * image) : wxFrame(parent, -1, title){
	this->SetBackgroundColour(parent->GetBackgroundColour());
	mainSizer = new wxBoxSizer(wxVERTICAL);
	this->SetSizer(mainSizer);

	imageDisplay = new ZoomImagePanel(this, image);
	this->GetSizer()->Add(imageDisplay, 0, wxEXPAND);
	this->GetSizer()->Layout();

	imageDisplay->FitImage();
}