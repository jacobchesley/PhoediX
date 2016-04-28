#include "PhoediX.h"

IMPLEMENT_APP(PhoediX)

// 'Main program' equivalent: the program execution "starts" here
bool PhoediX::OnInit(){

#ifdef CHECK_MEMORY_LEAK
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif

    if (!wxApp::OnInit()){
		return false;
	}
   
	wxInitAllImageHandlers();

    // Create and show the main window
	MainWindow * mainWindow = new MainWindow();
	mainWindow->Show();
    return true;
}