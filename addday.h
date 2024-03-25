#include <windows.h>
#include <time.h>
#include <process.h>
#include <vector>
#include <string>

SYSTEMTIME GetSystemTimeByTM(const tm* t);
void SetLocalTimeByTimeT(time_t timestamp);
tm GetLocalTimeByDay(int day, int hour, int minute, int second);
template<typename T>
void Split(const std::string& in, std::vector<T>& out, const std::string& delimiters);
void AddDay(int day, int hour, int minute, int second);
static unsigned CALLBACK Thread1(void* argv);
void PrintfTime();


template<typename T>
void Split(const std::string& in, std::vector<T>& out, const std::string& delimiters)
{
	static_assert(std::is_integral<T>::value, "Split Integral required.");
	std::string::size_type last_pos = in.find_first_not_of(delimiters, 0);
	std::string::size_type pos = in.find_first_of(delimiters, last_pos);
	while (std::string::npos != pos || std::string::npos != last_pos)
	{
		if constexpr (sizeof(T) == sizeof(int))
		{
			out.push_back(static_cast<T>(atoi(in.substr(last_pos, pos - last_pos).c_str())));
		}
		else if constexpr (sizeof(T) == sizeof(long long))
		{
			out.push_back(static_cast<T>(atoll(in.substr(last_pos, pos - last_pos).c_str())));
		}
		last_pos = in.find_first_not_of(delimiters, pos);
		pos = in.find_first_of(delimiters, last_pos);
	}
}
