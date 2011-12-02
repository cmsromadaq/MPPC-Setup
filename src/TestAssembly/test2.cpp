#include <iostream>
#include <TestAssembly.h>
#include <termios.h>
#include <fcntl.h>

int main(){
	int arduino;
	struct termios toptions;
        std::cout << "Starting..." << std::endl;
	arduino = open("/dev/ttyUSB0", O_RDWR | O_NOCTTY | O_NDELAY);
        if(arduino==-1){
                std::cout << "ERROR: TestAssembly::Connect: Could not establish connection with Test Assembly" << std::endl;
                return 1;
        }
        if (tcgetattr(arduino, &toptions) < 0) {
                std::cout << "ERROR: TestAssembly::Connect: Couldn't get term attributes" << std::endl;
                return 1;
        }
        cfsetispeed(&toptions, B9600);
        cfsetospeed(&toptions, B9600);

        // 8N1
        toptions.c_cflag &= ~PARENB;
        toptions.c_cflag &= ~CSTOPB;
        toptions.c_cflag &= ~CSIZE;
        toptions.c_cflag |= CS8;
        // no flow control
        toptions.c_cflag &= ~CRTSCTS;
        toptions.c_cflag |= CREAD | CLOCAL;  // turn on READ & ignore ctrl lines
        toptions.c_iflag &= ~(IXON | IXOFF | IXANY); // turn off s/w flow ctrl
        toptions.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG); // make raw
        toptions.c_oflag &= ~OPOST; // make raw

        // see: http://unixwiz.net/techtips/termios-vmin-vtime.html
        toptions.c_cc[VMIN]  = 0;
        //toptions.c_cc[VTIME] = 20;
        toptions.c_cc[VTIME] = 0;

        if( tcsetattr(arduino, TCSANOW, &toptions) < 0) {
                perror("init_serialport: Couldn't set term attributes");
                return 1;
	}
	std::string data;
        data = ":SETADDR 1 2 3";
	int len,n;
	len = data.length();
	n = write(arduino,data.c_str(),len);
	if(len != n){
                        if(len>0 && n<1){
                                std::cout << "ERROR: TestAssembly::Write: The communication with Test Assembly is lost" << std::endl;
                        }
                        return 1;
	}
	char buffer;
        int i=0,j=0;
	std::cout << "TestAssembly::Read: reading ";
                do{
                        j++;
                        std::cout << ".";
                        //n = read(this->arduino,(void *)buffer,1);
                        n = read(arduino,&buffer,1);
                        std::cout << "-" << buffer << "-";
                        if(n==-1){
                                std::cout << std::endl << "ERROR: TestAssembly::Read: The communication with Test Assembly is lost" << std::endl;
                                return 1;
                        }
                        if(j>1000){
                                std::cout << std::endl << "ERROR: TestAssembly::Read: excede the number of intent. There are no response from the controller." << std::endl;
                                return 1;
                        }
                        if(n==0){
                                usleep(10*1000); // wait 10 ms
                                continue;
                        }
                        data = data + buffer;
                        i++;
                }while(buffer != '\0');
                data += '\0';
                std::cout << "*************** " << data << " ***********" << std::endl;
		close(arduino);
                return 0;
}
