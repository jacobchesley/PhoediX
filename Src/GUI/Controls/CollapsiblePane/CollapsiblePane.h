// Copyright 2016 Jacob Chesley

#ifndef COLLAPSIBLE_PANE_H
#define COLLAPSIBLE_PANE_H

#include "wx/wxprec.h"

#ifndef WX_PRECOMP	
#include "wx/wx.h"
#endif

#include "GUI/Controls/PhoediXButton/PhoediXButton.h"
#include "wx/dc.h"
#include "wx/dcbuffer.h"

wxDECLARE_EVENT(COLLAPSE_PANE_OPEN_CLOSE, wxCommandEvent);

enum {
	ID_COLLAPSE_PANE_OPEN_CLOSE
};

class CollapsiblePane : public wxPanel {

public:
	CollapsiblePane(wxWindow * parent, wxString name);
	void AttachWindow(wxWindow * attach);
	void SetTextBackgroundColour(wxColour color);
	void SetTextForegroundColour(wxColour color);
	void SetTextFont(wxFont font);
	void Collapse();
	void Open();
	bool IsOpen();

private:

	void OnCollapse(wxCommandEvent& WXUNUSED(event));

	wxBoxSizer * mainSizer;
	wxBoxSizer * buttonSizer;
	wxBoxSizer * indentAndWindowSizer;
	PhoediXButton * collapseButton;
	wxWindow * attachedWindow;
	wxWindow * parentWindow;

	bool windowAttached;
	bool isDisplayed;

	enum Button {
		COLLAPSE
	};

	class CollapseArrow : public wxPanel{
	public:
		CollapseArrow(wxWindow * parent);
		void Expand();
		void Collapse();
		void PaintNow();
		void SetArrowSize(int size);
		void SetOffset(int x, int y);

	private:
		void OnPaint(wxPaintEvent& WXUNUSED(evt));
		void Render(wxDC& dc);
		bool isCollapsed;
		int arrowSize;
		int xOffset;
		int yOffset;

	};
	CollapseArrow * arrow;
};
#endif