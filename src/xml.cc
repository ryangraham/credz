#include "xml.h"

#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include "doctest/doctest.h"
#include "pugixml.hpp"

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
  pugi::xml_document doc;
  pugi::xml_parse_result result = doc.load_string(assertion.c_str());
  if (!result) {
    std::cerr << "XML parser failed on SAML assertion" << std::endl;
    std::cerr << assertion << std::endl;
    exit(1);
  }

  for (pugi::xml_node attribute : doc.child("saml2p:Response")
                                      .child("saml2:Assertion")
                                      .child("saml2:AttributeStatement")
                                      .children("saml2:Attribute")) {
    std::string name = attribute.attribute("Name").value();
    if (name == ROLE_ATTRIBUTE_NAME) {
      std::string value = attribute.child_value("saml2:AttributeValue");
      std::vector<std::string> arns = split(value);
      role role{arns[1], arns[0]};
      output.push_back(role);
    }
  }

  return output;
}

}  // namespace xml

TEST_CASE("split string on comma") {
  std::string input = "role_arn,principal_arn";

  auto results = xml::split(input);
  CHECK(results[0] == "role_arn");
  CHECK(results[1] == "principal_arn");
}

TEST_CASE("get_roles") {
  std::string xml =
      R"(<?xml version="1.0" encoding="UTF-8"?>)"
      R"(<saml2p:Response>)"
      R"(<saml2:Assertion>)"
      R"(<saml2:AttributeStatement xmlns:saml2="urn:oasis:names:tc:SAML:2.0:assertion">)"
      R"(<saml2:Attribute Name="https://aws.amazon.com/SAML/Attributes/Role" NameFormat="urn:oasis:names:tc:SAML:2.0:attrname-format:uri">)"
      R"(<saml2:AttributeValue xmlns:xs="http://www.w3.org/2001/XMLSchema" xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" xsi:type="xs:string">arn:aws:iam::123456789123:saml-provider/okta,arn:aws:iam::123456789123:role/engineers</saml2:AttributeValue>)"
      R"(</saml2:Attribute>)"
      R"(<saml2:Attribute Name="https://aws.amazon.com/SAML/Attributes/RoleSessionName" NameFormat="urn:oasis:names:tc:SAML:2.0:attrname-format:basic">)"
      R"(<saml2:AttributeValue xmlns:xs="http://www.w3.org/2001/XMLSchema" xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" xsi:type="xs:string">ryang@scooterz</saml2:AttributeValue>)"
      R"(</saml2:Attribute>)"
      R"(<saml2:Attribute Name="https://aws.amazon.com/SAML/Attributes/SessionDuration" NameFormat="urn:oasis:names:tc:SAML:2.0:attrname-format:basic">)"
      R"(<saml2:AttributeValue xmlns:xs="http://www.w3.org/2001/XMLSchema" xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" xsi:type="xs:string">28800</saml2:AttributeValue>)"
      R"(</saml2:Attribute>)"
      R"(</saml2:AttributeStatement>)"
      R"(</saml2:Assertion>)"
      R"(</saml2p:Response>)";

  auto roles = xml::get_roles(xml);
  CHECK(roles[0].principal_arn ==
        "arn:aws:iam::123456789123:saml-provider/okta");
  CHECK(roles[0].role_arn == "arn:aws:iam::123456789123:role/engineers");
}
