#pragma once
#include <string>
#include <sstream>
#include <algorithm>
#include <type_traits>
#include <vector>
#include <regex>
#include <cctype>
#include <functional>
#include <boost\format.hpp>
#include <boost\lexical_cast.hpp>

namespace Khellendros
{
		template <typename CharT>
		class BasicString final        //不可继承
		{
		public:
/*==================== Cter & Dtor ====================*/
			using ThisT = BasicString;
			using size_type = std::size_t;
			using iterator = typename std::basic_string<CharT>::iterator;
			using const_iterator = typename std::basic_string<CharT>::const_iterator;
			//转发构造函数，将参数转发给std::basic_string
			template <
				typename... T,
				//防止转发构造函数屏蔽其他构造函数
				typename = std::enable_if_t<
				!std::is_same<ThisT,std::decay_t<T>>::value||
				//字符数组不进行完美转发（减少报错）
				std::is_same<const char*,std::decay_t<T>>::value
				>
			>
			BasicString(T&&... args) :_content(std::forward<T>(args)...) {}
			BasicString(const CharT* const charArr) :_content(charArr) {}
			BasicString() = default;
			~BasicString() = default;
			BasicString(const ThisT&) = default;
			BasicString(ThisT&&)noexcept = default;
			BasicString& operator=(const ThisT&) = default;
			BasicString& operator=(ThisT&&)noexcept = default;

/*=================== Forwards ====================*/
			auto begin() { return _content.begin(); }
			auto begin()const { return _content.cbegin(); }
			auto end() { return _content.end(); }
			auto end()const { return _content.cend(); }
			auto cbegin()const { return _content.cbegin(); }
			auto cend()const { return _content.cend(); }
			auto rbegin() { return _content.rbegin(); }
			auto rbegin()const { return _content.crbegin(); }
			auto rend() { return _content.rend(); }
			auto rend()const { return _content.crend(); }
			auto crbegin()const { return _content.crbegin(); }
			auto crend()const { return _content.crend(); }

			friend bool operator==(const ThisT& lhs,const ThisT& rhs) { return lhs._content == rhs._content; }
			friend bool operator!=(const ThisT& lhs, const ThisT& rhs) { return !(lhs == rhs); }
			friend bool operator<(const ThisT& lhs, const ThisT& rhs) { return lhs._content < rhs._content; }
			friend bool operator>(const ThisT& lhs, const ThisT& rhs) { return rhs < lhs; }
			friend bool operator<=(const ThisT& lhs, const ThisT& rhs) { return !(lhs > rhs); }
			friend bool operator>=(const ThisT& lhs, const ThisT& rhs) { return !(lhs < rhs); }

			decltype(auto) operator+=(const ThisT& rhs) { return _content += rhs._content; }
			friend decltype(auto) operator+(const ThisT& lhs, const ThisT& rhs) { return lhs._content + rhs._content; }
			decltype(auto) operator[](size_type index) { return _content[index]; }
			auto operator[](size_type index)const { return _content[index]; }
			size_type size() { return _content.size(); }
			bool empty() { return _content.empty(); }
			decltype(auto) c_str() { return _content.c_str(); }
			const std::basic_string<CharT>& std_str()const { return _content; }
			std::basic_string<CharT>& std_str() { return _content; }
			operator std::basic_string<CharT>() { return _content; }//可以隐式转换成std::basic_string
			friend std::ostream& operator<<(std::ostream& os, const ThisT& rhs)
			{
				os << rhs._content;
				return os;
			}
			friend std::istream& operator >> (std::istream& is, ThisT& rhs)
			{
				is >> rhs._content;
				return is;
			}

/*==================== Additions ====================*/
			//分割字符串（单个字符作为分隔符）
			std::vector<ThisT> split(char spliter)const
			{
				vector<ThisT> result;
				auto iter1 = cbegin();
				auto iter2 = cbegin();
				while (iter1!=cend())
				{
					iter2 = std::find(iter2, cend(), spliter);
					result.emplace_back(iter1, iter2);
					if(iter2!=cend())	++iter2;
					iter1 = iter2;
				}
				return result;
			}

			//分割字符串（正则表达式作为分隔符）
			std::vector<ThisT> split(const std::basic_regex<CharT>& rgx)const
			{
				using TokenIterT = std::regex_token_iterator<const_iterator>;
				TokenIterT iter(cbegin(), cend(), rgx, -1);
				return std::vector<ThisT>(iter, TokenIterT());
			}

			//格式化字符串（通过函数参数）
			template<typename... Args>
			static ThisT format(const ThisT& fmt, Args&&... args)
			{
				return do_format(boost::format(fmt._content), std::forward<Args>(args)...).str();
			}

			//格式化字符串（通过迭代器）
			template<typename IterT>
			static ThisT format_iter(const ThisT& fmt, const IterT& beg, const IterT& end) 
			{
				auto iter = beg;
				boost::format f(fmt._content);
				while (iter != end) {
					f%*iter++;
				}
				return f.str();
			}

			//格式化字符串（通过容器）
			template<typename ContainerT>
			static ThisT format_list(const ThisT& fmt, const ContainerT& c) 
			{ return format_iter(fmt,std::cbegin(c), std::cend(c)); }

			//转化为boost::format
			friend boost::format format(const ThisT& s) { return boost::format(s._content); }
			//查找第一个子串
			const_iterator find_child(const ThisT& child)const
			{
				return std::search(cbegin(), cend(), child.cbegin(), child.cend());
			}
			iterator find_child(const ThisT& child)
			{
				return std::search(begin(), end(), child.cbegin(), child.cend());
			}
			//是否包含子串
			bool contains(const ThisT& child)const
			{
				return find_child(child) != end();
			}
			//子串数量
			size_type child_count(const ThisT& child)const
			{
				size_type count = 0;
				for (auto iter = std::search(cbegin(), cend(), child.cbegin(), child.cend());
					 iter != cend(); 
					iter = std::search(iter, cend(), child.cbegin(), child.cend())) {
					++iter;
					++count;
				}
				return count;
			}

			//组合字符串（被调用字符串作为分隔符）
			template<typename... Args>
			ThisT join(Args&&... args)const
			{
				std::basic_string<CharT> temp;
				do_join(temp, std::forward<Args>(args)...);
				return temp;
			}

			//转化为输入流
			std::basic_istringstream<CharT> as_istream()const
			{ return std::basic_istringstream<CharT>(_content); }
			//转化为输出流
			std::basic_ostringstream<CharT> as_ostream()
			{ return std::basic_ostringstream<CharT>(_content); }

			//替换子串
			void replace(const std::basic_regex<CharT>& rgx, const ThisT& newstr)
			{
				_content = std::regex_replace(_content, rgx, newstr._content);
			}

			//匹配
			bool match(const std::basic_regex<CharT>& rgx)const
			{
				return std::regex_match(_content, rgx);
			}

			//匹配开头
			bool begin_with(const ThisT& match)const
			{
				ThisT re = format("%1%.*", match);
				return std::regex_match(_content, std::basic_regex<CharT>(re._content));
			}
			//匹配结尾
			bool end_with(const ThisT& match)const
			{
				ThisT re = format(".*%1%", match);
				return std::regex_match(_content, std::basic_regex<CharT>(re._content));
			}
			//转换成大写
			ThisT upper()const
			{
				auto temp = *this;
				for (char& c : temp)
					if ('a' <= c&&c <= 'z')c -= Atoa;
				return temp;
			}
			//转换成小写
			ThisT lower()const
			{
				auto temp = *this;
				for (char& c : temp)
					if ('A' <= c&&c <= 'Z')c += Atoa;
				return temp;
			}
			//首字母大写
			ThisT capitalize()const
			{
				auto temp = *this;
				bool flag = true;
				for (char& c : temp)
				{
					if (std::isalpha(c)) {
						if (flag) {
							if (std::islower(c))
								c -= Atoa;
							flag = false;
						}
						else if (std::isupper(c)) {
							c += Atoa;
						}
					}
					else flag = true;
				}
				return temp;
			}
			//判断大小写
			bool is_upper()const
			{
				return std::all_of(cbegin(), cend(), [](const char& c) {
					return !std::islower(c);
				});
			}
			bool is_lower()const
			{
				return std::all_of(cbegin(), cend(), [](const char& c) {
					return !std::isupper(c);
				});
			}
			//转化成其他类型
			template<typename T>
			T cast_to()const { return boost::lexical_cast<T>(_content); }
		private:
			std::basic_string<CharT> _content;
			constexpr static std::size_t Atoa = 'a' - 'A';

			//实现细节
			static boost::format& do_format(boost::format& fmt) { return fmt; }
			template<typename T,typename... Args>
			static boost::format& do_format(boost::format& fmt, T&& arg, Args&&... args)
			{
				fmt % std::forward<T>(arg);
				return do_format(fmt, std::forward<Args>(args)...);
			}
			template<typename T>
			void do_join(std::basic_string<CharT>& result, T&& arg)const {
				result += boost::lexical_cast<std::basic_string<CharT>>(std::forward<T>(arg));
			}
			template<typename T,typename... Args>
			void do_join(std::basic_string<CharT>& result, T&& arg, Args&&... args)const
			{
				result += boost::lexical_cast<std::basic_string<CharT>>(std::forward<T>(arg));
				result += _content;
				do_join(result, std::forward<Args>(args)...);
			}
		};
		using String = BasicString<char>;
}