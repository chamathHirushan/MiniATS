#pragma once
#include <string>
#include <vector>

std::vector<std::string> extractWords(const std::string csvLine, char separator = ',');
void test();