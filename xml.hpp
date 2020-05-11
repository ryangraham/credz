#include <iostream>
#include <string>
#include <vector>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>

#define ROLE_ATTRIBUTE_NAME "https://aws.amazon.com/SAML/Attributes/Role"

std::vector<std::string> get_roles(const std::string &assertion)
{
    std::vector<std::string> output;
    boost::property_tree::ptree tree;
    std::istringstream input(assertion);

    read_xml(input, tree);
    for (const auto &attribute : tree.get_child("saml2p:Response.saml2:Assertion.saml2:AttributeStatement"))
    {
        std::string name = attribute.second.get<std::string>("<xmlattr>.Name", "");
        if (name == ROLE_ATTRIBUTE_NAME)
        {
            std::string role = attribute.second.get_child("saml2:AttributeValue").get_value<std::string>();
            output.push_back(role);
        }
    }

    return output;
}
