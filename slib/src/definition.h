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
 * @file slib_definition.h
 * @date 10/25/20
 * @version 1.0
 * @brief
 */

#pragma once

typedef std::tuple<std::string, std::function<void(void)>>
    expression_match_evaluator_t;

typedef std::vector<expression_match_evaluator_t> statement_parser_t;

using fs_path_t = std::filesystem::path;
using token_values_t =
    std::list<std::pair<std::string, std::function<std::string(void)>>>;

// @brief storage classes
class fn_prototype_t {
public:
  std::string function_name = {};
  std::string documentation = {};
  std::string return_type = {};
  std::string guid = {};
  std::vector<std::string> parameters = {};
};

class slib_command_context_t {
public:
  slib_command_context_t(){};
  slib_command_context_t(slib_command_context_t &cmd) = default;
  // commandline option storage
  std::vector<fs_path_t> command_line_files = {};
  bool cmd_overwrite_files = {};
  bool cmd_client_api = {};
  bool cmd_shared_library = {};
  bool cmd_document = {};
  bool cmd_guid = {};
  bool cmd_uuidgen = {};
  bool cmd_help = {};
  bool cmd_quick_reference = {};
  bool cmd_stub = {};
  bool processing_needed = true;
  fs_path_t cmd_output_directory = {};
  smatch match;
  bool bmatch = {};
};

class slib_definition_t : public slib_command_context_t {
public:
  slib_definition_t() {}
  slib_definition_t &operator=(slib_definition_t &&) = default;
  slib_definition_t(slib_command_context_t &cmd)
      : slib_command_context_t(cmd) {}

  // input files
  fs_path_t slib_filename = {};
  fs_path_t client_api_template_h_input = {};
  fs_path_t shared_library_template_cpp_input = {};
  fs_path_t shared_library_template_h_input = {};
  fs_path_t license_h_input = {};
  fs_path_t license_cpp_input = {};

  // definition class
  std::string class_name = {};
  std::string documentation = {};
  std::string enclosing_namespace = {};
  std::string enclosing_namespace_documentation = {};
  std::string guid = {};

  std::vector<fn_prototype_t> public_member_functions = {};

  // output file
  fs_path_t client_api_h_output = {};
  fs_path_t shared_library_cpp_output = {};
  fs_path_t shared_library_h_output = {};

  std::vector<fs_path_t> files_generated = {};

  std::string clang_format = {};

  /* context */
  std::string complete_statement = {};
  std::string queued_documentation = {};
  std::vector<std::string> search_context;

  // regex
  bool search(const std::string &s);
  bool parse(const string &str, const statement_parser_t &parser);

  void lib();
  void api();
  void tlib();
  void lic();
  void fmt();
  void ns();
  void store_h_cpp_slot(fs_path_t &header_slot, fs_path_t &cpp_slot);

  // reading process.
  bool read();
  bool is_valid();
  void finalize(void);
  bool process_statement();
  bool process_def();
  void add_parameter_definitions(fn_prototype_t &proto, std::string params);

  // client api generation
  bool generate_client_api_interface();
  std::string class_interface();
  std::string public_member_functions_class_api();
  std::string private_guid_function_ties();
  std::string format_fn_implementation(fn_prototype_t proto);
  std::string format_fn_cpp_prototype(fn_prototype_t proto);
  std::string format_fn_std_function_decl(fn_prototype_t proto);

  // shared library generation
  bool generate_shared_library_interface();

  // formatting
  std::string format_guid_cpp_expr(std::string shex);
  std::string format_guid_underlined(std::string shex);

  std::string format_proto(fn_prototype_t &proto);

  bool format_files();
  bool parse_command_line(int argc, char **argv);
};
