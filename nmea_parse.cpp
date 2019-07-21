#include <boost/config/warning_disable.hpp>
#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/phoenix_core.hpp>
#include <boost/spirit/include/phoenix_operator.hpp>
#include <boost/spirit/include/phoenix_operator.hpp>
#include <boost/fusion/include/adapt_struct.hpp>
#include <boost/fusion/include/io.hpp>
#include <iostream>
#include <vector>
#include <string>
#include <complex>
#include <fstream>
#include <boost/algorithm/string.hpp>

namespace nmea::parse {

enum class latitude_direction_t {
    north,
    south
};

enum class longitude_direction_t {
    east,
    west
};

enum class fix_quality_t {
    invalid,
    gps_fix,
    dgps_fix,
    pps_fix,
    real_time_kinematic,
    float_rtk,
    dead_reckoning,
    manual_input_mode,
    simulation_mode
};

enum class position_system_mode_indicator_t {
    autonomous,
    differential,
    estimated,
    manual,
    invalid
};

enum class gsa_mode_t {
    manual,
    automatic
};

enum class gsa_fix_type_t {
    unavailable,
    _2d,
    _3d
};

enum class data_status_t {
    valid,
    invalid
};

struct utc_time_t {
    unsigned int hours;
    unsigned int minutes;
    float seconds;
};

struct ut_date_t {
    unsigned int dd;
    unsigned int mm;
    unsigned int yy;
};

struct latitude_t {
    unsigned int degrees;
    float minutes;
    latitude_direction_t dir;
};

struct longitude_t {
    unsigned int degrees;
    float minutes;
    longitude_direction_t dir;
};

// Global Positioning System Fix Data
struct gpgga {
    utc_time_t time;
    latitude_t latitude;
    longitude_t longitude;
    fix_quality_t fix_quality;
    unsigned int sats_used;
    float hdop;
    float msl_altitude; // mean sea level
    char msl_alt_units;
    float geoid_separation;
    char geoid_units;
    float age_of_diff_corr;
    unsigned int diff_ref_station_id;
    unsigned int checksum;
};

// Geographic Position - Latitude / Longitude
struct gpgll {
    latitude_t latitude;
    longitude_t longitude;
    utc_time_t time;
    data_status_t data_status;
    position_system_mode_indicator_t position_system_mode_indicator;
    unsigned int checksum;
};

struct gpgsa {
    gsa_mode_t gsa_mode;
    gsa_fix_type_t gsa_fix_type;
    std::vector<unsigned int> satellites;
    float dilution_of_precision;
    float horizontal_dilution_of_precision;
    float vertical_dilution_of_precision;
    unsigned int checksum;
};

struct gpgsv_entry {
    unsigned int satellite_id_number;
    unsigned int elevation;
    unsigned int azimuth;
    unsigned int signal_noise_ratio;    
};

// GNSS Satellites in View
struct gpgsv {
    unsigned int number_of_messages;
    unsigned int message_number;
    unsigned int satellites_in_view;
    std::vector<gpgsv_entry> gpgsv_entries;
    unsigned int checksum;
};

// Recommended Minimum Specific GNSS Data
struct gprmc {
    utc_time_t time;
    data_status_t data_status;
    latitude_t latitude;
    longitude_t longitude;
    float speed_over_ground;
    float course_over_ground;
    ut_date_t date;
    position_system_mode_indicator_t position_system_mode_indicator;
    unsigned int checksum;
};

// Course Over Ground and Ground Speed
struct gpvtg {
    float course_over_ground_true;
    float course_over_ground_magnetic;
    float ground_speed_knots;
    float ground_speed_kmph;
    position_system_mode_indicator_t position_system_mode_indicator;
    unsigned int checksum;
};

typedef boost::variant<
    nmea::parse::gpgga,
    nmea::parse::gpgll,
    nmea::parse::gpgsa,
    nmea::parse::gpgsv,
    nmea::parse::gprmc,
    nmea::parse::gpvtg
    >
nmea_message;

} // namespace nmea::parse

BOOST_FUSION_ADAPT_STRUCT(
    nmea::parse::utc_time_t,
    (unsigned int, hours)
    (unsigned int, minutes)
    (float, seconds)
)

BOOST_FUSION_ADAPT_STRUCT(
    nmea::parse::ut_date_t,
    (unsigned int, dd)
    (unsigned int, mm)
    (unsigned int, yy)
)

BOOST_FUSION_ADAPT_STRUCT(
    nmea::parse::latitude_t,
    (unsigned int, degrees)
    (float, minutes)
    (nmea::parse::latitude_direction_t, dir)
)

BOOST_FUSION_ADAPT_STRUCT(
    nmea::parse::longitude_t,
    (unsigned int, degrees)
    (float, minutes)
    (nmea::parse::longitude_direction_t, dir)
)

BOOST_FUSION_ADAPT_STRUCT(
    nmea::parse::gpgga,
    (nmea::parse::utc_time_t, time)
    (nmea::parse::latitude_t, latitude)
    (nmea::parse::longitude_t, longitude)
    (nmea::parse::fix_quality_t, fix_quality)
    (unsigned int, sats_used)
    (float, hdop)
    (float, msl_altitude)
    (char, msl_alt_units)
    (float, geoid_separation)
    (char, geoid_units)
    (float, age_of_diff_corr)
    (unsigned int, diff_ref_station_id)
    (unsigned int, checksum)
)

BOOST_FUSION_ADAPT_STRUCT (
    nmea::parse::gpgll,
    (nmea::parse::latitude_t, latitude)
    (nmea::parse::longitude_t, longitude)
    (nmea::parse::utc_time_t, time)
    (nmea::parse::data_status_t, data_status)
    (nmea::parse::position_system_mode_indicator_t, position_system_mode_indicator)
    (unsigned int, checksum)
)

BOOST_FUSION_ADAPT_STRUCT (
    nmea::parse::gpgsa,
    (nmea::parse::gsa_mode_t, gsa_mode)
    (nmea::parse::gsa_fix_type_t, gsa_fix_type)
    (std::vector<unsigned int>, satellites)
    (float, dilution_of_precision)
    (float, horizontal_dilution_of_precision)
    (float, vertical_dilution_of_precision)
    (unsigned int, checksum)
)

BOOST_FUSION_ADAPT_STRUCT (
    nmea::parse::gpgsv_entry,
    (unsigned int, satellite_id_number)
    (unsigned int, elevation)
    (unsigned int, azimuth)
    (unsigned int, signal_noise_ratio)
)

BOOST_FUSION_ADAPT_STRUCT (
    nmea::parse::gpgsv,
    (unsigned int, number_of_messages)
    (unsigned int, message_number)
    (unsigned int, satellites_in_view)
    (std::vector<nmea::parse::gpgsv_entry>, gpgsv_entries)
    (unsigned int, checksum)
)

BOOST_FUSION_ADAPT_STRUCT (
    nmea::parse::gprmc,
    //(std::string, message_id)
    (nmea::parse::utc_time_t, time)
    (nmea::parse::data_status_t, data_status)
    (nmea::parse::latitude_t, latitude)
    (nmea::parse::longitude_t, longitude)
    (float, speed_over_ground)
    (float, course_over_ground)
    (nmea::parse::ut_date_t, date)
    (nmea::parse::position_system_mode_indicator_t, position_system_mode_indicator)
    (unsigned int, checksum)
)

BOOST_FUSION_ADAPT_STRUCT (
    nmea::parse::gpvtg,
    (float, course_over_ground_true)
    (float, course_over_ground_magnetic)
    (float, ground_speed_knots)
    (float, ground_speed_kmph)
    (nmea::parse::position_system_mode_indicator_t, position_system_mode_indicator)
    (unsigned int, checksum)
)
    
namespace nmea::parse {

namespace qi = boost::spirit::qi;
namespace ascii = boost::spirit::ascii;

struct latitude_direction_parser : qi::symbols<char, latitude_direction_t>
{
    latitude_direction_parser()
    {
        add
            ("N", latitude_direction_t::north)
            ("S", latitude_direction_t::south)
            ;
    }
};

struct longitude_direction_parser : qi::symbols<char, longitude_direction_t>
{
    longitude_direction_parser()
    {
        add
            ("E", longitude_direction_t::east)
            ("W", longitude_direction_t::west)
            ;
    }
};

struct fix_quality_parser : qi::symbols<char, fix_quality_t>
{
    fix_quality_parser()
    {
        add
            ("0", fix_quality_t::invalid)
            ("1", fix_quality_t::gps_fix)
            ("2", fix_quality_t::dgps_fix)
            ("3", fix_quality_t::pps_fix)
            ("4", fix_quality_t::real_time_kinematic)
            ("5", fix_quality_t::float_rtk)
            ("6", fix_quality_t::dead_reckoning)
            ("7", fix_quality_t::manual_input_mode)
            ("8", fix_quality_t::simulation_mode)
            ;
    }
};

struct data_status_parser : qi::symbols<char, data_status_t>
{
    data_status_parser()
    {
        add
            ("A", data_status_t::valid)
            ("V", data_status_t::invalid)
            ;
    }
};

struct position_system_mode_indicator_parser : qi::symbols<char, position_system_mode_indicator_t>
{
    position_system_mode_indicator_parser()
    {
        add
            ("A", position_system_mode_indicator_t::autonomous)
            ("D", position_system_mode_indicator_t::differential)
            ("E", position_system_mode_indicator_t::estimated)
            ("M", position_system_mode_indicator_t::manual)
            ("N", position_system_mode_indicator_t::invalid)
            ;
    }
};

struct gsa_mode_parser : qi::symbols<char, gsa_mode_t>
{
    gsa_mode_parser()
    {
        add
            ("M", gsa_mode_t::manual)
            ("A", gsa_mode_t::automatic)
            ;
    }
};

struct gsa_fix_type_parser : qi::symbols<char, gsa_fix_type_t>
{
    gsa_fix_type_parser()
    {
        add
            ("1", gsa_fix_type_t::unavailable)
            ("2", gsa_fix_type_t::_2d)
            ("3", gsa_fix_type_t::_3d)
            ;
    }
};

template <typename Iterator>
struct utc_time_parser : qi::grammar<Iterator, nmea::parse::utc_time_t()>
{
    utc_time_parser() : utc_time_parser::base_type(start)
    {
        using qi::uint_parser;
        using qi::float_;
        
        start %= // hhmmss.sss
            uint_parser<unsigned int, 10, 2, 2>() >>
            uint_parser<unsigned int, 10, 2, 2>() >>
            float_
            ;
    }

    qi::rule<Iterator, nmea::parse::utc_time_t()> start;
};

template <typename Iterator>
struct ut_date_parser : qi::grammar<Iterator, nmea::parse::ut_date_t()>
{
    ut_date_parser() : ut_date_parser::base_type(start)
    {
        using qi::uint_parser;
        
        start %= // ddmmyy
            uint_parser<unsigned int, 10, 2, 2>() >>
            uint_parser<unsigned int, 10, 2, 2>() >>
            uint_parser<unsigned int, 10, 2, 2>()
            ;
    }

    qi::rule<Iterator, nmea::parse::ut_date_t()> start;
};

template <typename Iterator>
struct latitude_parser : qi::grammar<Iterator, nmea::parse::latitude_t()>
{
    latitude_parser() : latitude_parser::base_type(start)
    {
        using qi::uint_parser;
        using qi::float_;
        
        start %=
            uint_parser<unsigned int, 10, 2, 2>() >> // 2-digit degrees
            float_ >>  // float minutes
            ',' >>
            latitude_direction_ // n/s indicator
            ;        
    };

    qi::rule<Iterator, nmea::parse::latitude_t()> start;
    latitude_direction_parser latitude_direction_;
};

template <typename Iterator>
struct longitude_parser : qi::grammar<Iterator, nmea::parse::longitude_t()>
{
    longitude_parser() : longitude_parser::base_type(start)
    {
        using qi::uint_parser;
        using qi::float_;

        start %=
            uint_parser<unsigned int, 10, 3, 3>() >> // 3-digit degrees
            float_ >> // float minutes
            ',' >>
            longitude_direction_ // e/w indicator
            ;
    }

    qi::rule<Iterator, nmea::parse::longitude_t()> start;
    longitude_direction_parser longitude_direction_;
};

typedef qi::uint_parser<unsigned int, 16, 2, 2> checksum;

template <typename Iterator>
struct checksum_parser : qi::grammar<Iterator, unsigned int()>
{
    checksum_parser() : checksum_parser::base_type(start)
    {
        using qi::uint_parser;

        start %=
            uint_parser<unsigned int, 16, 2, 2>();
    }
    qi::rule<Iterator, unsigned int()> start;
};

template <typename Iterator>
struct gpgga_parser : qi::grammar<Iterator, nmea::parse::gpgga()>
{
    gpgga_parser() : gpgga_parser::base_type(start)
    {
        using qi::omit;
        using qi::string;
        using qi::char_;
        using qi::float_;
        using qi::uint_;
        using qi::uint_parser;
        using qi::phrase_parse;
        using qi::_pass;
        using qi::_1;
        using ascii::space;
        
        start %=
            omit[string("GGA")] >> ',' >>
            utc_time_ >> ',' >>
            latitude_ >> ',' >>
            longitude_ >> ',' >>
            fix_quality_ >> ',' >>
            uint_[ _pass = (_1 >= 0 && _1 <= 12) ] >> ',' >> // number of satellites being tracked
            float_ >> ',' >> // horizontal dilution of precision
            float_ >> ',' >> char_('M') >> ',' >> // MSL (mean sea level) altitude, meters
            -(float_) >> ',' >> -(char_('M')) >> ',' // height of geoid (MSL) above WGS84 ellipsoid
            -(float_) >> ',' >> // time since last DGPS update (empty if no using DGPS)
            uint_ >> // DGPS station ID number
            '*' >> checksum_
            ;
    }

    qi::rule<Iterator, nmea::parse::gpgga()> start;
    utc_time_parser<Iterator> utc_time_;
    latitude_parser<Iterator> latitude_;
    longitude_parser<Iterator> longitude_;    
    fix_quality_parser fix_quality_;
    checksum_parser<Iterator> checksum_;
};

template <typename Iterator>
struct gpgll_parser : qi::grammar<Iterator, nmea::parse::gpgll()>
{
    gpgll_parser() : gpgll_parser::base_type(start)
    {
        using qi::omit;
        using qi::string;
        using qi::char_;
        using qi::float_;
        using qi::uint_;
        using qi::uint_parser;
        using qi::_pass;
        using qi::_1;
        
        start %=
            omit[string("GLL")] >> ',' >>
            latitude_ >> ',' >>
            longitude_ >> ',' >>
            utc_time_ >> ',' >>
            data_status_ >> ',' >>
            position_system_mode_indicator_ >>
            '*' >> checksum_
            ;
    }
    
    qi::rule<Iterator, nmea::parse::gpgll()> start;
    latitude_parser<Iterator> latitude_;
    longitude_parser<Iterator> longitude_;
    utc_time_parser<Iterator> utc_time_;
    data_status_parser data_status_;
    position_system_mode_indicator_parser position_system_mode_indicator_;
    checksum_parser<Iterator> checksum_;
};

template <typename Iterator>
struct gpgsa_parser : qi::grammar<Iterator, nmea::parse::gpgsa()>
{
    gpgsa_parser() : gpgsa_parser::base_type(start)
    {
        using qi::omit;
        using qi::string;
        using qi::float_;
        using qi::repeat;
        using qi::uint_parser;
        
        start %=
            omit[string("GSA")] >> ',' >>
            gsa_mode_ >> ',' >>
            gsa_fix_type_ >> ',' >>
            repeat(12)[-(uint_parser<unsigned int, 10, 2, 2>()) >> ','] >> // list of 12 sats, some may be empty
            float_ >> ',' >> // dilution of precision, 0.5 through 99.9
            float_ >> ',' >> // horizontal dilution of precision, 0.5 through 99.9
            float_ >> // vertical dilution of precision, 0.5 through 99.9
            '*' >> checksum_
            ;
    }

    qi::rule<Iterator, nmea::parse::gpgsa()> start;
    gsa_mode_parser gsa_mode_;
    gsa_fix_type_parser gsa_fix_type_;
    checksum_parser<Iterator> checksum_;
};

template <typename Iterator>
struct gpgsv_entry_parser : qi::grammar<Iterator, nmea::parse::gpgsv_entry()>
{
    gpgsv_entry_parser() : gpgsv_entry_parser::base_type(start)
    {
        using qi::uint_;
        
        start %=
            uint_ >> ',' >> // satellite id
            uint_ >> ',' >> // azimuth
            uint_ >> ',' >> // elevation
            -(uint_) // signal to noise ratio, null when not tracking
            ;
    }

    qi::rule<Iterator, nmea::parse::gpgsv_entry()> start;
};

template <typename Iterator>
struct gpgsv_parser : qi::grammar<Iterator, nmea::parse::gpgsv()>
{
    gpgsv_parser() : gpgsv_parser::base_type(start)
    {
        using qi::omit;
        using qi::string;
        using qi::uint_;
        using qi::repeat;
        using qi::uint_parser;

        start %=
            omit[string("GSV")] >> ',' >> // message id
            uint_ >> ',' >> // number of messages
            uint_ >> ',' >> // message number
            uint_ >> // satellites in view
            repeat(1, 4)[',' >> entry_]  >> // satellites
            '*' >> checksum_ // checksum
            ;
    }

    qi::rule<Iterator, nmea::parse::gpgsv()> start;    
    gpgsv_entry_parser<Iterator> entry_;
    checksum_parser<Iterator> checksum_;
};

template <typename Iterator>
struct gprmc_parser : qi::grammar<Iterator, nmea::parse::gprmc()>
{
    gprmc_parser() : gprmc_parser::base_type(start)
    {
        using qi::omit;
        using qi::string;
        using qi::float_;
        using qi::uint_parser;

        start %=
            omit[string("RMC")] >> ',' >> // message id
            time_ >> ',' >> // UTC time
            data_status_ >> ',' >> // data status
            latitude_ >> ',' >> // Latitude
            longitude_ >> ','>> // Longitude
            float_ >> ',' >> // speed over ground
            -(float_) >> ',' >> // course over ground (blank?)
            date_ >> ',' >> // UT date
            ',' >> ',' >> // magnetic variation float,dir (blank?)
            position_system_mode_indicator_ >> // position system mode indicator
            '*' >> checksum_ // checksum
            ;
    }

    qi::rule<Iterator, nmea::parse::gprmc()> start;
    utc_time_parser<Iterator>  time_;
    data_status_parser data_status_;
    latitude_parser<Iterator> latitude_;
    longitude_parser<Iterator> longitude_;
    ut_date_parser<Iterator> date_;
    position_system_mode_indicator_parser position_system_mode_indicator_;
    checksum_parser<Iterator> checksum_;
};

template <typename Iterator>
struct gpvtg_parser : qi::grammar<Iterator, nmea::parse::gpvtg()>
{
    gpvtg_parser() : gpvtg_parser::base_type(start)
    {
        using qi::omit;
        using qi::string;
        using qi::float_;
        using qi::uint_parser;
        
        start %=
            omit[string("VTG")] >> ',' >> // message id
            -(float_) >> ',' >> 'T' >> ',' >> // course over ground true
            -(float_) >> ',' >> 'M' >> ',' >> // course over ground magnetic
            float_ >> ',' >> 'N' >> ',' >> // ground speed knots
            float_ >> ',' >> 'K' >> ',' >> // ground speed kmph
            position_system_mode_indicator_ >> // position system mode indicator
            '*' >> uint_parser<unsigned int, 16, 2, 2>() // checksum
            ;
    }

    qi::rule<Iterator, nmea::parse::gpvtg()> start;
    position_system_mode_indicator_parser position_system_mode_indicator_;
};

template <typename Iterator>
struct nmea_parser : qi::grammar<Iterator, nmea::parse::nmea_message()>
{
    nmea_parser() : nmea_parser::base_type(start)
    {
        using qi::omit;
        using qi::string;
        
        start %=
            omit[string("$GP")] >>
            (gpgga_ |
             gpgll_ |
             gpgsa_ |
             gpgsv_ |
             gprmc_ |
             gpvtg_)
            ;
    }

    qi::rule<Iterator, nmea::parse::nmea_message()> start;
    
    gpgga_parser<Iterator> gpgga_;
    gpgll_parser<Iterator> gpgll_;
    gpgsa_parser<Iterator> gpgsa_;
    gpgsv_parser<Iterator> gpgsv_;
    gprmc_parser<Iterator> gprmc_;
    gpvtg_parser<Iterator> gpvtg_;
};


} // namespace nmea::parse

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

    // repeat parsing all the samples num_repeats time (for benchmarking)
    for (unsigned long i = 0; i < num_repeats; ++i) {

        unsigned long successful_parses = 0;
        unsigned long failed_parses = 0;
        
        // parse each sample. report failures.
        for (std::string& sample: samples) {
            nmea::parse::nmea_message msg;
            std::string::const_iterator iter = sample.begin();
            std::string::const_iterator end = sample.end();
            bool parsed = parse(iter, end, p, msg);
            bool at_end = (iter == end);

            if (!(parsed && at_end)) {
                std::cerr << "failure: " << sample << std::endl;
                failed_parses++;
            } else {
                successful_parses++;
            }

            win &= parsed;
        }

        //std::cout << "succeeded: " << successful_parses << std::endl;
        //std::cout << "failed: " << failed_parses << std::endl;
        
    }

    if (win) std::cout << "very nice!" << std::endl;
    else std::cout << "wah woo whoa!" << std::endl;


    
    return 0;
}
