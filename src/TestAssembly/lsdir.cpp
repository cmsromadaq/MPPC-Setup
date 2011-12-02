#include <sys/types.h>
#include <dirent.h>
#include <unistd.h>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


int main(int argc, char **argv){
	DIR *dp;
	dirent *d;
	std::string file;

	if(dp = opendir(argv[1])){
		while(d = readdir(dp)){
			if(!strcmp(d->d_name,".") || !strcmp(d->d_name,".."))
				continue;
			file.assign(d->d_name);
			//std::cout << file.substr(0,6) << " " << file.substr(0,6).compare("ttyUSB") << std::endl;
			if(file.substr(0,6).compare("ttyUSB") == 0 && file.length()==7)
				std::cout << file << "\t" << file.length() << std::endl;
			//std::cout << d->d_name << std::endl;
		}
	}else{
		perror("opendir");
		return 1;
	}
	return 0;
}
