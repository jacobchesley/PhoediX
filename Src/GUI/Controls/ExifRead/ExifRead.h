#ifndef EXIF_READ_H
#define EXIF_READ_H

// for compilers that support precompilation, includes "wx/wx.h"
#include "wx/wxprec.h"

// for all others, include the necessary headers explicitly
#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "GUI/Icons/Icons.h"
#include "GUI/Colors/Colors.h"
#include "Processing/Image/Image.h"
#include <list>

class ExifRead : public wxPanel {

public:
	ExifRead(wxWindow * parent);
	void AddExifData(Image * image);
	void AddExifData(std::map<size_t, void*> exifData);
	void AddExifRow(size_t tag, void * data);
	void ClearExif();

private:
	wxGridSizer * sizer;
	wxVector<wxStaticText*> labels;
	wxVector<wxTextCtrl*> values;
	wxVector<size_t> tags;
};

class ExifReadFrame : public wxFrame {
public:
	ExifReadFrame(wxWindow * parent, wxString name, Image * image);
	
private:

	wxBoxSizer * mainSizer;
	wxScrolledWindow * scrollWin;

	wxBoxSizer * scrollSizer;
	ExifRead * exifTable;
};

class ExifReadWindow : public wxWindow {
public:
	ExifReadWindow(wxWindow * parent);
	void ClearExif();
	void AddExif(Image * image);

private:

	wxBoxSizer * mainSizer;
	wxScrolledWindow * scrollWin;

	wxBoxSizer * scrollSizer;
	ExifRead * exifTable;
};

#endif