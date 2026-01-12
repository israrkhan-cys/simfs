#ifndef CRYPTO_H
#define CRYPTO_H

#include <string>
#include <vector>
#include <map>
using namespace std;

struct Cryptocurrency {
    string id;
    string symbol;
    string name;
    double price_usd;
    double price_change_24h;
    double market_cap;
    double volume_24h;
    time_t last_updated;
    Cryptocurrency() : price_usd(0), price_change_24h(0), market_cap(0), volume_24h(0), last_updated(0) {}
};

class CryptoAPI {
private:
    map<string, Cryptocurrency> cache;
    time_t lastFetch;
    
    static size_t WriteCallback(void* contents, size_t size, size_t nmemb, string* output);
    
public:
    CryptoAPI();
    
    // Get single cryptocurrency
    Cryptocurrency getCryptoPrice(const string& cryptoId);
    
    // Get top n 
    vector<Cryptocurrency> getTopCryptos(int limit = 10);
    
    // Convert between cryptocurrencies
    string convertCrypto(const string& from, const string& to, double amount);
    
    // Search cryptocurrencies
    vector<string> searchCrypto(const string& query);
    
    // Pretty print functions
    string getPrettyCrypto(const string& cryptoId);
    string getPrettyTop(int limit = 5);
    string getMarketOverview();
    
private:
    string makeRequest(const string& url);
    Cryptocurrency parseCryptoJSON(const string& json, const string& cryptoId);
    vector<Cryptocurrency> parseTopCryptosJSON(const string& json, int limit);
    string getTrendEmoji(double change);
    string formatCurrency(double amount);
    string formatLargeNumber(double number);
};

#endif