#include <iostream>
#include <sstream>
#include <algorithm>
#include <string>

std::string toLower(std::string s)
{
	std::transform(s.begin(), s.end(), s.begin(),
		[](unsigned char c) { return std::tolower(c); });
	return s;
}

int main(int argc, char* argv[]) {
	for (int i = 1; i < 1; i++) {
		printf("Are you coming here?\n");
	}

	std::string token;
	std::istringstream ss(toLower("Intel(R) HD Graphics"));
	std::string numbering("");
	while (std::getline(ss, token, ' '));
	for (int idx = 0; idx < token.size(); idx++) {
		if (isdigit(token[idx])) {
			numbering.append({ token[idx] });
		}
	}
	if (numbering.size() > 0) {
		int sVersion = stoi(numbering.substr(0, 2));
		if (sVersion >= 60) {
			return true;
		}
	}

}