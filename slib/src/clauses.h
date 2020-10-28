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
 * @file statement.h
 * @date 10/25/20
 * @version 1.0
 * @brief
 */

#pragma once

const std::string is_shared_library_output =
    "^(shared-library|shared_library|shared\\s*library|shared|library|lib) "
    "*([_a-zA-Z0-9][_a-zA-Z0-9\\.]*)\\s*([_a-zA-Z0-9\\.][_a-zA-Z0-9\\.]*)\\s*;"
    "$";

const std::string is_client_api_output =
    "^(client-api|client_api|client\\s*api|client|api) "
    "*([_a-zA-Z0-9][_a-zA-Z0-9\\.]*)\\s*;$";

const std::string is_shared_library_template_input =
    "^(shared-library-template"
    "|shared_library_template"
    "|shared\\s*library\\s*template\\s*|template|tlibrary|tlib) *"
    "([_a-zA-Z0-9][_a-zA-Z0-9\\.]*)\\s*([_a-zA-Z0-9][_a-zA-Z0-9\\.]*)\\s*;$";

const std::string is_license_input =
    "^(license|lic) "
    "*([_a-zA-Z0-9][_a-zA-Z0-9\\.]*)\\s*([_a-zA-Z0-9][_a-zA-Z0-9\\.]*)\\s*;$";

const std::string is_clang_format_setting =
    "^(clang-format|clang_format|clang\\s*"
    "format|format|fmt) *(LLVM|Google|Chromium|Mozilla|WebKit)\\s*;$";

const std::string is_namespace_def =
    "^(namespace|ns|nm) *([_a-zA-Z][_a-zA-Z]*)\\s*;$";

const std::string is_guid_statement_def =
    "^(?:\\[)([0-9A-Fa-f]{8})(?:-)([0-9A-Fa-f]{4})(?:-)([0-9A-Fa-f]{4})-("
    "[0-9A-Fa-f]{4})-([0-9A-Fa-f]{12})(?:\\])\\s*(.*);$";

const std::string is_class_def = "^(class) *([_a-zA-Z][_a-zA-Z]*)";

const std::string is_bind_def =
    "^([_a-zA-Z][_a-zA-Z]*) *([_a-zA-Z][_-zA-Z]*) *(?:\\()(.*)\\)";
