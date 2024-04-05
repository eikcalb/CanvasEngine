#pragma once
#include <string>

namespace Utils {
    // Sourced from: https://stackoverflow.com/a/217605
    static inline void ltrim(std::string & s) {
            s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](unsigned char ch) {
                return !std::isspace(ch);
                }));
        }

    // Sourced from: https://stackoverflow.com/a/217605
        static inline void rtrim(std::string & s) {
            s.erase(std::find_if(s.rbegin(), s.rend(), [](unsigned char ch) {
                return !std::isspace(ch);
                }).base(), s.end());
        }

	static void TrimString(std::string &input) {
        ltrim(input);
        rtrim(input);
	}

};
