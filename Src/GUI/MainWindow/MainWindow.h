#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

// for compilers that support precompilation, includes "wx/wx.h"
#include "wx/wxprec.h"

// for all others, include the necessary headers explicitly
#ifndef WX_PRECOMP
	#include "wx/wx.h"
#endif

#include "Processing\Processor\Processor.h"
#include "Processing\ImageHandler\ImageHandler.h"
#include "GUI\ImageDisplay\ImagePanel\ImagePanel.h"

#ifdef CHECK_MEMORY_LEAK
#define CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>

#ifdef _DEBUG
#ifndef DBG_NEW
#define DBG_NEW new ( _NORMAL_BLOCK , __FILE__ , __LINE__ )
#define new DBG_NEW
#endif
#endif  // _DEBUG
#endif

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

	void OnClose(wxCloseEvent& closeEvent);

	wxBoxSizer * sizer;

	wxMenuBar * menuBar;
	wxMenu * menuFile;
	wxMenu * menuView;
	wxMenu * menuHelp;

	Processor * processor;
	ImagePanel * imagePanel;

	enum MenuBar {
		ID_SHOW_LOAD_FILE,
		ID_EXIT
	};
};		
#endif