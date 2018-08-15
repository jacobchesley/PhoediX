// Copyright 2018 Jacob Chesley.
// See LICENSE.TXT in root of project for license information.

#include "AboutWindow.h"
#include "App/PhoediX.h"
#include "libraw.h"
#include "tiffio.h"

AboutWindow::AboutWindow(wxWindow * parent) : wxFrame(parent, -1, "About PhoediX", wxDefaultPosition, wxDefaultSize, wxDEFAULT_FRAME_STYLE & ~(wxRESIZE_BORDER | wxMAXIMIZE_BOX)){
	
	this->SetBackgroundColour(parent->GetBackgroundColour());
	
	Icons icons;
	wxIcon theIcon;
	theIcon.CopyFromBitmap(wxBitmap(icons.pxIcon));
	this->SetIcon(theIcon);

	// 2 Columns, 10 pixel vertical gap, 20 pixel horizontal gap
	aboutSizer = new wxFlexGridSizer(2, 10, 15);
	libraryAboutSizer = new wxFlexGridSizer(2, 10, 20);

	mainSizer = new wxBoxSizer(wxVERTICAL);
	this->SetSizer(mainSizer);

	wxString app32or64 = "";
	if (sizeof(void*) == 4) { app32or64 = " - 32 Bit"; }
	if (sizeof(void*) == 8) { app32or64 = " - 64 Bit"; }
	aboutPhoediX = new wxStaticText(this, -1, "About PhoediX" + app32or64);
	aboutPhoediX->SetForegroundColour(Colors::TextWhite);
	aboutPhoediX->SetFont(wxFont(13, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD));

	versionTitle = new wxStaticText(this, -1, "PhoediX Version");
	versionValue = new wxStaticText(this, -1, PHOEDIX_VERSION_STRING);
	versionTitle->SetForegroundColour(Colors::TextLightGrey);
	versionValue->SetForegroundColour(Colors::TextLightGrey);

	char buildDateTime[100];
	memset(buildDateTime, '\0', 100 * sizeof(char));

	strcat(buildDateTime, PHOEDIX_BUILD_DATE_STRING);
	strcat(buildDateTime, " ");
	strcat(buildDateTime, PHOEDIX_BUILD_TIME_STRING);
	buildDateValue = new wxStaticText(this, -1, buildDateTime);

	buildDateTitle = new wxStaticText(this, -1, "PhoediX Build Date");
	buildDateTitle->SetForegroundColour(Colors::TextLightGrey);
	buildDateValue->SetForegroundColour(Colors::TextLightGrey);

	authorTitle = new wxStaticText(this, -1, "Created By");
	authorValue = new wxStaticText(this, -1, PHOEDIX_AUTHOR_STRING);
	authorTitle->SetForegroundColour(Colors::TextLightGrey);
	authorValue->SetForegroundColour(Colors::TextLightGrey);

	websiteTitle = new wxStaticText(this, -1, "Website");
	websiteValue = new wxStaticText(this, -1, PHOEDIX_WEBSITE_STRING);
	websiteTitle->SetForegroundColour(Colors::TextLightGrey);
	websiteValue->SetForegroundColour(Colors::TextLightGrey);
	websiteValue->SetFont(wxFont(websiteValue->GetFont().GetPointSize(), wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD, true));
	websiteValue->Connect(wxEVT_LEFT_DOWN, wxMouseEventHandler(AboutWindow::OnPhoediXWebsiteClick), NULL, this);
	websiteValue->Connect(wxEVT_ENTER_WINDOW, wxMouseEventHandler(AboutWindow::OnWebsiteMouseEnter), NULL, this);
	websiteValue->Connect(wxEVT_LEAVE_WINDOW, wxMouseEventHandler(AboutWindow::OnWebsiteMouseLeave), NULL, this);

	aboutSizer->Add(versionTitle);
	aboutSizer->Add(versionValue);

	aboutSizer->Add(buildDateTitle);
	aboutSizer->Add(buildDateValue);

	aboutSizer->Add(authorTitle);
	aboutSizer->Add(authorValue);

	aboutSizer->Add(websiteTitle);
	aboutSizer->Add(websiteValue);

	aboutLibraries = new wxStaticText(this, -1, "Libraries Used");
	aboutLibraries->SetForegroundColour(Colors::TextWhite);
	aboutLibraries->SetFont(wxFont(11, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD));

	wxWidgetsVersionTitle = new wxStaticText(this, -1, "wxWidgets Version");
	wxWidgetsVersionValue = new wxStaticText(this, -1, wxVERSION_STRING);
	wxWidgetsVersionTitle->SetForegroundColour(Colors::TextLightGrey);
	wxWidgetsVersionValue->SetForegroundColour(Colors::TextLightGrey);
	wxWidgetsVersionValue->SetFont(wxFont(wxWidgetsVersionValue->GetFont().GetPointSize(), wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, true));
	wxWidgetsVersionValue->Connect(wxEVT_LEFT_DOWN, wxMouseEventHandler(AboutWindow::OnwxWidgetsWebsiteClick), NULL, this);
	wxWidgetsVersionValue->Connect(wxEVT_ENTER_WINDOW, wxMouseEventHandler(AboutWindow::OnWebsiteMouseEnter), NULL, this);
	wxWidgetsVersionValue->Connect(wxEVT_LEAVE_WINDOW, wxMouseEventHandler(AboutWindow::OnWebsiteMouseLeave), NULL, this);

	librawVersionTitle = new wxStaticText(this, -1, "LibRaw Version");
	librawVersionValue = new wxStaticText(this, -1, LibRaw::version());
	librawVersionTitle->SetForegroundColour(Colors::TextLightGrey);
	librawVersionValue->SetForegroundColour(Colors::TextLightGrey);
	librawVersionValue->SetFont(wxFont(librawVersionValue->GetFont().GetPointSize(), wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, true));
	librawVersionValue->Connect(wxEVT_LEFT_DOWN, wxMouseEventHandler(AboutWindow::OnLibRawWebsiteClick), NULL, this);
	librawVersionValue->Connect(wxEVT_ENTER_WINDOW, wxMouseEventHandler(AboutWindow::OnWebsiteMouseEnter), NULL, this);
	librawVersionValue->Connect(wxEVT_LEAVE_WINDOW, wxMouseEventHandler(AboutWindow::OnWebsiteMouseLeave), NULL, this);

	libtiffVersionTitle = new wxStaticText(this, -1, "Libtiff Version");
	libtiffVersionValue = new wxStaticText(this, -1, TIFFGetVersion());
	libtiffVersionTitle->SetForegroundColour(Colors::TextLightGrey);
	libtiffVersionValue->SetForegroundColour(Colors::TextLightGrey);
	libtiffVersionValue->SetFont(wxFont(libtiffVersionValue->GetFont().GetPointSize(), wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, true));
	libtiffVersionValue->Connect(wxEVT_LEFT_DOWN, wxMouseEventHandler(AboutWindow::OnLibTiffWebsiteClick), NULL, this);
	libtiffVersionValue->Connect(wxEVT_ENTER_WINDOW, wxMouseEventHandler(AboutWindow::OnWebsiteMouseEnter), NULL, this);
	libtiffVersionValue->Connect(wxEVT_LEAVE_WINDOW, wxMouseEventHandler(AboutWindow::OnWebsiteMouseLeave), NULL, this);

	libraryAboutSizer->Add(wxWidgetsVersionTitle);
	libraryAboutSizer->Add(wxWidgetsVersionValue);

	libraryAboutSizer->Add(librawVersionTitle);
	libraryAboutSizer->Add(librawVersionValue);
	
	libraryAboutSizer->Add(libtiffVersionTitle);
	libraryAboutSizer->Add(libtiffVersionValue);
	
	okayButton = new PhoediXButton(this, ID_OKAY, "OK");
	okayButton->SetForegroundColour(Colors::TextLightGrey);
	okayButton->SetBackgroundColour(Colors::BackGrey);
	okayButton->SetFont(wxFont(11, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL));

	mainSizer->Add(aboutPhoediX, 0, wxALIGN_CENTER);
	mainSizer->AddSpacer(5);
	mainSizer->Add(aboutSizer);
	mainSizer->AddSpacer(50);
	mainSizer->Add(aboutLibraries);
	mainSizer->AddSpacer(5);
	mainSizer->Add(libraryAboutSizer);
	mainSizer->AddSpacer(20);
	mainSizer->Add(okayButton, 0, wxALIGN_RIGHT);

	this->Bind(wxEVT_CLOSE_WINDOW, (wxObjectEventFunction)&AboutWindow::OnClose, this);
	this->Bind(wxEVT_BUTTON, (wxObjectEventFunction)&AboutWindow::OnOkay, this, ID_OKAY);

	this->Fit();
	this->CentreOnParent();
}

void AboutWindow::OnPhoediXWebsiteClick(wxMouseEvent& WXUNUSED(evt)){
	wxLaunchDefaultBrowser(PHOEDIX_WEBSITE_STRING);
}

void AboutWindow::OnwxWidgetsWebsiteClick(wxMouseEvent& WXUNUSED(evt)){
	wxLaunchDefaultBrowser(WXWIDGETS_WEBSITE_STRING);
}

void AboutWindow::OnLibRawWebsiteClick(wxMouseEvent& WXUNUSED(evt)){
	wxLaunchDefaultBrowser(LIBRAW_WEBSITE_STRING);
}

void AboutWindow::OnLibTiffWebsiteClick(wxMouseEvent& WXUNUSED(evt)){
	wxLaunchDefaultBrowser(LIBTIFF_WEBSITE_STRING);
}

void AboutWindow::OnWebsiteMouseEnter(wxMouseEvent& WXUNUSED(evt)){
	SetCursor(wxCURSOR_HAND);
}
void AboutWindow::OnWebsiteMouseLeave(wxMouseEvent& WXUNUSED(evt)){
	SetCursor(wxCURSOR_ARROW);
}

void AboutWindow::OnClose(wxCloseEvent& WXUNUSED(evt)){
	this->Hide();
}
void AboutWindow::OnOkay(wxCommandEvent& WXUNUSED(evt)){
	this->Hide();
}
