#pragma once
#include <string>
#include <vector>

std::vector<std::string> extractTokens(const std::string& csvLine, char separator = ',');
void test();