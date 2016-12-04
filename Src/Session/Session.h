// Copyright 2016 Jacob Chesley

#ifndef PHOEDIX_SESSION_H
#define PHOEDIX_SESSION_H

// for compilers that support precompilation, includes "wx/wx.h"
#include "wx/wxprec.h"

// for all others, include the necessary headers explicitly
#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "wx/xml/xml.h"

#include "SessionEditList\SessionEditList.h"
#include "Processing\Snapshot\Snapshot.h"
#include "wx\datetime.h"

class PhoediXSession{

public:
	PhoediXSession();
	void Destroy();

	void LoadSessionFromFile(wxString filePath);
	void SaveSessionToFile(wxString filePath);
	
	wxString GetImageFilePath();
	void SetImageFilePath(wxString imageFilePath);

	PhoediXSessionEditList *GetEditList();

	wxVector<Snapshot*> GetSnapshots();
	void SetSnapshots(wxVector<Snapshot*> snapshots);

	float GetImageZoomLevel();
	void SetImageZoomLevel(float zoom);

	int GetImageScrollX();
	void SetImageScrollX(int x);

	int GetImageScrollY();
	void SetImageScrollY(int y);

	void SetImageScrollWidth(int width);
	int GetImageScrollWidth();
	void SetImageScrollHeight(int height);
	int GetImageScrollHeight();

	int GetHistogramDisplaySelect();
	void SetHistogramDisplaySelect(int selection);

	wxString GetPerspective();
	void SetPerspective(wxString newPerspective);

	void GenerateID();
	int GetID();

	void SetName(wxString newName);
	wxString GetName();

	void SetProjectPath(wxString path);
	wxString GetProjectPath();

private:

	wxString projectPath;
	wxString imgFile;
	PhoediXSessionEditList *editList;
	wxVector <Snapshot*>  snapshotsList;
	float imgZoom;
	int imgScrollX;
	int imgScrollY;
	int imgWidth;
	int imgHeight;
	wxString auiPerspective;
	int histogramDisplaySelect;
	int id;

	wxString name;
};
#endif