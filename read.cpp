
#include "zmodo.hpp"

#include <iostream>
#include <iomanip>

#define BUF_SIZE 65536


int main(int argc, char* argv[]) {
    if (argc < 2) {
        // Tell the user how to run the program
        std::cerr << "Usage: " << argv[0] << " ip" << std::endl;
        return -1;
    }
    char* ip_str = argv[1];

	std::cerr << "Starting..." << std::endl;
	uint8_t buffer[BUF_SIZE];

	Zmodo zmodo(ip_str,8000);
	zmodo.connect();

	for (int i=0;i<100;i++) {

		try {
			// read a frame (auto-connects)
			ssize_t fl = zmodo.read_frame(buffer,sizeof(buffer));
			//std::cerr << "Frame length: " << fl << std::endl;
			// output the frame
			fwrite(buffer, 1, fl,stdout);
		} catch (ZmodoException&e) {
			std::cout << e.what() << std::endl;
		}
	}
	
}

