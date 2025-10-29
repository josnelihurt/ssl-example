#include "ssl_utils.h"
#include <iostream>
#include <unistd.h>
#include <cstdlib>

int main() {
    SSLClient client("mTLS Valid Client Cert", "mTLS request with valid client certificate - should succeed with mutual authentication");
    
    const char* server_host = std::getenv("SERVER_HOST");
    const std::string host = server_host ? server_host : "nginx";
    const std::string url = "https://" + host + ":8444/";
    const std::string caCert = "/certs/root-ca.crt";
    const std::string clientCert = "/certs/client-valid.crt";
    const std::string clientKey = "/certs/client-valid.key";
    
    // Set CA certificate and valid client certificate
    client.setCACert(caCert);
    client.setClientCert(clientCert, clientKey);
    
    client.printTestHeader();
    std::cout << "URL: " << url << std::endl;
    std::cout << "CA Certificate: " << caCert << std::endl;
    std::cout << "Client Certificate: " << clientCert << " (valid - signed by Root CA)" << std::endl;
    std::cout << "Client Key: " << clientKey << std::endl;
    std::cout << "SSL Verification: Enabled" << std::endl;
    
    bool success = client.testMTLS(url, clientCert, clientKey);
    
    client.printResult(success, "SUCCESS: Should succeed with valid client certificate", 
                      success ? "SUCCESS (expected)" : "FAIL (unexpected)");
    
    return 0;
}
