#include "../include/weather.h"
#include <curl/curl.h>
#include <sstream>
#include <iomanip>
#include <iostream>
#include <cstring>    
#include <ctime>      

using namespace std;

// Constructor -taking API key from main 
Weather::Weather(const string& key) : apiKey(key) {
    curl_global_init(CURL_GLOBAL_DEFAULT);
}

// Callback function for CURL
size_t Weather::WriteCallback(void* contents, size_t size, size_t nmemb, string* output) {
    size_t totalSize = size * nmemb;
    output->append((char*)contents, totalSize);
    return totalSize;
}

//Fxn for making an HTTP request
string Weather::makeRequest(const string& url) {
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
    
    long http_code = 0;
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);
    
    curl_easy_cleanup(curl);
    
    if (http_code != 200) {
        return "Error: API returned HTTP " + to_string(http_code);
    }
    
    return response;
}

// Parse JSON response with city name
string Weather::parseWeatherJSON(const string& json, const string& city) {
    // Check for "city not found" error
    if (json.find("\"cod\":\"404\"") != string::npos || 
        json.find("\"message\":\"city not found\"") != string::npos) {
        return "❌ Error: City '" + city + "' not found. Please check spelling.";
    }
    
    string description = "Unknown";
    double temperature = 0;
    int humidity = 0;
    double windSpeed = 0;
    double feelsLike = 0;
    time_t timestamp = time(nullptr);
    
    // Extract description
    size_t descPos = json.find("\"description\":\"");
    if (descPos != string::npos) {
        size_t start = descPos + 15;
        size_t end = json.find("\"", start);
        description = json.substr(start, end - start);
    }
    
    // Extract temperature
    size_t tempPos = json.find("\"temp\":");
    if (tempPos != string::npos) {
        size_t start = tempPos + 7;
        size_t end = json.find(",", start);
        try {
            temperature = stod(json.substr(start, end - start));
        } catch (...) {
            temperature = 293.15; // 20°C default
        }
    }
    
    // Extract humidity
    size_t humidPos = json.find("\"humidity\":");
    if (humidPos != string::npos) {
        size_t start = humidPos + 11;
        size_t end = json.find(",", start);
        try {
            humidity = stoi(json.substr(start, end - start));
        } catch (...) {
            humidity = 50;
        }
    }
    
    // Extract wind speed
    size_t windPos = json.find("\"speed\":");
    if (windPos != string::npos) {
        size_t start = windPos + 8;
        size_t end = json.find(",", start);
        try {
            windSpeed = stod(json.substr(start, end - start));
        } catch (...) {
            windSpeed = 0;
        }
    }
    
    // Extract feels_like temperature
    size_t feelsPos = json.find("\"feels_like\":");
    if (feelsPos != string::npos) {
        size_t start = feelsPos + 13;
        size_t end = json.find(",", start);
        try {
            feelsLike = stod(json.substr(start, end - start));
        } catch (...) {
            feelsLike = temperature;
        }
    }
    
    // Extract timestamp
    size_t timePos = json.find("\"dt\":");
    if (timePos != string::npos) {
        size_t start = timePos + 5;
        size_t end = json.find(",", start);
        try {
            timestamp = stol(json.substr(start, end - start));
        } catch (...) {
            // Keep current time
        }
    }
    
    // Convert timestamp to readable time
    char timeStr[100];
    strftime(timeStr, sizeof(timeStr), "%H:%M", localtime(&timestamp));
    
    // Format output
    stringstream ss;
    ss << "🌤️  Weather in " << city << ":\n";
    ss << "────────────────────────\n";
    ss << getWeatherEmoji(description) << " " << description << "\n";
    ss << "🌡️  Temperature: " << formatTemperature(temperature) << "\n";
    ss << "🌡️  Feels like: " << formatTemperature(feelsLike) << "\n";
    ss << "💧 Humidity: " << humidity << "%\n";
    ss << "💨 Wind: " << fixed << setprecision(1) << windSpeed << " m/s\n";
    ss << "🕐 Updated: " << timeStr << " local time\n";
    ss << "────────────────────────";
    
    return ss.str();
}

// Main public function
string Weather::getWeather(const string& city) {
    if (apiKey.empty()) {
        return "❌ Error: API key not set. Please set your OpenWeatherMap API key.";
    }
    
    string url = "https://api.openweathermap.org/data/2.5/weather?q=" 
                      + city + "&appid=" + apiKey;
    
    string response = makeRequest(url);
    
    // Check if it's an error message
    if (response.find("Error:") == 0) {
        return response;
    }
    
    return parseWeatherJSON(response, city);
}

// Helper: Format temperature
string Weather::formatTemperature(double kelvin) {
    double celsius = kelvin - 273.15;
    double fahrenheit = celsius * 9/5 + 32;
    
    stringstream ss;
    ss << fixed << setprecision(1);
    ss << celsius << "°C (" << fahrenheit << "°F)";
    return ss.str();
}

// Helper: Get emoji
string Weather::getWeatherEmoji(const string& condition) {
    string lower = condition;
    // Convert to lowercase
    for (char& c : lower) {
        c = tolower(c);
    }
    
    if (lower.find("clear") != string::npos) return "☀️";
    if (lower.find("cloud") != string::npos) return "☁️";
    if (lower.find("rain") != string::npos) return "🌧️";
    if (lower.find("thunder") != string::npos) return "⛈️";
    if (lower.find("snow") != string::npos) return "❄️";
    if (lower.find("mist") != string::npos || lower.find("fog") != string::npos) return "🌫️";
    if (lower.find("wind") != string::npos) return "💨";
    
    return "🌈";
}