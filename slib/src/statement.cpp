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
 * @file statement.cpp
 * @date 10/25/20
 * @version 1.0
 * @brief
 */
#include "base_inc.h"

using namespace std;

/**
 * the routine decides how to interpret the statement using regular
 * expressions and sets the class information.
 */
bool slib_definition_t::process_statement() {
  return parse(complete_statement,
               {{is_shared_library_output, [&]() { lib(); }},
                {is_client_api_output, [&]() { api(); }},
                {is_shared_library_template_input, [&]() { tlib(); }},
                {is_license_input, [&]() { lic(); }},
                {is_clang_format_setting, [&]() { fmt(); }},
                {is_namespace_def, [&]() { ns(); }},
                {is_guid_statement_def, [&]() { process_def(); }}});
}

/**
 *
 */
bool slib_definition_t::process_def() {
  using namespace std::regex_constants;
  using namespace std;

  // regex is dependable, these are coded literal. The routine parses the
  // last part to identify what type of syntax it is. class or function
  // binding. The function prototype is processed separately.

  const std::size_t num_declaration_tokens = 7;

  if (match.size() != num_declaration_tokens)
    return false;

  // guids are made of 5 parts within the declaration.
  const std::size_t num_guid_tokens = 5;

  string sguid = {};
  for (std::size_t j = 1; j <= num_guid_tokens; j++)
    sguid += string(match[j]);

  // check the declaration
  string sdeclaration = match[6];
  search_context.push_back(sdeclaration);

  bool valid_def = false;

  // is it a class def?
  if (search(is_class_def)) {
    class_name = match[2];
    guid = sguid;
    documentation = queued_documentation;
    valid_def = true;

    // is it a bind fn?
  } else if (search(is_bind_def)) {
    fn_prototype_t proto = {};
    proto.return_type = match[1];
    proto.function_name = match[2];
    proto.guid = sguid;
    proto.documentation = queued_documentation;

    std::string params = match[3];
    add_parameter_definitions(proto, params);

    // add to prototypes, the proto variable has the vector of parameter
    // types, function name, etc.
    public_member_functions.emplace_back(proto);
    valid_def = false;
  }
  search_context.pop_back();
  return valid_def;
}

/**
 * adds the parameter splitting them by ,
 */
void slib_definition_t::add_parameter_definitions(fn_prototype_t &proto,
                                                  std::string params) {
  size_t begin = 0;
  size_t pos = 0;
  std::string token;

  // add each parameter
  while ((pos = params.find(",", begin)) != std::string::npos) {
    token = params.substr(begin, pos - begin);
    trim(token);
    proto.parameters.emplace_back(token);
    begin = pos + 1;
  }
  token = params.substr(begin);
  trim(token);
  proto.parameters.emplace_back(token);
}

/**
 * formats the guid as a c++ source. this is used to extend the class with
 * static variables.
 */
string slib_definition_t::format_guid_cpp_expr(string shex) {
  string ret = {};
  stringstream ss;
  for (auto it = shex.begin(); it < shex.end(); it += 2) {
    ss << "0x" << *it << *(it + 1);
    if (distance(it, shex.end()) > 2)
      ss << ", ";
  }
  return ss.str();
}

/**
 * formats the guid as a c++ source. this is used to extend the class with
 * static variables.
 */
string slib_definition_t::format_guid_underlined(string shex) {
  string ret = {};
  stringstream ss;
  for (auto it = shex.begin(); it < shex.end(); it += 2) {
    ss << *it << *(it + 1);
    if (distance(it, shex.end()) > 2)
      ss << "_";
  }
  return ss.str();
}
/**
 *
 */
string slib_definition_t::format_proto(fn_prototype_t &n) {
  string ret = n.return_type + "(";

  for (auto iter = n.parameters.begin(); iter != n.parameters.end(); ++iter) {
    // do something for all iterations
    ret += *iter;
    if (std::next(iter) != n.parameters.end())
      ret += ", ";
  }
  ret += ")";
  return ret;
}
