// Copyright 2016 Jacob Chesley

#include "PixelPeepWindow.h"

PixelPeepWindow::PixelPeepWindow(wxWindow * parent) : wxScrolledWindow(parent){

	this->FitInside();
	this->SetScrollRate(5, 5);

	this->SetClientSize(this->GetVirtualSize());
}