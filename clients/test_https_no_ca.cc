#include "ssl_utils.h"
#include <iostream>
#include <unistd.h>
#include <cstdlib>

int main() {
    SSLClient client("HTTPS No CA", "HTTPS request without CA certificate - should fail with certificate verification error");
    
    const char* server_host = std::getenv("SERVER_HOST");
    const std::string host = server_host ? server_host : "nginx";
    const std::string url = "https://" + host + ":8443/";
    
    client.printTestHeader();
    std::cout << "URL: " << url << std::endl;
    std::cout << "CA Certificate: None" << std::endl;
    std::cout << "SSL Verification: Enabled" << std::endl;
    
    bool success = client.testHTTPS(url, true, true);
    
    client.printResult(success, "FAIL: Certificate verification should fail", 
                      success ? "SUCCESS (unexpected)" : "FAIL (expected)");
    
    return 0;
}
