#pragma once

#include <string>
#include <curl/curl.h>

class SSLClient {
public:
    SSLClient(const std::string& name, const std::string& description);
    ~SSLClient();

    // Test methods
    bool testHTTPS(const std::string& url, bool verifyPeer = true, bool verifyHost = true);
    bool testMTLS(const std::string& url, const std::string& clientCert = "", const std::string& clientKey = "");
    
    // Configuration methods
    void setCACert(const std::string& caCertPath);
    void setClientCert(const std::string& certPath, const std::string& keyPath);
    void setInsecure(bool insecure);

    // Utility methods
    void printTestHeader();
    void printResult(bool success, const std::string& expectedResult, const std::string& actualResult = "");

private:
    std::string name_;
    std::string description_;
    std::string caCertPath_;
    std::string clientCertPath_;
    std::string clientKeyPath_;
    bool insecure_;
    
    CURL* curl_;
    
    // Callback for writing response data
    static size_t WriteCallback(void* contents, size_t size, size_t nmemb, std::string* userp);
    
    // Helper methods
    void setupCommonOptions();
    void setupSSLOptions();
    std::string getCurrentTime();
};
