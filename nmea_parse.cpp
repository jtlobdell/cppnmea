#include <iostream>
#include <vector>
#include <string>
#include <complex>
#include <fstream>
#include <boost/algorithm/string.hpp>
#include <queue>

#include "nmea.hpp"
#include "parse.hpp"

int main(int argc, char *argv[])
{
    unsigned long num_repeats = 1;
    if (argc == 2) {
        try {
            num_repeats = std::stoul(std::string(argv[1]));
        } catch (std::invalid_argument&) {
            std::cerr << "num_repeats must be an unsigned long" << std::endl;
            return 1;
        } catch (std::out_of_range&) {
            std::cerr << "num_repeats is out of range" << std::endl;
            return 1;
        }
    }
    
    std::ifstream in("samples.txt");
    std::vector<std::string> samples;
    std::string line;

    // store each line of samples.txt into samples vector
    while (std::getline(in, line)) {
        boost::trim_right(line);
        if (line.size() > 0) {
            samples.push_back(line);
        }
    }
    
    in.close();

    typedef std::string::const_iterator iterator_type;
    typedef nmea::parse::nmea_parser<iterator_type> nmea_parser;
    nmea_parser p;

    bool win = true;
    std::cout << "num samples: " << samples.size() << std::endl;
    std::cout << "total lines parsed: " << num_repeats * samples.size() << std::endl;

    std::queue<nmea::nmea_message> messages;

    // repeat parsing all the samples num_repeats time (for benchmarking)
    for (unsigned long i = 0; i < num_repeats; ++i) {

        unsigned long successful_parses = 0;
        unsigned long failed_parses = 0;
        
        // parse each sample. report failures.
        for (std::string& sample: samples) {
            nmea::nmea_message msg;
            std::string::const_iterator iter = sample.begin();
            std::string::const_iterator end = sample.end();
            bool parsed = parse(iter, end, p, msg);
            bool at_end = (iter == end);

            if (!(parsed && at_end)) {
                std::cerr << "failure: " << sample << std::endl;
                failed_parses++;
            } else {
                successful_parses++;
                messages.push(msg);
            }

            win &= parsed;
        }
    }

    if (win) std::cout << "very nice!" << std::endl;
    else std::cout << "wah woo whoa!" << std::endl;

    // all done parsing, now do something with it
    while (messages.size() > 0) {
        nmea::nmea_message msg = messages.front();
        messages.pop();

        if (msg.type() == typeid(nmea::gpgga)) {
            nmea::gpgga gga = boost::get<nmea::gpgga>(msg);

            switch (gga.fix_quality) {
                case nmea::fix_quality_t::invalid:
                    std::cout << "invalid fix" << std::endl;
                    break;
                case nmea::fix_quality_t::gps_fix:
                    std::cout << "gps fix" << std::endl;
                    break;
                case nmea::fix_quality_t::dgps_fix:
                    std::cout << "dgps fix" << std::endl;
                    break;
                case nmea::fix_quality_t::pps_fix:
                    std::cout << "pps fix" << std::endl;
                    break;
                case nmea::fix_quality_t::real_time_kinematic:
                    std::cout << "real time kinematic" << std::endl;
                    break;
                case nmea::fix_quality_t::float_rtk:
                    std::cout << "float real time kinematic" << std::endl;
                    break;
                case nmea::fix_quality_t::dead_reckoning:
                    std::cout << "dead reckoning" << std::endl;
                    break;
                case nmea::fix_quality_t::manual_input_mode:
                    std::cout << "manual input mode" << std::endl;
                    break;
                case nmea::fix_quality_t::simulation_mode:
                    std::cout << "simulation mode" << std::endl;
                    break;
            }

            auto& time = gga.time;

            std::cout << "time (utc): " << time.hours << ":" << time.minutes << ":" << time.seconds << std::endl;
            
            auto& lat = gga.latitude;
            auto& lon = gga.longitude;
            
            std::cout << "latitude: " << lat.degrees << " degrees, " << lat.minutes << " minutes, ";
            if (lat.dir == nmea::latitude_direction_t::north) {
                std::cout << " north";
            } else if (lat.dir == nmea::latitude_direction_t::south) {
                std::cout << " south";
            }
            std::cout << std::endl;

            std::cout << "longitude: " << lon.degrees << " degrees, " << lon.minutes << " minutes, ";
            if (lon.dir == nmea::longitude_direction_t::east) {
                std::cout << " east";
            } else if (lon.dir == nmea::longitude_direction_t::west) {
                std::cout << " west";
            }
            std::cout << std::endl;

            std::cout << "num sats tracked: " << gga.sats_tracked << std::endl;
            std::cout << "horizontal degree of precision: " << gga.hdop << std::endl;
            std::cout << "altitude (MSL): " << gga.msl_altitude << std::endl;
            std::cout << "geoid separation (M): " << gga.geoid_separation << std::endl;

            std::cout << "time since last dgps update: ";
            if (gga.time_since_dgps_update == boost::none) {
                std::cout << "null";
            } else {
                std::cout << *gga.time_since_dgps_update;
            }
            std::cout << std::endl;

            std::cout << "station id: ";
            if (gga.dgps_station_id == boost::none) {
                std::cout << "null";
            } else {
                std::cout << *gga.dgps_station_id;
            }
            std::cout << std::endl;

            std::cout << "checksum: " << gga.checksum << std::endl;

        } else {
            continue;
        }

        std::cout << "-------------------------------------------" << std::endl;
    }
    
    return 0;
}
