nmea_parse.o:
	arm-linux-gnueabihf-g++ -o nmea_parse -fPIC --std=c++17 -I/usr/arm-linux-gnueabihf/include/ nmea_parse.cpp
	arm-linux-gnueabihf-g++ -o nmea_parse_O3 -O3 -fPIC -std=c++17 -I/usr/arm-linux-gnueabihf/include/ nmea_parse.cpp
	arm-linux-gnueabihf-g++ -o nmea_parse_O2 -O2 -fPIC -std=c++17 -I/usr/arm-linux-gnueabihf/include/ nmea_parse.cpp
	arm-linux-gnueabihf-g++ -o nmea_parse_O1 -O1 -fPIC -std=c++17 -I/usr/arm-linux-gnueabihf/include/ nmea_parse.cpp
	arm-linux-gnueabihf-g++ -o nmea_parse_Os -Os -fPIC -std=c++17 -I/usr/arm-linux-gnueabihf/include/ nmea_parse.cpp
