#pragma once
#include <boost/property_tree/ptree.hpp>
#include <string>

#include "aws.h"
#include "path.h"

namespace ini {

bool profile_exists(const boost::property_tree::ptree &root,
                    const std::string &profile_name);

aws::profile get_profile(const boost::property_tree::ptree &root,
                         const std::string &profile_name);

void put_profile(boost::property_tree::ptree root, const aws::profile &profile);

boost::property_tree::ptree load_file();

}  // namespace ini
