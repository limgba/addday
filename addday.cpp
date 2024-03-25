#include <iostream>
#include <process.h>
#include "addday.h"

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

tm GetLocalTimeByDay(int day, int hour, int minute, int second)
{
	time_t now_second;
	time(&now_second);
	now_second = now_second + day * 86400 + hour * 3600 + minute * 60 + second;
	tm t;
	localtime_s(&t, &now_second);
	return t;
}

void AddDay(int day, int hour, int minute, int second)
{
	tm t = ::GetLocalTimeByDay(day, hour, minute, second);
	SYSTEMTIME systemtime = ::GetSystemTimeByTM(&t);
	SetLocalTime(&systemtime);
}

unsigned CALLBACK Thread1(void* argv)
{
	if (nullptr == argv)
	{
		return 1;
	}
	time_t now_second = time(nullptr);
	time_t* old_second_ptr = (time_t*)argv;
	if (now_second == *old_second_ptr)
	{
		return 2;
	}
	++*old_second_ptr;
	return 0;
}

void PrintfTime()
{
	SYSTEMTIME t;
	GetLocalTime(&t);
	printf("%.4d-%.2d-%.2d %.2d:%.2d:%.2d\n", t.wYear, t.wMonth, t.wDay, t.wHour, t.wMinute, t.wSecond);
}

int main()
{
	std::string add_time_str;
	int add_day = 0;
	int add_hour = 0;
	int add_minute = 0;
	int add_second = 0;
	time_t now_second = time(nullptr);
	HANDLE thread1 = (HANDLE)_beginthreadex(nullptr, 0, Thread1, &now_second, 0, nullptr);
	while (std::getline(std::cin, add_time_str))
	{
		std::vector<int> add_time_vec;
		::Split(add_time_str, add_time_vec, " ");
		if (add_time_vec.empty())
		{
			::PrintfTime();
			continue;
		}
		if (add_time_vec.size() > 0)
		{
			add_day = add_time_vec[0];
			if (add_day == 111)
			{
				::SetLocalTimeByTimeT(now_second);
				::PrintfTime();
				continue;
			}
		}
		if (add_time_vec.size() > 1)
		{
			add_hour = add_time_vec[1];
		}
		if (add_time_vec.size() > 2)
		{
			add_minute = add_time_vec[2];
		}
		if (add_time_vec.size() > 3)
		{
			add_second = add_time_vec[3];
		}
		::AddDay(add_day, add_hour, add_minute, add_second);
		::PrintfTime();
	}
	CloseHandle(thread1);
}
