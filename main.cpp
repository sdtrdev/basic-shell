#include <iostream>
#include <memory>
#include <unordered_map>
#include <string>
#include <sstream>
#include <vector>
#include <algorithm>
#include <cstdlib>
#include <unistd.h>
#include <sys/wait.h>
#include <errno.h>

void sys_exit(const char *msg) {
	perror(msg);
	exit(EXIT_FAILURE);
}

std::vector<std::string> split(std::string const& str, char delim = ' ') {
	std::istringstream stream{str};
	std::vector<std::string> temp_vec;
	std::string temp;
	while(std::getline(stream, temp, delim)) {
		temp_vec.emplace_back(temp.begin(), temp.end());
	}
	return temp_vec;
}

constexpr const char* default_shell_name = "Ahmet";
std::vector<std::string> paths(split(std::getenv("PATH"), ':'));

int main(int argc, char **argv) {
	std::string temp;
	for(;;) {
		char cwd[255];
		std::cout << getcwd(cwd,255) << " > ";
		getline(std::cin, temp);
		auto splitted = split(temp);
		if(splitted.size() <= 0 || splitted[0] == "") {
			continue;
		}
		if(splitted[0] == "cd" && splitted.size() == 2) {
			chdir(splitted[1].c_str());
			continue;
		}
		std::vector<const char *> cvec;
		int i{};
		std::for_each(splitted.begin(),splitted.end(), [&cvec, &i](const auto& item) {
			cvec.push_back(const_cast<char*>(item.c_str()));
		});
		std::string path;
		std::for_each(std::begin(paths), std::end(paths), [&](const auto& item) {
			std::string t{item};
			t += '/';
			t += splitted[0];
			if(access(t.c_str(), F_OK) != -1) {
				path = t;
			}
		});
		if(path.length() == 0 && splitted[0].size() > 2 && splitted[0][0] == '.' && splitted[0][1] == '/') {
			path = splitted[0].substr(2);
		}
		else if(path.length() == 0) {
			std::cout << splitted[0] << " NOT FOUND!" << "\n";
			continue;
		}
		pid_t child_process = fork();
		if(child_process == 0) {
			execv(path.c_str(),const_cast<char*const*>(cvec.data()));
		}
		else {
			wait(NULL);
		}
	}
	return 0;
}