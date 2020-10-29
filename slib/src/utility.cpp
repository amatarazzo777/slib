/*
 * This file is part of the ux_gui_stream distribution
 * (https://github.com/amatarazzo777/ux_gui_stream).
 * Copyright (c) 2020 Anthony Matarazzo.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, version 3.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

/**
 * @author Anthony Matarazzo
 * @file utility.cpp
 * @date 10/25/20
 * @version 1.0
 * @brief
 */
#include <fstream>
#include <iostream>
#include <regex>
#include <sstream>
#include <streambuf>
#include <string>
#include <filesystem>

#include <cstdio>
#include <memory>
#include <stdexcept>
#include <string>
#include <array>

#include "utility.h"

using namespace std;
/**
 *
 * https://stackoverflow.com/questions/2602013/read-whole-ascii-file-into-c-stdstring
 */
bool read_text_file(std::filesystem::path &sfilename,
                    std::string &output_data) {
  if (!std::filesystem::is_regular_file(sfilename))
    return false;

  try {
    ifstream finput(sfilename);
    finput.seekg(0, std::ios::end);
    output_data.reserve(finput.tellg());
    finput.seekg(0, std::ios::beg);
    output_data.assign((std::istreambuf_iterator<char>(finput)),
                       std::istreambuf_iterator<char>());
    finput.close();

  } catch (const std::exception &e) { // caught by reference to base
    std::cout << e.what() << "'\n";
    return false;
  }

  return true;
}

// https://stackoverflow.com/questions/20446201/how-to-check-if-string-ends-with-txt/20446257
// https://stackoverflow.com/questions/11635/case-insensitive-string-comparison-in-c
bool has_suffix(const std::string &str, const std::string &suffix,
                bool bcase_insensitive) {
  bool bhas = false;
  if (str.size() < suffix.size())
    bhas = false;
  else {
    if (bcase_insensitive) {
      auto str_pos = str.end() - suffix.size();
      bhas = std::equal(str_pos, str.end(), suffix.begin(), [](auto a, auto b) {
        return std::tolower(a) == std::tolower(b);
      });
    } else
      bhas =
          str.compare(str.size() - suffix.size(), suffix.size(), suffix) == 0;
  }
  return bhas;
}

// https://www.techiedelight.com/trim-string-cpp-remove-leading-trailing-spaces/
const std::string WHITESPACE = " \n\r\t\f\v";
void ltrim(std::string &s) {
  size_t start = s.find_first_not_of(WHITESPACE);
  s = (start == std::string::npos) ? "" : s.substr(start);
}

void rtrim(std::string &s) {
  size_t end = s.find_last_not_of(WHITESPACE);
  s = (end == std::string::npos) ? "" : s.substr(0, end + 1);
}

void trim(std::string &s) {
  ltrim(s);
  rtrim(s);
}


