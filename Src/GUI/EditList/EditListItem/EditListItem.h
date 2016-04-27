#ifndef EDIT_LIST_ITEM_H
#define EDIT_LIST_ITEM_H

// for compilers that support precompilation, includes "wx/wx.h"
#include "wx/wxprec.h"

// for all others, include the necessary headers explicitly
#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "GUI\Colors\Colors.h"
#include "Debugging\MemoryLeakCheck.h"

enum {
	ID_EDIT_UP,
	ID_EDIT_DOWN,
	ID_EDIT_DELETE
};

wxDECLARE_EVENT(EDIT_UP_EVENT, wxCommandEvent);
wxDECLARE_EVENT(EDIT_DOWN_EVENT, wxCommandEvent);
wxDECLARE_EVENT(EDIT_DELETE_EVENT, wxCommandEvent);


class EditListItem : public wxPanel {
public:
	EditListItem(wxWindow * parent, wxString title = wxEmptyString, int Sequence = 0);

	void SetTitle(wxString title);
	wxString GetTitle();

	void SetSequence(size_t Sequence);
	int GetSequence();

private:

	void OnUp(wxCommandEvent& WXUNUSED(event));
	void OnDown(wxCommandEvent& WXUNUSED(event));
	void OnDelete(wxCommandEvent& WXUNUSED(event));
	
	wxWindow * parWindow;
	wxBoxSizer * sizer;
	wxBoxSizer * textSizer;
	wxBoxSizer * upDownButtonSizer;

	wxStaticText * titleText;
	wxButton * upButton;
	wxButton * downButton;
	wxButton * deleteButton;

	size_t seq;

	enum Buttons{
		UP_BUTTON,
		DOWN_BUTTON,
		DELETE_BUTTON
	};
};
#endif