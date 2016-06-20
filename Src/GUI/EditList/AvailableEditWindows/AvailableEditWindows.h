#ifndef AVAILABLE_EDIT_WINDOWS_H
#define AVAILABLE_EDIT_WINDOWS_H

// for compilers that support precompilation, includes "wx/wx.h"
#include "wx/wxprec.h"

// for all others, include the necessary headers explicitly
#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "GUI\EditList\EditWindow\EditWindow.h"

#include "ContrastWindow\ContrastWindow.h"
#include "GreyscaleWindow\ConvertGreyscaleWindow.h"
#include "ScaleBrightnessWindow\ScaleBrightnessWindow.h"
#include "ShiftBrightnessWindow\ShiftBrightnessWindow.h"
#include "RotationWindow\RotationWindow.h"
#include "ChannelTransformWindow\ChannelTransformWindow.h"

#include "GUI\EditList\AvailableEdits\AvailableEdits.h"
#include "Debugging\MemoryLeakCheck.h"

class AvailableEditWindows {

	public:
		static EditWindow* GetEditWindow(int editID, wxWindow* parent, Processor * processor);
};

#endif