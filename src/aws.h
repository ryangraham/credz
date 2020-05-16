
#include <aws/core/Aws.h>
#include <aws/sts/STSClient.h>
#include <aws/sts/model/AssumeRoleWithSAMLRequest.h>

#pragma once

#include <string>

namespace aws {
struct profile {
  std::string name;
  std::string aws_access_key_id;
  std::string aws_secret_access_key;
  std::string aws_session_token;
};

profile assume_role_with_saml(const std::string &assertion,
                              const std::string &principal_arn,
                              const std::string &role_arn);

}  // namespace aws
