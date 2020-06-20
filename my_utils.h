#pragma once
#include <string>

void PrintProgressBar(float progress);
// void UpdateProgressBar();

std::string readFile2(const std::string &fileName);
std::wstring widen(const std::string& utf8_string);
std::wstring readFileWideChars(const std::string &fileName);
