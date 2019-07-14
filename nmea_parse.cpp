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

struct utc_time_t {
    unsigned int hh;
    unsigned int mm;
    unsigned int ss;
    unsigned int sss;
};

struct latitude_t {
    unsigned int dd;
    unsigned int mm;
    unsigned int mmmm;
};

struct longitude_t {
    unsigned int ddd;
    unsigned int mm;
    unsigned int mmmm;
};

enum class position_fix_indicator_t {
    invalid,
    gps_sps_mode,
    differential_gps_sps_mode,
    dead_reckoning,
    unsupported
};

struct gpgga {
    std::string message_id;
    utc_time_t time;
    latitude_t latitude;
    char latitude_dir;
    longitude_t longitude;
    char longitude_dir;
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

} // namespace nmea::parse

BOOST_FUSION_ADAPT_STRUCT(
    nmea::parse::utc_time_t,
    (unsigned int, hh)
    (unsigned int, mm)
    (unsigned int, ss)
    (unsigned int, sss)
)

BOOST_FUSION_ADAPT_STRUCT(
    nmea::parse::latitude_t,
    (unsigned int, dd)
    (unsigned int, mm)
    (unsigned int, mmmm)
)

BOOST_FUSION_ADAPT_STRUCT(
    nmea::parse::longitude_t,
    (unsigned int, ddd)
    (unsigned int, mm)
    (unsigned int, mmmm)
)

BOOST_FUSION_ADAPT_STRUCT(
    nmea::parse::gpgga,
    (std::string, message_id)
    (nmea::parse::utc_time_t, time)
    (nmea::parse::latitude_t, latitude)
    (char, latitude_dir)
    (nmea::parse::longitude_t, longitude)
    (char, longitude_dir)
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

namespace nmea::parse {

namespace qi = boost::spirit::qi;
namespace ascii = boost::spirit::ascii;

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
    
template <typename Iterator>
struct gpgga_parser : qi::grammar<Iterator, nmea::parse::gpgga(), ascii::space_type>
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

        time_ %= // hhmmss.sss
            uint_parser<unsigned int, 10, 2, 2>() >>
            uint_parser<unsigned int, 10, 2, 2>() >>
            uint_parser<unsigned int, 10, 2, 2>() >>
            '.' >>
            uint_parser<unsigned int, 10, 3, 3>()
            ;

        latitude_ %= // ddmm.mmmm
            uint_parser<unsigned int, 10, 2, 2>() >>
            uint_parser<unsigned int, 10, 2, 2>() >>
            '.' >>
            uint_parser<unsigned int, 10, 4, 4>()
            ;

        longitude_ %= // dddmm.mmmm
            uint_parser<unsigned int, 10, 3, 3>() >>
            uint_parser<unsigned int, 10, 2, 2>() >>
            '.' >>
            uint_parser<unsigned int, 10, 4, 4>()
            ;
        
        start %=
            string("$GPGGA") >> ',' >> // start -> $GPGGA
            time_ >> ',' >> // UTC time hhmmss.sss
            latitude_ >> ',' >> // latitude
            char_("nNsS") >> ',' >> // n/s indicator
            longitude_ >> ',' >> // longitude
            char_("eEwW") >> ',' >> // e/w indicator
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

    qi::rule<Iterator, nmea::parse::gpgga(), ascii::space_type> start;
    qi::rule<Iterator, nmea::parse::utc_time_t(), ascii::space_type> time_;
    qi::rule<Iterator, nmea::parse::latitude_t(), ascii::space_type> latitude_;
    qi::rule<Iterator, nmea::parse::longitude_t(), ascii::space_type> longitude_;
    position_fix_indicator_parser position_fix_indicator_;
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
    using boost::spirit::ascii::space;
    typedef std::string::const_iterator iterator_type;
    typedef nmea::parse::gpgga_parser<iterator_type> gpgga_parser;
    gpgga_parser p;

    bool win = false;

    for (unsigned long i = 0; i < num_samples; ++i) {
        nmea::parse::gpgga msg;
        std::string::const_iterator iter = sample.begin();
        std::string::const_iterator end = sample.end();
        win = phrase_parse(iter, end, p, space, msg);
        win &= (iter == end);
    }

    if (win) std::cout << "very nice!" << std::endl;
    else std::cout << "wah woo whoa!" << std::endl;

    return 0;
}
