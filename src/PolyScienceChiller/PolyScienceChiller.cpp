#include <iostream>
#include <fstream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <dirent.h>
#include <PolyScienceChiller.h>
//#include <unistd.h>

#ifndef __SUPER_TRANSLATOR_INT2STRING__
#define __SUPER_TRANSLATOR_INT2STRING__
#include <sstream>
std::string int2str(int i){
	std::ostringstream letter;
	letter << i;
	return letter.str();
}
#endif

#ifndef __SUPER_TRANSLATOR_FLOAT2STRING__
#define __SUPER_TRANSLATOR_FLOAT2STRING__
#include <sstream>
std::string float2str(float i){
        std::ostringstream letter;
	std::string result;
	//leter.setf(0,ios::floatfield);
	letter << i;
	if(i<1.0){
		letter.precision(2);
		result.assign("00");
		result.append(letter.str());
	}else if(i<10.0){
		letter.precision(3);
		//result.assign("00");
		result.assign("0");
		result.append(letter.str());
		
	}else if(i<100){
		letter.precision(4);
		//result.assign("0");
		result.append(letter.str());
	}else if(i<1000){
		letter.precision(5);
		result.assign(letter.str());
	}else{
		std::cerr << "\e[0;31mERROR:\e[0m index exceed the limit" << std::endl;
		return "";
	}
        return result;
}
#endif

PolyScienceChiller::PolyScienceChiller(const char* device){
	this->baud = 57600;
	this->endChar = '\r';
	this->Connect(device);
	this->debug = false;
	//this->Identify();
}


PolyScienceChiller::PolyScienceChiller(){
	this->baud = 57600;
	this->endChar = '\r';
	std::string device;
	device = this->GetPort();
	this->Connect(device.c_str());
	this->debug = false;
}

int PolyScienceChiller::Connect(){
	std::string device;
	return this->GetPort();
}

int PolyScienceChiller::Connect(const char* device){
	this->handler = open(device, O_RDWR | O_NOCTTY | O_NDELAY);
	if(this->handler==-1){
		std::cerr << "\e[0;31mERROR:\e[0m PolyScienceChiller::Connect: Could not establish connection with the Chiller in: " << device << std::endl;
		this->connected = false;
		exit(1);
	}
	if (tcgetattr(this->handler, &this->toptions) < 0) {
		std::cerr << "\e[0;31mERROR:\e[0m PolyScienceChiller::Connect: Couldn't get term attributes\n" ;
		this->connected = false;
		this->Close();
		return 1;
	}
	cfsetispeed(&this->toptions, B57600);
	cfsetospeed(&this->toptions, B57600);
	this->toptions.c_cflag &= ~PARENB;
	this->toptions.c_cflag &= ~CSTOPB;
	this->toptions.c_cflag &= ~CSIZE; //Mask the character size bits
	this->toptions.c_cflag |= CS8;	  //Select 8 Data Bits
	this->toptions.c_cflag &= ~CRTSCTS;
	this->toptions.c_cflag |= CREAD | CLOCAL;  // turn on READ & ignore ctrl lines
	this->toptions.c_iflag &= ~(IXON | IXOFF | IXANY); // turn off s/w flow ctrl
	this->toptions.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG); // make raw
	this->toptions.c_oflag &= ~OPOST; // make raw
	// see: http://unixwiz.net/techtips/termios-vmin-vtime.html
	this->toptions.c_cc[VMIN]  = 0;
	this->toptions.c_cc[VTIME] = 0;

	if( tcsetattr(handler, TCSANOW, &toptions) < 0) {
		std::cerr << "\e[0;31mERROR:\e[0m PolyScienceChiller::Connect: Couldn't set term attributes" << std::endl;
		this->connected = false;
		return 1;
	}
	if(tcflush(this->handler,TCIOFLUSH)){
		std::cerr << "\e[0;31mERROR:\e[0m PolyScienceChiller::Connect: impossible to flush the port" << std::endl;
		this->Close();
		return 1;
	}
	this->connected = true;
	return 0;
}

PolyScienceChiller::~PolyScienceChiller(){
	this->Close();
}

int PolyScienceChiller::Write(std::string data){
	int len,n;
	//std::string temp(&endChar);
	if(this->IsOpen()){
		data.append(&endChar);
		if(this->debug)
			std::cout << "Command: " << data << std::endl;
		len = data.length();
		if(tcflush(this->handler,TCIOFLUSH)){
			std::cerr << "\e[0;31mERROR:\e[0m PolyScienceChiller::Connect: Imposible to flush the port" << std::endl;
			return 1;
		}
		n = write(this->handler,data.c_str(),len);
		if(this->debug)
			std::cout << "Bits written: " << n << std::endl;
		if(tcdrain(this->handler)){
			std::cerr << "\e[0;31mERROR:\e[0m PolyScienceChiller::Write: The command cannot be accomplished" << std::endl;
			this->Close();
			return 1;
		}
		if(len != n){
			if(len>0 && n<1){
				std::cerr << "\e[0;31mERROR:\e[0m PolyScienceChiller::Write: The communication with Test Assembly is lost" << std::endl;
				this->connected = false;
			}
			this->Close();
			return 1;
		}
		return 0;
	}else{
		std::cerr << "\e[0;31mERROR:\e[0m PolyScienceChiller::Write: The communication is not established with Test Assembly" << std::endl;
		this->Close();
		return 1;
	}
}

int PolyScienceChiller::Read(std::string *data){
	char buffer;
	int j=0,n;
	if(this->IsOpen()){
		data->assign("");
		do{
			j++;
			n = read(this->handler,&buffer,1);
			if(this->debug)
				std::cout << "**" << buffer << "-" << (int)buffer << "-" << errno<< "*" << std::endl;
			if(n==-1 && errno != 11){
				std::cerr << "\e[0;31mERROR:\e[0m PolyScienceChiller::Read: The communication with Test Assembly is lost" << std::endl;
				this->connected = false;
				this->Close();
				return 1;
			}
			if(j>100){
				std::cerr << "\e[0;31mERROR:\e[0m PolyScienceChiller::Read: excede the number of intent. There are no response from the chiller." << std::endl;
				this->Close();
				return 1;
			}
			if(buffer != 8 && buffer != 10 && buffer != 13){
				data->append(1,buffer);
			}
			if(n==0 || errno == 11){
				usleep(100*1000); // wait 10 ms
				if(errno==11)
					usleep(100000);
				continue;
			}
		}while(buffer != this->endChar && buffer != 10);
		return 0;
	}else{
		std::cerr << "\e[0;31mERROR:\e[0m PolyScienceChiller::Read: The communication is not established with Test Assembly" << std::endl;
		this->Close();
		return 1;
	}
}

int PolyScienceChiller::Close(){
	if(this->IsOpen()){
		close(this->handler);
		return 0;
	}else{
		std::cerr << "\e[0;31mERROR:\e[0m PolyScienceChiller::Close: The communication is not established with Test Assembly" << std::endl;
		return 1;
	}
}

void PolyScienceChiller::SetSetpoint(float temperature){
	if(this->IsOpen()){
		std::string command,ack;
		command.assign("SS");
		command.append(float2str(temperature));
		std::string data, result, message;
		if(this->debug)
			std::cout << "PolyScienceChiller::SetSetpoint: Writing data: " << command << std::endl;
		if(this->Write(command)){
			std::cerr << "\e[0;31mERROR:\e[0m PolyScienceChiller::SetSetpoint: writing the data" << std::endl;
			exit(1);
		}
		sleep(5);
		if(this->debug)
			std::cout << "PolyScienceChiller::SetAddress: Reading ACK" << std::endl;
		if(this->Read(&result)){
			std::cerr << "\e[0;31mERROR:\e[0m PolyScienceChiller::SetSetpoint: reading the ACK" << std::endl;
			exit(1);
		}
		ack.assign("!");
		if(result.compare(ack)==0){
			std::cout << "PolyScienceChiller Controller: Temperature setted to: " << temperature << std::endl;
			return;
		}else{
			std::cerr << "\e[0;31mERROR:\e[0m PolyScienceChiller::SetSetpoint: wrong message: " << command << " - " << result << std::endl;
			exit(1);
		}
	}else{
		std::cerr << "\e[0;31mERROR:\e[0m PolyScienceChiller::SetSetpoint: The communication is not established with the Chiller" << std::endl;
		exit(1);
	}
	return;
}

void PolyScienceChiller::GetSetpoint(float *temperature){
	if(this->IsOpen()){
		std::string command;
		command.assign("RS");
		std::string data,result;
		if(this->debug)
			std::cout << "PolyScienceChiller::GetSetpoint: Writing data: " << command << std::endl;
		if(this->Write(command)){
			std::cerr << "\e[0;31mERROR:\e[0m PolyScienceChiller::GetSetpoint: writing the data" << std::endl;
			exit(1);
		}
		sleep(5);
		if(this->debug)
			std::cout << "PolyScienceChiller::GetAddress: Reading ACK" << std::endl;
		if(this->Read(&result)){
			std::cerr << "\e[0;31mERROR:\e[0m PolyScienceChiller::GetSetpoint: reading the ACK" << std::endl;
			exit(1);
		}
		sscanf(result.c_str(),"%f",temperature);
	}else{
		std::cerr << "\e[0;31mERROR:\e[0m PolyScienceChiller::GetSetpoint: The communication is not established with the Chiller" << std::endl;
		exit(1);
	}
	return;
}

void PolyScienceChiller::GetInternalTemperature(float *temperature){
        if(this->IsOpen()){
                std::string command;
                command.assign("RT");
                std::string data,result;
                if(this->debug)
                        std::cout << "PolyScienceChiller::GetInternalTemp: Writing data: " << command << std::endl;
                if(this->Write(command)){
                        std::cerr << "\e[0;31mERROR:\e[0m PolyScienceChiller::GetInternalTemp: writing the data" << std::endl;
                        exit(1);
                }
                sleep(5);
                if(this->debug)
                        std::cout << "PolyScienceChiller::GetInternalTemp: Reading ACK" << std::endl;
                if(this->Read(&result)){
                        std::cerr << "\e[0;31mERROR:\e[0m PolyScienceChiller::GetInternalTemp: reading the ACK" << std::endl;
                        exit(1);
                }
		sscanf(result.c_str(),"%f",temperature);
        }else{
                std::cerr <<"\e[0;31mERROR:\e[0m PolyScienceChiller::GetInternalTemp: The communication is not established with the Chiller" << std::endl;
                exit(1);
        }
        return;
}

void PolyScienceChiller::SetOnOff(bool onoff){
	if(this->IsOpen()){
		std::string command,result,ack;
		command = "SO";
		if(onoff)
			command.append("1");
		else
			command.append("0");
		if(this->Write(command)){
			std::cerr << "\e[0;31mERROR:\e[0m PolyScienceChiller::OnOff: writing data" << std::endl;
			exit(1);
		}
		if(this->Read(&result)){
			std::cerr << "\e[0;31mERROR:\e[0m PolyScienceChiller::OnOff: reading the ACK" << std::endl;
			exit(1);
		}
		ack.assign("!");
		if(result.substr(1,1).compare(ack)==0){
			return;
		}else if(result.substr(1,1).compare("?")==0){
			std::cerr << "\e[0;31mERROR:\e[0m PolyScienceChiller::OnOff: chiller aready in the state: " << result << std::endl;
			exit(1);
		}else{
			std::cerr << "\e[0;31mERROR:\e[0m PolyScienceChiller::OnOff: wrong ACK: " << result << std::endl;
			exit(1);
		}
	}else{
		std::cerr << "\e[0;31mERROR:\e[0m PolyScienceChiller::OnOff: The communication is not established with the Chiller" << std::endl;
		exit(1);
	}
}

void PolyScienceChiller::SetPumpSpeed(int speed){
	if(speed>70 || speed<0){
		std::cerr << "\e[0;31mERROR:\e[0m PolyScienceChiller::SetPumpSpeed: speed exceed the limits" << std::endl;
		return;
	}
	if(this->IsOpen()){
		std::string command,result;
		command = "SM";
		command.append(int2str(speed));
		if(this->Write(command)){
			std::cerr << "\e[0;31mERROR:\e[0m PolyScienceChiller::SetPumpSpeeds: writing data" << std::endl;
			exit(1);
		}
		if(this->Read(&result)){
			std::cerr << "\e[0;31mERROR:\e[0m PolyScienceChiller::SetPumpSpeeds: reading the ACK" << std::endl;
			exit(1);
		}
		if(result.compare("!\n")==0){
			return;
		}else if(result.compare("?\n")){
			std::cerr << "\e[0;31mERROR:\e[0m PolyScienceChiller::SetPumpSpeeds: something wrong in command: " << command << std::endl;
			exit(1);
		}else{
			std::cerr << "\e[0;31mERROR:\e[0m PolyScienceChiller::SetPumpSpeeds: wrong ACK" << std::endl;
			exit(1);
		}
	}else{
		std::cerr << "\e[0;31mERROR:\e[0m PolyScienceChiller::SetPumpSpeeds: The communication is not established with the Chiller" << std::endl;
		exit(1);
	}
	return;
}

void PolyScienceChiller::GetPumpSpeed(int *speed){
	if(this->IsOpen()){
		std::string command,result;
		command = "RM";
		if(this->Write(command)){
			std::cerr << "\e[0;31mERROR:\e[0m PolyScienceChiller::GetPumpSpeeds: writing data" << std::endl;
			exit(1);
		}
		if(this->Read(&result)){
			std::cerr << "\e[0;31mERROR:\e[0m PolyScienceChiller::SetPumpSpeeds: reading the ACK" << std::endl;
			exit(1);
		}
		*speed = atoi(result.c_str());
		return;
	}else{
		std::cerr << "\e[0;31mERROR:\e[0m PolyScienceChiller::SetPumpSpeeds: The communication is not established with the Chiller" << std::endl;
		exit(1);
	}
	return;
}

int PolyScienceChiller::GetPort(){
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
				std::cerr <<"\e[0;31mERROR:\e[0m No device found in " << deviceList[i] << std::endl;
				continue;
			}
		}while(message.compare(this->Identify().substr(0,23)) != 0 && i<deveviceNumber);
		if(i==deveviceNumber){
			std::cerr <<"\e[0;31mERROR:\e[0m No device found connected" << std::endl;
			return 1;
		}
		if(IsOpen()){
			return 0;
		}
		
	}else{
		std::cerr << "\e[0;31mERROR:\e[0m PolyScienceChiller::GetPort: opendir" << std::endl;
		return 1;
	}
	return 1;
}

bool PolyScienceChiller::IsOpen(){
	return this->connected;
}

void PolyScienceChiller::Debug(bool deb){
	this->debug = deb;
}

std::string PolyScienceChiller::Identify(){
	if(this->IsOpen()){
		std::string command,result,message;
		command.assign("RB");
		if(this->Write(command)){
			std::cerr <<"\e[0;31mERROR:\e[0m PolyScienceChiller::Identify: writing data" << std::endl;
			return "";
		}
		if(this->Read(&result)){
			std::cerr <<"\e[0;31mERROR:\e[0m PolyScienceChiller::Identify: reading the ACK" << std::endl;
			return "";
		}
		return std::string("Chiller PolyScience 9102A, Firmware version: "+result);
	}else{
		std::cerr <<"\e[0;31mERROR:\e[0m PolyScienceChiller::Identify: The communication is not established with the Chiller" << std::endl;
		return "";
	}
	return "";
}

