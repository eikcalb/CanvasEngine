#pragma once
#define WIN32_LEAN_AND_MEAN // Required to prevent winsock/WinSock2 redifinition

#include <chrono>
#include <string>
#include <windows.h>

namespace Utils {
	// Sourced from: https://stackoverflow.com/a/217605
	static inline void ltrim(std::string& s) {
		s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](unsigned char ch) {
			return !std::isspace(ch);
			}));
	}

	// Sourced from: https://stackoverflow.com/a/217605
	static inline void rtrim(std::string& s) {
		s.erase(std::find_if(s.rbegin(), s.rend(), [](unsigned char ch) {
			return !std::isspace(ch);
			}).base(), s.end());
	}

	static void TrimString(std::string& input) {
		ltrim(input);
		rtrim(input);
	}

	static inline std::string GetErrorMessage(HRESULT hr)
	{
		LPSTR messageBuffer = nullptr;
		size_t size = FormatMessageA(
			FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
			NULL,
			hr,
			MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
			(LPSTR)&messageBuffer,
			0,
			NULL);

		std::string message(messageBuffer, size);
		LocalFree(messageBuffer);
		return message;
	}

	static inline double GetTime() {
		auto now = std::chrono::high_resolution_clock::now();
		auto duration = now.time_since_epoch();
		return std::chrono::duration_cast<std::chrono::duration<double>>(duration).count();
	}

	static inline std::vector<byte> stringToBytes(const std::string& str) {
		std::vector<byte> bytes(str.begin(), str.end());
		return bytes;
	}
};
