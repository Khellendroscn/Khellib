#include "Concurrent.h"
#include "String.h"
#include <iostream>
#include <functional>
#include <utility>
#include <regex>

using namespace std;
using namespace Khellendros;

int testCount = 0;
int passCount = 0;

using TestList = std::vector<std::pair<std::string, std::function<bool()>>>;

#define RUN_TESTS(test_list,flag)\
do{\
	flag = true;\
	for(auto& p:test_list){\
		++testCount;\
		if(p.second()) ++passCount;\
		else {\
			cout<<"Unpass: "<<p.first<<" @ "<<__FILE__<<" : "<<__LINE__<<std::endl;\
			flag = false;\
		}\
	}\
}while(0)

bool test_string_init()
{
	String empts;
	String str = "init";
	String clone = std::move(str);
	return clone == "init";
}
bool test_string_split()
{
	String s = "aaa bbb ccc";
	String s2 = "aaa/**/bbb/**/ccc";
	const std::vector<String> result{ "aaa", "bbb", "ccc" };
	return s.split(' ') == result&&s2.split(std::regex("/\\*\\*/")) == result;
}
bool test_string_format() 
{
	auto date = String::format("Today:\t%4d/%2d/%2d", 2017, 1, 20);
	const std::vector<std::string> strs{ "aaa","bbb","ccc" };
	auto str = String::format_list("%1% %2% %3%", strs);
	auto fmt = format(String("%1% %2%"));
	return date == "Today:\t2017/ 1/20"&&str == "aaa bbb ccc" &&
		str == "aaa bbb ccc"&&
		(fmt%"Hello"%"World").str() == "Hello World";
}
bool test_string_search()
{
	String s = "Wonderful C++! C++! C++!";
	char cpp = *s.find_child("C++");
	return s.contains("C++") && cpp == 'C' && !s.contains("Java") && s.child_count("C++") == 3;
}
bool test_string_join()
{
	String s = " , ";
	s = s.join(1, 2.25, "Hi");
	return s == "1 , 2.25 , Hi";
}
bool test_string_stream()
{
	String str = "1 2 3 4";
	auto sin = str.as_istream();
	std::vector<int> vec;
	for (int i = 0; i < 4; ++i)
	{
		int temp;
		sin >> temp;
		vec.push_back(temp);
	}
	return vec == std::vector<int>{1, 2, 3, 4};
}
bool test_string_replace()
{
	String str = "Java Java Java";
	str.replace(std::regex("Java"), "C++");
	return str == "C++ C++ C++";
}
bool test_string_match()
{
	String str = "Hello World.cpp";
	return str.match(std::regex(".*\\.cpp")) &&
		str.begin_with("\\w") &&  str.end_with(".cpp");
}
bool test_string_capitalize()
{
	String str = "HElLO123 &*%woRld!";
	return str.upper().is_upper() && str.lower().is_lower() &&
		str.capitalize() == "Hello123 &*%World!";
}
bool test_string_cast()
{
	int n = String("123").cast_to<int>();
	double d = String("3.14").cast_to<double>();
	return n == 123 && d == 3.14;
}
bool test_string()
{
	TestList string_tests{
		{"init",test_string_init},
		{"split",test_string_split },
		{"format",test_string_format},
		{"search",test_string_search},
		{"join",test_string_join},
		{"stream",test_string_stream},
		{"replace",test_string_replace},
		{"match",test_string_match },
		{"capitalize",test_string_capitalize},
		{"cast",test_string_cast}
	};
	bool flag;
	RUN_TESTS(string_tests,flag);
	return flag;
}
void test_concurrent_detachExec()
{
	++testCount;
	for (int i = 0; i < 10; ++i)
	{
		detachExec([] {return 1; });
	}
	++passCount;
}
void test_concurrent_reallyAsyc()
{
	++testCount;
	for (int i = 0; i < 10; ++i)
	{
		asyncExec([] {return 1; });
	}
	++passCount;
}
void test_concurrent_ThreadRall()
{
	++testCount;
	ThreadGuard(std::thread([] {return 1; }),ThreadGuard::DtorAction::join);
	ThreadGuard(std::thread([] {return 1; }), ThreadGuard::DtorAction::detach);
	++passCount;
}
void test_concurrent()
{
	test_concurrent_detachExec();
	test_concurrent_reallyAsyc();
	test_concurrent_ThreadRall();
}
int main()
{
	test_concurrent();
	test_string();
	cout << "pass : all = " << passCount << " : " << testCount << endl;
	if (passCount == testCount)cout << "All passed!" << endl;
	system("pause");
	return 0;
}
