// Copyright 2018 Jacob Chesley.
// See LICENSE.TXT in root of project for license information.

#include "ZoomImageFrame.h"

ZoomImageFrame::ZoomImageFrame(wxWindow * parent, wxString title) : wxFrame(parent, -1, title){

	Icons icons;
	wxIcon theIcon;
	theIcon.CopyFromBitmap(wxBitmap(icons.pxIcon));
	this->SetIcon(theIcon);

	this->SetBackgroundColour(parent->GetBackgroundColour());
	mainSizer = new wxBoxSizer(wxVERTICAL);
	this->SetSizer(mainSizer);

	imageDisplay = new ZoomImagePanel(this);
	this->GetSizer()->Add(imageDisplay, 0, wxEXPAND);
	this->GetSizer()->Layout();

	imageDisplay->FitImage();
	imageDisplay->SetFitImage(true);
	imageDisplay->SetZoom(100.0);
}

ZoomImageFrame::ZoomImageFrame(wxWindow * parent, wxString title, wxImage * image) : wxFrame(parent, -1, title){

	Icons icons;
	wxIcon theIcon;
	theIcon.CopyFromBitmap(wxBitmap(icons.pxIcon));
	this->SetIcon(theIcon);

	this->SetBackgroundColour(parent->GetBackgroundColour());
	mainSizer = new wxBoxSizer(wxVERTICAL);
	this->SetSizer(mainSizer);

	imageDisplay = new ZoomImagePanel(this, image);
	this->GetSizer()->Add(imageDisplay, 1, wxEXPAND);
	this->GetSizer()->Layout();

	imageDisplay->FitImage();
	imageDisplay->SetFitImage(true);
	imageDisplay->SetZoom(100.0);
}

ZoomImageFrame::ZoomImageFrame(wxWindow * parent, wxString title, Image * image) : wxFrame(parent, -1, title){

	Icons icons;
	wxIcon theIcon;
	theIcon.CopyFromBitmap(wxBitmap(icons.pxIcon));
	this->SetIcon(theIcon);

	this->SetBackgroundColour(parent->GetBackgroundColour());
	mainSizer = new wxBoxSizer(wxVERTICAL);
	this->SetSizer(mainSizer);

	imageDisplay = new ZoomImagePanel(this, image);
	this->GetSizer()->Add(imageDisplay, 0, wxEXPAND);
	this->GetSizer()->Layout();

	imageDisplay->FitImage();
	imageDisplay->SetFitImage(true);
	imageDisplay->SetZoom(100.0);
}