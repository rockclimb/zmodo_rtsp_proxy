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
// "based on code from liveMedia"
// Copyright (c) 1996-2020 Live Networks, Inc.  All rights reserved.
// Copyright (c) 2020 Andrew Ross. All rights reserved.

#include "ZmodoSource.hh"
#include <InputFile.hh>
#include <iostream>
////////// ZmodoSource //////////

#define CONNECTION_CHECK_TIME 1000*1000*60

ZmodoSource*
ZmodoSource::createNew(UsageEnvironment& env, const std::string& ip, uint16_t port) {
  ZmodoSource* newSource
    = new ZmodoSource(env, ip, port);
  return newSource;
}

ZmodoSource::ZmodoSource(UsageEnvironment& env, const std::string& ip, uint16_t port)
  : FramedSource(env), fHaveStartedReading(False), framecount(0), connectionTestTask(NULL) {
  std::cerr << "Opening file: " << ip << std::endl;
  
  zmodo = new Zmodo(ip,port);
  //makeSocketNonBlocking(fileno(fFid));
}

ZmodoSource::~ZmodoSource() {
	std::cerr << "Shutting down..." << std::endl;
    envir().taskScheduler().unscheduleDelayedTask(connectionTestTask);
	zmodo->disconnect();
	delete zmodo;
}

void ZmodoSource::doGetNextFrame() {
  /*if (feof(fFid) || ferror(fFid) ) {
    handleClosure();
    return;
  }*/

  if (!fHaveStartedReading) {
    // Await readable data from the file:
	try {
		zmodo->connect();
	} catch (ZmodoException& e) {
		handleClosure();
		return;
	}
    connectionTestTask = envir().taskScheduler().scheduleDelayedTask(CONNECTION_CHECK_TIME, (TaskFunc*)&checkConnectionHandler, this);
    fHaveStartedReading = True;
  }
  
  doReadFromCam();
}

void ZmodoSource::checkConnectionHandler(ZmodoSource* source) {
	source->checkConnection();
}

void ZmodoSource::checkConnection() {
#ifdef DEBUG
	std::cerr << "Checking connection..." << std::endl;
#endif
	if( (zmodo->testConnection() == false) || (framecount == 0) ) {
		std::cerr << "  Connection failed!" << std::endl;
		this->doStopGettingFrames();
		handleClosure();
		return;
	}
	framecount = 0;
	connectionTestTask = envir().taskScheduler().scheduleDelayedTask(CONNECTION_CHECK_TIME, (TaskFunc*)&checkConnectionHandler, this);
}

void ZmodoSource::doStopGettingFrames() {
	std::cerr << "Stopping..." << std::endl;
	envir().taskScheduler().unscheduleDelayedTask(connectionTestTask);
	zmodo->disconnect();
	fHaveStartedReading = False;
}

void ZmodoSource::camReadableHandler(ZmodoSource* source, int /*mask*/) {
  source->doReadFromCam();
}

void ZmodoSource::doReadFromCam() {
  if (!isCurrentlyAwaitingData()) return; // we're not ready for the data yet

#ifdef DEBUG
  std::cerr << "max: " << fMaxSize << std::endl;
#endif

  // Try to read as many bytes as will fit in the buffer provided
  fFrameSize = zmodo->read_frame(fTo, fMaxSize);
  if (fFrameSize == 0) {
    handleClosure();
    return;
  }
  // ?needed?
  fNumTruncatedBytes = zmodo->get_remaining();
  
#ifdef DEBUG
  std::cerr << "trunc: " << fNumTruncatedBytes << std::endl;
#endif
  
  gettimeofday(&fPresentationTime, NULL);
  framecount++;


  // Because the read was done from the event loop, we can call the
  // 'after getting' function directly, without risk of infinite recursion:
  //afterGetting(this);
  nextTask() = envir().taskScheduler().scheduleDelayedTask(0,
                              (TaskFunc*)FramedSource::afterGetting, this);
}
