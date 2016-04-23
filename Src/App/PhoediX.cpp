#include "PhoediX.h"

IMPLEMENT_APP(PhoediX)

// 'Main program' equivalent: the program execution "starts" here
bool PhoediX::OnInit(){

    if (!wxApp::OnInit()){
		return false;
	}
   
    // Create and show the main window
	MainWindow * mainWindow = new MainWindow();
	mainWindow->Show();
    return true;
}