#include "xml.h"

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include "doctest/doctest.h"

#define ROLE_ATTRIBUTE_NAME "https://aws.amazon.com/SAML/Attributes/Role"

namespace xml {

std::vector<std::string> split(const std::string &role) {
  std::vector<std::string> results;
  std::stringstream ss(role);

  while (ss.good()) {
    std::string substr;
    std::getline(ss, substr, ',');
    results.push_back(substr);
  }

  return results;
}

std::vector<role> get_roles(const std::string &assertion) {
  std::vector<role> output;
  boost::property_tree::ptree tree;
  std::istringstream input(assertion);

  read_xml(input, tree);
  for (const auto &attribute : tree.get_child(
           "saml2p:Response.saml2:Assertion.saml2:AttributeStatement")) {
    std::string name = attribute.second.get<std::string>("<xmlattr>.Name", "");
    if (name == ROLE_ATTRIBUTE_NAME) {
      std::string value = attribute.second.get_child("saml2:AttributeValue")
                              .get_value<std::string>();
      std::vector<std::string> arns = split(value);
      role role{arns[1], arns[0]};
      output.push_back(role);
    }
  }

  return output;
}

}  // namespace xml

TEST_CASE("split arns on comma") {
  std::string input = "role_arn,principal_arn";

  auto results = xml::split(input);
  CHECK(results[0] == "role_arn");
  CHECK(results[1] == "principal_arn");
}
