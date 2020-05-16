#include "aws.h"

#include <string>

namespace aws {

profile assume_role_with_saml(const std::string &assertion,
                              const std::string &principal_arn,
                              const std::string &role_arn) {
  std::string aws_access_key_id;
  std::string aws_secret_access_key;
  std::string aws_session_token;

  Aws::SDKOptions options;
  Aws::InitAPI(options);
  {
    Aws::STS::Model::AssumeRoleWithSAMLRequest request;
    request.SetPrincipalArn(principal_arn.c_str());
    request.SetRoleArn(role_arn.c_str());
    request.SetSAMLAssertion(assertion.c_str());

    Aws::STS::STSClient sts_client;
    auto result = sts_client.AssumeRoleWithSAML(request);
    if (!result.IsSuccess())
      throw(std::runtime_error(result.GetError().GetMessage().c_str()));

    auto credentials = result.GetResult().GetCredentials();
    aws_access_key_id = credentials.GetAccessKeyId().c_str();          // NOLINT
    aws_secret_access_key = credentials.GetSecretAccessKey().c_str();  // NOLINT
    aws_session_token = credentials.GetSessionToken().c_str();         // NOLINT
  }
  Aws::ShutdownAPI(options);

  profile profile;
  profile.aws_access_key_id = aws_access_key_id;
  profile.aws_secret_access_key = aws_secret_access_key;
  profile.aws_session_token = aws_session_token;

  return profile;
}

}  // namespace aws