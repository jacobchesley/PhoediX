// Copyright 2016 Jacob Chesley

#ifndef DIRECTORY_SELECTIONS_H
#define DIRECTORY_SELECTIONS_H

// for compilers that support precompilation, includes "wx/wx.h"
#include "wx/wxprec.h"

// for all others, include the necessary headers explicitly
#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "wx/dir.h"

#include "GUI/Controls/PhoediXButton/PhoediXButton.h"
#include "GUI/Colors/Colors.h"
#include "GUI/AUIManager/AUIManager.h"
#include "GUI/Icons/Icons.h"
#include "Debugging/MemoryLeakCheck.h"

enum {
	ID_DELETE_EVT,
	ID_DIR_EXISTS,
	ID_NO_DIR_EXISTS
};

wxDECLARE_EVENT(DELETE_DIR_EVENT, wxCommandEvent);
wxDECLARE_EVENT(DIR_EXISTS, wxCommandEvent);
wxDECLARE_EVENT(NO_DIR_EXISTS, wxCommandEvent);

class DirectorySelections : public wxScrolledWindow {
public:

	DirectorySelections(wxWindow * parent);
	wxVector<wxString> GetDirectoryList();

	void SendDirectoriesExist();
	void SendNoDirectoriesExist();

private:
	wxBoxSizer * mainLayout;
	PhoediXButton * addDirectory;
	wxWindow * par;

	void OnAddDirectory(wxCommandEvent& WXUNUSED(evt));
	void AddDirectoryDisplayItem();
	void RedrawPanel();
	void OnDeleteDir(wxCommandEvent& evt);

	enum {
		ID_ADD_DIRECTORY
	};


	class DirectoryDisplayItem : public wxPanel {
	public:
		DirectoryDisplayItem(DirectorySelections * parent);
		wxVector<wxString> GetDirectoriesNames();
		void SetSequence(int sequence);
		int GetSequence();

		enum {
			DELETE_EVT
		};

	private:

		void OnShowDirectory(wxCommandEvent & WXUNUSED(evt));
		void OnDelete(wxCommandEvent & WXUNUSED(evt));
		void OnText(wxCommandEvent& WXUNUSED(evt));

		int seq;

		DirectorySelections * dirSelects;
		wxBoxSizer * mainLayout;
		wxTextCtrl * directoryText;
		PhoediXButton * directoryButton;
		wxCheckBox * subDirectoriesCheck;
		wxStaticText * subDirectoriesLabel;
		wxButton * deleteButton;
		enum {
			ID_SHOW_DIRECTORY,
			ID_DELETE
		};
	};
	wxVector<DirectoryDisplayItem*> directoryItems;

};
#endif