// Copyright 2018 Jacob Chesley.
// See LICENSE.TXT in root of project for license information.

#ifndef PHOEDIX_SETTINGS_H
#define PHOEDIX_SETTINGS_H

// for compilers that support precompilation, includes "wx/wx.h"
#include "wx/wxprec.h"

// for all others, include the necessary headers explicitly
#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "wx/aui/aui.h"

#include "Debugging/MemoryLeakCheck.h"

class PhoedixSettings {

public:
	static int GetBitDepth();
	static void SetBitDepth(int newBitDepth);

	static int GetColorSpace();
	static void SetColorSpace(int newColorSpace);

	static int GetNumThreads();
	static void SetNumThreads(int newNumThreads);

private:
	static int bitDepth;
	static int colorSpace;
	static int numThreads;
};
#endif