#include "ssl_utils.h"
#include <iostream>
#include <unistd.h>
#include <cstdlib>

int main() {
    SSLClient client("mTLS No Client Cert", "mTLS request without client certificate - should fail with SSL handshake error");
    
    const char* server_host = std::getenv("SERVER_HOST");
    const std::string host = server_host ? server_host : "nginx";
    const std::string url = "https://" + host + ":8444/";
    const std::string caCert = "/certs/root-ca.crt";
    
    // Set CA certificate but no client certificate
    client.setCACert(caCert);
    
    client.printTestHeader();
    std::cout << "URL: " << url << std::endl;
    std::cout << "CA Certificate: " << caCert << std::endl;
    std::cout << "Client Certificate: None" << std::endl;
    std::cout << "SSL Verification: Enabled" << std::endl;
    
    bool success = client.testMTLS(url, "", "");
    
    client.printResult(success, "FAIL: Server should reject connection without client certificate", 
                      success ? "SUCCESS (unexpected)" : "FAIL (expected)");
    
    return 0;
}
