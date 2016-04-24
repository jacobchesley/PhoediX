#ifndef PHOEDIX_H
#define PHOEDIX_H

// for compilers that support precompilation, includes "wx/wx.h"
#include "wx/wxprec.h"

// for all others, include the necessary headers explicitly
#ifndef WX_PRECOMP
	#include "wx/wx.h"
#endif

#include "GUI\MainWindow\MainWindow.h"

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

class PhoediX : public wxApp {
public:

	// Initialization of the application.  This is where the Main User Interface will be create
	virtual bool OnInit();
};

#endif