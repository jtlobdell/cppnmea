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

namespace nmea::parse {

enum class latitude_direction_t {
    north,
    south
};

enum class longitude_direction_t {
    east,
    west
};

enum class position_fix_indicator_t {
    invalid,
    gps_sps_mode,
    differential_gps_sps_mode,
    dead_reckoning,
    unsupported
};

enum class position_system_mode_indicator_t {
    autonomous,
    differential,
    estimated,
    manual,
    invalid
};

enum class data_status_t {
    valid,
    invalid
};

struct utc_time_t {
    unsigned int hh;
    unsigned int mm;
    unsigned int ss;
    unsigned int sss;
};

struct ut_date_t {
    unsigned int dd;
    unsigned int mm;
    unsigned int yy;
};

struct latitude_t {
    unsigned int dd;
    unsigned int mm;
    unsigned int mmmm;
    latitude_direction_t dir;
};

struct longitude_t {
    unsigned int ddd;
    unsigned int mm;
    unsigned int mmmm;
    longitude_direction_t dir;
};

// Global Positioning System Fix Data
struct gpgga {
    std::string message_id;
    utc_time_t time;
    latitude_t latitude;
    longitude_t longitude;
    position_fix_indicator_t position_fix_indicator;
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
    std::string message_id;
    latitude_t latitude;
    longitude_t longitude;
    utc_time_t time;
    data_status_t data_status;
    position_system_mode_indicator_t position_system_mode_indicator;
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
    std::string message_id;
    unsigned int number_of_messages;
    unsigned int message_number;
    unsigned int satellites_in_view;
    std::vector<gpgsv_entry> gpgsv_entries;
    unsigned int checksum;
};

// Recommended Minimum Specific GNSS Data
struct gprmc {
    std::string message_id;
    utc_time_t time;
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
    std::string message_id;
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
    nmea::parse::gpgsv,
    nmea::parse::gprmc,
    nmea::parse::gpvtg
    >
nmea_message;

} // namespace nmea::parse

BOOST_FUSION_ADAPT_STRUCT(
    nmea::parse::utc_time_t,
    (unsigned int, hh)
    (unsigned int, mm)
    (unsigned int, ss)
    (unsigned int, sss)
)

BOOST_FUSION_ADAPT_STRUCT(
    nmea::parse::ut_date_t,
    (unsigned int, dd)
    (unsigned int, mm)
    (unsigned int, yy)
)

BOOST_FUSION_ADAPT_STRUCT(
    nmea::parse::latitude_t,
    (unsigned int, dd)
    (unsigned int, mm)
    (unsigned int, mmmm)
    (nmea::parse::latitude_direction_t, dir)
)

BOOST_FUSION_ADAPT_STRUCT(
    nmea::parse::longitude_t,
    (unsigned int, ddd)
    (unsigned int, mm)
    (unsigned int, mmmm)
    (nmea::parse::longitude_direction_t, dir)
)

BOOST_FUSION_ADAPT_STRUCT(
    nmea::parse::gpgga,
    (std::string, message_id)
    (nmea::parse::utc_time_t, time)
    (nmea::parse::latitude_t, latitude)
    (nmea::parse::longitude_t, longitude)
    (nmea::parse::position_fix_indicator_t, position_fix_indicator)
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
    (std::string, message_id)
    (nmea::parse::latitude_t, latitude)
    (nmea::parse::longitude_t, longitude)
    (nmea::parse::utc_time_t, time)
    (nmea::parse::data_status_t, data_status)
    (nmea::parse::position_system_mode_indicator_t, position_system_mode_indicator)
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
    (std::string, message_id)
    (unsigned int, number_of_messages)
    (unsigned int, message_number)
    (unsigned int, satellites_in_view)
    (std::vector<nmea::parse::gpgsv_entry>, gpgsv_entries)
    (unsigned int, checksum)
)

BOOST_FUSION_ADAPT_STRUCT (
    nmea::parse::gprmc,
    (std::string, message_id)
    (nmea::parse::utc_time_t, time)
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
    (std::string, message_id)
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

struct position_fix_indicator_parser : qi::symbols<char, position_fix_indicator_t>
{
    position_fix_indicator_parser()
    {
        add
            ("0", position_fix_indicator_t::invalid)
            ("1", position_fix_indicator_t::gps_sps_mode)
            ("2", position_fix_indicator_t::differential_gps_sps_mode)
            ("3", position_fix_indicator_t::unsupported)
            ("4", position_fix_indicator_t::unsupported)
            ("5", position_fix_indicator_t::unsupported)
            ("6", position_fix_indicator_t::dead_reckoning)
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

template <typename Iterator>
struct utc_time_parser : qi::grammar<Iterator, nmea::parse::utc_time_t()>
{
    utc_time_parser() : utc_time_parser::base_type(start)
    {
        using qi::uint_parser;
        
        start %= // hhmmss.sss
            uint_parser<unsigned int, 10, 2, 2>() >>
            uint_parser<unsigned int, 10, 2, 2>() >>
            uint_parser<unsigned int, 10, 2, 2>() >>
            '.' >>
            uint_parser<unsigned int, 10, 3, 3>()
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
        
        start %= // ddmm.mmmm,[N/S indicator]
            uint_parser<unsigned int, 10, 2, 2>() >>
            uint_parser<unsigned int, 10, 2, 2>() >>
            '.' >>
            uint_parser<unsigned int, 10, 4, 4>() >>
            ',' >>
            latitude_direction_
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

        start %= // dddmm.mmmm,[E/W indicator]
            uint_parser<unsigned int, 10, 3, 3>() >>
            uint_parser<unsigned int, 10, 2, 2>() >>
            '.' >>
            uint_parser<unsigned int, 10, 4, 4>() >>
            ',' >>
            longitude_direction_
            ;
    }

    qi::rule<Iterator, nmea::parse::longitude_t()> start;
    longitude_direction_parser longitude_direction_;
};

template <typename Iterator>
struct gpgga_parser : qi::grammar<Iterator, nmea::parse::gpgga()>
{
    gpgga_parser() : gpgga_parser::base_type(start)
    {
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
            string("$GPGGA") >> ',' >> // start -> $GPGGA
            utc_time_ >> ',' >> // UTC time hhmmss.sss
            latitude_ >> ',' >> // latitude
            longitude_ >> ',' >> // longitude
            position_fix_indicator_ >> ',' >> // Position fix indicator
            uint_[ _pass = (_1 >= 0 && _1 <= 12) ] >> ',' >> // satellites used
            float_ >> ',' >> // horizontal dilution of precision
            float_ >> ',' >> // MSL altitude (mean sea level)
            char_('M') >> ',' >> // units (could this ever be different?)
            -(float_) >> ',' >> // Geoid separation
            -(char_('M')) >> ',' >> // units
            -(float_) >> ',' >> // age of diff. corr. (null fields when dgps is not used)
            uint_ >> '*' >> // diff. ref. station id
            uint_parser<unsigned int, 16, 2, 2>() // checksum
            ;
    }

    qi::rule<Iterator, nmea::parse::gpgga()> start;
    utc_time_parser<Iterator> utc_time_;
    latitude_parser<Iterator> latitude_;
    longitude_parser<Iterator> longitude_;    
    position_fix_indicator_parser position_fix_indicator_;
};

template <typename Iterator>
struct gpgll_parser : qi::grammar<Iterator, nmea::parse::gpgll()>
{
    gpgll_parser() : gpgll_parser::base_type(start)
    {
        using qi::string;
        using qi::char_;
        using qi::float_;
        using qi::uint_;
        using qi::uint_parser;
        using qi::_pass;
        using qi::_1;
        
        start %=
            string("$GPGLL") >> ',' >>
            latitude_ >> ',' >>
            longitude_ >> ',' >>
            utc_time_ >> ',' >>
            data_status_ >> ',' >>
            position_system_mode_indicator_ >>
            '*' >>
            uint_parser<unsigned int, 16, 2, 2>() // checksum
            ;
    }
    
    qi::rule<Iterator, nmea::parse::gpgll()> start;
    latitude_parser<Iterator> latitude_;
    longitude_parser<Iterator> longitude_;
    utc_time_parser<Iterator> utc_time_;
    data_status_parser data_status_;
    position_system_mode_indicator_parser position_system_mode_indicator_;
};

// untested
template <typename Iterator>
struct gpgsv_entry_parser : qi::grammar<Iterator, nmea::parse::gpgsv_entry()>
{
    gpgsv_entry_parser() : gpgsv_entry_parser::base_type(start)
    {
        using qi::uint_;
        
        start %=
            uint_ >> ',' >>
            uint_ >> ',' >>
            uint_ >> ',' >>
            uint_
            ;
    }

    qi::rule<Iterator, nmea::parse::gpgsv_entry()> start;
};

// untested
template <typename Iterator>
struct gpgsv_parser : qi::grammar<Iterator, nmea::parse::gpgsv(), qi::locals<unsigned int>>
{
    gpgsv_parser() : gpgsv_parser::base_type(start)
    {
        using qi::string;
        using qi::uint_;
        using qi::_a;
        using qi::_1;
        using qi::repeat;
        using qi::uint_parser;

        start %=
            string("$GPGSV") >> ',' >> // message id
            uint_ >> ',' >> // number of messages
            uint_ >> ',' >> // message number
            uint_[_a = _1] >> ',' >> // satellites in view
            repeat(_a)[entry_ >> ','] >> ',' >> // satellites
            '*' >> uint_parser<unsigned int, 10, 2, 2>() // checksum
            ;
    }

    qi::rule<Iterator, nmea::parse::gpgsv(), qi::locals<unsigned int>> start;
    gpgsv_entry_parser<Iterator> entry_;
};

// untested
template <typename Iterator>
struct gprmc_parser : qi::grammar<Iterator, nmea::parse::gprmc()>
{
    gprmc_parser() : gprmc_parser::base_type(start)
    {
        using qi::string;
        using qi::float_;
        using qi::uint_parser;

        start %=
            string("$GPRMC") >> ',' >> // message id
            time_ >> ',' >> // UTC time
            latitude_ >> ',' >> // Latitude
            longitude_ >> ','>> // Longitude
            float_ >> ',' >> // speed over ground
            float_ >> ',' >> // course over ground
            date_ >> ',' >> // UT date
            position_system_mode_indicator_ >> ','  >> // position system mode indicator
            '*' >> uint_parser<unsigned int, 10, 2, 2>() // checksum
            ;
    }

    qi::rule<Iterator, nmea::parse::gprmc()> start;
    utc_time_parser<Iterator>  time_;
    latitude_parser<Iterator> latitude_;
    longitude_parser<Iterator> longitude_;
    ut_date_parser<Iterator> date_;
    position_system_mode_indicator_parser position_system_mode_indicator_;
};

// untested
template <typename Iterator>
struct gpvtg_parser : qi::grammar<Iterator, nmea::parse::gpvtg()>
{
    gpvtg_parser() : gpvtg_parser::base_type(start)
    {
        using qi::string;
        using qi::float_;
        using qi::uint_parser;
        
        start %=
            string("$GPVTG") >> ',' >> // message id
            float_ >> ',' >> // course over ground true
            float_ >> ',' >> // course over ground magnetic
            float_ >> ',' >> // ground speed knots
            float_ >> ',' >> // ground speed kmph
            position_system_mode_indicator_ >> ',' >> // position system mode indicator
            '*' >> uint_parser<unsigned int, 10, 2, 2>() // checksum
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
        start %=
            gpgga_ |
            gpgll_ |
            gpgsv_ |
            gprmc_ |
            gpvtg_
            ;
    }

    qi::rule<Iterator, nmea::parse::nmea_message()> start;
    
    gpgga_parser<Iterator> gpgga_;
    gpgll_parser<Iterator> gpgll_;
    gpgsv_parser<Iterator> gpgsv_;
    gprmc_parser<Iterator> gprmc_;
    gpvtg_parser<Iterator> gpvtg_;
};


} // namespace nmea::parse

int main(int argc, char *argv[])
{
    unsigned long num_samples = 100;
    if (argc == 2) {
        try {
            num_samples = std::stoul(std::string(argv[1]));
        } catch (std::invalid_argument&) {
            std::cerr << "num_samples must be an unsigned long" << std::endl;
            return 1;
        } catch (std::out_of_range&) {
            std::cerr << "num_sampels is out of range" << std::endl;
            return 1;
        }
    }
    
    std::string sample = "$GPGGA,161229.487,3723.2475,N,12158.3416,W,1,07,1.0,9.0,M,,,,0000*18";
    typedef std::string::const_iterator iterator_type;
    typedef nmea::parse::nmea_parser<iterator_type> nmea_parser;
    nmea_parser p;

    bool win = false;

    for (unsigned long i = 0; i < num_samples; ++i) {
        nmea::parse::nmea_message msg;
        std::string::const_iterator iter = sample.begin();
        std::string::const_iterator end = sample.end();
        win = parse(iter, end, p, msg);
        win &= (iter == end);
    }

    if (win) std::cout << "very nice!" << std::endl;
    else std::cout << "wah woo whoa!" << std::endl;

    return 0;
}
