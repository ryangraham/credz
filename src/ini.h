#pragma once
#include <cfg/cfg.h>

#include <string>

#include "aws.h"

namespace ini {

void put_profile(cfg::ctree root, const aws::profile &profile);

cfg::ctree load_file();

void write_config(const std::string &org, const std::string &username,
                  std::string config_file);

}  // namespace ini
