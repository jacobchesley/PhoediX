// Copyright 2018 Jacob Chesley.
// See LICENSE.TXT in root of project for license information.

#ifndef EDIT_LIST_ITEM_H
#define EDIT_LIST_ITEM_H

// for compilers that support precompilation, includes "wx/wx.h"
#include "wx/wxprec.h"

// for all others, include the necessary headers explicitly
#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "GUI/Controls/PhoediXButton/PhoediXButton.h"
#include "GUI/Colors/Colors.h"
#include "GUI/AUIManager/AUIManager.h"
#include "GUI/Controls/PhoediXButton/PhoediXButton.h"
#include "GUI/EditList/EditWindow/EditWindow.h"
#include "GUI/Icons/Icons.h"
#include "Debugging/MemoryLeakCheck.h"

enum {
	ID_EDIT_UP,
	ID_EDIT_DOWN,
	ID_EDIT_DELETE,
	ID_EDIT_DISABLE,
	ID_EDIT_COPY,
	ID_EDIT_PASTE,
	ID_EDIT_DRAG,
	ID_EDIT_DRAG_COMPLETE,
	ID_HIGHLIGHT_TIMER,
	ID_MOUSE_TIMER
};

wxDECLARE_EVENT(EDIT_UP_EVENT, wxCommandEvent);
wxDECLARE_EVENT(EDIT_DOWN_EVENT, wxCommandEvent);
wxDECLARE_EVENT(EDIT_DELETE_EVENT, wxCommandEvent);
wxDECLARE_EVENT(EDIT_DISABLE_EVENT, wxCommandEvent);
wxDECLARE_EVENT(EDIT_COPY_EVENT, wxCommandEvent);
wxDECLARE_EVENT(EDIT_PASTE_EVENT, wxCommandEvent);
wxDECLARE_EVENT(EDIT_DRAG_EVENT, wxCommandEvent);
wxDECLARE_EVENT(EDIT_DRAG_COMPLETE_EVENT, wxCommandEvent);


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

	void SetHighlighted(bool highlight, bool dimHighlight = false);
	bool GetHighlighted();

private:

	void OnRightClick(wxMouseEvent& WXUNUSED(event));
	void OnPopupMenuClick(wxCommandEvent& inEvt);
	void OnUp(wxCommandEvent& WXUNUSED(event));
	void OnDown(wxCommandEvent& WXUNUSED(event));
	void OnDelete(wxCommandEvent& WXUNUSED(event));
	void OnOpenEdit(wxCommandEvent& WXUNUSED(event));
	void OnDisable(wxCommandEvent& WXUNUSED(event));
	void OnDragStart(wxMouseEvent & evt);
	void OnTimerFire(wxTimerEvent& WXUNUSED(event));
	bool CheckDrag();
	void OnMouseEnter(wxMouseEvent& WXUNUSED(mouseEvent));
	void OnMouseLeave(wxMouseEvent& WXUNUSED(mouseEvent));
	void OnMouseCheck(wxTimerEvent& WXUNUSED(event));
	void SetHighlightIfNeeded();
	
	wxWindow * parWindow;
	wxBoxSizer * sizer;
	wxBoxSizer * textSizer;
	wxBoxSizer * upDownButtonSizer;

	wxButton * disableButton;
	PhoediXButton * titleText;
	wxButton * upButton;
	wxButton * downButton;
	wxButton * deleteButton;
	bool upDownDisableDefined;

	EditWindow * editWin;

	size_t seq;
	bool isDisabled;
	bool isHighlighted;
	bool highlightTimerFired;
	bool fixedItem;

	wxTimer mouseTimer;
	bool mouseHighlightActive;

	enum Buttons{
		OPEN_EDIT_BUTTON,
		UP_BUTTON,
		DOWN_BUTTON,
		DELETE_BUTTON,
		DISABLE_BUTTON
	};

	enum PopupMenuActions{
		COPY_EDIT_PARAMS = 1,
		PASTE_EDIT_PARAMS

	};
};
#endif