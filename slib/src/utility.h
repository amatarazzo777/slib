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
 * @file utility.h
 * @date 10/25/20
 * @version 1.0
 * @brief
 */

#pragma once
// https://stackoverflow.com/questions/2602013/read-whole-ascii-file-into-c-stdstring
bool read_text_file(std::filesystem::path &sfilename, std::string &output_data);

// https://stackoverflow.com/questions/20446201/how-to-check-if-string-ends-with-txt/20446257
bool has_suffix(const std::string &str, const std::string &suffix,
                bool case_insensitive = false);

// https://www.techiedelight.com/trim-string-cpp-remove-leading-trailing-spaces/
void ltrim(std::string &s);
void rtrim(std::string &s);
void trim(std::string &s);
