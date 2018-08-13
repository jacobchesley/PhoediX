// Copyright 2018 Jacob Chesley.
// See LICENSE.TXT in root of project for license information.

#ifndef COLORS_H
#define COLORS_H

// for compilers that support precompilation, includes "wx/wx.h"
#include "wx/wxprec.h"

// for all others, include the necessary headers explicitly
#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "Debugging/MemoryLeakCheck.h"

class Colors {
public:
	static wxColor BackBlack;
	static wxColor BackDarkDarkDarkGrey;
	static wxColor BackDarkDarkGrey;
	static wxColor BackDarkGrey;
	static wxColor BackMedDarkGrey;
	static wxColor BackGrey;
	static wxColor BackLightGrey;
	static wxColor BackLightLightGrey;
	static wxColor BackWhite;
	
	static wxColor TextBlack;
	static wxColor TextDarkGrey;
	static wxColor TextGrey;
	static wxColor TextLightGrey;
	static wxColor TextWhite;

};

#endif