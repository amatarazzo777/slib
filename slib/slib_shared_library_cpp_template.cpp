#include <base/std_base.h>
#include "library_linkage.h"
#include "client_interface.h"

// default
uxdevice::client_interface_t::client_interface_t() {}

// loadns the symbols and the guid map.
uxdevice::client_interface_t::client_interface_t(
    const std::string &_library_name, double _version) {
  initialize(_library_name, _version);
}

// deconstructor
uxdevice::client_interface_t::~client_interface_t() { fn_library_close(); }

/**
 *
 * @brief The client interface module provides the separation layer between the
 * lighter client api objects which are template class factory composition
 * (units) to the shared library which contains the implementations. Across this
 * linkage no std library objects can be shared. Only structures composed of raw
 * data. The standard template library (STD) may be completely different between
 * the .so file and the client. Each side while using the shared library
 * internally, provides these raw linkages. Not forgoing the primary goal of
 * client composition in c++ source API invocation, these objects must be
 * accessed with ease.
 *
 * The method used is a version one where the client requests a specific
 * version. This is accomplished through one dl sym with the parameter of
 * version. This structure is common in the compile base between both.
 *
 * Objects and functions that are not supported in both version directions can
 * be maintained. Newer programs can run on older ux_gui_so with that
 * functionality not included.
 *
 * The limited functionality interface discovery and manufacture also means that
 * robust and largely text processing that use shape rendering will operate yet
 * in cases where the client does not support that type of rendering because of
 * its compile base can function.
 *
 * These linkages also provide the ability to let other
 *  add ons be distributed as a template or another shared library. This gives
 * the ability to expand using the base and apply it to different versions of
 * the ux_gui client. The downgrade_fn provides the ability for new software to
 * run on older code bases. The enables some types of dynamic software
 * distribution to embedded that the system base is not updated. However this is
 * not combined with any security at this layer for upgrade or downgrade
 * interfaces.
 *
 */
void uxdevice::client_interface_t::initialize(const std::string &library,
                                              double version_number) {

  /// @brief initialize the interface export functions
  try {
    fn_library_open(library);

    /// @brief informs about what the system version
    fn_system_version =
        std::bind<void(void)>(fn_library_symbol("fn_system_version"));

    /// @brief number of entries in the export guid interface. Note that the
    /// version is passed to this function as a double. i.e. 1.0, 1.1
    fn_guid_interface_linkage_size = bind<std::size_t(double)>(
        fn_library_symbol("fn_guid_interface_linkage_size"),
        std::placeholders::_1);

    /// @brief the size of the buffer and the
    fn_guid_interface_linkage = bind<void(double, void *, size_t)>(
        fn_library_symbol("fn_guid_interface_linkage"), std::placeholders::_1,
        std::placeholders::_2, std::placeholders::_3);

  } catch (const std::exception &e) {
    std::stringstream serror;
    serror << "The client_interface_t , requested file = " << library
           << " requested version = " << version_number
           << " could not be loaded. ";
    serror << e.what() << "  ";
    if (dlsym_error = dlerror())
      serror << dlsym_error;
    throw std::runtime_error(serror.str());
  }

  /// @brief now process the guid linkage interface.  get the guid linkage table
  /// from the shared library

  // version constant, static within the system
  system_verion = fn_system_version();

  // size
  std::size_t link_table_size = fn_guid_interface_linkage_size(version_number);

  // reserve memory and use raw buffers as to not disturb std cross boundary
  // compiler.
  std::vector<link_table_entry_t> link_table;
  link_table.reserve(link_table_size);

  // fill the link_table with the data. The link_table_entry_t has a guid alias
  // attached. for version decoding. This is a base guid and will cause issues
  // if the link_table_entry_t changes. This is due to the fact that the
  // link_table_entry_t version might be greater. A solution would be to install
  // format converter inside the structure. But the changing of this structure
  // is on a component base level, most likely will not occur. But this can be
  // error checked.
  fn_guid_interface_linkage_t(link_table_entry_t::alias, version_number,
                              link_table.data(), link_table.size());

  // fill in the std::function objects with the guid of the matching interface.
  for (auto n : link_table) {
    auto it = guid_index.find(n.guid);

    // unsupported, there is a problem. The function will not be set, so depend
    // on catching elsewhere. Perhaps functionality can be found and some
    // interfaces and os implementations might not support some functions. the
    // code should adapt where necessary. Yet some functions are required..
    if (it == guid_index.end())
      continue;

    // set the std::function
    it->second(*this, n.fn_ptr);
  }
}

/**
 * @internal
 * @fn terminate
 * @brief closes the loaded library, the symbols will no longer be valid.
 */
void uxdevice::client_interface_t::terminate(void) { fn_library_close(); }
}
