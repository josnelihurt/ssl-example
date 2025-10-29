#include "ssl_utils.h"
#include <iostream>
#include <unistd.h>
#include <cstdlib>

int main() {
    SSLClient client("HTTPS Insecure", "HTTPS request with insecure flag - should succeed but skip certificate verification");
    
    const char* server_host = std::getenv("SERVER_HOST");
    const std::string host = server_host ? server_host : "nginx";
    const std::string url = "https://" + host + ":8443/";
    
    // Set insecure mode
    client.setInsecure(true);
    
    client.printTestHeader();
    std::cout << "URL: " << url << std::endl;
    std::cout << "CA Certificate: None" << std::endl;
    std::cout << "SSL Verification: Disabled (insecure)" << std::endl;
    
    bool success = client.testHTTPS(url, false, false);
    
    client.printResult(success, "SUCCESS: Should succeed with insecure flag", 
                      success ? "SUCCESS (expected)" : "FAIL (unexpected)");
    
    return 0;
}
