// clang-format off


The class extends the uxapi namespace.
namespace uxapi {
class %%class_name%% : public client_interface_t {
  public:
	 interface_guid_t alias = {0x22, 0x84, 0xff, 0xb0, 0xbf, 0xdd, 0x4c, 0x35, 0xae, 0x9e, 0x78, 0xef, 0xde, 0xfb, 0x04, 0x54};
/* The class name which is the library interface  creating or inheriting this one will load and connect to the shared library.*/
graphic_library_interface_t() : client_interface_t(alias) {}


    /* interface functions that are public: */
    /* sets the device location */
void device(double p0,double p1)
	 {fn_50_b0_9f_81_82_f2_4f_0e_b8_4c_4c_c3_d3_5b_bb_c3(p0,p1);}

/*  The api creates the classes, tags them, and then uses this function. */
void input_resource(client_data_interface_base_t * p0)
	 {fn_99_28_5a_90_09_34_4d_2a_9e_79_f9_a6_7e_ce_f2_2d(p0);}

void linked_mapped_objects_find_size_t(std::size_t p0)
	 {fn_80_b6_18_c7_2c_85_4a_d8_89_e9_ec_ba_ce_05_74_49(p0);}

void linked_mapped_objects_find_string(char * p0,std::size_t p1)
	 {fn_d5_c9_1b_ff_fb_d2_4e_f0_9c_e0_98_e7_7e_a6_97_ab(p0,p1);}

void save(void p0)
	 {fn_50_b0_9f_81_82_f2_4f_0e_b8_4c_4c_c3_d3_5b_bb_c3(p0);}

void restore(void p0)
	 {fn_86_6a_dd_ed_e0_0f_46_69_a9_8c_d4_75_a8_6b_f2_10(p0);}

void push(content_type_t & p0)
	 {fn_7f_4f_83_65_cc_37_43_a6_a4_03_03_58_4d_4e_7d_b5(p0);}

void pop(void p0)
	 {fn_50_95_9b_45_45_11_49_1e_be_d6_7d_1a_22_65_05_e9(p0);}

void scale(double p0,double p1)
	 {fn_e6_7e_63_48_72_45_4d_5d_8b_64_aa_81_ec_0c_56_bb(p0,p1);}

void transform(matrix_t & p0)
	 {fn_22_84_ff_b0_bf_dd_4c_35_ae_9e_78_ef_de_fb_04_54(p0);}

void matrix(matrix_t & p0)
	 {fn_f6_72_cb_66_16_fd_47_1f_b6_01_10_66_ca_d5_c0_35(p0);}

void identity(void p0)
	 {fn_30_aa_97_af_b4_60_44_52_94_9c_a2_02_b5_02_e3_48(p0);}

void translate(double p0,double p1)
	 {fn_a3_e4_23_87_91_af_44_2c_9d_66_ff_37_13_7b_29_83(p0,p1);}

void rotate(double p0)
	 {fn_ad_bb_20_40_e6_0d_4b_18_94_ac_cd_70_0d_51_ab_3d(p0);}

void device(double p0,double p1)
	 {fn_6b_a2_6f_9e_cd_eb_43_0b_92_77_d5_b6_7e_fb_58_d1(p0,p1);}

void device_distance(double p0,double p1)
	 {fn_58_d4_f2_9c_72_25_4f_a1_82_77_f2_b0_53_c8_87_e3(p0,p1);}

void device_offset(double p0,double p1)
	 {fn_7e_29_1c_b2_54_27_41_0f_bc_31_13_d5_a4_b6_3f_9b(p0,p1);}

void device_scale(double p0,double p1)
	 {fn_4a_fe_4d_4a_78_9e_49_33_b6_7b_82_f6_fb_44_32_bd(p0,p1);}

void user(double p0,double p1)
	 {fn_7b_ce_07_e9_d1_7d_41_14_b1_7c_9b_f4_88_82_21_07(p0,p1);}

void user_distance(double p0,double p1)
	 {fn_0b_80_b4_fc_09_82_4c_82_be_98_7f_3d_48_57_0e_9e(p0,p1);}

void notify_complete(void p0)
	 {fn_22_01_b8_be_12_26_4c_da_be_1a_dd_a8_35_59_c2_5e(p0);}


private:
	/* void device(double p0,double p1) */
std::function<void (double,double)> fn_50_b0_9f_81_82_f2_4f_0e_b8_4c_4c_c3_d3_5b_bb_c3;

/* void input_resource(client_data_interface_base_t * p0) */
std::function<void (client_data_interface_base_t *)> fn_99_28_5a_90_09_34_4d_2a_9e_79_f9_a6_7e_ce_f2_2d;

/* void linked_mapped_objects_find_size_t(std::size_t p0) */
std::function<void (std::size_t)> fn_80_b6_18_c7_2c_85_4a_d8_89_e9_ec_ba_ce_05_74_49;

/* void linked_mapped_objects_find_string(char * p0,std::size_t p1) */
std::function<void (char *,std::size_t)> fn_d5_c9_1b_ff_fb_d2_4e_f0_9c_e0_98_e7_7e_a6_97_ab;

/* void save(void p0) */
std::function<void (void)> fn_50_b0_9f_81_82_f2_4f_0e_b8_4c_4c_c3_d3_5b_bb_c3;

/* void restore(void p0) */
std::function<void (void)> fn_86_6a_dd_ed_e0_0f_46_69_a9_8c_d4_75_a8_6b_f2_10;

/* void push(content_type_t & p0) */
std::function<void (content_type_t &)> fn_7f_4f_83_65_cc_37_43_a6_a4_03_03_58_4d_4e_7d_b5;

/* void pop(void p0) */
std::function<void (void)> fn_50_95_9b_45_45_11_49_1e_be_d6_7d_1a_22_65_05_e9;

/* void scale(double p0,double p1) */
std::function<void (double,double)> fn_e6_7e_63_48_72_45_4d_5d_8b_64_aa_81_ec_0c_56_bb;

/* void transform(matrix_t & p0) */
std::function<void (matrix_t &)> fn_22_84_ff_b0_bf_dd_4c_35_ae_9e_78_ef_de_fb_04_54;

/* void matrix(matrix_t & p0) */
std::function<void (matrix_t &)> fn_f6_72_cb_66_16_fd_47_1f_b6_01_10_66_ca_d5_c0_35;

/* void identity(void p0) */
std::function<void (void)> fn_30_aa_97_af_b4_60_44_52_94_9c_a2_02_b5_02_e3_48;

/* void translate(double p0,double p1) */
std::function<void (double,double)> fn_a3_e4_23_87_91_af_44_2c_9d_66_ff_37_13_7b_29_83;

/* void rotate(double p0) */
std::function<void (double)> fn_ad_bb_20_40_e6_0d_4b_18_94_ac_cd_70_0d_51_ab_3d;

/* void device(double p0,double p1) */
std::function<void (double,double)> fn_6b_a2_6f_9e_cd_eb_43_0b_92_77_d5_b6_7e_fb_58_d1;

/* void device_distance(double p0,double p1) */
std::function<void (double,double)> fn_58_d4_f2_9c_72_25_4f_a1_82_77_f2_b0_53_c8_87_e3;

/* void device_offset(double p0,double p1) */
std::function<void (double,double)> fn_7e_29_1c_b2_54_27_41_0f_bc_31_13_d5_a4_b6_3f_9b;

/* void device_scale(double p0,double p1) */
std::function<void (double,double)> fn_4a_fe_4d_4a_78_9e_49_33_b6_7b_82_f6_fb_44_32_bd;

/* void user(double p0,double p1) */
std::function<void (double,double)> fn_7b_ce_07_e9_d1_7d_41_14_b1_7c_9b_f4_88_82_21_07;

/* void user_distance(double p0,double p1) */
std::function<void (double,double)> fn_0b_80_b4_fc_09_82_4c_82_be_98_7f_3d_48_57_0e_9e;

/* void notify_complete(void p0) */
std::function<void (void)> fn_22_01_b8_be_12_26_4c_da_be_1a_dd_a8_35_59_c2_5e;


  };
} //  uxapi
