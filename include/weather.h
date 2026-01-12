#ifndef WEATHER_H
#define WEATHER_H


#include <string>
#include <algorithm>  

class Weather {
private:
    std::string apiKey;
    static size_t WriteCallback(void* contents, size_t size, size_t nmemb, std::string* output);
    
public:
    Weather(const std::string& key);
    
    // Main weather function
    std::string getWeather(const std::string& city);
    
    // Helper: Convert Kelvin to readable format
    std::string formatTemperature(double kelvin);
    
    // Helper: Get weather emoji
    std::string getWeatherEmoji(const std::string& condition);
    
private:
    std::string makeRequest(const std::string& url);
    std::string parseWeatherJSON(const std::string& json,const std::string& city);
};

#endif