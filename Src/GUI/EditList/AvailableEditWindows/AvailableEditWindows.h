// Copyright 2016 Jacob Chesley

#ifndef AVAILABLE_EDIT_WINDOWS_H
#define AVAILABLE_EDIT_WINDOWS_H

// for compilers that support precompilation, includes "wx/wx.h"
#include "wx/wxprec.h"

// for all others, include the necessary headers explicitly
#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "GUI\EditList\EditWindow\EditWindow.h"

#include "AdjustBrightnessWindow\AdjustBrightnessWindow.h"
#include "ChannelMixerWindow\ChannelMixerWindow.h"
#include "ContrastWindow\ContrastWindow.h"
#include "GreyscaleWindow\ConvertGreyscaleWindow.h"
#include "LABCurvesWindow\LABCurvesWindow.h"
#include "HSLCurvesWindow\HSLCurvesWindow.h"
#include "MirrorWindow\MirrorWindow.h"
#include "RawWindow\RawWindow.h"
#include "RGBCurvesWindow\RGBCurvesWindow.h"
#include "RotationWindow\RotationWindow.h"
#include "AdjustHSLWindow\AdjustHSLWindow.h"
#include "ShiftRGBWindow\ShiftRGBWindow.h"
#include "ScaleWindow\ScaleWindow.h"
#include "CropWindow\CropWindow.h"

#include "GUI\EditList\AvailableEdits\AvailableEdits.h"
#include "Debugging\MemoryLeakCheck.h"

class AvailableEditWindows {

	public:
		static EditWindow* GetEditWindow(int editID, wxWindow* parent, Processor * processor);
		static EditWindow* GetEditWindow(ProcessorEdit * edit, wxWindow* parent, Processor * processor);
		static int GetEditIDFromEdit(ProcessorEdit * edit);
};

#endif