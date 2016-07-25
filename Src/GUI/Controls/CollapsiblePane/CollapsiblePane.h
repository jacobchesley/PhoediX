#ifndef COLLAPSIBLE_PANE_H
#define COLLAPSIBLE_PANE_H

#include "wx/wxprec.h"

#ifndef WX_PRECOMP	
#include "wx/wx.h"
#endif

class CollapsiblePane : public wxPanel {

public:
	CollapsiblePane(wxWindow * parent, wxString name);
	void AttachWindow(wxWindow * attach);
	void SetTextBackgroundColour(wxColour color);
	void SetTextForegroundColour(wxColour color);
	void SetTextFont(wxFont font);

private:

	void OnCollapse(wxCommandEvent& WXUNUSED(event));

	wxBoxSizer * mainSizer;
	wxBoxSizer * indentAndWindowSizer;
	wxButton * collapseButton;
	wxWindow * attachedWindow;
	wxWindow * parentWindow;

	bool windowAttached;
	bool isDisplayed;

	enum Button {
		COLLAPSE
	};
};
#endif