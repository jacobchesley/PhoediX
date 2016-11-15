// Copyright 2016 Jacob Chesley

#ifndef SNAPSHOT_RENAME_DIALOG_H
#define SNAPSHOT_RENAME_DIALOG_H

// for compilers that support precompilation, includes "wx/wx.h"
#include "wx/wxprec.h"

// for all others, include the necessary headers explicitly
#ifndef WX_PRECOMP
	#include "wx/wx.h"
#endif

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
	wxButton * cancelButton;
	wxButton * okayButton;

};

#endif