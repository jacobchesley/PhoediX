// Copyright 2016 Jacob Chesley

#include "Settings.h"

int PhoedixSettings::bitDepth = 8;
int PhoedixSettings::colorSpace = 0;
int PhoedixSettings::numThreads = 0;

int PhoedixSettings::GetBitDepth() {
	return bitDepth;
}
void PhoedixSettings::SetBitDepth(int newBitDepth) {
	bitDepth = newBitDepth;
}

int PhoedixSettings::GetColorSpace() {
	return colorSpace;
}
void PhoedixSettings::SetColorSpace(int newColorSpace) {
	colorSpace = newColorSpace;
}

int PhoedixSettings::GetNumThreads() {
	return numThreads;
}
void PhoedixSettings::SetNumThreads(int newNumThreads) {
	numThreads = newNumThreads;
}