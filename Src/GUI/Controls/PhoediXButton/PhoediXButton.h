// Copyright 2016 Jacob Chesley

#ifndef PHOEDIX_BUTTON_H
#define PHOEDIX_BUTTON_H

// for compilers that support precompilation, includes "wx/wx.h"
#include "wx/wxprec.h"

// for all others, include the necessary headers explicitly
#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "Debugging/MemoryLeakCheck.h"

class PhoediXButton : public wxPanel {
public:

	PhoediXButton(wxWindow * parent, wxWindowID id, const wxString& label = wxEmptyString);
	void SetBackgroundColour(wxColor newColor);
	void SetForegroundColour(wxColor newColor);
	bool SetFont(const wxFont &font);

private:

	wxBoxSizer * sizer;
	wxBoxSizer * outerSizer;
	wxStaticText * text;
	void LeftDown(wxCommandEvent& WXUNUSED(clickEvent));
	void RightDown(wxCommandEvent& WXUNUSED(clickEvent));
};

#endif