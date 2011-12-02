#include <iostream>
#include <stdio.h>    /* Standard input/output definitions */
#include <stdlib.h>
#include <stdint.h>   /* Standard types */
#include <string.h>   /* String function definitions */
#include <unistd.h>   /* UNIX standard function definitions */
#include <fcntl.h>    /* File control definitions */
#include <errno.h>    /* Error number definitions */
#include <sys/ioctl.h>
#include <getopt.h>
#include <TestAssembly.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#include <unistd.h>

std::string Num2Str(int i);


TestAssembly::TestAssembly(const char* device){
	this->baud = 9600;
	this->endChar = '\n';
	this->Connect(device);
	this->debug = false;
	this->Identify();
}


TestAssembly::TestAssembly(){
	this->endChar = '\n';
	this->baud = 9600;
	std::string device;
	device = this->GetPort();
	this->Connect(device.c_str());
	this->debug = false;
}

int TestAssembly::Connect(){
	std::string device;
	return this->GetPort();
}

int TestAssembly::Connect(const char* device){
	this->arduino = open(device, O_RDWR | O_NOCTTY | O_NDELAY);
	if(this->arduino==-1){
		std::cerr << "\e[0;31mERROR:\e[0m TestAssembly::Connect: Could not establish connection with Test Assembly in: " << device << std::endl;
		this->connected = false;
		return 1;
	}
	if (tcgetattr(this->arduino, &this->toptions) < 0) {
		std::cerr << "\e[0;31mERROR:\e[0m TestAssembly::Connect: Couldn't get term attributes" << std::endl;
		this->connected = false;
		this->Close();
		return 1;
	}
	cfsetispeed(&this->toptions, B9600);
	cfsetospeed(&this->toptions, B9600);
	this->toptions.c_iflag &= ~(IGNBRK | BRKINT | PARMRK | ISTRIP | INLCR | IGNCR | ICRNL | IXON | IXOFF);
	this->toptions.c_oflag &= ~OPOST;
	this->toptions.c_cflag &= ~(CSIZE | PARENB | HUPCL);
	this->toptions.c_cflag |= CS8;
	this->toptions.c_lflag &= ~(ECHO | ECHONL | ICANON | ISIG | IEXTEN);
	// see: http://unixwiz.net/techtips/termios-vmin-vtime.html
	this->toptions.c_cc[VMIN]  = 0;
	this->toptions.c_cc[VTIME] = 0;
	
	if( tcsetattr(arduino, TCSANOW, &toptions) < 0) {
		std::cerr << "\e[0;31mERROR:\e[0m init_serialport: Couldn't set term attributes" << std::endl;
		this->connected = false;
		return 1;
	}
	if(tcflush(this->arduino,TCIOFLUSH)){
		std::cerr << "\e[0;31mERROR:\e[0m TestAssembly::Connect: impossible to flush the port" << std::endl;
		this->Close();
		return 1;
	}
	this->connected = true;
	return 0;
}

TestAssembly::~TestAssembly(){
	this->Close();
}

int TestAssembly::Write(std::string data){
	int len,n;
	if(this->IsOpen()){
		len = data.length();
		if(tcflush(this->arduino,TCIOFLUSH)){
			std::cerr << "\e[0;31mERROR:\e[0m TestAssembly::Connect: Imposible to flush the port" << std::endl;
			return 1;
		}
		n = write(this->arduino,data.c_str(),len);
		if(tcdrain(this->arduino)){
			std::cerr << "\e[0;31mERROR:\e[0m TestAssembly::Write: The command cannot be accomplished" << std::endl;
			this->Close();
			return 1;
		}
		if(len != n){
			if(len>0 && n<1){
				std::cerr << "\e[0;31mERROR:\e[0m TestAssembly::Write: The communication with Test Assembly is lost" << std::endl;
				this->connected = false;
			}
			this->Close();
			return 1;
		}
		return 0;
	}else{
		std::cerr << "\e[0;31mERROR:\e[0m TestAssembly::Write: The communication is not established with Test Assembly" << std::endl;
		this->Close();
		return 1;
	}
}

int TestAssembly::Read(std::string *data){
	char buffer;
	int j=0,n;
	if(this->IsOpen()){
		data->assign("");
		do{
			j++;
			n = read(this->arduino,&buffer,1);
			if(this->debug)
				std::cout << "**" << buffer << "-" << (int)buffer << "-" << errno<< "*" << std::endl;
			if(n==-1 && errno != 11){
				std::cerr << "\e[0;31mERROR:\e[0m TestAssembly::Read: The communication with Test Assembly is lost" << std::endl;
				this->connected = false;
				this->Close();
				return 1;
			}
			if(j>100){
				std::cerr << "\e[0;31mERROR:\e[0m TestAssembly::Read: excede the number of intent. There are no response from the controller." << std::endl;
				this->Close();
				return 1;
			}
			
			if(buffer != '0' && buffer != 8 && buffer != 10 && buffer != 13){
				data->append(1,buffer);
			}
			if(n==0 || errno == 11){
				usleep(100*1000); // wait 10 ms
				if(errno==11)
					usleep(100000);
				continue;
			}
		}while(buffer != this->endChar && buffer != '\r' && buffer != '\n');
		return 0;
	}else{
		std::cerr << "\e[0;31mERROR:\e[0m TestAssembly::Read: The communication is not established with Test Assembly" << std::endl;
		this->Close();
		return 1;
	}
}

int TestAssembly::Close(){
	if(this->IsOpen()){
		close(this->arduino);
		return 0;
	}else{
		std::cerr << "\e[0;31mERROR:\e[0m TestAssembly::Close: The communication is not established with Test Assembly" << std::endl;
		return 1;
	}
}

void TestAssembly::SetAddress(int addr){
	if(this->IsOpen()){
		if(addr < 0 || addr > 7){
			fprintf(stderr,"Address exced limits\n");
			exit(1);
		}
		size_t first,last;
		std::string data,result,message,ack;
		data.assign(":SETADDR ");
		data.append(Num2Str(addr));
		data.append(" ");
		if(this->debug)
			std::cout << "TestAssembly::SetAddress: Writing data: " << data<< std::endl;
		if(this->Write(data)){
			std::cerr << "\e[0;31mERROR:\e[0m TestAssembly::SetAddress: writing the data" << std::endl;
			exit(1);
		}
		sleep(5);
		if(this->debug)
			std::cout << "TestAssembly::SetAddress: Reading ACK" << std::endl;
		if(this->Read(&result)){
			std::cerr << "\e[0;31mERROR:\e[0m TestAssembly::SetAddress: reading the ACK" << std::endl;
			exit(1);
		}
		if(this->debug)
			std::cout << "TestAssembly::SetAddress: ACK: " << data << std::endl;
		first = result.find_first_of(":");
		ack.assign(":ACK:SETADDR");
		last = ack.length();
		if(result.substr(first,last).compare(ack)==0){
			if(this->debug)
				std::cout << "TestAssembly Controller: Address setted to: " << addr << std::endl;
			return;
		}else if(result.compare(":ERROR:Wrong Address")){
			std::cerr << "\e[0;31mERROR:\e[0m TestAssembly::SetAddress: wrong command: " << data << std::endl;
			exit(1);
		}else{
			if(this->debug){
				std::cout << "ERROR: TestAssembly::SetAddress: wrong message: " << data << std::endl;
				std::cout << "Largo 1: " << data << std::endl;
				std::cout << "Largo 1: " << data.length() << "***" << data << "***" << std::endl;
				std::cout << "Largo 2: " << ack.length() << "***" << ack << "***" << std::endl;
				std::cout << "Difference: " << data.compare(ack) << std::endl;
			}
			exit(1);
		}
	}else{
		std::cerr << "\e[0;31mERROR:\e[0m TestAssembly::SetAddress: The communication is not established with Test Assembly" << std::endl;
		exit(1);
	}
	return;
}

void TestAssembly::EnableVoltageMeasurements(){
	if(this->IsOpen()){
		size_t first,last;
		std::string data,result,message,ack,ack2;
		data = ":ONVOL";
		if(this->Write(data)){
			std::cerr << "\e[0;31mERROR:\e[0m TestAssembly::EnableVoltageMeasurements: writing data" << std::endl;
			exit(1);
		}
		if(this->Read(&result)){
			std::cerr << "\e[0;31mERROR:\e[0m TestAssembly::EnableVoltageMeasurements: reading the ACK" << std::endl;
			exit(1);
		}
		first = result.find_first_of(":");
		ack.assign(":ACK:ONVOL");
		ack2.assign(":ACK:ONNVOL");
		last = ack.length();
		if(result.substr(first,last).compare(ack)==0 || result.substr(first,last+1).compare(ack2)==0){
			return;
		}else if(result.compare(":ERROR:ONVOL")){
			std::cerr << "ERROR: TestAssembly::EnableVoltageMeasurements: wrong command " << data << " with result " << result << std::endl;
			exit(1);
		}else{
			std::cerr << "\e[0;31mERROR:\e[0m TestAssembly::EnableVoltageMeasurements: wrong ACK" << std::endl;
			exit(1);
		}
	}else{
		std::cerr << "\e[0;31mERROR:\e[0m TestAssembly::EnableVoltageMeasurements: The communication is not established with Test Assembly" << std::endl;
		exit(1);
	}
}

void TestAssembly::DisableVoltageMeasurements(){
	if(this->IsOpen()){
		size_t first,last;
		std::string data,result,message,ack;
		data = ":OFFVOL";
		if(this->Write(data)){
			std::cerr << "\e[0;31mERROR:\e[0m TestAssembly::DisableVoltageMeasurements: writing data" << std::endl;
			exit(1);
		}
		if(this->Read(&result)){
			std::cerr << "\e[0;31mERROR:\e[0m TestAssembly::DisableVoltageMeasurements: reading the ACK" << std::endl;
			exit(1);
		}
		ack.assign(":ACK:OFFVOL");
		first = result.find_first_of(":");
		last = ack.length();
		if(result.substr(first,last).compare(ack)==0){
			return;
		}else if(result.compare(":ERROR:OFFVOL")){
			std::cerr << "\e[0;31mERROR:\e[0m TestAssembly::DisableVoltageMeasurements: wrong command: " << data << std::endl;
			exit(1);
		}else{
			std::cerr << "\e[0;31mERROR:\e[0m TestAssembly::DisableVoltageMeasurements: wrong ACK" << std::endl;
			exit(1);
		}
	}else{
		std::cerr << "\e[0;31mERROR:\e[0m TestAssembly::DisableVoltageMeasurements: The communication is not established with Test Assembly" << std::endl;
		exit(1);
	}
	return;
}

void TestAssembly::GetError(std::string *error){
	if(this->IsOpen()){
		std::string data;
		data = ":WATCHERROR";
		if(this->Write(data))
			exit(1);
		if(this->Read(error))
			exit(1);
		return;
	}else{
		std::cerr << "\e[0;31mERROR:\e[0m TestAssembly::GetError: The communication is not established with Test Assembly" << std::endl;
		exit(1);
	}
}

void TestAssembly::Reset(){
	if(this->IsOpen()){
		size_t first,last;
		std::string data,result,message,ack;
		data = ":RST";
		if(tcflush(this->arduino,TCIOFLUSH)){
			std::cerr << "\e[0;31mERROR:\e[0m TestAssembly::Reset: flushing the device" << std::endl;
			exit(1);
		}
		if(this->Write(data)){
			std::cerr << "\e[0;31mERROR:\e[0m TestAssembly::Reset: writing data" << std::endl;
			exit(1);
		}
		if(this->Read(&result)){
			std::cerr << "\e[0;31mERROR:\e[0m TestAssembly::Reset: reading the ACK" << std::endl;
			exit(1);
		}
		ack.assign(":ACK:RST");
		first = result.find_first_of(":");
		last = ack.length();
		if(result.substr(first,last).compare(ack)==0){
			return;
		}else if(result.compare(":ERROR:Reset")){
			std::cerr << "\e[0;31mERROR:\e[0m TestAssembly::Reset: wrong command: " << data << std::endl;
			exit(1);
		}else{
			std::cerr << "\e[0;31mERROR:\e[0m TestAssembly::Reset: wrong ACK" << std::endl;
			exit(1);
		}
	}else{
		std::cerr << "\e[0;31mERROR:\e[0m TestAssembly::Reset: The communication is not established with Test Assembly" << std::endl;
		exit(1);
	}
	return;
}

int TestAssembly::GetPort(){
	int i=0;
	int deveviceNumber,j;
	DIR *dp;
	dirent *d;
	std::string message,device,deviceList[10];
	if((dp = opendir("/dev/"))){
		while((d = readdir(dp))){
			if(!strcmp(d->d_name,".") || !strcmp(d->d_name,".."))
				continue;
			device.assign(d->d_name);
			if(device.substr(0,6).compare("ttyUSB") == 0 && device.length() == 7){
				deviceList[i].assign(device);
				i++;
			}
		}
		deveviceNumber = i;
		i=-1;
		j=0;
		message.assign("MPPC controler - VER1.1");
		do{
			i++;
			if(i!=0){
				this->Close();
			}
			if(this->Connect(deviceList[i].c_str())){
				std::cerr << "\e[0;31mERROR:\e[0m No device found in " << deviceList[i] << std::endl;
				continue;
			}
		}while(message.compare(this->Identify().substr(0,23)) != 0 && i<deveviceNumber);
		if(i==deveviceNumber){
			std::cerr << "\e[0;31mERROR:\e[0m No device found connected" << std::endl;
			return 1;
		}
		if(IsOpen()){
			return 0;
		}
		
	}else{
		std::cerr << "\e[0;31mERROR:\e[0m TestAssembly::GetPort: opendir" << std::endl;
		return 1;
	}
	return 1;
}

bool TestAssembly::IsOpen(){
	return this->connected;
}

void TestAssembly::Debug(bool deb){
	this->debug = deb;
}

std::string TestAssembly::Identify(){
	if(this->IsOpen()){
		std::string data,result,message,ack;
		data = ":IDN?";
		if(this->Write(data)){
			std::cerr << "\e[0;31mERROR:\e[0m TestAssembly::Identify: writing data" << std::endl;
			//return "";
			exit(1);
		}
		if(this->Read(&result)){
			std::cerr << "\e[0;31mERROR:\e[0m TestAssembly::Identify: reading the ACK" << std::endl;
			//return "";
			exit(1);
		}
		return result;
	}else{
		std::cerr << "\e[0;31mERROR:\e[0m TestAssembly::Identify: The communication is not established with Test Assembly" << std::endl;
		//return "";
		exit(1);
	}
	exit(1);
	//return "";
}


#ifndef __SUPER_TRANSLATOR_INT2STRING__
#define __SUPER_TRANSLATOR_INT2STRING__
#include <sstream>
std::string Num2Str(int i){
	std::ostringstream leter;
	leter << i;
	return leter.str();
}
#endif
