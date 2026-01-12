#include "../include/crypto.h"
#include <curl/curl.h>
#include <sstream>
#include <iomanip>
#include <iostream>
#include <ctime>
#include <cmath>
#include <algorithm>
using namespace std;

// Constructor
CryptoAPI::CryptoAPI() : lastFetch(0) {
    curl_global_init(CURL_GLOBAL_DEFAULT);
}

// Callback function for CURL
size_t CryptoAPI::WriteCallback(void* contents, size_t size, size_t nmemb, string* output) {
    size_t totalSize = size * nmemb;
    output->append((char*)contents, totalSize);
    return totalSize;
}

// Make HTTP request
string CryptoAPI::makeRequest(const string& url) {
    CURL* curl = curl_easy_init();
    if (!curl) return "Error: Failed to initialize CURL";
    
    string response;
    
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
    curl_easy_setopt(curl, CURLOPT_USERAGENT, "Linux-Simulator/1.0");
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 10L);
    
    CURLcode res = curl_easy_perform(curl);
    
    if (res != CURLE_OK) {
        curl_easy_cleanup(curl);
        return "Error: " + string(curl_easy_strerror(res));
    }
    
    curl_easy_cleanup(curl);
    return response;
}

// Parse single cryptocurrency
Cryptocurrency CryptoAPI::parseCryptoJSON(const string& json, const string& cryptoId) {
    Cryptocurrency crypto;
    crypto.id = cryptoId;
    
    //get price of coin 
    size_t pricePos = json.find("\"usd\":");
    if (pricePos != string::npos) {
        size_t start = pricePos + 6;
        size_t end = json.find(",", start);
        try {
            crypto.price_usd = stod(json.substr(start, end - start));
        } catch (...) {
            crypto.price_usd = 0;
        }
    }
    
    //24h change
    size_t changePos = json.find("\"usd_24h_change\":");
    if (changePos != string::npos) {
        size_t start = changePos + 17;
        size_t end = json.find(",", start);
        try {
            crypto.price_change_24h = stod(json.substr(start, end - start));
        } catch (...) {
            crypto.price_change_24h = 0;
        }
    }
    
    // Extract market cap (if available)
    size_t marketCapPos = json.find("\"usd_market_cap\":");
    if (marketCapPos != string::npos) {
        size_t start = marketCapPos + 17;
        size_t end = json.find(",", start);
        try {
            crypto.market_cap = stod(json.substr(start, end - start));
        } catch (...) {
            crypto.market_cap = 0;
        }
    }
    
    // Extract volume (if available)
    size_t volumePos = json.find("\"usd_24h_vol\":");
    if (volumePos != string::npos) {
        size_t start = volumePos + 14;
        size_t end = json.find(",", start);
        try {
            crypto.volume_24h = stod(json.substr(start, end - start));
        } catch (...) {
            crypto.volume_24h = 0;
        }
    }
    
    // Extract timestamp
    size_t timePos = json.find("\"last_updated_at\":");
    if (timePos != string::npos) {
        size_t start = timePos + 18;
        size_t end = json.find("}", start);
        try {
            crypto.last_updated = stol(json.substr(start, end - start));
        } catch (...) {
            crypto.last_updated = time(nullptr);
        }
    }
    
    // Set symbol and name based on common IDs
    if (cryptoId == "bitcoin") {
        crypto.symbol = "BTC";
        crypto.name = "Bitcoin";
    } else if (cryptoId == "ethereum") {
        crypto.symbol = "ETH";
        crypto.name = "Ethereum";
    } else if (cryptoId == "cardano") {
        crypto.symbol = "ADA";
        crypto.name = "Cardano";
    } else if (cryptoId == "solana") {
        crypto.symbol = "SOL";
        crypto.name = "Solana";
    } else if (cryptoId == "dogecoin") {
        crypto.symbol = "DOGE";
        crypto.name = "Dogecoin";
    } else {
        // Default - use first 3 chars of ID as symbol
        crypto.symbol = cryptoId.substr(0, 3);
        transform(crypto.symbol.begin(), crypto.symbol.end(), crypto.symbol.begin(), ::toupper);
        crypto.name = cryptoId;
        // Capitalize first letter
        if (!crypto.name.empty()) {
            crypto.name[0] = toupper(crypto.name[0]);
        }
    }
    
    return crypto;
}

// Get single cryptocurrency price
Cryptocurrency CryptoAPI::getCryptoPrice(const string& cryptoId) {
    // Check cache (2 minute cache for crypto)
    auto now = time(nullptr);
    auto it = cache.find(cryptoId);
    if (it != cache.end() && (now - it->second.last_updated < 120)) {
        return it->second;
    }
    
    string url = "https://api.coingecko.com/api/v3/simple/price?ids=" 
                      + cryptoId + "&vs_currencies=usd&include_24hr_change=true&include_market_cap=true&include_24hr_vol=true&include_last_updated_at=true";
    
    string response = makeRequest(url);
    
    if (response.find("Error:") == 0 || response.empty()) {
        // Return empty crypto on error
        return Cryptocurrency();
    }
    
    Cryptocurrency crypto = parseCryptoJSON(response, cryptoId);
    cache[cryptoId] = crypto;
    
    return crypto;
}

// Get top cryptocurrencies
vector<Cryptocurrency> CryptoAPI::getTopCryptos(int limit) {
    string url = "https://api.coingecko.com/api/v3/coins/markets?vs_currency=usd&order=market_cap_desc&per_page=" 
                      + to_string(limit) + "&page=1&sparkline=false";
    
    string response = makeRequest(url);
    return parseTopCryptosJSON(response, limit);
}

// Parse top cryptocurrencies
vector<Cryptocurrency> CryptoAPI::parseTopCryptosJSON(const string& json, int limit) {
    vector<Cryptocurrency> cryptos;
    
    // Simple parsing for array format
    size_t pos = 0;
    int count = 0;
    
    while (count < limit && pos != string::npos) {
        Cryptocurrency crypto;
        
        // Find id
        size_t idPos = json.find("\"id\":\"", pos);
        if (idPos == string::npos) break;
        
        size_t idStart = idPos + 6;
        size_t idEnd = json.find("\"", idStart);
        crypto.id = json.substr(idStart, idEnd - idStart);
        
        // Find symbol
        size_t symbolPos = json.find("\"symbol\":\"", idEnd);
        if (symbolPos != string::npos) {
            size_t symbolStart = symbolPos + 10;
            size_t symbolEnd = json.find("\"", symbolStart);
            crypto.symbol = json.substr(symbolStart, symbolEnd - symbolStart);
            transform(crypto.symbol.begin(), crypto.symbol.end(), crypto.symbol.begin(), ::toupper);
        }
        
        // Find name
        size_t namePos = json.find("\"name\":\"", symbolPos);
        if (namePos != string::npos) {
            size_t nameStart = namePos + 8;
            size_t nameEnd = json.find("\"", nameStart);
            crypto.name = json.substr(nameStart, nameEnd - nameStart);
        }
        
        // Find price
        size_t pricePos = json.find("\"current_price\":", namePos);
        if (pricePos != string::npos) {
            size_t priceStart = pricePos + 16;
            size_t priceEnd = json.find(",", priceStart);
            try {
                crypto.price_usd = stod(json.substr(priceStart, priceEnd - priceStart));
            } catch (...) {
                crypto.price_usd = 0;
            }
        }
        
        // Find 24h change
        size_t changePos = json.find("\"price_change_percentage_24h\":", pricePos);
        if (changePos != string::npos) {
            size_t changeStart = changePos + 31;
            size_t changeEnd = json.find(",", changeStart);
            try {
                crypto.price_change_24h = stod(json.substr(changeStart, changeEnd - changeStart));
            } catch (...) {
                crypto.price_change_24h = 0;
            }
        }
        
        // Find market cap
        size_t marketCapPos = json.find("\"market_cap\":", changePos);
        if (marketCapPos != string::npos) {
            size_t marketCapStart = marketCapPos + 13;
            size_t marketCapEnd = json.find(",", marketCapStart);
            try {
                crypto.market_cap = stod(json.substr(marketCapStart, marketCapEnd - marketCapStart));
            } catch (...) {
                crypto.market_cap = 0;
            }
        }
        
        cryptos.push_back(crypto);
        count++;
        pos = marketCapPos;
    }
    
    return cryptos;
}

// Convert between cryptocurrencies
string CryptoAPI::convertCrypto(const string& from, const string& to, double amount) {
    Cryptocurrency fromCrypto = getCryptoPrice(from);
    Cryptocurrency toCrypto = getCryptoPrice(to);
    
    if (fromCrypto.price_usd == 0 || toCrypto.price_usd == 0) {
        return "❌ Error: Could not fetch cryptocurrency prices";
    }
    
    double usdValue = amount * fromCrypto.price_usd;
    double converted = usdValue / toCrypto.price_usd;
    
    stringstream ss;
    ss << "💱 " << amount << " " << fromCrypto.symbol << " = " 
       << fixed << setprecision(6) << converted << " " << toCrypto.symbol << "\n";
    ss << "($" << fixed << setprecision(2) << usdValue << ")";
    
    return ss.str();
}

//print single crypto
string CryptoAPI::getPrettyCrypto(const string& cryptoId) {
    Cryptocurrency crypto = getCryptoPrice(cryptoId);
    
    if (crypto.price_usd == 0) {
        return "❌ Error: Could not fetch data for '" + cryptoId + "'\n"
               "Try common names: bitcoin, ethereum, solana, cardano, dogecoin";
    }
    
    stringstream ss;
    ss << "💰 " << crypto.name << " (" << crypto.symbol << ")\n";
    ss << "────────────────────────\n";
    ss << "💵 Price: $" << formatCurrency(crypto.price_usd) << "\n";
    ss << getTrendEmoji(crypto.price_change_24h) << " 24h Change: " 
       << fixed << setprecision(2) << crypto.price_change_24h << "%\n";
    
    if (crypto.market_cap > 0) {
        ss << "📊 Market Cap: $" << formatLargeNumber(crypto.market_cap) << "\n";
    }
    
    if (crypto.volume_24h > 0) {
        ss << "📈 24h Volume: $" << formatLargeNumber(crypto.volume_24h) << "\n";
    }
    
    // Add some fun based on price change
    if (crypto.price_change_24h > 10) ss << "🚀 To the moon!\n";
    else if (crypto.price_change_24h < -10) ss << "😭 Diamond hands!\n";
    else if (crypto.price_change_24h > 0) ss << "📈 Bullish!\n";
    else ss << "🐻 Bearish sentiment\n";
    
    ss << "────────────────────────";
    
    return ss.str();
}

//print top crypto coins 
string CryptoAPI::getPrettyTop(int limit) {
    vector<Cryptocurrency> top = getTopCryptos(limit);
    
    if (top.empty()) {
        return "❌ Error: Could not fetch cryptocurrency data";
    }
    
    stringstream ss;
    ss << "🏆 Top " << limit << " Cryptocurrencies:\n";
    ss << "────────────────────────────────────────\n";
    
    for (size_t i = 0; i < top.size(); i++) {
        const auto& crypto = top[i];
        ss << setw(2) << (i + 1) << ". " 
           << setw(15) << left << crypto.name 
           << " (" << crypto.symbol << ")\n";
        ss << "    $" << formatCurrency(crypto.price_usd) 
           << " " << getTrendEmoji(crypto.price_change_24h)
           << " " << fixed << setprecision(2) << crypto.price_change_24h << "%\n";
    }
    
    ss << "────────────────────────────────────────";
    return ss.str();
}

// Get market overview
string CryptoAPI::getMarketOverview() {
    auto top = getTopCryptos(3);
    if (top.empty()) {
        return "❌ Error: Could not fetch market data";
    }
    
    double totalChange = 0;
    for (const auto& crypto : top) {
        totalChange += crypto.price_change_24h;
    }
    double avgChange = totalChange / top.size();
    
    stringstream ss;
    ss << "📊 Crypto Market Overview\n";
    ss << "────────────────────────\n";
    ss << "📈 Market Sentiment: ";
    
    if (avgChange > 5) ss << "🚀 Very Bullish";
    else if (avgChange > 0) ss << "📈 Bullish";
    else if (avgChange > -5) ss << "📉 Bearish";
    else ss << "💀 Very Bearish";
    
    ss << "\n📊 Dominance: \n";
    
    for (size_t i = 0; i < min(top.size(), size_t(3)); i++) {
        const auto& crypto = top[i];
        ss << "  " << (i + 1) << ". " << crypto.name 
           << " (" << crypto.symbol << "): " 
           << fixed << setprecision(1) << crypto.price_change_24h << "%\n";
    }
    
    ss << "────────────────────────";
    return ss.str();
}

// Helper fxns=======


string CryptoAPI::getTrendEmoji(double change) {
    if (change > 10) return "🚀📈";
    if (change > 5) return "📈🚀";
    if (change > 0) return "📈";
    if (change < -10) return "📉🔥";
    if (change < -5) return "📉💀";
    if (change < 0) return "📉";
    return "➡️";
}

// Helper: Format currency
string CryptoAPI::formatCurrency(double amount) {
    stringstream ss;
    
    if (amount >= 1000) {
        ss << fixed << setprecision(2) << amount;
    } else if (amount >= 1) {
        ss << fixed << setprecision(4) << amount;
    } else if (amount >= 0.01) {
        ss << fixed << setprecision(6) << amount;
    } else {
        ss << scientific << setprecision(4) << amount;
    }
    
    return ss.str();
}

string CryptoAPI::formatLargeNumber(double number) {
    stringstream ss;
    
    if (number >= 1e12) {
        ss << fixed << setprecision(2) << (number / 1e12) << "T";
    } else if (number >= 1e9) {
        ss << fixed << setprecision(2) << (number / 1e9) << "B";
    } else if (number >= 1e6) {
        ss << fixed << setprecision(2) << (number / 1e6) << "M";
    } else if (number >= 1e3) {
        ss << fixed << setprecision(2) << (number / 1e3) << "K";
    } else {
        ss << fixed << setprecision(2) << number;
    }
    
    return ss.str();
}