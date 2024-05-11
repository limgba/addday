#include "addday.h"
#include <iostream>
#include <process.h>
#include <fstream>

std::unordered_map<std::string, std::string> config = ReadConfig();

SYSTEMTIME GetSystemTimeByTM(const tm* t)
{
	SYSTEMTIME st;
	if (nullptr == t)
	{
		GetLocalTime(&st);
		return st;
	}
	st.wYear = 1900 + t->tm_year;
	st.wMonth = 1 + t->tm_mon;
	st.wDayOfWeek = t->tm_wday;
	st.wDay = t->tm_mday;
	st.wHour = t->tm_hour;
	st.wMinute = t->tm_min;
	st.wSecond = t->tm_sec;
	st.wMilliseconds = 0;
	return st;
}

void SetLocalTimeByTimeT(time_t timestamp)
{
	tm t;
	localtime_s(&t, &timestamp);
	SYSTEMTIME st = ::GetSystemTimeByTM(&t);
	SetLocalTime(&st);
}

tm GetLocalTimeByDay(int day)
{
	time_t now_second;
	time(&now_second);
	tm t;
	now_second = now_second + day * 86400;
	localtime_s(&t, &now_second);
	return t;
}

tm GetLocalTimeByTime(int hour, int minute, int second)
{
	time_t now_second;
	time(&now_second);
	tm t;
	if (hour != 0 || minute != 0 || second != 0)
	{
		localtime_s(&t, &now_second);
		now_second = now_second
			+ (hour - t.tm_hour) * 3600
			+ (minute - t.tm_min) * 60
			+ (second - t.tm_sec)
			;
	}
	localtime_s(&t, &now_second);
	return t;
}

void AddDay(int day)
{
	int interval_ms = stoi(config["interval_ms"]);
	if (interval_ms > 0)
	{
		if (day > 0)
		{
			for (int i = 0; i < day; ++i)
			{
				tm t = ::GetLocalTimeByDay(1);
				SYSTEMTIME systemtime = ::GetSystemTimeByTM(&t);
				SetLocalTime(&systemtime);
				if (i != day - 1)
				{
					::PrintfTime();
				}
				Sleep(interval_ms);
			}
		}
		else if (day < 0)
		{
			int max_day = -day;
			for (int i = 0; i < max_day; ++i)
			{
				tm t = ::GetLocalTimeByDay(-1);
				SYSTEMTIME systemtime = ::GetSystemTimeByTM(&t);
				SetLocalTime(&systemtime);
				if (i != max_day - 1)
				{
					::PrintfTime();
				}
				Sleep(interval_ms);
			}
		}
		return;
	}
	else
	{
		tm t = ::GetLocalTimeByDay(day);
		SYSTEMTIME systemtime = ::GetSystemTimeByTM(&t);
		SetLocalTime(&systemtime);
	}
}

void AddTime(int hour, int minute, int second)
{
	tm t = ::GetLocalTimeByTime(0, 0, 0);
	t.tm_hour += hour;
	t.tm_min += minute;
	t.tm_sec += second;
	SYSTEMTIME systemtime = ::GetSystemTimeByTM(&t);
	SetLocalTime(&systemtime);
}

void SetTime(int hour, int minute, int second)
{
	tm t = ::GetLocalTimeByTime(hour, minute, second);
	SYSTEMTIME systemtime = ::GetSystemTimeByTM(&t);
	SetLocalTime(&systemtime);
}

unsigned CALLBACK Thread1(void* argv)
{
	if (nullptr == argv)
	{
		return 1;
	}
	while (true)
	{
		time_t* old_second_ptr = (time_t*)argv;
		++*old_second_ptr;
		Sleep(1000);
	}
	return 0;
}

void PrintfTime()
{
	SYSTEMTIME t;
	GetLocalTime(&t);
	printf("周%d %.4d-%.2d-%.2d %.2d:%.2d:%.2d\n", t.wDayOfWeek, t.wYear, t.wMonth, t.wDay, t.wHour, t.wMinute, t.wSecond);
}

std::unordered_map<std::string, std::string> ReadConfig()
{
	std::unordered_map<std::string, std::string> map;
	std::ifstream ifs("config.ini");
	if (!ifs.is_open())
	{
		return map;
	}
	std::string line_str;
	while (std::getline(ifs, line_str))
	{
		size_t pos = line_str.find("=");
		if (std::string::npos == pos)
		{
			continue;
		}
		map[line_str.substr(0, pos)] = line_str.substr(pos + 1);
	}
	return map;
}

time_t ReadXmlTime(const std::string& path, const std::string& key)
{
	std::ifstream ifs(path);
	if (!ifs.is_open())
	{
		return 0;
	}
	time_t timestamp = 0;
	std::string line_str;
	while (std::getline(ifs, line_str))
	{
		size_t pos = line_str.find(key);
		if (std::string::npos == pos)
		{
			continue;
		}
		tm t;
		pos = pos + key.size() + 1;
		if (pos + 4 >= line_str.size())
		{
			break;
		}
		t.tm_year = std::stoi(line_str.substr(pos, 4)) - 1900;

		pos += 5;
		if (pos + 2 >= line_str.size())
		{
			break;
		}
		t.tm_mon = std::stoi(line_str.substr(pos, 2)) - 1;

		pos += 3;
		if (pos + 2 >= line_str.size())
		{
			break;
		}
		t.tm_mday = std::stoi(line_str.substr(pos, 2));

		pos += 3;
		if (pos + 2 >= line_str.size())
		{
			break;
		}
		t.tm_hour = std::stoi(line_str.substr(pos, 2));

		pos += 3;
		if (pos + 2 >= line_str.size())
		{
			break;
		}
		t.tm_min = std::stoi(line_str.substr(pos, 2));

		pos += 3;
		if (pos + 2 >= line_str.size())
		{
			break;
		}
		t.tm_sec = std::stoi(line_str.substr(pos, 2));

		timestamp = mktime(&t);
		break;
	}
	ifs.close();
	return timestamp;
}


int main()
{
	HANDLE thread1 = nullptr;
	std::string add_time_str;
	time_t now_second = 0;
	now_second = ReadXmlTime(config["file"], config["key"]);
	std::string delimiters = config["delimiters"];
	std::string add_str = config["add"];
	if (0 == now_second)
	{
		time(&now_second);
		thread1 = (HANDLE)_beginthreadex(nullptr, 0, Thread1, &now_second, 0, nullptr);
	}
	::PrintfTime();
	while (std::getline(std::cin, add_time_str))
	{
		bool is_add_time = false;
		if (!add_time_str.empty() && add_time_str.substr(0, 1) == add_str)
		{
			add_time_str = add_time_str.substr(1);
			is_add_time = true;
		}

		std::vector<int> add_time_vec;
		::Split(add_time_str, add_time_vec, delimiters);
		size_t size = add_time_vec.size();
		if (0 == size)
		{
			::PrintfTime();
			continue;
		}

		int add_day = 0;
		int hour = 0;
		int minute = 0;
		int second = 0;
		if (1 == size && !is_add_time)
		{
			add_day = add_time_vec[0];
			if (add_day == 0)
			{
				::SetLocalTimeByTimeT(now_second);
				::PrintfTime();
				continue;
			}
			::AddDay(add_day);
		}
		else
		{
			if (size > 0)
			{
				hour = add_time_vec[0];
			}
			if (size > 1)
			{
				minute = add_time_vec[1];
			}
			if (size > 2)
			{
				second = add_time_vec[2];
			}
			else if (size == 2 && !is_add_time)
			{
				tm _tm = ::GetLocalTimeByTime(0, 0, 0);
				second = _tm.tm_sec;
			}
			if (is_add_time)
			{
				::AddTime(hour, minute, second);
			}
			else
			{
				::SetTime(hour, minute, second);
			}
		}
		::PrintfTime();
	}
	if (nullptr != thread1)
	{
		CloseHandle(thread1);
	}
}
