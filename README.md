# ZModo RTSP Proxy

This project connects to your ZModo camera running their proprietary 
protocol and exposes the stream via RTSP. It relies on the live555 
project for H264 and RTSP support.

## Prerequisites
* live555
* cmake

## Installing

Standard cmake build/install process:

```
mkdir build
cd build
cmake ..
make
make install
```

## Running

ZmodoH264VideoStreamer sets up a single stream, always connected to the 
camera:
```
ZmodoH264VideoStreamer ip-address
```

ZmodoOnDemandRTSPServer starts an RTSP server which connects to the 
camera when needed:
```
ZmodoOnDemandRTSPServer ip-address
```

zmodo_set_time is used to set the date/time of your camera in case you 
are preventing it from accessing the internet.
```
zmodo_set_time ip-address
```

## Authors

* Andrew Ross

## License

This project is licensed under the GNU Lesser General Public License - 
see the LICENSE file for details.

## Acknowledgments

* live555 for the library
