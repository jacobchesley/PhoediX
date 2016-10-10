#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

// for compilers that support precompilation, includes "wx/wx.h"
#include "wx/wxprec.h"

// for all others, include the necessary headers explicitly
#ifndef WX_PRECOMP
	#include "wx/wx.h"
#endif

#include "wx\aui\aui.h"
#include "wx\thread.h"
#include "wx\timer.h"

#include "Processing\Processor\Processor.h"
#include "Processing\ImageHandler\ImageHandler.h"
#include "GUI\MainWindow\ExportWindow\ExportWindow.h"
#include "GUI\MainWindow\PixelPeepWindow\PixelPeepWindow.h"
#include "GUI\MainWindow\SettingsWindow\SettingsWindow.h"
#include "GUI\ImageDisplay\ZoomImagePanel\ZoomImagePanel.h"
#include "GUI\EditList\EditListPanel\EditListPanel.h"
#include "GUI\AUI Manager\AUIManager.h"
#include "GUI\Colors\Colors.h"
#include "GUI\HistogramDisplay\HistogramDisplay.h"
#include "GUI\LibraryWindow\LibraryWindow.h"
#include "Session\Session.h"
#include "Debugging\MemoryLeakCheck.h"

/**
	Main Window is the main display window of PhoediX.
*/
class MainWindow : public wxFrame {
	
public:
	/**
		Constructor for the main window.
	*/
	MainWindow();


private:

	void OnNewProject(wxCommandEvent& WXUNUSED(event));
	void ShowLoadProject(wxCommandEvent& WXUNUSED(event));
	void ShowSaveProject(wxCommandEvent& WXUNUSED(event));
	void ShowLoadFile(wxCommandEvent& WXUNUSED(event));
	void ShowExport(wxCommandEvent& WXUNUSED(event));
	void ShowImage(wxCommandEvent& WXUNUSED(event));
	void ShowPixelPeep(wxCommandEvent& WXUNUSED(event));
	void ShowLibrary(wxCommandEvent& WXUNUSED(event));
	void ShowSettings(wxCommandEvent& WXUNUSED(event));
	void ShowAbout(wxCommandEvent& WXUNUSED(event));
	void ShowEditList(wxCommandEvent& evt);
	void ShowHistograms(wxCommandEvent& evt);
	void OnReprocessTimer(wxTimerEvent& evt);

	void OpenImage(wxString imagePath, wxString imageName);
	void ReloadImage(wxCommandEvent& WXUNUSED(evt));

	void OnOpenWindow(wxCommandEvent& evt);
	void OpenSession(PhoediXSession session);
	void SaveCurrentSession();
	void SetUniqueID(PhoediXSession * session);
	void CheckUncheckSession(int sessionID);

	void SetSizeProperties();

	void SetMenuChecks();
	void OnPaneClose(wxAuiManagerEvent& evt);

	void SetStatusbarText(wxString text);
	void OnClose(wxCloseEvent& closeEvent);

	void RecieveMessageFromProcessor(wxCommandEvent& messageEvt);
	void RecieveNumFromProcessor(wxCommandEvent& numEvt);

	wxAuiManager * auiManager;

	wxMenuBar * menuBar;
	wxMenu * menuFile;
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

	Processor * processor;

	ZoomImagePanel * imagePanel;
	HistogramDisplay * histogramDisplay;

	PhoediXSession currentSession;
	wxVector<PhoediXSession> allSessions;

	int numnUnnamedProjectsOpen;

	enum MenuBar {
		ID_NEW_PROJECT,
		ID_SHOW_LOAD_PROJECT,
		ID_SHOW_SAVE_PROJECT,
		ID_SHOW_LOAD_FILE,
		ID_SHOW_EXPORT,
		ID_EXIT,
		ID_SHOW_IMAGE,
		ID_SHOW_EDIT_LIST,
		ID_SHOW_HISTOGRAMS,
		ID_SHOW_SETTINGS,
		ID_SHOW_PIXEL_PEEP,
		ID_SHOW_LIBRARY
	};

	class ImagePanelUpdateThread : public wxThread {

		public:
			ImagePanelUpdateThread(ZoomImagePanel * imagePanel, Processor * processor, HistogramDisplay * histogramDisplay, ExportWindow * exportWindow);
			void StopWatching();

		protected:
			virtual wxThread::ExitCode Entry();

		private:
			bool continueWatch;
			ZoomImagePanel * imgPanel;
			HistogramDisplay * histogramDisp;
			Processor * proc;
			ExportWindow * exportWin;
	};

	ImagePanelUpdateThread * imgPanelThread;

};		
#endif