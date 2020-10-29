
class generator_t {
public:
  generator_t(fs_path_t &_target_export_header,
              fs_path_t &_slib_template_input);

  bool process();
  bool insert_guid_markers(std::string &target);
  bool batch_guid_gen(std::size_t amount, std::vector<std::string> &guids);
  fs_path_t target_export_header_input = {};
  fs_path_t slib_template_input = {};
  fs_path_t output_name = {};
};
