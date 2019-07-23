C++ lib for parsing and handling NMEA messages. Very early development.

It uses Boost Spirit for parsing. Because of this, compiling is quite resource-intensive.
I tried building it on a Raspberry Pi Zero and ran out of memory. Oops! Now I'm using a cross compiler.

Despite that, I think it will end up being blazing fast.

Benchmarking on my Raspberry Pi Zero (as of commit 39506ac):

CPU info:

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

Single instance:

```
john@raspberrypi ~ $ ./nmea_parse_O2.rpi 100
sample size: 1116
will parse 111600 sentences
-------------------------------------------
time taken (microseconds): 1518618
time taken (seconds): 1.51862
microseconds per sample: 13.6077
```

Running the previous benchmark 100 times, and averaging the microseconds per sample:

```
john@raspberrypi ~ $ for n in `seq 100`; do ./nmea_parse_O2.rpi 100 | grep "per sample" | cut -f 4 -d ' '; done | python -c "import sys; nums = [float(n) for n in sys.stdin]; print(sum(nums)/len(nums))"
14.982175
```

To summarize:
- about 15us per sample
- no failed parses with my sample set
