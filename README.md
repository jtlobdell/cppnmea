*cppnmea* implements a Boost.Spirit-based parser for NMEA sentences. It also defines NMEA types and associates them with user-defined callback functions. The result is a very fast and easy to use parser.

## Requirements:
* Boost Spirit
* Modern compiler with C++17 support

## Quick Intro

### Include
```
#include <cppnmea.hpp>
```

### Create a parser
```
nmea::Parser np;
```

### Associate a parsed type with a callback function
```
np.setCallback<nmea::gpgga>(
	[](const nmea::gpgga& gga) {
	    // do stuff
		std::cout << "sats tracked: " << gga.sats_tracked << std::endl;
	}
);
```

### Handle failed parses

```
np.setFailureCallback(
	[](std::string_view str) {
		// do stuff
		std::cerr << "failed to parse sentence: " << str << std::endl;
	}
);
```

### Parse a sentence

```
std::string s = "...";
np.parse(s);
```

### Full example

See [example/nmea_parse.cpp](example/nmea_parse.cpp) for a complete example.

The example reads samples.txt (not provided) line-by-line for NMEA sentences then parses them. Data from $GPGGA sentences (`nmea::gpgga`) are placed into a queue. After all the samples are processed, the queue is emptied, with each `nmea::gpgga` is given to a printing function. The time it takes to parse is also calculated and reported. The example also contains mostly unused functions for printing or converting the parsed types to strings.

## Types

For specific details on parsed data types, see [include/cppnmea/types.hpp](include/cppnmea/types.hpp).


## Benchmarks

### Raspberry Pi Zero W (as of commit 39506ac)

#### Summary

About 15us per sample

#### CPU info

```
john@raspberrypi ~ $ cat /proc/cpuinfo
processor	: 0
model name	: ARMv6-compatible processor rev 7 (v6l)
BogoMIPS	: 697.95
Features	: half thumb fastmult vfp edsp java tls 
CPU implementer	: 0x41
CPU architecture: 7
CPU variant	: 0x0
CPU part	: 0xb76
CPU revision	: 7

Hardware	: BCM2835
Revision	: 9000c1
Serial		: 00000000bc5a3643
```

#### Single instance

```
john@raspberrypi ~ $ ./nmea_parse_O2.rpi 100
sample size: 1116
will parse 111600 sentences
-------------------------------------------
time taken (microseconds): 1518618
time taken (seconds): 1.51862
microseconds per sample: 13.6077
```

#### Average of 100 runs

```
john@raspberrypi ~ $ for n in `seq 100`; do ./nmea_parse_O2.rpi 100 | grep "per sample" | cut -f 4 -d ' '; done | python -c "import sys; nums = [float(n) for n in sys.stdin]; print(sum(nums)/len(nums))"
14.982175
```
