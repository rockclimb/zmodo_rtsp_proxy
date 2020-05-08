#ifndef ZMODO_HPP
#define ZMODO_HPP

#include <exception>
#include <string>
#include <iostream>
#include <iomanip>
#include <sys/socket.h> 
#include <netinet/ip.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <errno.h>
#include <cstring>
#include <unistd.h>
#include <algorithm>

struct ZmodoException : public std::exception {
   const char * what () const throw () {
      return "Zmodo Exception";
   }
};

class Zmodo {
public:
	static const int32_t zmodo_header_size = 16;
	static const int32_t frame_header_size = 32;

	Zmodo(const std::string& ip, uint16_t port, bool autoconnect = false) 
	  : remaining(0), connected(false), mAutoconnect(autoconnect) {
		// assign IP, PORT 
		camaddr.sin_family = AF_INET; 
		camaddr.sin_addr.s_addr = inet_addr(ip.c_str());
		camaddr.sin_port = htons(port);
	}
	
	~Zmodo() {
		this->disconnect();
	}

	void create_socket() {
		sockfd = socket(AF_INET, SOCK_STREAM, 0);
		if (sockfd == -1) { 
			std::cerr << "socket creation failed..." << std::endl; 
			std::cerr << "  " << errno << ":" << strerror(errno) << std::endl;
			exit(0); 
		}
		else {
			std::cerr << "Socket successfully created.." << std::endl; 
		}
	}
	
	void set_timeout() {	
		struct timeval tv;
		tv.tv_sec = 5;
		tv.tv_usec = 0;
		setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv, sizeof tv);
	}

	void connect() {
		if (connected) {
			return;
		}
		static const uint8_t zmodo_req[] = 
			{ 0x55,0x55,0xaa,0xaa,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x50 };

		create_socket();
		set_timeout();

		if (::connect(sockfd, (struct sockaddr *)&camaddr, sizeof(camaddr)) != 0) { 
			std::cerr << "connection with the camera failed..." << std::endl;
			std::cerr << "  " << errno << ":" << strerror(errno) << std::endl;
			throw ZmodoException();
		}
		else {
			std::cerr << "connected to the camera.." << std::endl; 
		}

		// send the initial request and read ZModo header from response
		this->write(zmodo_req, sizeof(zmodo_req));
	
		// read the response (zmodo_header_size bytes)
		ssize_t read = this->read(buf,zmodo_header_size);
		if (read < zmodo_header_size) {
			throw ZmodoException();
		}

#ifdef DEBUG
		std::cerr << "Got intial connection response:" << std::endl; 
		std::cerr << "  " << std::hex << std::setfill('0')
					<< " " << std::setw(8) << *((uint32_t*)(buf))
					<< " " << std::setw(8) << *((uint32_t*)(buf+4))
					<< " " << std::setw(8) << *((uint32_t*)(buf+8))
					<< " " << std::setw(8) << *((uint32_t*)(buf+12)) << std::dec << std::endl;
#endif

		remaining = 0;
		connected = true;
	}

	void disconnect() {
		::close(sockfd);
		remaining = 0;
		connected = false;
	}

	void reconnect() {
		std::cerr << "reconnecting..." << std::endl;
		disconnect();
		connect();
	}

	// TODO - failure modes? -1 = error, 0=EOF
	// errno: 
	ssize_t read(uint8_t*buf, ssize_t n) {
		ssize_t k=0;
		for (k=0; k < n;) {
			ssize_t r = ::read(sockfd, buf+k, n-k);
			if (r < 0) {
				switch(errno) {
				case EAGAIN:
					return k;
				default:
					std::cerr << "read failed..." << r << std::endl; 
					std::cerr << "  " << errno << ":" << strerror(errno) << std::endl;
					exit(0);
				}
			}
			k+=r;
		}
		return k;
	}

	// TODO: check return value
	ssize_t write(const uint8_t*buffer, ssize_t n) {
		return ::write(sockfd, buffer, n);
	}
	
	ssize_t read_frame(uint8_t*buffer, ssize_t n) {
		if (connected == false) {
			if (mAutoconnect == true) {
				this->connect();
			} else {
				return 0;
			}
		}
		
		if (remaining == 0) {
			// need to read another header
			ssize_t read = this->read(buf,frame_header_size);
			if (read < frame_header_size) {
				if (mAutoconnect == true) {
					this->reconnect();
				} else {
					this->disconnect();
				}
				return 0;
			}

#ifdef DEBUG
			std::cerr << "Frame header: " << std::endl;
				std::cerr << "  " << std::hex << std::setfill('0')
					<< " " << std::setw(8) << *((uint32_t*)(buf))
					<< " " << std::setw(8) << *((uint32_t*)(buf+4))
					<< " " << std::setw(8) << *((uint32_t*)(buf+8))
					<< " " << std::setw(8) << *((uint32_t*)(buf+12))
					<< " " << std::setw(8) << *((uint32_t*)(buf+16))
					<< " " << std::setw(8) << *((uint32_t*)(buf+20))
					<< " " << std::setw(8) << *((uint32_t*)(buf+24))
					<< " " << std::setw(8) << *((uint32_t*)(buf+28)) << std::dec << std::endl;
#endif
			
			remaining = *((uint32_t*)(buf+4));
		}
		
		// recv frame - TODO: check return
		ssize_t ret = this->read(buffer,std::min(remaining,n));

#ifdef DEBUG
		std::cerr << "read: " << ret << " of " << remaining << ", buffer size " << n << std::endl;
#endif
		if (ret < 1) {
			// end of file or error - disconnect
			this->disconnect();
			return 0;
		}

		remaining -= ret;
		return ret;
	}

	ssize_t get_remaining() {
		return remaining;
	}
	
	int get_socket() {
		return sockfd;
	}
	
	bool testConnection() {
		// is the connection OK?
		// try sending a byte
		int error = 0;
		socklen_t len = sizeof (error);
		int retval = getsockopt (sockfd, SOL_SOCKET, SO_ERROR, &error, &len);
		if ( (retval != 0) || (error != 0) ) {
			return false;
		}
		return true;
	}
	
private:
	struct sockaddr_in camaddr; 
	int sockfd;
	ssize_t remaining;
	uint8_t buf[frame_header_size];
	bool connected;
	bool mAutoconnect;
};



#endif
