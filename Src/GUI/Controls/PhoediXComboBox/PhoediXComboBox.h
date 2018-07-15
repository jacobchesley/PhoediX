// Copyright 2016 Jacob Chesley

#ifndef PHOEDIX_COMBOBOX_H
#define PHOEDIX_COMBOBOX_H

// for compilers that support precompilation, includes "wx/wx.h"
#include "wx/wxprec.h"

// for all others, include the necessary headers explicitly
#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "Debugging/MemoryLeakCheck.h"

class PhoediXComboBox : public wxComboBox {
public:

	PhoediXComboBox(wxWindow * parent, wxWindowID id);

private:
	void OnText(wxKeyEvent& evt);
	bool focused;
};

#endif