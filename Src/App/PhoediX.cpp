// Copyright 2016 Jacob Chesley

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

	//Icons icons;
	//wxBitmap splashBitmap(icons.SplashBackground);

	//wxSplashScreen* splash = new wxSplashScreen(splashBitmap, wxSPLASH_CENTRE_ON_SCREEN | wxSPLASH_TIMEOUT, 4000, NULL, -1, wxDefaultPosition, wxDefaultSize,wxBORDER_SIMPLE | wxSTAY_ON_TOP);
	//wxYield();
   
    // Create and show the main window
	MainWindow * mainWindow = new MainWindow(this);
	mainWindow->Show();
    return true;
}