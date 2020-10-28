#include <fstream>
#include <iostream>
#include <regex>
#include <sstream>
#include <list>
#include <streambuf>
#include <string>
#include <filesystem>

using namespace std;
namespace fs = std::filesystem;

#include "definition.h"
#include "clauses.h"
#include "documentation.h"
#include "utility.h"

bool slib_definition_t::search(const std::string &sexpr) {
  using namespace std::regex_constants;
  using namespace std;

  bmatch = regex_search(search_context.back(), match,
                        regex(sexpr, ECMAScript | icase));
  return bmatch;
}

/**
 *
 */
bool slib_definition_t::parse(const string &str,
                              const statement_parser_t &parser) {
  search_context.push_back(complete_statement);
  for (auto n : parser)
    if (search(get<0>(n)))
      (get<1>(n))();

  search_context.pop_back();

  return true;
}

/*
 *  def statement storage. These functions are called when the regex matches.
 *
 */
void slib_definition_t::lib(void) {
  store_h_cpp_slot(shared_library_h_output, shared_library_cpp_output);
}

void slib_definition_t::api(void) { client_api_h_output = match[2]; }

void slib_definition_t::tlib(void) {
  store_h_cpp_slot(shared_library_template_h_input,
                   shared_library_template_cpp_input);
}

void slib_definition_t::lic(void) {
  store_h_cpp_slot(license_h_input, license_cpp_input);
}

void slib_definition_t::fmt(void) { clang_format = match[2]; }

void slib_definition_t::ns(void) {
  enclosing_namespace = match[2];
  enclosing_namespace_documentation = queued_documentation;
}

void slib_definition_t::store_h_cpp_slot(fs_path_t &header_slot,
                                         fs_path_t &cpp_slot) {

  string ext = fs_path_t(match[2]).extension();

  // one shot, .cpp
  if (ext == ".cpp" || ext == ".c" || ext == ".cc" || ext == ".cxx") {
    cpp_slot = match[2];
    header_slot = match[3];
  } else {
    cpp_slot = match[3];
    header_slot = match[2];
  }
}

/**
 * read_slib - reads the file into the slib_definition_t class.
 */
bool slib_definition_t::read(void) {

  try {
    cout << endl << "Reading " << slib_filename << "." << endl;

    if (!fs::is_regular_file(slib_filename)) {
      cout << "Cannot open file " << slib_filename << endl;
      cout << "Current working path: " << fs::current_path() << endl;
      return false;
    }

    ifstream slib_ifstream(slib_filename);

    if (!slib_ifstream.is_open() || !slib_ifstream.good()) {
      slib_ifstream.close();
      cout << "Cannot open file " << slib_filename << endl;
      cout << "Current working path: " << fs::current_path() << endl;

      return false;
    }

    std::size_t current_line_number = {};
    std::size_t statement_beginning_line_number = {};

    string current_line = {};
    string process_line = {};

    bool multiline_comment = false;

    while (getline(slib_ifstream, current_line)) {
      current_line_number++;
      // trim and skip over blank

      // ignore blanks and comments
      trim(current_line);
      if (current_line.size() == 0 || current_line.compare(0, 2, "//") == 0)
        continue;

      // strip out multi-line comments
      if (!multiline_comment) {
        auto comment_pos = current_line.find("/*");
        if (comment_pos != current_line.npos)
          multiline_comment = true;

        // multiline but on a single line embedded comment
        if (multiline_comment) {
          auto comment_pos_end = current_line.find("*/");
          if (comment_pos_end != current_line.npos) {
            current_line.erase(comment_pos, comment_pos_end + 2 - comment_pos);
            multiline_comment = false;
            trim(current_line);
          } else {
            current_line.erase(comment_pos);
          }
        }
      } else {
        auto comment_pos_end = current_line.find("*/");
        if (comment_pos_end != current_line.npos) {
          current_line.erase(0, comment_pos_end + 2);
          multiline_comment = false;
          trim(current_line);
        } else {
          continue;
        }
      }

      // queue lines until a semicolon is met. ";" eol
      // if a buffer has accumulated, adjust the text input so that lines when
      // joined have at least one space between them. Lines that start with a @
      // are queued as documentation.
      if (complete_statement.size())
        complete_statement.push_back(' ');
      else
        statement_beginning_line_number = current_line_number;

      // documentation
      if (current_line[0] == '@') {
        // strip off the @ and append
        if (queued_documentation.size())
          queued_documentation.push_back(' ');
        queued_documentation += current_line.substr(1);

      } else {
        complete_statement += current_line;
      }

      // queue up until ";" found
      auto semipos = complete_statement.find(";");
      if (semipos == complete_statement.npos)
        continue;

      // the current_statement has been fill with text that has a ;
      if (!process_statement()) {
        if (statement_beginning_line_number == current_line_number)
          cout << "Syntax error on line " << endl;
        else
          cout << "Syntax error on lines " << statement_beginning_line_number
               << " - " << current_line_number << endl;

        cout << complete_statement << endl;
        slib_ifstream.close();
        return false;
      }

      // save wrap around after semicolon for next line processing.
      if (semipos + 1 > complete_statement.size())
        complete_statement = {};
      else
        complete_statement = complete_statement.substr(semipos + 1);

      queued_documentation = {};
    }
    slib_ifstream.close();
  } catch (const std::exception &e) { // caught by reference to base
    std::cout << e.what() << "'\n";
    return false;
  }

  // options built in automatically when not set.
  // like building the output file named based upon the class
  // with a structured name for client and api.
  finalize();

  // validate process
  return is_valid();
}

/**
 * if a complete path and filename is given, it is used.
 */
void slib_definition_t::finalize(void) {

  if (client_api_template_h_input.empty())
    client_api_template_h_input =
        slib_filename.parent_path() / "slib_client_api_template.h";

  if (shared_library_template_cpp_input.empty())
    shared_library_template_cpp_input =
        slib_filename.parent_path() / "slib_shared_library_template.cpp";

  if (shared_library_template_h_input.empty())
    shared_library_template_h_input =
        slib_filename.parent_path() / "slib_shared_library_template.h";

  if (license_cpp_input.empty())
    license_cpp_input =
        slib_filename.parent_path() / "slib_license_default.cpp";

  if (license_h_input.empty())
    license_h_input = slib_filename.parent_path() / "slib_license_default.h";

#if 0
  // definition class
  std::string class_name = {};
  std::string documentation = {};
  std::string enclosing_namespace = {};
  std::string enclosing_namespace_documentation = {};
  std::string guid = {};
  std::vector<fn_prototype_t> public_member_functions = {};
#endif
  std::string s_filename_stem = slib_filename.stem();
  if (client_api_h_output.empty())
    client_api_h_output =
        slib_filename.parent_path() /
        (class_name + "_" + s_filename_stem + "_client_api.h");

  if (shared_library_cpp_output.empty())
    shared_library_cpp_output =
        slib_filename.parent_path() /
        (class_name + "_" + s_filename_stem + "_shared_library.h");

  if (shared_library_h_output.empty())
    shared_library_h_output =
        slib_filename.parent_path() /
        (class_name + "_" + s_filename_stem + "_shared_library.h");

  if (clang_format.empty())
    clang_format = "LLVM";
}

/*
 *
 */
bool slib_definition_t::is_valid(void) {

  bool b = true;

  if (class_name.empty()) {
    b = false;
    cout << "class name empty. " << endl;
  }
  if (guid.empty()) {
    b = false;
    cout << "class GUID empty. " << endl;
  }
  if (shared_library_template_cpp_input.empty()) {
    b = false;
    cout << "shared library input '.cpp' template empty. " << endl;
  }
  if (shared_library_template_h_input.empty()) {
    b = false;
    cout << "shared library input '.h' template empty. " << endl;
  }

  if (client_api_h_output.empty()) {
    b = false;
    cout << "client API output '.h' empty. " << endl;
  }
  if (shared_library_cpp_output.empty()) {
    b = false;
    cout << "shared library output '.cpp' empty. " << endl;
  }
  if (shared_library_h_output.empty()) {
    b = false;
    cout << "shared library output '.h' empty. " << endl;
  }

  if (!cmd_overwrite_files) {
    if (std::filesystem::is_regular_file(client_api_h_output)) {
      b = false;
      cout << "Error: " << client_api_h_output << " already exists." << endl;
    }
    if (std::filesystem::is_regular_file(shared_library_cpp_output)) {
      b = false;
      cout << "Error: " << shared_library_cpp_output << " already exists."
           << endl;
    }
    if (std::filesystem::is_regular_file(shared_library_h_output)) {
      b = false;
      cout << "Error: " << shared_library_h_output << " already exists."
           << endl;
    }
  }

  return b;
}

/**
 *
 */
bool slib_definition_t::generate_client_api_interface() {
  std::string license = {};

  token_values_t tokens = {
      {"license_h_input",
       [&]() {
         string s;
         read_text_file(license_h_input, s);
         return s;
       }},

      {"enclosing_namespace_documentation",
       [&]() { return enclosing_namespace_documentation; }},

      {"enclosing_namespace", [&]() { return enclosing_namespace; }},

      {"class_interface", [&]() { return class_interface(); }},

      {"public_member_functions_class_api",
       [&]() { return public_member_functions_class_api(); }},

      {"private_guid_function_ties",
       [&]() { return private_guid_function_ties(); }}};

  string source_template = {};
  read_text_file(client_api_template_h_input, source_template);

  // process.
  for (auto n : tokens) {
    std::stringstream ss;
    ss << "[%]{2}\\s*(" << n.first << ")\\s*[%]{2}";
    source_template =
        regex_replace(source_template, regex(ss.str()), n.second());
  }

  // write out
  ofstream output_ofstream(client_api_h_output);
  output_ofstream << source_template;
  output_ofstream.close();

  files_generated.emplace_back(client_api_h_output);

  return true;
}

/**
 *
 */
std::string slib_definition_t::class_interface() {
  stringstream ss;
  ss << "interface_guid_t alias = {" << format_guid_cpp_expr(guid) << "};"
     << endl
     << "/* " << documentation << "*/" << endl
     << class_name << "() : client_interface_t(alias) {}" << endl;

  return ss.str();
}

/**
 *
 */
std::string slib_definition_t::public_member_functions_class_api() {
  stringstream ss;
  for (auto n : public_member_functions) {
    if (!n.documentation.empty())
      ss << "/* " << n.documentation << " */" << endl;
    ss << format_fn_cpp_prototype(n) << endl
       << "\t" << format_fn_implementation(n) << endl;
  }
  return ss.str();
}

/**
 *
 */
std::string slib_definition_t::format_fn_implementation(fn_prototype_t proto) {
  stringstream ss;
  ss << " {"
     << "fn_" << format_guid_underlined(proto.guid) << "(";
  for (std::size_t cnt = 0; cnt < proto.parameters.size(); cnt++)
    ss << "p" << cnt << ",";
  ss.seekp(-1, std::ios_base::end);
  ss << ");}" << endl;
  return ss.str();
}

/**
 *
 */
std::string slib_definition_t::format_fn_cpp_prototype(fn_prototype_t proto) {
  stringstream ss;
  int parameter_cnt = {};

  ss << proto.return_type << " " << proto.function_name << "(";
  for (auto param : proto.parameters)
    ss << param << " p" << parameter_cnt++ << ",";
  ss.seekp(-1, std::ios_base::end);
  ss << ")";
  return ss.str();
}

/**
 *
 */
std::string slib_definition_t::private_guid_function_ties() {
  stringstream ss;

  for (auto n : public_member_functions)
    ss << "/* " << format_fn_cpp_prototype(n) << " */" << endl
       << format_fn_std_function_decl(n) << endl;

  return ss.str();
}

/**
 *
 */
std::string
slib_definition_t::format_fn_std_function_decl(fn_prototype_t proto) {
  stringstream ss;
  std::size_t parameter_cnt = {};

  ss << "std::function<";
  ss << proto.return_type << " "
     << "(";
  for (auto param : proto.parameters)
    ss << param << ",";
  ss.seekp(-1, std::ios_base::end);
  ss << ")> ";

  ss << "fn_" << format_guid_underlined(proto.guid) << ";" << endl;
  return ss.str();
}

/**
 * writes the implementation for the shared library connection.
 * The interface files match and the guid file is common. The dispatch
 * within the shared library implementation, the call sockets need to be
 * only filled in with calls to the functions within the project's
 * implementation.
 */
bool slib_definition_t::generate_shared_library_interface() {

  ofstream output_file(shared_library_cpp_output);
  std::string template_source = {};

  if (!read_text_file(shared_library_cpp_output, template_source)) {
    cout << "Error: the template files could not be found. " << endl
         << shared_library_cpp_output << endl
         << shared_library_h_output << endl;
    return false;
  }

  return true;
}

/**
 * runs the clang format on all files generated.
 */
bool slib_definition_t::format_files() {

  if (clang_format.size() == 0) {
    cout << "Warning: clang-format was not specified. files not "
            "formatted.";
    return false;
  }

  cout << "Formatting: " << endl;
  for (auto n : files_generated) {
    cout << "\t " << n;
    stringstream ss;
    ss << "clang-format --style=" << clang_format << " -i " << n;
    auto iret = system(ss.str().data());
    if (iret == -1) {
      cout << endl
           << endl
           << "clang-format had an error or is not installed." << endl
           << "command line : " << ss.str() << endl
           << "return code: " << iret << endl
           << endl;
      return false;
    }

    cout << "." << endl;
  }

  return true;
}

/**
 *
 */
bool slib_definition_t::parse_command_line(int argc, char **argv) {
  fs_path_t *ptr_store_next = {};

  const unordered_map<string, function<void(void)>> option_flags = {
      {"--o", [&]() { cmd_overwrite_files = true; }},
      {"--api", [&]() { cmd_client_api = true; }},
      {"--lib", [&]() { cmd_shared_library = true; }},
      {"--doc", [&]() { cmd_document = true; }},
      {"--guid", [&]() { cmd_guid = true; }},
      {"--uuidgen", [&]() { cmd_uuidgen = true; }},
      {"--help", [&]() { cmd_help = true; }},
      {"--ref", [&]() { cmd_quick_reference = true; }},
      {"--dir", [&]() { ptr_store_next = &cmd_output_directory; }},
      {"--stub", [&]() { cmd_stub = true; }}};

  // no params.
  if (argc < 2) {
    cout << "No command line arguments given." << endl;
    usage();
    return 0;
  }

  // iterate arguments from command line. Each are separate by space or filtered
  // through the command line for quotes. etc. , the quotes are out and file
  // names are separated.
  for (int i = 1; i < argc; i++) {
    std::string token = argv[i];

    // when this pointer is set, it stored the setting from the command line
    // into the string pointed to. this provides the ability to name options
    // '--option' that store a string value. It has to be set back to null after
    // use. The value is set within the option_flags unordered_map.
    if (ptr_store_next) {
      *ptr_store_next = token;
      ptr_store_next = {};
      continue;
    }

    // is it a command options
    if (token.substr(0, 2) == "--") {
      auto it = option_flags.find(token);
      if (it == option_flags.end()) {
        cout << "Error: invalid command line option. " << token;
        return false;
      }
      it->second();

    } else {
      // assume its a filename, use the std::filesystem to check validity
      command_line_files.emplace_back(token);
      if (!command_line_files.back().has_filename()) {
        cout << "Error: invalid filename given. " << token;
        return false;
      }
    }
  }

  // check the string command line arguments for validity.
  if (!cmd_output_directory.empty()) {
    if (cmd_output_directory.has_filename()) {
      cout << "Error: invalid directory given. " << endl;
      cout << cmd_output_directory << " specifies a file name, not a path."
           << endl;
      return false;
    }

    if (!cmd_output_directory.has_parent_path()) {
      cout << "Error: invalid directory given. " << endl;
      cout << cmd_output_directory << " does not specify a path." << endl;
      return false;
    }
    if (!exists(cmd_output_directory)) {
      cout << "Error: directory " << cmd_output_directory << " does not exist. "
           << endl;
      return false;
    }
  }

  // makes program exit with processing but shows the text
  // the variable defaults to true.
  if (cmd_help || cmd_quick_reference)
    processing_needed = false;

  return true;
}
