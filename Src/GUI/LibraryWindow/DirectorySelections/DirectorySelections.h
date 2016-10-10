#ifndef DIRECTORY_SELECTIONS_H
#define DIRECTORY_SELECTIONS_H

// for compilers that support precompilation, includes "wx/wx.h"
#include "wx/wxprec.h"

// for all others, include the necessary headers explicitly
#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "GUI\Colors\Colors.h"
#include "GUI\AUI Manager\AUIManager.h"
#include "GUI\Icons\Icons.h"
#include "Debugging\MemoryLeakCheck.h"

enum {
	ID_DELETE_EVT

};

wxDECLARE_EVENT(DELETE_DIR_EVENT, wxCommandEvent);


class DirectorySelections : public wxScrolledWindow {
public:

	DirectorySelections(wxWindow * parent);
	wxVector<wxString> GetDirectoryList();

private:
	wxBoxSizer * mainLayout;
	
	wxButton * addDirectory;
	
	void OnAddDirectory(wxCommandEvent& WXUNUSED(evt));
	void AddDirectoryDisplayItem();
	void RedrawPanel();
	void OnDeleteDir(wxCommandEvent& evt);

	enum {
		ID_ADD_DIRECTORY
	};


	class DirectoryDisplayItem : public wxPanel {
	public:
		DirectoryDisplayItem(wxWindow * parent);
		wxString GetDirectoryName();
		void SetSequence(int sequence);
		int GetSequence();

		enum {
			DELETE_EVT
		};

	private:

		void OnShowDirectory(wxCommandEvent & WXUNUSED(evt));
		void OnDelete(wxCommandEvent & WXUNUSED(evt));
		int seq;

		wxWindow * parWindow;
		wxBoxSizer * mainLayout;
		wxTextCtrl * directoryText;
		wxButton * directoryButton;
		wxButton * deleteButton;
		enum {
			ID_SHOW_DIRECTORY,
			ID_DELETE
		};
	};
	wxVector<DirectoryDisplayItem*> directoryItems;

};
#endif