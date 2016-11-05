// Copyright 2016 Jacob Chesley

#ifndef EDIT_LIST_ITEM_H
#define EDIT_LIST_ITEM_H

// for compilers that support precompilation, includes "wx/wx.h"
#include "wx/wxprec.h"

// for all others, include the necessary headers explicitly
#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "GUI\Colors\Colors.h"
#include "GUI\AUI Manager\AUIManager.h"
#include "GUI\EditList\EditWindow\EditWindow.h"
#include "GUI\Icons\Icons.h"
#include "Debugging\MemoryLeakCheck.h"

enum {
	ID_EDIT_UP,
	ID_EDIT_DOWN,
	ID_EDIT_DELETE,
	ID_EDIT_DISABLE,
	ID_EDIT_COPY,
	ID_EDIT_PASTE
};

wxDECLARE_EVENT(EDIT_UP_EVENT, wxCommandEvent);
wxDECLARE_EVENT(EDIT_DOWN_EVENT, wxCommandEvent);
wxDECLARE_EVENT(EDIT_DELETE_EVENT, wxCommandEvent);
wxDECLARE_EVENT(EDIT_DISABLE_EVENT, wxCommandEvent);
wxDECLARE_EVENT(EDIT_COPY_EVENT, wxCommandEvent);
wxDECLARE_EVENT(EDIT_PASTE_EVENT, wxCommandEvent);

class EditListItem : public wxPanel {
public:
	EditListItem(wxWindow * parent, wxString title = wxEmptyString, int Sequence = 0, EditWindow * editWindow = NULL, bool disableButtons = false);
	~EditListItem();

	EditWindow* GetEditWindow();

	void DestroyItem();

	void SetTitle(wxString title);
	wxString GetTitle();

	void SetSequence(size_t Sequence);
	int GetSequence();

	void SetDisabled(bool disable);
	bool GetDisabled();

private:

	void OnRightClick(wxMouseEvent& WXUNUSED(event));
	void OnPopupMenuClick(wxCommandEvent& inEvt);
	void OnUp(wxCommandEvent& WXUNUSED(event));
	void OnDown(wxCommandEvent& WXUNUSED(event));
	void OnDelete(wxCommandEvent& WXUNUSED(event));
	void OnOpenEdit(wxCommandEvent& WXUNUSED(event));
	void OnDisable(wxCommandEvent& WXUNUSED(event));

	wxWindow * parWindow;
	wxBoxSizer * sizer;
	wxBoxSizer * textSizer;
	wxBoxSizer * upDownButtonSizer;

	wxButton * disableButton;
	wxButton * titleText;
	wxButton * upButton;
	wxButton * downButton;
	wxButton * deleteButton;

	EditWindow * editWin;

	size_t seq;
	bool isDisabled;

	enum Buttons{
		OPEN_EDIT_BUTTON,
		UP_BUTTON,
		DOWN_BUTTON,
		DELETE_BUTTON,
		DISABLE_BUTTON
	};

	enum PopupMenuActions{
		COPY_EDIT_PARAMS,
		PASTE_EDIT_PARAMS

	};
};
#endif