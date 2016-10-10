#ifndef LIBRARY_IMAGE_H
#define LIBRARY_IMAGE_H

// for compilers that support precompilation, includes "wx/wx.h"
#include "wx/wxprec.h"

// for all others, include the necessary headers explicitly
#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "GUI\ImageDisplay\wxImagePanel\wxImagePanel.h"

class LibraryImage : public wxPanel {

public:
	LibraryImage(wxWindow * parent, wxImage * img);
	void ChangeImage(wxImage * newImage);
	void SetName(wxString name);
	wxString GetName();
	void SetPath(wxString path);
	wxString GetPath();

	bool GetSelected();
	wxImage * GetImage();

private:

	wxImage * img;
	wxBoxSizer * mainLayout;
	wxBoxSizer * subLayout;
	WXImagePanel * imageDisplay;
	wxCheckBox * selectBox;
	wxStaticText * nameDisplay;
	wxString path;

};

#endif