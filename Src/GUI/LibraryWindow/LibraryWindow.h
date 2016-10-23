#ifndef LIBRARY_WINDOW_H
#define LIBRARY_WINDOW_H

// for compilers that support precompilation, includes "wx/wx.h"
#include "wx/wxprec.h"

// for all others, include the necessary headers explicitly
#ifndef WX_PRECOMP
	#include "wx/wx.h"
#endif

#include "wx\dir.h"
#include "wx\file.h"
#include "wx\wrapsizer.h"

#include "GUI\LibraryWindow\DirectorySelections\DirectorySelections.h"
#include "GUI\Colors\Colors.h"
#include "GUI\Controls\DoubleSlider\DoubleSlider.h"
#include "GUI\AUI Manager\AUIManager.h"
#include "GUI\Icons\Icons.h"
#include "GUI\LibraryWindow\LibraryImage\LibraryImage.h"
#include "Processing\ImageHandler\ImageHandler.h"
#include "Processing\Processor\Processor.h"
#include "Debugging\MemoryLeakCheck.h"


class LibraryWindow : public wxScrolledWindow {

public:
	LibraryWindow(wxWindow * parent);

protected:
			
	void AddLibraryImage(wxImage* newImage, wxString fileName, wxString filePath);

private:
	
	void OnShowDirectories(wxCommandEvent & WXUNUSED(evt));
	void OnImport(wxCommandEvent & WXUNUSED(evt));
	void OnClear(wxCommandEvent & WXUNUSED(evt));
	void OnResize(wxSizeEvent & WXUNUSED(evt));
	void OnAddImage(AddLibraryImageEvent & evt);
	bool CheckIfImageInDisplay(wxString imagePath);

	wxBoxSizer * mainLayout;

	wxBoxSizer * toolbarLayout;
	wxButton * showDirectoriesButton;
	wxButton * importButton;
	wxButton * clearButton;

	wxWrapSizer * imagesLayout;

	wxVector<LibraryImage*> libraryImages;
	wxVector<wxString> includedImagePaths;

	DirectorySelections * directorySelections;

	enum MenuBar{
		ID_SHOW_DIRECTORY_LIST,
		ID_IMPORT,
		ID_CLEAR
	};

	class LoadImagesThread : public wxThread {
	public:
		LoadImagesThread(LibraryWindow * parent);
	protected:

		virtual ExitCode Entry();
	private:
		LibraryWindow * par;

	};
};

#endif