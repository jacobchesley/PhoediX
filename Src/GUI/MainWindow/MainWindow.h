#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

// for compilers that support precompilation, includes "wx/wx.h"
#include "wx/wxprec.h"

// for all others, include the necessary headers explicitly
#ifndef WX_PRECOMP
	#include "wx/wx.h"
#endif

#include "Processing\Processor\Processor.h"
#include "Processing\ImageFileLoader\ImageFileLoader.h"

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

	wxMenuBar * menuBar;
	wxMenu * menuFile;
	wxMenu * menuView;
	wxMenu * menuHelp;

	Processor * processor;

	enum MenuBar {
		ID_SHOW_LOAD_FILE,
		ID_EXIT
	};
};		
#endif