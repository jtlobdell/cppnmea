C++ lib for parsing and handling NMEA messages. Very early development.

It uses Boost Spirit for parsing. Because of this, compiling is quite resource-intensive.
I tried building it on a Raspberry Pi Zero and ran out of memory. Oops! Now I'm using a cross compiler.

Despite that, I think it will end up being blazing fast.

A benchmark on my Raspberry Pi Zero (as of commit 9a9f246):

```
john@raspberrypi ~ $ cat /proc/cpuinfo 
processor	: 0
model name	: ARMv6-compatible processor rev 7 (v6l)
BogoMIPS	: 997.08
Features	: half thumb fastmult vfp edsp java tls 
CPU implementer	: 0x41
CPU architecture: 7
CPU variant	: 0x0
CPU part	: 0xb76
CPU revision	: 7

Hardware	: BCM2835
Revision	: 9000c1
Serial		: 00000000bc5a3643
john@raspberrypi ~ $ time ./nmea_parse_O2.rpi 1000
num samples: 1116
total lines parsed: 1116000
very nice!
./nmea_parse_O2.rpi 1000  12.70s user 0.01s system 99% cpu 12.733 total

```

In this particular case it averaged 11.4us per sample.

---

The parser now succeeds in parsing all of my samples. This doesn't necessarily mean it is correct.

It does not yet do anything with the parsed data. (to-do)