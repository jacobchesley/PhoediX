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
#include "GUI\ImageDisplay\ZoomImagePanel\ZoomImagePanel.h"
#include "GUI\EditList\EditListPanel\EditListPanel.h"
#include "GUI\AUI Manager\AUIManager.h"
#include "GUI\Colors\Colors.h"
#include "GUI\HistogramDisplay\HistogramDisplay.h"
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
	void ShowLoadProject(wxCommandEvent& WXUNUSED(event));
	void ShowSaveProject(wxCommandEvent& WXUNUSED(event));
	void ShowLoadFile(wxCommandEvent& WXUNUSED(event));
	void ShowImage(wxCommandEvent& WXUNUSED(event));
	void ShowEditList(wxCommandEvent& WXUNUSED(event));
	void ShowHistograms(wxCommandEvent& WXUNUSED(event));
	void OnReprocessTimer(wxTimerEvent& WXUNUSED(event));

	void SetSizeProperties();

	void SetStatusbarText(wxString text);
	void OnClose(wxCloseEvent& closeEvent);

	void RecieveMessageFromProcessor(wxCommandEvent& messageEvt);

	wxAuiManager * auiManager;

	wxMenuBar * menuBar;
	wxMenu * menuFile;
	wxMenu * menuView;
	wxMenu * menuWindow;
	wxMenu * menuHelp;
	wxStaticText * statusBarText;
	EditListPanel * editList;

	Processor * processor;

	ZoomImagePanel * imagePanel;
	HistogramDisplay * histogramDisplay;

	PhoediXSession session;


	enum MenuBar {
		ID_SHOW_LOAD_PROJECT,
		ID_SHOW_SAVE_PROJECT,
		ID_SHOW_LOAD_FILE,
		ID_EXIT,
		ID_SHOW_IMAGE,
		ID_SHOW_EDIT_LIST,
		ID_SHOW_HISTOGRAMS
	};

	class ImagePanelUpdateThread : public wxThread {

		public:
			ImagePanelUpdateThread(ZoomImagePanel * imagePanel, Processor * processor, HistogramDisplay * histogramDisplay);
			void StopWatching();

		protected:
			virtual wxThread::ExitCode Entry();

		private:
			bool continueWatch;
			ZoomImagePanel * imgPanel;
			HistogramDisplay * histogramDisp;
			Processor * proc;
	};

	ImagePanelUpdateThread * imgPanelThread;

};		
#endif