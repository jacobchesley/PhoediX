// Copyright 2016 Jacob Chesley

#ifndef PHOEDIX_H
#define PHOEDIX_H

#define PHOEDIX_VERSION_STRING "0.1.0"
#define PHOEDIX_BUILD_DATE_STRING __DATE__
#define PHOEDIX_BUILD_TIME_STRING __TIME__
#define PHOEDIX_AUTHOR_STRING "Jacob Chesley"
#define PHOEDIX_WEBSITE_STRING "www.phoedix.com"

// for compilers that support precompilation, includes "wx/wx.h"
#include "wx/wxprec.h"

// for all others, include the necessary headers explicitly
#ifndef WX_PRECOMP
	#include "wx/wx.h"
#endif

#include "wx\splash.h"
#include "GUI\Icons\Icons.h"
#include "GUI\MainWindow\MainWindow.h"
#include "Debugging\MemoryLeakCheck.h"

class PhoediX : public wxApp {
public:

	// Initialization of the application.  This is where the Main User Interface will be create
	virtual bool OnInit();
};

#endif