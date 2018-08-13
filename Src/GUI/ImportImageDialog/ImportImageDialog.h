// Copyright 2018 Jacob Chesley.
// See LICENSE.TXT in root of project for license information.

#ifndef IMPORT_IMAGE_DIALOG_H
#define IMPORT_IMAGE_DIALOG_H

// for compilers that support precompilation, includes "wx/wx.h"
#include "wx/wxprec.h"

// for all others, include the necessary headers explicitly
#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "GUI/Controls/PhoediXButton/PhoediXButton.h"
#include "GUI/Colors/Colors.h"

class ImportImageDialog : public wxDialog {
public:
	ImportImageDialog(wxWindow * parent);

	enum {
		ID_IMPORT_CURRENT_PROJECT,
		ID_IMPORT_NEW_PROJECT
	};
private:

	void OnButton(wxCommandEvent & evt);

	wxBoxSizer * mainSizer;
	wxStaticText * message;

	wxBoxSizer * buttonSizer;
	PhoediXButton * currentButton;
	PhoediXButton * newButton;
};

#endif
