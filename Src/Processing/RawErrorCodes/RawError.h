// Copyright 2016 Jacob Chesley

#ifndef RAW_ERROR_H
#define RAW_ERROR_H

// for compilers that support precompilation, includes "wx/wx.h"
#include "wx/wxprec.h"

// for all others, include the necessary headers explicitly
#ifndef WX_PRECOMP
	#include "wx/wx.h"
#endif


#include "libraw.h"

class RawError {

public:
	static wxString GetStringFromError(int librawErrorCode);
private:

};
#endif
