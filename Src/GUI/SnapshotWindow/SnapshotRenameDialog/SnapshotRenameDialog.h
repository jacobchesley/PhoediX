// Copyright 2018 Jacob Chesley.
// See LICENSE.TXT in root of project for license information.

#ifndef SNAPSHOT_RENAME_DIALOG_H
#define SNAPSHOT_RENAME_DIALOG_H

// for compilers that support precompilation, includes "wx/wx.h"
#include "wx/wxprec.h"

// for all others, include the necessary headers explicitly
#ifndef WX_PRECOMP
	#include "wx/wx.h"
#endif

#include "GUI/Controls/PhoediXButton/PhoediXButton.h"
#include "GUI/Colors/Colors.h"

class SnapshotRenameDialog : public wxDialog{
public:
	SnapshotRenameDialog(wxWindow * parent, wxString oldSnapshotName);
	wxString GetNewName();

private:

	wxBoxSizer * mainSizer;
	wxStaticText * message;
	wxTextCtrl * newName;

	wxBoxSizer * buttonSizer;
	PhoediXButton * cancelButton;
	PhoediXButton * okayButton;

};

#endif