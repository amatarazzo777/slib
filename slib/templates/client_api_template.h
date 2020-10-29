%%license_h_input%%
#pragma once
%%enclosing_namespace_documentation%% 
namespace %%enclosing_namespace%% {

	
#pragma once

%%include "client_interface_base.h"%%


class %%class_name%% : public client_interface_t {
public: 
	%%class_interface%%

    /* interface functions that are public: */
    %%public_member_functions_class_api%% 

private: 
	%%private_guid_function_ties%%
};
} //  %%enclosing_namespace%%
