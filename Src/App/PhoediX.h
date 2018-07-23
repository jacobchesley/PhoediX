// Copyright 2016 Jacob Chesley

#ifndef PHOEDIX_H
#define PHOEDIX_H

#define PHOEDIX_VERSION_STRING "0.1.0"
#define PHOEDIX_BUILD_DATE_STRING __DATE__
#define PHOEDIX_BUILD_TIME_STRING __TIME__
#define PHOEDIX_AUTHOR_STRING "Jacob Chesley"
#define PHOEDIX_WEBSITE_STRING "www.phoedix.com"

#define WXWIDGETS_WEBSITE_STRING "www.wxwidgets.org"
#define LIBRAW_WEBSITE_STRING "www.libraw.org"
#define LIBTIFF_WEBSITE_STRING "www.simplesystems.org/libtiff"

// for compilers that support precompilation, includes "wx/wx.h"
#include "wx/wxprec.h"

// for all others, include the necessary headers explicitly
#ifndef WX_PRECOMP
	#include "wx/wx.h"
#endif

#include <wx/cmdline.h>
#include "wx/splash.h"
#include "GUI/Icons/Icons.h"
#include "GUI/MainWindow/MainWindow.h"
#include "Debugging/MemoryLeakCheck.h"
#include "Debugging/Logger/Logger.h"
#include "Processing/Image/Image.h"

class PhoediX : public wxApp {
public:

	// Initialization of the application.  This is where the Main User Interface will be create
	virtual bool OnInit();
	virtual void OnInitCmdLine(wxCmdLineParser& parser);
	virtual bool OnCmdLineParsed(wxCmdLineParser& parser);

private:
	wxArrayString filesToOpen;
};

static const wxCmdLineEntryDesc cmdLineDesc[] =
{
	{ wxCMD_LINE_SWITCH, "h", "help", "displays help on the command line parameters", wxCMD_LINE_VAL_NONE, wxCMD_LINE_OPTION_HELP },
	{wxCMD_LINE_PARAM, NULL, NULL, "", wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_MULTIPLE | wxCMD_LINE_PARAM_OPTIONAL },
	{ wxCMD_LINE_NONE }
};

#endif
