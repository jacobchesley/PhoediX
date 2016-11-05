// Copyright 2016 Jacob Chesley

#ifndef SNAPSHOT_WINDOW_H
#define SNAPSHOT_WINDOW_H

// for compilers that support precompilation, includes "wx/wx.h"
#include "wx/wxprec.h"

// for all others, include the necessary headers explicitly
#ifndef WX_PRECOMP
	#include "wx/wx.h"
#endif

#include "wx\dir.h"
#include "wx\file.h"
#include "wx\wrapsizer.h"

#include "GUI\LibraryWindow\DirectorySelections\DirectorySelections.h"
#include "GUI\Colors\Colors.h"
#include "GUI\Controls\DoubleSlider\DoubleSlider.h"
#include "GUI\AUI Manager\AUIManager.h"
#include "GUI\Icons\Icons.h"
#include "Processing\Processor\Processor.h"
#include "Debugging\MemoryLeakCheck.h"


class SnapshotWindow : public wxScrolledWindow {

public:
	SnapshotWindow(wxWindow * parent);
	void AddSnapshot(wxVector<ProcessorEdit*> editList);

private:

};

#endif