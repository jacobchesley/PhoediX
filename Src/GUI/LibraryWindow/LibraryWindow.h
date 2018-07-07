// Copyright 2016 Jacob Chesley

#ifndef LIBRARY_WINDOW_H
#define LIBRARY_WINDOW_H

// for compilers that support precompilation, includes "wx/wx.h"
#include "wx/wxprec.h"

// for all others, include the necessary headers explicitly
#ifndef WX_PRECOMP
	#include "wx/wx.h"
#endif

#include "wx/dir.h"
#include "wx/file.h"
#include "wx/wrapsizer.h"
#include "wx/progdlg.h"

#include "GUI/Controls/PhoediXButton/PhoediXButton.h"
#include "GUI/LibraryWindow/DirectorySelections/DirectorySelections.h"
#include "GUI/Colors/Colors.h"
#include "GUI/Controls/DoubleSlider/DoubleSlider.h"
#include "GUI/AUIManager/AUIManager.h"
#include "GUI/Icons/Icons.h"
#include "GUI/LibraryWindow/LibraryImage/LibraryImage.h"
#include "Processing/ImageHandler/ImageHandler.h"
#include "Processing/Processor/Processor.h"
#include "Settings/Settings.h"
#include "Debugging/MemoryLeakCheck.h"

enum {
	ID_POPULATION_STARTED,
	ID_POPULATION_COMPLETE
};

wxDECLARE_EVENT(POPULATION_STARTED_EVENT, wxCommandEvent);
wxDECLARE_EVENT(POPULATION_COMPLETE_EVENT, wxCommandEvent);

class LibraryWindow : public wxScrolledWindow {

public:
	LibraryWindow(wxWindow * parent);

protected:
			
	void AddLibraryImage(wxImage* newImage, wxString fileName, wxString filePath);
	bool CheckIfImageInDisplay(wxString imagePath);

private:
	
	void OnShowDirectories(wxCommandEvent & WXUNUSED(evt));
	void OnImport(wxCommandEvent & WXUNUSED(evt));
	void OnHoverClearButton(wxMouseEvent & WXUNUSED(evt));
	void OnPopupMenuClick(wxCommandEvent& inEvt);
	void OnCopy(wxCommandEvent& WXUNUSED(evt));
	void OnMove(wxCommandEvent& WXUNUSED(evt));
	void ClearAll();
	void ClearSelected();
	void ClearUnselected();
	void OnResize(wxSizeEvent & WXUNUSED(evt));
	void OnAddImage(AddLibraryImageEvent & evt);
	void OnPopulationStart(wxSizeEvent & WXUNUSED(evt));
	void OnPopulationComplete(wxSizeEvent & WXUNUSED(evt));

	wxVector<wxString> GetSelectedFileNames();

	wxBoxSizer * mainLayout;

	wxWrapSizer * toolbarLayout;
	PhoediXButton * showDirectoriesButton;
	PhoediXButton * importButton;
	PhoediXButton * clearButton;
	PhoediXButton * copyButton;
	PhoediXButton * moveButton;

	wxWrapSizer * imagesLayout;
	wxSortedArrayString imagePaths;

	wxVector<LibraryImage*> libraryImages;
	wxVector<wxString> includedImagePaths;

	DirectorySelections * directorySelections;

	wxCriticalSection locker;
	bool populationStarted;
	bool populationCanceled;

	enum MenuBar{
		ID_SHOW_DIRECTORY_LIST = 1,
		ID_IMPORT,
		ID_CLEAR_ALL,
		ID_CLEAR_SELECTED,
		ID_CLEAR_UNSELECTED,
		ID_COPY_TO,
		ID_MOVE_TO,
		ID_CANCEL_PROCESS
	};

	class CopyImagesThread : public wxThread {
	public:
		CopyImagesThread(LibraryWindow * parent, wxVector<wxString> filesToCopy, wxString destination, bool deleteAfterCopy);
	protected:

		virtual ExitCode Entry();
	private:
		LibraryWindow * par;
		wxVector<wxString> toCopy;
		wxString destFolder;
		bool doDelete;
	};

	class LoadImagesThread : public wxThread {
	public:
		LoadImagesThread(LibraryWindow * parent);
		void Cancel();
	protected:

		virtual ExitCode Entry();
	private:
		LibraryWindow * par;
		bool canceled;

	};

	class LoadSubsetImagesThread : public wxThread {
	public:
		LoadSubsetImagesThread (LibraryWindow * parent, wxArrayString imagesToLoad, wxMutex * mutLockIn, wxCondition * condition, int numThreads, int * threadsComplete);
		void Cancel();
	protected:

		virtual ExitCode Entry();
	private:
		LibraryWindow * par;
		bool canceled;
		wxArrayString toLoad;
		wxMutex * mutLock;
		wxCondition * cond;
		int threads;
		int * complete;

	};

	LoadImagesThread * testImgThread;
};

#endif