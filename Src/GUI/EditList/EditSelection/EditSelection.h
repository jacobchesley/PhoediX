#ifndef EDIT_SELECTION_H
#define EDIT_SELECTION_H

// for compilers that support precompilation, includes "wx/wx.h"
#include "wx/wxprec.h"

// for all others, include the necessary headers explicitly
#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "GUI\EditList\AvailableEdits\AvailableEdits.h"
#include "wx\listctrl.h"
#include "GUI\Colors\Colors.h"
#include "Debugging\MemoryLeakCheck.h"

enum {
	ID_EDIT_ADD
};

wxDECLARE_EVENT(EDIT_ADD_EVENT, wxCommandEvent);

class EditSelection : public wxScrolledWindow {

public:
	EditSelection(wxWindow * parent);
	void AddEditSelection(wxString editName, wxString editDescription);

private:
	void OnClose(wxCommandEvent& WXUNUSED(event));
	void OnAdd(wxCommandEvent& WXUNUSED(event));

	wxBoxSizer * mainSizer;
	wxListCtrl * editList;
	wxButton * addButton;

	wxWindow * parWindow;

	enum Buttons {
		ADD_EDIT
	};
};
#endif