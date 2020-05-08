#!/usr/bin/env python3

import socket
import sys
import struct
import time

def recvall(sock, n):
    # Helper function to recv n bytes or return None if EOF is hit
    data = bytearray()
    while len(data) < n:
        packet = sock.recv(n - len(data))
        if not packet:
            return None
        data.extend(packet)
    return data

def connect(ip):
    s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    s.settimeout(10)
    s.connect((ip, 8000))
    return s

def main():
	if len(sys.argv) < 2:
		sys.stderr.write("Usage: "+sys.argv[0]+" ip-address\n")
		exit(-1)
	ip = sys.argv[1]
	try:
		s = connect(ip)
		payload = struct.pack("<BBBB",0x01,0x00,0x2d,0x00)
		plen = len(payload)
		hdr = struct.pack("<IIHH",0xaaaa5555,plen,0x0000,0x9132)
		s.sendall(hdr+payload)
		s.close()

		s = connect(ip)
		#now = time.gmtime(time.time())
		now = time.localtime()
		payload = struct.pack("<BBBBBBBB",now.tm_year % 100, now.tm_mon,
			now.tm_mday, now.tm_hour, now.tm_min, now.tm_sec, 0x00, 0x00)
		plen = len(payload)
		hdr = struct.pack("<IIHH",0xaaaa5555,plen,0x0000,0xa600)
		s.sendall(hdr+payload)

	except Exception as e:
		pass
 
if __name__ == "__main__":
    main()
