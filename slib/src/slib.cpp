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
#include <string>

using namespace std;

// @brief storage classes
class fn_prototype_t {
public:
  string function_name = {};
  string documentation = {};
  string return_type = {};
  string guid = {};
  vector<string> parameters = {};
};

class slib_definition_t {
public:
  string class_name = {};
  string documentation = {};
  string enclosing_namespace = {};
  string enclosing_namespace_documentation = {};
  string guid = {};
  string output_filename = {};
  string clang_format = {};
  vector<fn_prototype_t> prototypes;
};

// function prototypes
bool read_slib(const std::string &input_file, slib_definition_t &slib_decl);
bool process_statement(slib_definition_t &decl, std::string &line_statement,
                       std::string &doc);
void write_interface_header(slib_definition_t &decl);

void add_parameter_definitions(fn_prototype_t &proto, std::string params);

string format_guid(string shex);
string format_proto(fn_prototype_t &proto);

// https://stackoverflow.com/questions/20446201/how-to-check-if-string-ends-with-txt/20446257
bool has_suffix(const std::string &str, const std::string &suffix);

// https://www.techiedelight.com/trim-string-cpp-remove-leading-trailing-spaces/
void ltrim(std::string &s);
void rtrim(std::string &s);
void trim(std::string &s);

/**
 * main
 *
 */
int main(int argc, char **argv) {
  cout << "Shared Library Interface (slib) generates .h file\nby Anthony "
          "Matarazzo"
       << endl;

  if (argc != 2) {
    cout << "usage: slib infile" << endl << endl;

    cout << "slib is a command line source generation utility. If is used for "
            "c++ development. It makes use of shared libraries using the "
            "standard library as interface. It reads a slib file and writes a "
            "header file to do this. The header file  "
            "will link to the named library (.so or .dll) and resolve the "
            "symbols automatically. "
            "The header file can be included in your project"
            "to manage shared library and interface object that are version "
            "oriented and associated by a GUID.\n\n"

         << endl;
    return 0;
  }

  string infile = argv[1];

  // read file
  cout << "Reading file " << infile << endl;

  slib_definition_t decl;
  bool ret = false;

  ret = read_slib(infile, decl);
  if (!ret)
    return 0;

  write_interface_header(decl);

  if (decl.clang_format.size() != 0) {
    string scmd = "clang-format --style=" + decl.clang_format + " -i " +
                  decl.output_filename;
    cout << "Formatting." << endl;
    auto iret = system(scmd.data());
    if (iret == -1)
      cout << "warning clang-format is  had an error.  unable to format. "
           << endl
           << "command line : " << scmd << endl
           << "return code: " << iret << "\n\n";
  }

  cout << "The header file was generated successfully. " << endl;
}

/**
 * read_slib - reads the file into the slib_definition_t class.
 */
bool read_slib(const std::string &input_file, slib_definition_t &decl) {

  try {
    std::ifstream infile(input_file);
    std::size_t line_number = {};
    string line = {};
    string process_line = {};
    string documentation = {};

    while (getline(infile, line)) {
      line_number++;
      // trim and skip over blank

      // ignore blanks and comments
      trim(line);
      if (line.size() == 0 || line.compare(0, 2, "//") == 0)
        continue;

      // queue lines until a semicolon is met. ";" eol
      // is a buffer has accumulated, adjust the text input so that lines when
      // join have at least one space between them. Lines that start with a @
      // are queued as documentation.
      if (process_line.size())
        process_line.push_back(' ');

      if (line[0] == '@') {
        // strip off the @ and append
        if (documentation.size())
          documentation.push_back(' ');
        documentation += line.substr(1);

      } else {
        process_line += line;
      }

      auto semipos = process_line.find(";");
      if (semipos == process_line.npos)
        continue;

      if (!process_statement(decl, process_line, documentation)) {
        cout << "Syntax error on line " << line_number << endl << line << endl;
        infile.close();
        return false;
      }

      // save wrap around after semicolon for next line processing.
      if (semipos + 1 > process_line.size())
        process_line = {};
      else
        process_line = process_line.substr(semipos + 1);

      documentation = {};
    }
    infile.close();
  } catch (const std::exception &e) { // caught by reference to base
    std::cout << e.what() << "'\n";
    return false;
  }

  return true;
}

/**
 * the routine decides how to interpret the statement using regular expressions
 * and sets the class information.
 */
bool process_statement(slib_definition_t &decl, std::string &line_statement,
                       std::string &doc) {

  const string is_clang_format_option =
      "^(clang-format) *([_a-zA-Z][_a-zA-Z]*)\\s*;$";

  const string is_namespace_def = "^(namespace) *([_a-zA-Z][_a-zA-Z]*)\\s*;$";

  const string extract_guid_and_def =
      "^(?:\\[)([0-9A-Fa-f]{8})(?:-)([0-9A-Fa-f]{4})(?:-)([0-9A-Fa-f]{4})-("
      "[0-9A-Fa-f]{4})-([0-9A-Fa-f]{12})(?:\\])\\s*(.*);$";

  const string is_class_def = "^(class) *([_a-zA-Z][_a-zA-Z]*)";

  const string is_bind_def =
      "^([_a-zA-Z][_a-zA-Z]*) *([_a-zA-Z][_-zA-Z]*) *(?:\\()(.*)\\)";

  string documentation = doc;

  smatch match;
  bool bvalid = false;

  if (regex_search(line_statement, match, regex(is_clang_format_option))) {
    decl.clang_format = match[2];

  } else if (regex_search(line_statement, match, regex(is_namespace_def))) {
    decl.enclosing_namespace = match[2];
    decl.enclosing_namespace_documentation = documentation;

  } else {

    bvalid = regex_search(line_statement, match, regex(extract_guid_and_def));

    // regex is dependable, these are coded literal. The routine parses the last
    // part to identify what type of syntax it is. class or function binding.
    // The function prototype is processed separately.
    const std::size_t num_declaration_tokens = 7;

    if (bvalid)
      if (match.size() != num_declaration_tokens)
        bvalid = false;

    if (!bvalid)
      return false;

    // guids are made of 5 parts within the declaration.
    const std::size_t num_guid_tokens = 5;

    string sguid = {};
    for (std::size_t j = 1; j <= num_guid_tokens; j++)
      sguid += string(match[j]);

    sguid = format_guid(sguid);

    // check the declaration
    string sdeclaration = match[6];

    // is it a class def?
    if (regex_search(sdeclaration, match, regex(is_class_def))) {
      decl.class_name = match[2];
      decl.guid = sguid;
      decl.documentation = documentation;

      // is it a bind fn?
    } else if (regex_search(sdeclaration, match, regex(is_bind_def))) {
      fn_prototype_t proto = {};
      proto.return_type = match[1];
      proto.function_name = match[2];
      proto.guid = sguid;
      proto.documentation = documentation;

      std::string params = match[3];
      add_parameter_definitions(proto, params);

      // add to prototypes, the proto variable has the vector of parameter
      // types, function name, etc.
      decl.prototypes.emplace_back(proto);
      documentation = {};

    } else {
      return false;
    }
  }

  return true;
}

/**
 * adds the parameter splitting them by ,
 */
void add_parameter_definitions(fn_prototype_t &proto, std::string params) {
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
string format_guid(string shex) {
  string ret = {};
  int cnt = 0;
  for (std::size_t i = 0; i < shex.size(); i += 2) {
    ret += "0x" + shex.substr(i, 2);
    // no comma on last one
    if (i < shex.size() - 2)
      ret += ", ";

    cnt++;
    if (cnt == 8 && i != shex.size() - 2) {
      cnt = 0;
      ret += "\n\t";
    }
  }
  return ret;
}

/**
 *
 */
void write_interface_header(slib_definition_t &decl) {
  std::string license = {};

  // if license exists.
  try {
    std::ifstream license_file("header_license.txt");
    std::stringstream buffer;
    buffer << license_file.rdbuf();
    license = buffer.str();
    license_file.close();

  } catch (const std::exception &e) {

    license = R"(
	  /*
	   * This file is part of the PROJECT_NAME
	   * (GITHUB_URL).
	   * Copyright (c) 2020 AUTOR.
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
	  )";
  }

  decl.output_filename = decl.class_name;

  // build out file name from class name, remove _t if it is there. the class
  // header has the same name as the class.
  if (has_suffix(decl.output_filename, "_t")) {
    decl.output_filename.pop_back();
    decl.output_filename.pop_back();
  }
  decl.output_filename += ".h";
  ofstream header_file(decl.output_filename);

  // license header
  header_file << license << endl;

  // guid namespace for the functions
  header_file
      << "/* the namespace provides lookup for the guid by the same name. */\n";
  header_file << "namespace interface_alias {\n";
  header_file << "typedef static const std::array<std::unit8_t, 16> "
                 "interface_guid_t;\n\n";

  for (auto n : decl.prototypes) {
    header_file << "interface_guid_t " << n.function_name << " = \n"
                << "\t{" << n.guid << "};\n\n";
  }
  header_file << "} // namespace interface_alias\n\n\n";

  // the class is inside a namespace
  if (decl.enclosing_namespace.size()) {
    header_file << "/* " << decl.enclosing_namespace_documentation << "*/ \n";
    header_file << "namespace " << decl.enclosing_namespace << " { \n";
  }

  // class declaration
  header_file << "class " << decl.class_name
              << " : public client_interface_t { \n"
              << "public:\n";

  header_file << "\t\t/* "
              << "Interface used as public:"
              << " */ \n";
  // public function member variables.
  for (auto n : decl.prototypes) {
    if (n.documentation.size() > 0)
      header_file << "\t\t/* " << n.documentation << " */ \n";
    string std_function_proto = format_proto(n);

    header_file << "\tstd::function<" << std_function_proto << "> "
                << n.function_name << " = {};\n\n";
  }
  header_file << "\n\n";

  header_file << "\t\t/* "
              << "Map map used to tie functions to their pointer. "
              << " */ \n";

  header_file << "\tstatic const client_interface_t::interface_guid_map_t \n"
                 "\t\tguid_index = {\n\n";

  for (auto iter = decl.prototypes.begin(); iter != decl.prototypes.end();
       ++iter) {

    auto n = *iter;

    header_file << "\t\t{interface::alias::" << n.function_name << ",\n "
                << "\t\t[](auto o, auto fn) {\n"
                << "\t\t\to." << n.function_name << " = std::bind<"
                << format_proto(n) << ">("
                << "fn";

    std::stringstream splaces;
    for (std::size_t j = 1; j <= n.parameters.size(); j++) {
      splaces << ",std::placeholders::_" << j;
    }
    header_file << splaces.str() << ");}}";

    header_file << "\n";

    if (std::next(iter) != decl.prototypes.end())
      header_file << ", \n";
  }
  header_file << "}; \n";
  header_file << "};\n";

  // the class is inside a namespace
  if (decl.enclosing_namespace.size()) {
    header_file << "} // namespace " << decl.enclosing_namespace;
  }

  header_file.close();
}

/**
 *
 */
string format_proto(fn_prototype_t &n) {
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

// https://stackoverflow.com/questions/20446201/how-to-check-if-string-ends-with-txt/20446257
bool has_suffix(const std::string &str, const std::string &suffix) {
  return str.size() >= suffix.size() &&
         str.compare(str.size() - suffix.size(), suffix.size(), suffix) == 0;
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
