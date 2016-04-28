#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

// for compilers that support precompilation, includes "wx/wx.h"
#include "wx/wxprec.h"

// for all others, include the necessary headers explicitly
#ifndef WX_PRECOMP
	#include "wx/wx.h"
#endif

#include "wx\aui\aui.h"

#include "Processing\Processor\Processor.h"
#include "Processing\ImageHandler\ImageHandler.h"
#include "GUI\ImageDisplay\ImagePanel\ImagePanel.h"
#include "GUI\EditList\EditListPanel\EditListPanel.h"
#include "GUI\Colors\Colors.h"
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

	void ShowLoadFile(wxCommandEvent& WXUNUSED(event));
	void SetSizeProperties();

	void SetStatusbarText(wxString text);
	void OnClose(wxCloseEvent& closeEvent);

	wxAuiManager * auiManager;

	wxMenuBar * menuBar;
	wxMenu * menuFile;
	wxMenu * menuView;
	wxMenu * menuHelp;
	wxStaticText * statusBarText;

	Processor * processor;

	ImagePanel * imagePanel;
	EditListPanel * editList;

	enum MenuBar {
		ID_SHOW_LOAD_FILE,
		ID_EXIT
	};
};		
#endif