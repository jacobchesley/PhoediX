// Copyright 2016 Jacob Chesley

#ifndef AVAILABLE_EDIT_WINDOWS_H
#define AVAILABLE_EDIT_WINDOWS_H

// for compilers that support precompilation, includes "wx/wx.h"
#include "wx/wxprec.h"

// for all others, include the necessary headers explicitly
#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "GUI/ImageDisplay/ZoomImagePanel/ZoomImagePanel.h"
#include "GUI/EditList/EditWindow/EditWindow.h"
#include "GUI/EditList/AvailableEdits/AvailableEdits.h"
#include "Debugging/MemoryLeakCheck.h"

#include "AdjustBrightnessWindow/AdjustBrightnessWindow.h"
#include "AdjustHSLWindow/AdjustHSLWindow.h"
#include "AdjustLABWindow/AdjustLABWindow.h"
#include "AdjustRGBWindow/AdjustRGBWindow.h"
#include "ChannelMixerWindow/ChannelMixerWindow.h"
#include "ContrastWindow/ContrastWindow.h"
#include "ConvertGreyscaleWindow/ConvertGreyscaleWindow.h"
#include "CropWindow/CropWindow.h"
#include "HSLCurvesWindow/HSLCurvesWindow.h"
#include "LABCurvesWindow/LABCurvesWindow.h"
#include "MirrorWindow/MirrorWindow.h"
#include "RawWindow/RawWindow.h"
#include "RGBCurvesWindow/RGBCurvesWindow.h"
#include "RotationWindow/RotationWindow.h"
#include "ScaleWindow/ScaleWindow.h"

class AvailableEditWindows {

	public:
		static EditWindow* GetEditWindow(int editID, wxWindow* parent, Processor * processor, ZoomImagePanel * imgPanel);
		static EditWindow* GetEditWindow(ProcessorEdit * edit, wxWindow* parent, Processor * processor, ZoomImagePanel * imgPanel);
		static int GetEditIDFromEdit(ProcessorEdit * edit);
};

#endif