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
 * @file generator.cpp
 * @date 10/25/20
 * @version 1.0
 * @brief
 */
#include "base_inc.h"

using namespace std;

/**
 *
 */

generator_t::generator_t(fs_path_t &_target_export_header,
                         fs_path_t &_slib_template_input) {
  target_export_header_input = _target_export_header;
  slib_template_input = _slib_template_input;
}

/**
 *
 */
bool generator_t::process() {
  using namespace std::regex_constants;
  using namespace std;

  output_name = target_export_header_input / ".slib";
  cout << "Reading " << target_export_header_input << endl;
  std::string sinput;
  if (!read_text_file(target_export_header_input, sinput)) {
    cout << "Error, cannot open file " << target_export_header_input << endl;
    return false;
  }

  std::string sexpr =
      "(?:\\/\\/\\s*(?:\\[slib-export-begin\\]))([\\S\\s]*)(?:\\/\\/"
      "\\s*\\[slib-export-end\\])";
  smatch match;
  if (!std::regex_search(sinput, match, std::regex(sexpr, ECMAScript))) {
    cout << "Error, the input file " << target_export_header_input
         << " was not processed successfully." << endl
         << "The markers '// [slib-export-begin]' and '// [slib-export-end]' "
            "were not located."
         << endl;
    return false;
  }

  std::string target_export_prototypes = match[1];
  if (!insert_guid_markers(target_export_prototypes))
    return false;

  // [slib-export]

  output_name = target_export_header_input / ".slib";

  cout << "Reading template " << slib_template_input << endl;

  std::string slib_template;
  if (!read_text_file(slib_template_input, slib_template))
    return false;

  token_values_t tokens = {
      {"interface_name",
       [&]() {
         stringstream ss;
         ss << output_name.stem() << "_slib_interface_t";
         return ss.str();
       }},
      {"namespace",
       [&]() {
         stringstream ss;
         ss << output_name.stem();
         return ss.str();
       }},
      {"interface", [&]() { return target_export_prototypes; }}};

  // process.
  for (auto n : tokens) {
    std::stringstream ss;
    ss << "[%]{2}\\s*(" << n.first << ")\\s*[%]{2}";
    slib_template = regex_replace(slib_template, regex(ss.str()), n.second());
  }

  // write out
  cout << "Writing " << output_name << endl;

  ofstream output_ofstream(output_name);
  output_ofstream << slib_template;
  output_ofstream.close();
  return true;
}

/**
 *
 */
bool generator_t::insert_guid_markers(std::string &target) {
	 using namespace std::regex_constants;
	  using namespace std;
  std::string sexpr = "([a-z][\\S ]*[^;];)";
  smatch match;
  if (!std::regex_search(target, match, std::regex(sexpr)))
    return false;

  std::stringstream ss;
  std::size_t target_pos_begin = {};

  std::vector<std::string> guids = {};
  if (!batch_guid_gen(match.size(), guids))
    return false;
  for (std::size_t idx = 0; idx < match.size(); idx++) {
    cout << match[idx] << endl;
  }
  for (std::size_t idx = 0; idx < match.size(); idx++) {
    ss << target.substr(target_pos_begin, match.position(idx) - 1) << guids[idx]
       << " " << match[idx];

    target_pos_begin = match.position(idx);
  }
  target = ss.str();
  return true;
}

/**
 *
 */
bool generator_t::batch_guid_gen(std::size_t amount,
                                 std::vector<std::string> &guids) {
  std::stringstream uuidgen_result = {};
  stringstream cmd;
  cout << "Generating GUIDs " << amount;
  for (std::size_t idx = 0; idx < amount; idx++) {
    system("uuidgen > tmp.guid");
    string sguid = {};
    fs_path_t guid_file("tmp.guid");
    if (!read_text_file(guid_file, sguid))
      return false;
    trim(sguid);
    sguid = "[" + sguid + "]";
    guids.emplace_back(sguid);
  }

  return true;
}
