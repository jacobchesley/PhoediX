// Copyright 2016 Jacob Chesley

#ifndef LIBRARY_IMAGE_H
#define LIBRARY_IMAGE_H

// for compilers that support precompilation, includes "wx/wx.h"
#include "wx/wxprec.h"

// for all others, include the necessary headers explicitly
#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "GUI/Colors/Colors.h"
#include "GUI/ImageDisplay/wxImagePanel/wxImagePanel.h"
#include "GUI/ImageDisplay/ZoomImageFrame/ZoomImageFrame.h"
#include "Processing/ImageHandler/ImageHandler.h"

class LibraryImage : public wxPanel {

public:
	LibraryImage(wxWindow * parent, wxImage * img, wxString fileName, wxString filePath);
	void ChangeImage(wxImage * newImage);
	void SetName(wxString name);
	wxString GetName();
	void SetPath(wxString path);
	wxString GetPath();

	bool GetSelected();
	wxImage * GetImage();

private:

	void OnLeftDoubleClick(wxMouseEvent& WXUNUSED(evt));

	wxImage * img;
	wxBoxSizer * mainLayout;
	wxBoxSizer * subLayout;
	WXImagePanel * imageDisplay;
	wxCheckBox * selectBox;
	wxStaticText * nameDisplay;
	wxString path;

};

class AddLibraryImageEvent : public wxEvent{

public:
	AddLibraryImageEvent(wxEventType eventType, int winid, wxImage * img, wxString fileName, wxString filePath);
	wxImage * GetImage() const;
	wxString GetFileName() const;
	wxString GetFilePath() const;
	virtual wxEvent * Clone() const;

private:
	wxImage * libImage;
	wxString name;
	wxString path;
};

enum {
	ID_ADD_LIB_IMAGE_EVT
};

wxDECLARE_EVENT(ADD_LIB_IMAGE_EVENT, AddLibraryImageEvent);

#endif