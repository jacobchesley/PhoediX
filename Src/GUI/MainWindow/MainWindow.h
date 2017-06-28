// Copyright 2016 Jacob Chesley

#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

// for compilers that support precompilation, includes "wx/wx.h"
#include "wx/wxprec.h"

// for all others, include the necessary headers explicitly
#ifndef WX_PRECOMP
	#include "wx/wx.h"
#endif

#include "wx/aui/aui.h"
#include "wx/thread.h"
#include "wx/timer.h"
#include "wx/msgdlg.h"

#include "Processing/Processor/Processor.h"
#include "Processing/ImageHandler/ImageHandler.h"
#include "GUI/ExportWindow/ExportWindow.h"
#include "GUI/PixelPeepWindow/PixelPeepWindow.h"
#include "GUI/SettingsWindow/SettingsWindow.h"
#include "GUI/ImageDisplay/ZoomImagePanel/ZoomImagePanel.h"
#include "GUI/EditList/EditListPanel/EditListPanel.h"
#include "GUI/SnapshotWindow/SnapshotWindow.h"
#include "GUI/AUIManager/AUIManager.h"
#include "GUI/Colors/Colors.h"
#include "GUI/HistogramDisplay/HistogramDisplay.h"
#include "GUI/AboutWindow/AboutWindow.h"
#include "GUI/ImportImageDialog/ImportImageDialog.h"
#include "GUI/LibraryWindow/LibraryWindow.h"
#include "GUI/SupportedCamerasWindow/SupportedCamerasWindow.h"
#include "Session/Session.h"
#include "Debugging/MemoryLeakCheck.h"

enum {
	ID_UPDATE_IMAGE
};

wxDECLARE_EVENT(UPDATE_IMAGE_EVENT, wxCommandEvent);

/**
	Main Window is the main display window of PhoediX.
*/
class MainWindow : public wxFrame {
	
public:
	/**
		Constructor for the main window.
	*/
	MainWindow(wxApp * application);
	bool OriginalImageDispalyed();
	void OpenFiles(wxArrayString files);

private:

	void OnNewProject(wxCommandEvent& WXUNUSED(event));
	void ShowLoadProject(wxCommandEvent& WXUNUSED(event));
	void ShowSaveProject(wxCommandEvent& WXUNUSED(event));
	void QuickSaveProject(wxCommandEvent& WXUNUSED(event));
	void CloseCurrentProject(wxCommandEvent& WXUNUSED(event));
	void CloseAllProjects(wxCommandEvent& WXUNUSED(event));
	void ShowLoadFile(wxCommandEvent& WXUNUSED(event));
	void ShowExport(wxCommandEvent& WXUNUSED(event));
	void ShowImage(wxCommandEvent& WXUNUSED(event));
	void ShowPixelPeep(wxCommandEvent& WXUNUSED(event));
	void ShowSnapshots(wxCommandEvent& WXUNUSED(event));
	void ShowLibrary(wxCommandEvent& WXUNUSED(event));
	void ShowOriginal(wxCommandEvent& WXUNUSED(event));
	void ShowOriginalWindow(wxCommandEvent& WXUNUSED(event));
	void ShowSettings(wxCommandEvent& WXUNUSED(event));
	void ShowAbout(wxCommandEvent& WXUNUSED(event));
	void ShowSupportedCameras(wxCommandEvent& WXUNUSED(event));
	void ShowEditList(wxCommandEvent& evt);
	void ShowHistograms(wxCommandEvent& evt);
	void OnReprocessTimer(wxTimerEvent& evt);
	void OnImagePanelMouse(wxMouseEvent & evt);

	void CreateNewProject();
	void OpenImage(wxString imagePath, bool rawWindowOpen = false);
	void ShowImageRelatedWindows();
	void ReloadImage(wxCommandEvent& WXUNUSED(evt));
	void LoadProject(wxString projectPath);
	void OnImportImageNewProject(wxCommandEvent& evt);

	void OnOpenWindow(wxCommandEvent& evt);
	void OpenSession(PhoediXSession * session);
	void SaveCurrentSession();
	void CloseSession(PhoediXSession * session);
	void SetUniqueID(PhoediXSession * session);
	void CheckUncheckSession(int sessionID);
	void EnableDisableMenuItemsNoProject(bool enable);

	void SetSizeProperties();

	void SetMenuChecks();
	void OnPaneClose(wxAuiManagerEvent& evt);

	void SetStatusbarText(wxString text);
	void OnClose(wxCloseEvent& WXUNUSED(evt));

	void RecieveMessageFromProcessor(wxCommandEvent& messageEvt);
	void RecieveNumFromProcessor(wxCommandEvent& numEvt);
	void RecieveRawComplete(wxCommandEvent& WXUNUSED(evt));
	void OnUpdateImage(wxCommandEvent& WXUNUSED(event));

	bool CheckSessionNeedsSaved(PhoediXSession * session);

	wxAuiManager * auiManager;

	wxMenuBar * menuBar;
	wxMenu * menuFile;
	wxMenu * menuCloseProjects;
	wxMenu * menuView;
	wxMenu * menuTools;
	wxMenu * menuWindow;
	wxMenu * menuHelp;
	wxStaticText * statusBarText;

	EditListPanel * editList;
	ExportWindow * exportWindow;
	SettingsWindow * settingsWindow;
	PixelPeepWindow * pixelPeepWindow;
	LibraryWindow * libraryWindow;
	SnapshotWindow * snapshotWindow;
	SupportedCamerasWindow * supportedCamerasWindow;
	AboutWindow * aboutWindow;

	Processor * processor;

	ZoomImagePanel * imagePanel;
	ZoomImagePanel * originalImagePanel;
	wxImage * emptyImage;
	Image * emptyPhxImage;
	HistogramDisplay * histogramDisplay;

	PhoediXSession currentSession;
	wxVector<PhoediXSession> allSessions;
	wxVector<PhoediXSession> savedSessions;

	int numnUnnamedProjectsOpen;

	wxTimer * reprocessCountdown;
	wxApp * app;

	enum MenuBar {
		ID_NEW_PROJECT = 1,
		ID_SHOW_LOAD_PROJECT,
		ID_SHOW_SAVE_PROJECT,
		ID_QUICK_SAVE_PROJECT,
		ID_SHOW_LOAD_FILE,
		ID_SHOW_EXPORT,
		ID_EXIT,
		ID_SHOW_IMAGE,
		ID_SHOW_EDIT_LIST,
		ID_SHOW_HISTOGRAMS,
		ID_SHOW_SETTINGS,
		ID_SHOW_ORIGINAL,
		ID_SHOW_ORIGINAL_WINDOW,
		ID_SHOW_PIXEL_PEEP,
		ID_SHOW_LIBRARY,
		ID_SHOW_SNAPSHOTS,
		ID_CLOSE_CURRENT_PROJECT,
		ID_CLOSE_ALL_PROJECTS,
		ID_SHOW_SUPPORTED_CAMERAS,
		ID_ABOUT
	};

	class ImagePanelUpdateThread : public wxThread {

		public:
			ImagePanelUpdateThread(MainWindow * mainWin, ZoomImagePanel * imagePanel, PixelPeepWindow * pixelPeepWindow, Processor * processor, HistogramDisplay * histogramDisplay, ExportWindow * exportWindow);
			void StopWatching();

		protected:
			virtual wxThread::ExitCode Entry();

		private:
			bool continueWatch;
			ZoomImagePanel * imgPanel;
			HistogramDisplay * histogramDisp;
			Processor * proc;
			ExportWindow * exportWin;
			PixelPeepWindow * pixelPeep;
			MainWindow * parent;
	};

	ImagePanelUpdateThread * imgPanelThread;

};		
#endif
