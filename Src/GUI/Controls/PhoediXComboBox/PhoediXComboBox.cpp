// Copyright 2016 Jacob Chesley

#include "PhoediXComboBox.h"

PhoediXComboBox::PhoediXComboBox(wxWindow * parent, wxWindowID id) : wxComboBox(parent, id) {
	this->Bind(wxEVT_TEXT, (wxObjectEventFunction)&PhoediXComboBox::OnText, this);
	this->Bind(wxEVT_CHAR, (wxObjectEventFunction)&PhoediXComboBox::OnText, this);
	this->Bind(wxEVT_COMMAND_TEXT_CUT, (wxObjectEventFunction)&PhoediXComboBox::OnText, this);
	focused = false;
}

void PhoediXComboBox::OnText(wxKeyEvent& evt) {

	int keyCode = evt.GetKeyCode();
	if (keyCode == WXK_DOWN || keyCode == WXK_UP) {
		evt.Skip(true);
	}
	else {
		evt.Skip(false);
	}
}
