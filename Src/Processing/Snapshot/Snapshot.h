// Copyright 2018 Jacob Chesley.
// See LICENSE.TXT in root of project for license information.

#ifndef SNAPSHOT_H
#define SNAPSHOT_H

// for compilers that support precompilation, includes "wx/wx.h"
#include "wx/wxprec.h"

// for all others, include the necessary headers explicitly
#ifndef WX_PRECOMP
	#include "wx/wx.h"
#endif
#include "Processing/ProcessorEdit/ProcessorEdit.h"
#include "Debugging/MemoryLeakCheck.h"

struct Snapshot{
	int snapshotIndex;
	wxString snapshotName;
	wxVector<ProcessorEdit*> editList;
	wxString auiPerspective;
};

#endif