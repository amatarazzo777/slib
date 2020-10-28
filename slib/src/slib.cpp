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
 * @file slib.cpp
 * @date 10/25/20
 * @version 1.0
 * @brief
 */
#include <fstream>
#include <iostream>
#include <regex>
#include <sstream>
#include <list>
#include <streambuf>
#include <string>
#include <filesystem>

using namespace std;

#include "definition.h"
#include "clauses.h"
#include "utility.h"
#include "documentation.h"

/**
 * main
 *
 */
int main(int argc, char **argv) {
  cout << "Shared Library Interface Generator (slib)" << endl;

  // read file
  slib_definition_t decl;

  // false returns tell not to continue, but might
  if (!decl.parse_command_line(argc, argv))
    return 0;

  if (decl.cmd_quick_reference)
    quick_reference();

  if (decl.cmd_help)
    full_documentation();

  if (!decl.processing_needed)
    return 0;

  for (auto n : decl.command_line_files) {
    decl.slib_filename = n;
    if (!decl.read()) {
      cout << endl << "No files were generated. " << endl;
      cout << "Exiting." << endl;
      return 0;
    }

    if (!decl.generate_client_api_interface()) {
      cout << "The client API could not be generated. " << endl << endl;
      cout << "Exiting." << endl;
      return 0;
    }

    if (!decl.generate_shared_library_interface()) {
      cout << "The shared library interface not be generated. " << endl;
      cout << "Exiting." << endl;
      return 0;
    }

    decl.format_files();

    cout << "These files were successfully generated:" << endl;
    for (auto n : decl.files_generated)
      cout << "\t" << n << endl;

    slib_command_context_t tmp = decl;
    decl = {};
    decl = tmp;
  }

  cout << "Finished." << endl;
}
