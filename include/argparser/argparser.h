#pragma once

#include <cstdlib>
#include <map>
#include <set>
#include <optional>
#include <string>
#include <sstream>
#include <iostream>
#include <typeinfo>

namespace argparser
{
	class arglist
	{
	public:
		arglist(int argc, char** argv);
		const std::string operator[](const std::string key);
		const std::set<std::string> get_unmatched() const;
	private:
		bool match_argument(std::string name);
	private:
		std::set<std::string> unmatched;
		std::map<std::string, std::string> map;
	};

	struct doc
	{
		explicit doc(std::string docstring) : docstring(docstring) {}
		std::string docstring;
	};

	template<class Class>
	struct def
	{
		explicit def(Class const& value) : value(value) {}
		Class const& value;
	};

	template<class Type>
	struct typestr_t
	{
		std::string operator()() {
			return typeid(Type).name();
		}
	};

	template<>
	struct typestr_t<std::string>
	{
		std::string operator()() {
			return "string";
		}
	};

	template<class Class>
	class parser
	{
	public:
		parser(arglist& list, Class& handle, const char* helpstr) :
			list(list), handle(handle) {
			parse_mode = list["help"].empty();
			if (!parse_mode && helpstr)
				std::cerr << helpstr << std::endl;
		}

		parser& build() {
			if (!list["help"].empty())
				exit(0);
			for (auto const& unmatched : list.get_unmatched())
				std::cout << "Unmatched argument '" << unmatched << "'\n";

			return *this;
		}

		template<
			class _Class,
			class _Type,
			class... _Extra
		> parser& bind(const std::string name, _Type _Class::*field,
			_Extra... extras) {
			if (!parse_mode) {
				std::cerr << std::endl;
				if constexpr (std::is_same<_Type, bool>::value) {
					if (name.empty()) {
						throw std::runtime_error("empty name");
					} else if (name.size() == 1) {
						std::cerr << "-" << name;
					} else {
						std::cerr << "--" << name;
					}
				} else {
					typestr_t<_Type> typestr;
					std::cerr << "--" << name << "=<" << typestr() << ">";
				}
				std::cerr << std::endl;
			}
			pre_extras(name, field, extras...);
			if (parse_mode) {
				std::string argval = list[name];
				if (!argval.empty())
					std::istringstream(argval) >> handle.*field;
			}
			return *this;
		}
	private:
		template<
			class _Class,
			class _Type
		> void pre_extras(const std::string name, _Type _Class::* field) {}

		template<
			class _Class,
			class _Type,
			class... _Extra
		> void pre_extras(const std::string name, _Type _Class::* field,
			doc& documentation, _Extra... extras) {
			if (!parse_mode) {
				std::cerr << "\t" << documentation.docstring << std::endl;
			}
			pre_extras(name, field, extras...);
		}

		template<
			class _Class,
			class _Type,
			class _Def,
			class... _Extra
		> void pre_extras(const std::string name, _Type _Class::* field,
			def<_Def>& default_value, _Extra... extras) {
			pre_extras(name, field, extras...);
			if (parse_mode) {
				handle.*field = default_value.value;
			} else {
				std::cerr << "\tdefault: " << default_value.value << std::endl;
			}
		}
	private:
		arglist list;
		Class& handle;
		bool parse_mode;
	};

	template<class Class>
	parser<Class> build_parser(int argc, char** argv, Class& obj,
		const char* help = nullptr) {
		return parser<Class>(arglist(argc, argv), obj, help);
	}
}