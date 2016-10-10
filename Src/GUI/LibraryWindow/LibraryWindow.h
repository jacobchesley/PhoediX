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
	
		
	void AddLibraryImage(LibraryImage* newLibImage);


private:
	
	void OnShowDirectories(wxCommandEvent & WXUNUSED(evt));
	void OnImport(wxCommandEvent & WXUNUSED(evt));
	void OnResize(wxSizeEvent & evt);
	void OnScroll(wxScrollEvent & evt);

	wxBoxSizer * mainLayout;

	wxBoxSizer * toolbarLayout;
	wxButton * showDirectoriesButton;
	wxButton * importButton;

	wxWrapSizer * imagesLayout;

	wxVector<LibraryImage*> libraryImages;

	DirectorySelections * directorySelections;

	enum MenuBar{
		ID_SHOW_DIRECTORY_LIST,
		ID_IMPORT
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