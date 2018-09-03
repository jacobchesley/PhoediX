// Copyright 2018 Jacob Chesley.
// See LICENSE.TXT in root of project for license information.

#ifndef ABOUT_WINDOW_H
#define ABOUT_WINDOW_H

// for compilers that support precompilation, includes "wx/wx.h"
#include "wx/wxprec.h"

// for all others, include the necessary headers explicitly
#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "GUI/Controls/PhoediXButton/PhoediXButton.h"
#include "GUI/Icons/Icons.h"
#include "GUI/Colors/Colors.h"

class AboutWindow : public wxFrame {

public:
	AboutWindow(wxWindow * parent);
private:

	void OnClose(wxCloseEvent& WXUNUSED(evt));
	void OnOkay(wxCommandEvent& WXUNUSED(evt));
	void OnWebsiteMouseEnter(wxMouseEvent& WXUNUSED(evt));
	void OnWebsiteMouseLeave(wxMouseEvent& WXUNUSED(evt));

	void OnPhoediXWebsiteClick(wxMouseEvent& WXUNUSED(evt));
	void OnwxWidgetsWebsiteClick(wxMouseEvent& WXUNUSED(evt));
	void OnLibRawWebsiteClick(wxMouseEvent& WXUNUSED(evt));
	void OnLibTiffWebsiteClick(wxMouseEvent& WXUNUSED(evt));

	wxBoxSizer * mainSizer;
	wxFlexGridSizer * aboutSizer;

	wxStaticText * aboutPhoediX;
	wxStaticText * applicationBit;

	wxStaticText * versionTitle;
	wxStaticText * versionValue;

	wxStaticText * buildDateTitle;
	wxStaticText * buildDateValue;

	wxStaticText * authorTitle;
	wxStaticText * authorValue;

	wxStaticText * websiteTitle;
	wxStaticText * websiteValue;

	wxFlexGridSizer * libraryAboutSizer;
	wxStaticText * aboutLibraries;

	wxStaticText * wxWidgetsVersionTitle;
	wxStaticText * wxWidgetsVersionValue;

	wxStaticText * librawVersionTitle;
	wxStaticText * librawVersionValue;

	wxStaticText * libtiffVersionTitle;
	wxStaticText * libtiffVersionValue;

	PhoediXButton * okayButton;

	enum{
		ID_OKAY
	};
	
};

#endif