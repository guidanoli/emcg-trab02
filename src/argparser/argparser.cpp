#include "argparser.h"

#include <regex>

using namespace argparser;

// Grammar

const auto regex = std::regex(
"^-([^-]+)$|"         // eg: -v
"^--([^=]+)=(.+)$|"   // eg: --verbosity=2
"^--([^=]+)$"         // eg: --verbose
);

arglist::arglist(int argc, char** argv)
{
	for (int i = 1; i < argc; ++i)
		if (!match_argument(argv[i]))
			std::cerr << "Invalid argument " << argv[i] << std::endl;
}

const std::string arglist::operator[](const std::string key) {
	auto const& entry = map.find(key);
	if (entry == map.end())
		return std::string("");
	unmatched.erase(entry->first);
	return entry->second;
}

const std::set<std::string> arglist::get_unmatched() const
{
	return unmatched;
}

bool arglist::match_argument(std::string arg)
{
	std::smatch match;
	if (!std::regex_match(arg, match, regex))
		return false;
	std::string c = match[1], // cf
	            k = match[2], // key of vf
	            v = match[3], // value of vf
	            f = match[4]; // mcf
	if (!c.empty()) {
		for (std::size_t i = 0; i < c.size(); ++i) {
			auto ch = c.substr(i, 1);
			unmatched.insert(ch);
			map.insert(std::make_pair(ch, "1"));
		}
	} else if (!k.empty()) {
		unmatched.insert(k);
		map.insert(std::make_pair(k, v));
	} else if (!f.empty()) {
		unmatched.insert(f);
		map.insert(std::make_pair(f, "1"));
	} else {
		return false;
	}
	return true;
}