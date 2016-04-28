#ifndef ICONS_H
#define ICONS_H
	
// for compilers that support precompilation, includes "wx/wx.h"
#include "wx/wxprec.h"

// for all others, include the necessary headers explicitly
#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "wx\mstream.h"

#include "up_button.h"
#include "down_button.h"
#include "delete_button.h"

class Icons {
public:

	Icons();
	wxImage UpButton;
	wxImage DownButton;
	wxImage DeleteButton;
};
#endif