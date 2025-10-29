#include "ssl_utils.h"
#include <iostream>
#include <unistd.h>
#include <cstdlib>

int main() {
    SSLClient client("HTTPS With CA", "HTTPS request with Root CA certificate - should succeed with proper certificate validation");
    
    const char* server_host = std::getenv("SERVER_HOST");
    const std::string host = server_host ? server_host : "nginx";
    const std::string url = "https://" + host + ":8443/";
    const std::string caCert = "/certs/root-ca.crt";
    
    // Set CA certificate
    client.setCACert(caCert);
    
    client.printTestHeader();
    std::cout << "URL: " << url << std::endl;
    std::cout << "CA Certificate: " << caCert << std::endl;
    std::cout << "SSL Verification: Enabled" << std::endl;
    
    bool success = client.testHTTPS(url, true, true);
    
    client.printResult(success, "SUCCESS: Should succeed with valid CA certificate", 
                      success ? "SUCCESS (expected)" : "FAIL (unexpected)");
    
    return 0;
}
