/**********
This library is free software; you can redistribute it and/or modify it under
the terms of the GNU Lesser General Public License as published by the
Free Software Foundation; either version 3 of the License, or (at your
option) any later version. (See <http://www.gnu.org/copyleft/lesser.html>.)

This library is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for
more details.

You should have received a copy of the GNU Lesser General Public License
along with this library; if not, write to the Free Software Foundation, Inc.,
51 Franklin Street, Fifth Floor, Boston, MA 02110-1301  USA
**********/
// Copyright (c) 2020 Andrew Ross. All rights reserved.
// zmodo camera interaction - test binary (not installed)

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

