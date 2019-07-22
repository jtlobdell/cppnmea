#include <iostream>
#include <iomanip>
#include <vector>
#include <string>
#include <complex>
#include <fstream>
#include <boost/algorithm/string.hpp>
#include <queue>
#include <map>

#include "nmea.hpp"
#include "parse.hpp"

namespace {

std::map<nmea::direction_t, std::string> direction_to_string = {
    {nmea::direction_t::north, "north"},
    {nmea::direction_t::south, "south"},
    {nmea::direction_t::east, "east"},
    {nmea::direction_t::west, "west"}
};

std::map<nmea::fix_quality_t, std::string> fix_quality_to_string = {
    {nmea::fix_quality_t::invalid, "invalid fix"},
    {nmea::fix_quality_t::gps_fix, "gps fix"},
    {nmea::fix_quality_t::dgps_fix, "dgps fix"},
    {nmea::fix_quality_t::pps_fix, "pps fix"},
    {nmea::fix_quality_t::real_time_kinematic, "real time kinematic"},
    {nmea::fix_quality_t::float_rtk, "float real time kinematic"},
    {nmea::fix_quality_t::dead_reckoning, "dead reckoning"},
    {nmea::fix_quality_t::manual_input_mode, "manual input mode"},
    {nmea::fix_quality_t::simulation_mode, "simulation mode"}
};

std::map<nmea::data_status_t, std::string> data_status_to_string = {
    {nmea::data_status_t::active, "active"},
    {nmea::data_status_t::invalid, "void"}
};

std::map<nmea::fix_mode_t, std::string> fix_mode_to_string = {
    {nmea::fix_mode_t::autonomous, "autonomous"},
    {nmea::fix_mode_t::differential, "differential"},
    {nmea::fix_mode_t::estimated, "estimated"},
    {nmea::fix_mode_t::manual, "manual"},
    {nmea::fix_mode_t::invalid, "invalid"}
};

std::map<nmea::gsa_mode_t, std::string> gsa_mode_to_string = {
    {nmea::gsa_mode_t::manual, "manual"},
    {nmea::gsa_mode_t::automatic, "automatic"}
};

std::map<nmea::gsa_fix_type_t, std::string> gsa_fix_type_to_string = {
    {nmea::gsa_fix_type_t::unavailable, "unavailable"},
    {nmea::gsa_fix_type_t::_2d, "2d"},
    {nmea::gsa_fix_type_t::_3d, "3d"}
};

void print_line()
{
    std::cout << "-------------------------------------------" << std::endl;
}

void print_position_2d(const nmea::position_2d_t& pos2d)
{
    auto& lat = pos2d.latitude;
    auto& lon = pos2d.longitude;
            
    std::cout << "latitude: " << lat.degrees << " degrees, " << lat.minutes << " minutes, " <<
        direction_to_string[lat.dir] <<
        std::endl;

    std::cout << "longitude: " << lon.degrees << " degrees, " << lon.minutes << " minutes, " <<
        direction_to_string[lon.dir] <<
        std::endl;
}

void print_time(const nmea::utc_time_t& time)
{
    std::cout << "time (utc): " <<
        time.hours << ":" << time.minutes << ":" << time.seconds <<
        std::endl;
}

void print_date(const nmea::ut_date_t& date)
{
    std::cout << "day: " << date.dd << ", month: " << date.mm << ", year: " << date.yy << std::endl;
}

template <typename T>
void print_optional(const boost::optional<T>& opt)
{
    if (opt == boost::none) {
        std::cout << "null";
    } else {
        std::cout << *opt;
    }    
}

void print_checksum(unsigned int checksum)
{
    std::cout << "checksum: " << checksum <<
        " (0x" << std::hex << checksum << ")" << std::dec <<
        std::endl;
}

void print_gpgga(const nmea::gpgga& gga)
{
    std::cout << "$GPGGA" << std::endl;
    
    std::cout << fix_quality_to_string[gga.fix_quality] << std::endl;
    print_time(gga.time);
    print_position_2d(gga.pos_2d);

    std::cout << "num sats tracked: " << gga.sats_tracked << std::endl;
    std::cout << "horizontal degree of precision: " << gga.hdop << std::endl;
    std::cout << "altitude (MSL): " << gga.msl_altitude << std::endl;
    std::cout << "geoid separation (M): " << gga.geoid_separation << std::endl;

    std::cout << "time since last dgps update: ";
    print_optional(gga.time_since_dgps_update);
    std::cout << std::endl;

    std::cout << "station id: ";
    print_optional(gga.dgps_station_id);
    std::cout << std::endl;

    print_checksum(gga.checksum);

    print_line();
}

void print_gpgll(const nmea::gpgll& gll)
{
    std::cout << "$GPGLL" << std::endl;
    
    print_position_2d(gll.pos_2d);
    print_time(gll.time);
    std::cout << "data status: " << data_status_to_string[gll.data_status] << std::endl;
    std::cout << "fix mode: " << fix_mode_to_string[gll.fix_mode] << std::endl;
    print_checksum(gll.checksum);

    print_line();
}

void print_gpgsa(const nmea::gpgsa& gsa)
{
    std::cout << "$GPGSA" << std::endl;
    std::cout << "gsa mode: " << gsa_mode_to_string[gsa.gsa_mode] << std::endl;
    std::cout << "gsa fix type: " << gsa_fix_type_to_string[gsa.gsa_fix_type] << std::endl;
    std::cout << "satellites (size: " << gsa.satellites.size() << "):" << std::endl;

    std::cout << "\t{";
    for (std::size_t i = 0; i < gsa.satellites.size(); ++i) {
        std::cout << gsa.satellites[i];
        if (i+1 < gsa.satellites.size()) std::cout << ",";
    }
    std::cout << "}" << std::endl;

    std::cout << "dilution of precision: " << gsa.dilution_of_precision << std::endl;
    std::cout << "horizontal dilution of precision: " << gsa.horizontal_dilution_of_precision <<
        std::endl;
    std::cout << "vertical dilution of precision: " << gsa.vertical_dilution_of_precision <<
        std::endl;
    print_checksum(gsa.checksum);

    print_line();
}

void print_gpgsv(const nmea::gpgsv& gsv)
{
    std::cout << "$GPGSV" << std::endl;

    std::cout << "message " << gsv.message_number << " of " << gsv.number_of_messages << std::endl;
    std::cout << "satellites in view: " << gsv.satellites_in_view << std::endl;
    
    std::cout << "satellites (size: " << gsv.gpgsv_entries.size() << "): {" << std::endl;
    for (auto& entry: gsv.gpgsv_entries) {
        std::cout << "\t{id: " << entry.satellite_id_number << ", " <<
            "elevation: " << entry.elevation << ", " <<
            "azimuth: " << entry.azimuth << ", " <<
            "snr: ";
        print_optional(entry.signal_noise_ratio);
        std::cout << "}" << std::endl;
    }
    std::cout << "}" << std::endl;
    
    print_checksum(gsv.checksum);
    print_line();
}

void print_gprmc(const nmea::gprmc& rmc)
{
    std::cout << "$GPRMC" << std::endl;

    print_time(rmc.time);
    std::cout << "data status: " << data_status_to_string[rmc.data_status] << std::endl;
    print_position_2d(rmc.pos_2d);
    std::cout << "speed over ground: " << rmc.speed_over_ground << std::endl;
    std::cout << "course over ground: " << rmc.course_over_ground << std::endl;
    print_date(rmc.date);
    
    std::cout << "magnetic variation: ";
    print_optional(rmc.magnetic_variation);
    std::cout << " degrees";
    std::cout << ", direction: ";
    if (rmc.magnetic_variation_dir == boost::none) {
        std::cout << "null";
    } else {
        if (rmc.magnetic_variation_dir == nmea::magnetic_variation_direction_t::east) {
            std::cout << "east";
        } else {
            std::cout << "west";
        }
    }
    std::cout << std::endl;
    
    std::cout << "fix mode: " << fix_mode_to_string[rmc.fix_mode] << std::endl;
    print_checksum(rmc.checksum);

    print_line();
}

void print_gpvtg(const nmea::gpvtg& vtg)
{
    std::cout << "$GPVTG" << std::endl;

    std::cout << "course over ground true: ";
    print_optional(vtg.course_over_ground_true);
    std::cout << std::endl;
    std::cout << "course over ground magnetic: ";
    print_optional(vtg.course_over_ground_magnetic);
    std::cout << std::endl;
    std::cout << "ground speed knots: " << vtg.ground_speed_knots << std::endl;
    std::cout << "ground speed kmph: " << vtg.ground_speed_kmph << std::endl;
    std::cout << "fix mode: " << fix_mode_to_string[vtg.fix_mode] << std::endl;
    print_checksum(vtg.checksum);

    print_line();
}

}; // anonymous namespace

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

    std::queue<nmea::nmea_sentence> sentences;

    // repeat parsing all the samples num_repeats time (for benchmarking)
    for (unsigned long i = 0; i < num_repeats; ++i) {

        unsigned long successful_parses = 0;
        unsigned long failed_parses = 0;
        
        // parse each sample. report failures.
        for (std::string& sample: samples) {
            nmea::nmea_sentence sentence;
            std::string::const_iterator iter = sample.begin();
            std::string::const_iterator end = sample.end();
            bool parsed = parse(iter, end, p, sentence);
            bool at_end = (iter == end);

            if (!(parsed && at_end)) {
                std::cerr << "failure: " << sample << std::endl;
                failed_parses++;
            } else {
                successful_parses++;
                sentences.push(sentence);
            }

            win &= parsed;
        }
    }

    if (win) std::cout << "very nice!" << std::endl;
    else std::cout << "wah woo whoa!" << std::endl;

    // all done parsing, now do something with it
    print_line();
    while (sentences.size() > 0) {
        nmea::nmea_sentence sentence = sentences.front();
        sentences.pop();

        if (sentence.type() == typeid(nmea::gpgga)) {
            nmea::gpgga gga = boost::get<nmea::gpgga>(sentence);
            //print_gpgga(gga);
        } else if (sentence.type() == typeid(nmea::gpgll)) {
            nmea::gpgll gll = boost::get<nmea::gpgll>(sentence);
            //print_gpgll(gll);
        } else if (sentence.type() == typeid(nmea::gpgsa)) {
            nmea::gpgsa gsa = boost::get<nmea::gpgsa>(sentence);
            //print_gpgsa(gsa);
        } else if (sentence.type() == typeid(nmea::gpgsv)) {
            nmea::gpgsv gsv = boost::get<nmea::gpgsv>(sentence);
            //print_gpgsv(gsv);
        } else if (sentence.type() == typeid(nmea::gprmc)) {
            nmea::gprmc rmc = boost::get<nmea::gprmc>(sentence);
            //print_gprmc(rmc);
        } else if (sentence.type() == typeid(nmea::gpvtg)) {
            nmea::gpvtg vtg = boost::get<nmea::gpvtg>(sentence);
            //print_gpvtg(vtg);
        } else {
            std::cout << "whomst is this message?" << std::endl;
        }
    }
    
    return 0;
}
