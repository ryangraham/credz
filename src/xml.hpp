#include <iostream>
#include <string>
#include <tuple>
#include <vector>
#include <boost/algorithm/string.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>

#define ROLE_ATTRIBUTE_NAME "https://aws.amazon.com/SAML/Attributes/Role"

struct assumable_role
{
    std::string role_arn;
    std::string principal_arn;
};

std::vector<std::string> split(const std::string &role)
{
    std::vector<std::string> results;
    boost::split(results, role, [](char c) { return c == ','; });
    return results;
}

std::vector<assumable_role> get_roles(const std::string &assertion)
{
    std::vector<assumable_role> output;
    boost::property_tree::ptree tree;
    std::istringstream input(assertion);

    read_xml(input, tree);
    for (const auto &attribute : tree.get_child("saml2p:Response.saml2:Assertion.saml2:AttributeStatement"))
    {
        std::string name = attribute.second.get<std::string>("<xmlattr>.Name", "");
        if (name == ROLE_ATTRIBUTE_NAME)
        {
            std::string value = attribute.second.get_child("saml2:AttributeValue").get_value<std::string>();
            std::vector<std::string> arns = split(value);
            assumable_role role{arns[1], arns[0]};
            output.push_back(role);
        }
    }

    return output;
}
