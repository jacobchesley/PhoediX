#include "ProcessorEdit.h"

ProcessorEdit::ProcessorEdit() {
	doMultithread = false;
	numThread = 0;
}

ProcessorEdit::ProcessorEdit(int editType, bool multiThead, int numThreads) {
	edit = editType;
	doMultithread = multiThead;
	numThread = numThreads;
}

void ProcessorEdit::AddParam(double param) {
	params.push_back(param);
}

void ProcessorEdit::ClearParams() {
	params.clear();
}

int ProcessorEdit::GetParamsSize() {
	return params.size();
}

double ProcessorEdit::GetParam(size_t index) {
	return params.at(index);
}

void ProcessorEdit::AddFlag(int flag) {
	flags.push_back(flag);
}

void ProcessorEdit::ClearFlags() {
	flags.clear();
}

int ProcessorEdit::GetFlagsSize() {
	return flags.size();
}

double ProcessorEdit::GetFlag(size_t index) {
	return flags.at(index);
}

int ProcessorEdit::GetEditType() {
	return edit;
}

void ProcessorEdit::SetEditType(int editType) {
	edit = editType;
}

bool ProcessorEdit::GetMultithread() {
	return doMultithread;
}

void ProcessorEdit::SetMultithread(bool multiThread) {
	doMultithread = multiThread;
}

int ProcessorEdit::GetNumThreads(){
	return numThread;
}

void ProcessorEdit::SetNumThreads(int numThreads) {
	numThread = numThreads;
}