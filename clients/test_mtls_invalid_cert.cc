#include "ssl_utils.h"
#include <iostream>
#include <unistd.h>
#include <cstdlib>

int main() {
    SSLClient client("mTLS Invalid Client Cert", "mTLS request with invalid client certificate - should fail with certificate verification error");
    
    const char* server_host = std::getenv("SERVER_HOST");
    const std::string host = server_host ? server_host : "nginx";
    const std::string url = "https://" + host + ":8444/";
    const std::string caCert = "/certs/root-ca.crt";
    const std::string clientCert = "/certs/client-invalid.crt";
    const std::string clientKey = "/certs/client-invalid.key";
    
    // Set CA certificate and invalid client certificate
    client.setCACert(caCert);
    client.setClientCert(clientCert, clientKey);
    
    client.printTestHeader();
    std::cout << "URL: " << url << std::endl;
    std::cout << "CA Certificate: " << caCert << std::endl;
    std::cout << "Client Certificate: " << clientCert << " (invalid - self-signed)" << std::endl;
    std::cout << "Client Key: " << clientKey << std::endl;
    std::cout << "SSL Verification: Enabled" << std::endl;
    
    bool success = client.testMTLS(url, clientCert, clientKey);
    
    client.printResult(success, "FAIL: Server should reject invalid client certificate", 
                      success ? "SUCCESS (unexpected)" : "FAIL (expected)");
    
    return 0;
}
