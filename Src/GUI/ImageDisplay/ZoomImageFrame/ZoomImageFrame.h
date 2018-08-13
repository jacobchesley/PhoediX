// Copyright 2018 Jacob Chesley.
// See LICENSE.TXT in root of project for license information.

#ifndef ZOOM_IMAGE_FRAME_H
#define ZOOM_IMAGE_FRAME_H

// for compilers that support precompilation, includes "wx/wx.h"
#include "wx/wxprec.h"

// for all others, include the necessary headers explicitly
#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "GUI/Icons/Icons.h"
#include "GUI/ImageDisplay/ZoomImagePanel/ZoomImagePanel.h"

class ZoomImageFrame : public wxFrame {
public:
	ZoomImageFrame(wxWindow * parent, wxString title);
	ZoomImageFrame(wxWindow * parent, wxString title, Image * img);
	ZoomImageFrame(wxWindow * parent, wxString title, wxImage * img);

private:

	wxBoxSizer * mainSizer;
	ZoomImagePanel * imageDisplay;
};
#endif