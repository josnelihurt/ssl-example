#include "ssl_utils.h"
#include <iostream>
#include <iomanip>
#include <sstream>
#include <ctime>

SSLClient::SSLClient(const std::string& name, const std::string& description)
    : name_(name), description_(description), insecure_(false), curl_(nullptr) {
    curl_global_init(CURL_GLOBAL_DEFAULT);
    curl_ = curl_easy_init();
}

SSLClient::~SSLClient() {
    if (curl_) {
        curl_easy_cleanup(curl_);
    }
    curl_global_cleanup();
}

void SSLClient::setCACert(const std::string& caCertPath) {
    caCertPath_ = caCertPath;
}

void SSLClient::setClientCert(const std::string& certPath, const std::string& keyPath) {
    clientCertPath_ = certPath;
    clientKeyPath_ = keyPath;
}

void SSLClient::setInsecure(bool insecure) {
    insecure_ = insecure;
}

void SSLClient::printTestHeader() {
    std::cout << "\n" << std::string(60, '=') << std::endl;
    std::cout << "TEST: " << name_ << std::endl;
    std::cout << "DESCRIPTION: " << description_ << std::endl;
    std::cout << "TIME: " << getCurrentTime() << std::endl;
    std::cout << std::string(60, '=') << std::endl;
}

void SSLClient::printResult(bool success, const std::string& expectedResult, const std::string& actualResult) {
    std::cout << "\nRESULT: " << (success ? "SUCCESS" : "FAILED") << std::endl;
    std::cout << "EXPECTED: " << expectedResult << std::endl;
    if (!actualResult.empty()) {
        std::cout << "ACTUAL: " << actualResult << std::endl;
    }
    std::cout << std::string(60, '-') << std::endl;
}

std::string SSLClient::getCurrentTime() {
    auto now = std::time(nullptr);
    auto tm = *std::localtime(&now);
    std::ostringstream oss;
    oss << std::put_time(&tm, "%Y-%m-%d %H:%M:%S");
    return oss.str();
}

size_t SSLClient::WriteCallback(void* contents, size_t size, size_t nmemb, std::string* userp) {
    size_t totalSize = size * nmemb;
    userp->append((char*)contents, totalSize);
    return totalSize;
}

void SSLClient::setupCommonOptions() {
    if (!curl_) return;
    
    curl_easy_setopt(curl_, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curl_, CURLOPT_TIMEOUT, 10L);
    curl_easy_setopt(curl_, CURLOPT_CONNECTTIMEOUT, 5L);
    curl_easy_setopt(curl_, CURLOPT_FOLLOWLOCATION, 1L);
    curl_easy_setopt(curl_, CURLOPT_VERBOSE, 1L);
}

void SSLClient::setupSSLOptions() {
    if (!curl_) return;
    
    // Set CA certificate if provided
    if (!caCertPath_.empty()) {
        curl_easy_setopt(curl_, CURLOPT_CAINFO, caCertPath_.c_str());
    }
    
    // Set client certificate if provided
    if (!clientCertPath_.empty()) {
        curl_easy_setopt(curl_, CURLOPT_SSLCERT, clientCertPath_.c_str());
    }
    if (!clientKeyPath_.empty()) {
        curl_easy_setopt(curl_, CURLOPT_SSLKEY, clientKeyPath_.c_str());
    }
    
    // Set SSL verification options
    if (insecure_) {
        curl_easy_setopt(curl_, CURLOPT_SSL_VERIFYPEER, 0L);
        curl_easy_setopt(curl_, CURLOPT_SSL_VERIFYHOST, 0L);
    } else {
        curl_easy_setopt(curl_, CURLOPT_SSL_VERIFYPEER, 1L);
        curl_easy_setopt(curl_, CURLOPT_SSL_VERIFYHOST, 2L);
    }
}

bool SSLClient::testHTTPS(const std::string& url, bool verifyPeer, bool verifyHost) {
    if (!curl_) {
        std::cout << "ERROR: Failed to initialize CURL" << std::endl;
        return false;
    }
    
    std::string response;
    CURLcode res;
    long httpCode = 0;
    
    // Reset curl handle
    curl_easy_reset(curl_);
    
    // Setup common options
    setupCommonOptions();
    
    // Setup SSL options
    setupSSLOptions();
    
    // Override SSL verification if specified
    if (insecure_) {
        curl_easy_setopt(curl_, CURLOPT_SSL_VERIFYPEER, 0L);
        curl_easy_setopt(curl_, CURLOPT_SSL_VERIFYHOST, 0L);
    } else {
        curl_easy_setopt(curl_, CURLOPT_SSL_VERIFYPEER, verifyPeer ? 1L : 0L);
        curl_easy_setopt(curl_, CURLOPT_SSL_VERIFYHOST, verifyHost ? 2L : 0L);
    }
    
    // Set URL and response buffer
    curl_easy_setopt(curl_, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl_, CURLOPT_WRITEDATA, &response);
    
    // Perform the request
    res = curl_easy_perform(curl_);
    
    if (res == CURLE_OK) {
        // Get HTTP response code
        curl_easy_getinfo(curl_, CURLINFO_RESPONSE_CODE, &httpCode);
        std::cout << "HTTP Response: " << response << std::endl;
        std::cout << "HTTP Status Code: " << httpCode << std::endl;
        
        // For HTTPS tests, success means HTTP 200 OK
        return (httpCode == 200);
    } else {
        std::cout << "CURL Error: " << curl_easy_strerror(res) << std::endl;
        return false;
    }
}

bool SSLClient::testMTLS(const std::string& url, const std::string& clientCert, const std::string& clientKey) {
    if (!curl_) {
        std::cout << "ERROR: Failed to initialize CURL" << std::endl;
        return false;
    }
    
    std::string response;
    CURLcode res;
    long httpCode = 0;
    
    // Reset curl handle
    curl_easy_reset(curl_);
    
    // Setup common options
    setupCommonOptions();
    
    // Setup SSL options for mTLS
    setupSSLOptions();
    
    // Set client certificate if provided
    if (!clientCert.empty()) {
        curl_easy_setopt(curl_, CURLOPT_SSLCERT, clientCert.c_str());
    }
    if (!clientKey.empty()) {
        curl_easy_setopt(curl_, CURLOPT_SSLKEY, clientKey.c_str());
    }
    
    // Set URL and response buffer
    curl_easy_setopt(curl_, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl_, CURLOPT_WRITEDATA, &response);
    
    // Perform the request
    res = curl_easy_perform(curl_);
    
    if (res == CURLE_OK) {
        // Get HTTP response code
        curl_easy_getinfo(curl_, CURLINFO_RESPONSE_CODE, &httpCode);
        std::cout << "HTTP Response: " << response << std::endl;
        std::cout << "HTTP Status Code: " << httpCode << std::endl;
        
        // For mTLS tests, success means HTTP 200 OK
        return (httpCode == 200);
    } else {
        std::cout << "CURL Error: " << curl_easy_strerror(res) << std::endl;
        return false;
    }
}
