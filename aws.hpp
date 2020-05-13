
#include <string>
#include <aws/core/Aws.h>
#include <aws/sts/STSClient.h>
#include <aws/sts/model/AssumeRoleWithSAMLRequest.h>

namespace aws
{

void get_creds(const std::string &assertion, const std::string &principal_arn, const std::string &role_arn)
{
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
        std::string aws_access_key_id = credentials.GetAccessKeyId().c_str();
        std::string aws_secret_access_key = credentials.GetSecretAccessKey().c_str();
        std::string aws_session_token = credentials.GetSessionToken().c_str();

        std::cout << "aws_access_key_id: " << aws_access_key_id << std::endl;
        std::cout << "aws_secret_access_key: " << aws_secret_access_key << std::endl;
        std::cout << "aws_session_token: " << aws_session_token << std::endl;
    }
    Aws::ShutdownAPI(options);
}

} // namespace aws