#include "DoubleSlider.h"

DoubleSlider::DoubleSlider(wxWindow * parent, double initVal, double minVal, double maxVal, int intervals, int textPrecision) : wxPanel(parent) {

	textPrec = textPrecision;
	wxString formatString = "%." + wxString::Format(wxT("%i"), textPrec) + "f";
	minText = new wxStaticText(this, -1, wxString::Format(formatString, minVal));
	maxText = new wxStaticText(this, -1, wxString::Format(formatString, maxVal));
	valText = new wxTextCtrl(this, - 1, wxString::Format(formatString, maxVal), wxDefaultPosition, wxDefaultSize, wxNO_BORDER | wxTE_PROCESS_ENTER);

	slide = new wxSlider(this, -1, 0, 0, intervals);
	
	min = minVal;
	max = maxVal;
	firedFromSetValue = false;

	topSizer = new wxBoxSizer(wxHORIZONTAL);
	bottomSizer = new wxBoxSizer(wxHORIZONTAL);
	sizer = new wxBoxSizer(wxVERTICAL);
	this->SetSizer(sizer);

	topSizer->Add(valText, 1, wxCENTER);
	bottomSizer->Add(minText);
	bottomSizer->Add(slide);
	bottomSizer->Add(maxText);
	
	this->GetSizer()->Add(topSizer, 1, wxCENTER);
	this->GetSizer()->Add(bottomSizer, 0, wxEXPAND);

	this->Bind(wxEVT_SLIDER, (wxObjectEventFunction)&DoubleSlider::OnSlide, this);
	this->Bind(wxEVT_TEXT_ENTER, (wxObjectEventFunction)&DoubleSlider::OnTextEnter, this);
	this->Bind(wxEVT_TEXT, (wxObjectEventFunction)&DoubleSlider::OnText, this);

	this->SetValue(initVal);

	// Convert current value to the text with specified precision
	double val = this->GetValue();
	wxString valStr = wxString::Format(formatString, val);

	// Update the text control with the controls value
	valText->SetValue(valStr);

	// Fit the value text control with the text
	this->ResizeValueText();

}

void DoubleSlider::SetBackgroundColour(wxColor newColor) {
	minText->SetBackgroundColour(newColor);
	maxText->SetBackgroundColour(newColor);
	valText->SetBackgroundColour(newColor);
	slide->SetBackgroundColour(newColor);
}

void DoubleSlider::SetForegroundColour(wxColor newColor) {
	minText->SetForegroundColour(newColor);
	maxText->SetForegroundColour(newColor);
	valText->SetForegroundColour(newColor);
}

void DoubleSlider::SetValuePosition(int newValuePosition) {
	
	valPos = newValuePosition;
	
	// Remove all edit items from the top and bottom sizers, but do not destroy them
	topSizer->Clear();
	bottomSizer->Clear();

	this->GetSizer()->Detach(topSizer);
	this->GetSizer()->Detach(bottomSizer);

	this->ResizeValueText();

	switch (valPos) {
		case DoubleSlider::VALUE_TOP_LEFT:

			valText->SetWindowStyle(wxTE_LEFT | wxNO_BORDER | wxTE_PROCESS_ENTER);
			topSizer->Add(valText);
			bottomSizer->Add(minText);
			bottomSizer->Add(slide);
			bottomSizer->Add(maxText);

			this->GetSizer()->Add(topSizer, 0, wxALIGN_LEFT);
			this->GetSizer()->Add(bottomSizer, 0, wxEXPAND);
			break;

		case DoubleSlider::VALUE_TOP_CENTER:

			valText->SetWindowStyle(wxTE_CENTER | wxNO_BORDER | wxTE_PROCESS_ENTER);
			topSizer->Add(valText, 0, wxCENTER);
			bottomSizer->Add(minText);
			bottomSizer->Add(slide);
			bottomSizer->Add(maxText);

			this->GetSizer()->Add(topSizer, 0, wxALIGN_CENTER);
			this->GetSizer()->Add(bottomSizer, 0, wxEXPAND);
			break;

		case DoubleSlider::VALUE_TOP_RIGHT:

			valText->SetWindowStyle(wxTE_RIGHT | wxNO_BORDER | wxTE_PROCESS_ENTER);
			topSizer->Add(valText);
			bottomSizer->Add(minText);
			bottomSizer->Add(slide);
			bottomSizer->Add(maxText);

			this->GetSizer()->Add(topSizer, 0, wxALIGN_RIGHT);
			this->GetSizer()->Add(bottomSizer, 0, wxEXPAND);
			break;

		case DoubleSlider::VALUE_BOTTOM_LEFT:

			valText->SetWindowStyle(wxTE_LEFT | wxNO_BORDER | wxTE_PROCESS_ENTER);
			topSizer->Add(minText);
			topSizer->Add(slide);
			topSizer->Add(maxText);
			bottomSizer->Add(valText);

			this->GetSizer()->Add(topSizer, 0, wxEXPAND);
			this->GetSizer()->Add(bottomSizer, 0, wxALIGN_LEFT);
			break;

		case DoubleSlider::VALUE_BOTTOM_CENTER:

			valText->SetWindowStyle(wxTE_CENTER | wxNO_BORDER | wxTE_PROCESS_ENTER);
			topSizer->Add(minText);
			topSizer->Add(slide);
			topSizer->Add(maxText);
			bottomSizer->Add(valText);

			this->GetSizer()->Add(topSizer, 0, wxEXPAND);
			this->GetSizer()->Add(bottomSizer, 0, wxALIGN_CENTER);
			break;

		case DoubleSlider::VALUE_BOTTOM_RIGHT:

			valText->SetWindowStyle(wxTE_RIGHT | wxNO_BORDER | wxTE_PROCESS_ENTER);
			topSizer->Add(minText);
			topSizer->Add(slide);
			topSizer->Add(maxText);
			bottomSizer->Add(valText);

			this->GetSizer()->Add(topSizer, 0, wxEXPAND);
			this->GetSizer()->Add(bottomSizer, 0, wxALIGN_RIGHT);
			break;

		case DoubleSlider::VALUE_INLINE_LEFT:

			valText->SetWindowStyle(wxTE_LEFT | wxNO_BORDER | wxTE_PROCESS_ENTER);
			topSizer->Add(valText);
			topSizer->AddSpacer(10);
			topSizer->Add(minText);
			topSizer->Add(slide);
			topSizer->Add(maxText);

			this->GetSizer()->Add(topSizer, 0, wxEXPAND);
			this->GetSizer()->Add(bottomSizer);
			break;

		case DoubleSlider::VALUE_INLINE_RIGHT:

			valText->SetWindowStyle(wxTE_RIGHT | wxNO_BORDER | wxTE_PROCESS_ENTER);
			topSizer->Add(minText);
			topSizer->Add(slide);
			topSizer->Add(maxText);
			topSizer->AddSpacer(10);
			topSizer->Add(valText);

			this->GetSizer()->Add(topSizer, 0, wxEXPAND);
			this->GetSizer()->Add(bottomSizer);
			break;
	}

	topSizer->Layout();
	bottomSizer->Layout();
	this->GetSizer()->Layout();
	this->Fit();
}

void DoubleSlider::OnSlide(wxCommandEvent& slideEvt) {

	// Convert current value to the text with specified precision
	double val = this->GetValue();
	wxString formatString = "%." + wxString::Format(wxT("%i"), textPrec) + "f";
	wxString valStr = wxString::Format(formatString, val);

	// Update the text control with the controls value
	valText->SetValue(valStr);

	// Fit the value text control with the text
	this->ResizeValueText();

	// Send the event up to the parent window for further handling
	slideEvt.Skip();
}

void DoubleSlider::OnText(wxCommandEvent& textEvt) {

	if (firedFromSetValue) { return; }

	// Get the current value text
	wxString strVal = valText->GetValue();
	double val;
	strVal.ToDouble(&val);

	// Position the slider based on the new text
	this->SetValue(val);

	// Fit the value text control with the text
	this->ResizeValueText();

	textEvt.Skip();
}

void DoubleSlider::OnTextEnter(wxCommandEvent& textEvt) {

	// Get the current value text
	wxString strVal = valText->GetValue();
	double val;
	strVal.ToDouble(&val);

	// Verify the text is in bounds.  If not, rewrite the text as min or max
	if (val < min) {

		// Set val to minimum if it is less than minimum
		val = min;

		// Format the string correctly and set the text
		wxString formatString = "%." + wxString::Format(wxT("%i"), textPrec) + "f";
		wxString valStr = wxString::Format(formatString, val);
		valText->SetValue(valStr);
	}
	else if (val > max) {

		// Set val to maximum if it is greater than maximum
		val = max;

		// Format the string correctly and set the text
		wxString formatString = "%." + wxString::Format(wxT("%i"), textPrec) + "f";
		wxString valStr = wxString::Format(formatString, val);
		valText->SetValue(valStr);
	}

	// Position the slider based on the new text
	this->SetValue(val);

	// Fit the value text control with the text
	this->ResizeValueText();

	textEvt.Skip();
}

void DoubleSlider::ResizeValueText() {

	// Get the width of the text of the value text, so we can size it correctly
	wxClientDC valDc(valText);
	wxString valStr = valText->GetValue();
	wxCoord textWidth = 0;
	wxCoord textHeight = 0;
	valDc.GetTextExtent(valStr, &textWidth, &textHeight);

	// Resize the value text with the current text size
	valText->SetMinSize(wxSize(textWidth + 10, textHeight));
	valText->Layout();
}

void DoubleSlider::ShowMinMax() {
	this->GetSizer()->Show(minText);
	this->GetSizer()->Show(maxText);
}

void DoubleSlider::HideMinMax() {
	this->GetSizer()->Hide(minText);
	this->GetSizer()->Hide(maxText);
}

void DoubleSlider::ShowVal() {
	this->GetSizer()->Show(valText);
}

void DoubleSlider::HideVal() {
	this->GetSizer()->Hide(valText);
}

double DoubleSlider::GetValue() {

	int location = slide->GetValue();
	int intervals = slide->GetMax();

	double diff = max - min;
	double scale = (double)location/(double)intervals;
	double diffScale = diff * scale;
	double diffScaleShift = diffScale + min;

	return diffScaleShift;
}

void DoubleSlider::SetValue(double newVal) {

	if (newVal < min) { newVal = min; }
	if (newVal > max) { newVal = max; }

	double diff = max - min;
	double locationScale = newVal / diff;
	int intervals = slide->GetMax();

	int newSlideLocation = (locationScale - (min/diff)) * intervals;
	slide->SetValue(newSlideLocation);

	// Convert current value to the text with specified precision
	wxString formatString = "%." + wxString::Format(wxT("%i"), textPrec) + "f";
	wxString valStr = wxString::Format(formatString, newVal);

	// Update the text control with the controls value
	firedFromSetValue = true;
	valText->SetValue(valStr);
	firedFromSetValue = false;

	// Fit the value text control with the text
	this->ResizeValueText();
}