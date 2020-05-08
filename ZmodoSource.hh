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
// "liveMedia"
// Copyright (c) 1996-2020 Live Networks, Inc.  All rights reserved.
// A file source that is a plain byte stream (rather than frames)
// C++ header

#ifndef _ZMODO_SOURCE_HH
#define _ZMODO_SOURCE_HH

#ifndef _FRAMED_SOURCE_HH
#include <FramedSource.hh>
#endif

#include "zmodo.hpp"

class ZmodoSource: public FramedSource {
public:
  static ZmodoSource* createNew(UsageEnvironment& env, const std::string& ip, uint16_t port);

protected:
  ZmodoSource(UsageEnvironment& env, const std::string& ip, uint16_t port);
	// called only by createNew()

  virtual ~ZmodoSource();

  static void camReadableHandler(ZmodoSource* source, int mask);
  void doReadFromCam();
  
  static void checkConnectionHandler(ZmodoSource* source) ;
  void checkConnection() ;

private:
  // redefined virtual functions:
  virtual void doGetNextFrame();
  virtual void doStopGettingFrames();

private:
  Zmodo * zmodo;
  Boolean fHaveStartedReading;
  int32_t framecount;
  TaskToken connectionTestTask;
};

#endif
