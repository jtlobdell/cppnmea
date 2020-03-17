#ifndef CPPNMEA_PARSERS_HPP
#define CPPNMEA_PARSERS_HPP

#include <cppnmea/types.hpp>

#include <boost/config/warning_disable.hpp>
#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/phoenix_core.hpp>
#include <boost/spirit/include/phoenix_operator.hpp>
#include <boost/spirit/include/phoenix_operator.hpp>
#include <boost/fusion/include/adapt_struct.hpp>
#include <boost/fusion/include/io.hpp>

BOOST_FUSION_ADAPT_STRUCT(
    nmea::utc_time_t,
    (unsigned int, hours)
    (unsigned int, minutes)
    (float, seconds)
)

BOOST_FUSION_ADAPT_STRUCT(
    nmea::ut_date_t,
    (unsigned int, dd)
    (unsigned int, mm)
    (unsigned int, yy)
)

BOOST_FUSION_ADAPT_STRUCT(
    nmea::latitude_t,
    (unsigned int, degrees)
    (float, minutes)
    (nmea::direction_t, dir)
)

BOOST_FUSION_ADAPT_STRUCT(
    nmea::longitude_t,
    (unsigned int, degrees)
    (float, minutes)
    (nmea::direction_t, dir)
)

BOOST_FUSION_ADAPT_STRUCT(
    nmea::position_2d_t,
    (nmea::latitude_t, latitude)
    (nmea::longitude_t, longitude)
)

BOOST_FUSION_ADAPT_STRUCT(
    nmea::gpgga,
    (nmea::utc_time_t, time)
    (nmea::position_2d_t, pos_2d)
    (nmea::fix_quality_t, fix_quality)
    (unsigned int, sats_tracked)
    (float, hdop)
    (float, msl_altitude)
    (float, geoid_separation)
    (std::optional<float>, time_since_dgps_update)
    (std::optional<unsigned int>, dgps_station_id)
    (unsigned int, checksum)
)

BOOST_FUSION_ADAPT_STRUCT (
    nmea::gpgll,
    (nmea::position_2d_t, pos_2d)
    (nmea::utc_time_t, time)
    (nmea::data_status_t, data_status)
    (nmea::fix_mode_t, fix_mode)
    (unsigned int, checksum)
)

BOOST_FUSION_ADAPT_STRUCT (
    nmea::gpgsa,
    (nmea::gsa_mode_t, gsa_mode)
    (nmea::gsa_fix_type_t, gsa_fix_type)
    (std::vector<unsigned int>, satellites)
    (float, dilution_of_precision)
    (float, horizontal_dilution_of_precision)
    (float, vertical_dilution_of_precision)
    (unsigned int, checksum)
)

BOOST_FUSION_ADAPT_STRUCT (
    nmea::gpgsv_entry,
    (unsigned int, satellite_id_number)
    (unsigned int, elevation)
    (unsigned int, azimuth)
    (std::optional<unsigned int>, signal_noise_ratio)
)

BOOST_FUSION_ADAPT_STRUCT (
    nmea::gpgsv,
    (unsigned int, number_of_messages)
    (unsigned int, message_number)
    (unsigned int, satellites_in_view)
    (std::vector<nmea::gpgsv_entry>, gpgsv_entries)
    (unsigned int, checksum)
)

BOOST_FUSION_ADAPT_STRUCT (
    nmea::gprmc,
    (nmea::utc_time_t, time)
    (nmea::data_status_t, data_status)
    (nmea::position_2d_t, pos_2d)
    (float, speed_over_ground)
    (float, course_over_ground)
    (nmea::ut_date_t, date)
    (std::optional<float>, magnetic_variation)
    (std::optional<nmea::magnetic_variation_direction_t>, magnetic_variation_dir)
    (nmea::fix_mode_t, fix_mode)
    (unsigned int, checksum)
)

BOOST_FUSION_ADAPT_STRUCT (
    nmea::gpvtg,
    (std::optional<float>, course_over_ground_true)
    (std::optional<float>, course_over_ground_magnetic)
    (float, ground_speed_knots)
    (float, ground_speed_kmph)
    (nmea::fix_mode_t, fix_mode)
    (unsigned int, checksum)
)

namespace nmea::spirit {

namespace qi = boost::spirit::qi;

struct direction_parser : qi::symbols<char, nmea::direction_t>
{
    direction_parser()
    {
        add
            ("N", nmea::direction_t::north)
            ("S", nmea::direction_t::south)
            ("E", nmea::direction_t::east)
            ("W", nmea::direction_t::west)
            ;
    }
};

struct magnetic_variation_direction_parser : qi::symbols<char, nmea::magnetic_variation_direction_t>
{
    magnetic_variation_direction_parser()
    {
        add
            ("E", nmea::magnetic_variation_direction_t::east)
            ("W", nmea::magnetic_variation_direction_t::west)
            ;
    }
};

struct fix_quality_parser : qi::symbols<char, nmea::fix_quality_t>
{
    fix_quality_parser()
    {
        add
            ("0", nmea::fix_quality_t::invalid)
            ("1", nmea::fix_quality_t::gps_fix)
            ("2", nmea::fix_quality_t::dgps_fix)
            ("3", nmea::fix_quality_t::pps_fix)
            ("4", nmea::fix_quality_t::real_time_kinematic)
            ("5", nmea::fix_quality_t::float_rtk)
            ("6", nmea::fix_quality_t::dead_reckoning)
            ("7", nmea::fix_quality_t::manual_input_mode)
            ("8", nmea::fix_quality_t::simulation_mode)
            ;
    }
};

struct data_status_parser : qi::symbols<char, nmea::data_status_t>
{
    data_status_parser()
    {
        add
            ("A", nmea::data_status_t::active)
            ("V", nmea::data_status_t::invalid) // actually 'void' but that is a keyword in C++
            ;
    }
};

struct fix_mode_parser : qi::symbols<char, nmea::fix_mode_t>
{
    fix_mode_parser()
    {
        add
            ("A", nmea::fix_mode_t::autonomous)
            ("D", nmea::fix_mode_t::differential)
            ("E", nmea::fix_mode_t::estimated)
            ("M", nmea::fix_mode_t::manual)
            ("N", nmea::fix_mode_t::invalid)
            ;
    }
};

struct gsa_mode_parser : qi::symbols<char, nmea::gsa_mode_t>
{
    gsa_mode_parser()
    {
        add
            ("M", nmea::gsa_mode_t::manual)
            ("A", nmea::gsa_mode_t::automatic)
            ;
    }
};

struct gsa_fix_type_parser : qi::symbols<char, nmea::gsa_fix_type_t>
{
    gsa_fix_type_parser()
    {
        add
            ("1", nmea::gsa_fix_type_t::unavailable)
            ("2", nmea::gsa_fix_type_t::_2d)
            ("3", nmea::gsa_fix_type_t::_3d)
            ;
    }
};

template <typename Iterator>
struct utc_time_parser : qi::grammar<Iterator, nmea::utc_time_t()>
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

    qi::rule<Iterator, nmea::utc_time_t()> start;
};

template <typename Iterator>
struct ut_date_parser : qi::grammar<Iterator, nmea::ut_date_t()>
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

    qi::rule<Iterator, nmea::ut_date_t()> start;
};

template <typename Iterator>
struct latitude_parser : qi::grammar<Iterator, nmea::latitude_t()>
{
    latitude_parser() : latitude_parser::base_type(start)
    {
        using qi::uint_parser;
        using qi::float_;
        using qi::_pass;
        using qi::_1;
                
        start %=
            uint_parser<unsigned int, 10, 2, 2>() >> // 2-digit degrees
            float_ >>  // float minutes
            ',' >>
            direction_[_pass = (_1 == nmea::direction_t::north || _1 == nmea::direction_t::south)]
            ;        
    };

    qi::rule<Iterator, nmea::latitude_t()> start;
    direction_parser direction_;
};

template <typename Iterator>
struct longitude_parser : qi::grammar<Iterator, nmea::longitude_t()>
{
    longitude_parser() : longitude_parser::base_type(start)
    {
        using qi::uint_parser;
        using qi::float_;
        using qi::_pass;
        using qi::_1;

        start %=
            uint_parser<unsigned int, 10, 3, 3>() >> // 3-digit degrees
            float_ >> // float minutes
            ',' >>
            dir_[_pass = (_1 == nmea::direction_t::east || _1 == nmea::direction_t::west)]
            ;
    }

    qi::rule<Iterator, nmea::longitude_t()> start;
    direction_parser dir_;
};

template <typename Iterator>
struct position_2d_parser : qi::grammar<Iterator, nmea::position_2d_t()>
{
    position_2d_parser() : position_2d_parser::base_type(start)
    {
        start %= latitude_ >> ',' >> longitude_;
    }
    
    qi::rule<Iterator, nmea::position_2d_t()> start;
    latitude_parser<Iterator> latitude_;
    longitude_parser<Iterator> longitude_;
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
struct gpgga_parser : qi::grammar<Iterator, nmea::gpgga()>
{
    gpgga_parser() : gpgga_parser::base_type(start)
    {
        using qi::lit;
        using qi::float_;
        using qi::uint_;
        using qi::_pass;
        using qi::_1;
        using qi::hold;
        
        start %=
            lit("GGA") >> ',' >>
            utc_time_ >> ',' >>
            position_2d_ >> ',' >>
            fix_quality_ >> ',' >>
            uint_[ _pass = (_1 >= 0u && _1 <= 12u) ] >> ',' >> // number of satellites being tracked
            float_ >> ',' >> // horizontal dilution of precision
            float_ >> ',' >> 'M' >> ',' >> // MSL (mean sea level) altitude, meters
            float_ >> ',' >> 'M' >> ',' >> // height of geoid (MSL) above WGS84 ellipsoid
            -float_ >> ',' >> // time since last DGPS update (empty if no using DGPS)
            -uint_ >> // DGPS station ID number
            '*' >> checksum_
            ;
    }

    qi::rule<Iterator, nmea::gpgga()> start;
    utc_time_parser<Iterator> utc_time_;
    position_2d_parser<Iterator> position_2d_;
    fix_quality_parser fix_quality_;
    checksum_parser<Iterator> checksum_;
};

template <typename Iterator>
struct gpgll_parser : qi::grammar<Iterator, nmea::gpgll()>
{
    gpgll_parser() : gpgll_parser::base_type(start)
    {
        using qi::lit;
        using qi::char_;
        using qi::float_;
        using qi::uint_;
        using qi::uint_parser;
        using qi::_pass;
        using qi::_1;
        
        start %=
            lit("GLL") >> ',' >>
            position_2d_ >> ',' >>
            utc_time_ >> ',' >>
            data_status_ >> ',' >>
            fix_mode_ >>
            '*' >> checksum_
            ;
    }
    
    qi::rule<Iterator, nmea::gpgll()> start;
    position_2d_parser<Iterator> position_2d_;
    utc_time_parser<Iterator> utc_time_;
    data_status_parser data_status_;
    fix_mode_parser fix_mode_;
    checksum_parser<Iterator> checksum_;
};

template <typename Iterator>
struct gpgsa_parser : qi::grammar<Iterator, nmea::gpgsa()>
{
    gpgsa_parser() : gpgsa_parser::base_type(start)
    {
        using qi::lit;
        using qi::float_;
        using qi::repeat;
        using qi::uint_parser;
        
        start %=
            lit("GSA") >> ',' >>
            gsa_mode_ >> ',' >>
            gsa_fix_type_ >> ',' >>
            repeat(12)[-(uint_parser<unsigned int, 10, 2, 2>()) >> ','] >> // list of 12 sats, some may be empty
            float_ >> ',' >> // dilution of precision, 0.5 through 99.9
            float_ >> ',' >> // horizontal dilution of precision, 0.5 through 99.9
            float_ >> // vertical dilution of precision, 0.5 through 99.9
            '*' >> checksum_
            ;
    }

    qi::rule<Iterator, nmea::gpgsa()> start;
    gsa_mode_parser gsa_mode_;
    gsa_fix_type_parser gsa_fix_type_;
    checksum_parser<Iterator> checksum_;
};

template <typename Iterator>
struct gpgsv_entry_parser : qi::grammar<Iterator, nmea::gpgsv_entry()>
{
    gpgsv_entry_parser() : gpgsv_entry_parser::base_type(start)
    {
        using qi::uint_;
        
        start %=
            uint_ >> ',' >> // satellite id
            uint_ >> ',' >> // azimuth
            uint_ >> ',' >> // elevation
            -uint_ // signal to noise ratio, null when not tracking
            ;
    }

    qi::rule<Iterator, nmea::gpgsv_entry()> start;
};

template <typename Iterator>
struct gpgsv_parser : qi::grammar<Iterator, nmea::gpgsv()>
{
    gpgsv_parser() : gpgsv_parser::base_type(start)
    {
        using qi::lit;
        using qi::uint_;
        using qi::repeat;
        using qi::uint_parser;

        start %=
            lit("GSV") >> ',' >>
            uint_ >> ',' >> // number of messages
            uint_ >> ',' >> // message number
            uint_ >> // satellites in view
            repeat(1, 4)[',' >> entry_]  >> // satellites
            '*' >> checksum_
            ;
    }

    qi::rule<Iterator, nmea::gpgsv()> start;
    gpgsv_entry_parser<Iterator> entry_;
    checksum_parser<Iterator> checksum_;
};

template <typename Iterator>
struct gprmc_parser : qi::grammar<Iterator, nmea::gprmc()>
{
    gprmc_parser() : gprmc_parser::base_type(start)
    {
        using qi::lit;
        using qi::float_;
        using qi::uint_parser;

        start %=
            lit("RMC") >> ',' >>
            time_ >> ',' >>
            data_status_ >> ',' >>
            position_2d_ >> ',' >>
            float_ >> ',' >> // speed over ground
            -float_ >> ',' >> // course over ground
            date_ >> ',' >>
            -float_ >> ',' >> -mag_var_dir_ >> ',' >> // magnetic variation degrees and direction
            fix_mode_ >>
            '*' >> checksum_
            ;
    }

    qi::rule<Iterator, nmea::gprmc()> start;
    utc_time_parser<Iterator>  time_;
    data_status_parser data_status_;
    position_2d_parser<Iterator> position_2d_;
    latitude_parser<Iterator> latitude_;
    longitude_parser<Iterator> longitude_;
    ut_date_parser<Iterator> date_;
    magnetic_variation_direction_parser mag_var_dir_;
    fix_mode_parser fix_mode_;
    checksum_parser<Iterator> checksum_;
};

template <typename Iterator>
struct gpvtg_parser : qi::grammar<Iterator, nmea::gpvtg()>
{
    gpvtg_parser() : gpvtg_parser::base_type(start)
    {
        using qi::lit;
        using qi::float_;
        using qi::uint_parser;
        
        start %=
            lit("VTG") >> ',' >>
            -float_ >> ',' >> 'T' >> ',' >> // course over ground true
            -float_ >> ',' >> 'M' >> ',' >> // course over ground magnetic
            float_ >> ',' >> 'N' >> ',' >> // ground speed knots
            float_ >> ',' >> 'K' >> ',' >> // ground speed kmph
            fix_mode_ >>
            '*' >> checksum_;
            ;
    }

    qi::rule<Iterator, nmea::gpvtg()> start;
    fix_mode_parser fix_mode_;
    checksum_parser<Iterator> checksum_;
};

template <typename Iterator>
struct nmea_parser : qi::grammar<Iterator, nmea::nmea_sentence()>
{
    nmea_parser() : nmea_parser::base_type(start)
    {
        using qi::lit;
        
        start %=
            lit("$GP") >>
            (gpgga_ |
             gpgll_ |
             gpgsa_ |
             gpgsv_ |
             gprmc_ |
             gpvtg_) >>
            -lit('\r') >>
            -lit('\n')
            ;
    }

    qi::rule<Iterator, nmea::nmea_sentence()> start;
    
    gpgga_parser<Iterator> gpgga_;
    gpgll_parser<Iterator> gpgll_;
    gpgsa_parser<Iterator> gpgsa_;
    gpgsv_parser<Iterator> gpgsv_;
    gprmc_parser<Iterator> gprmc_;
    gpvtg_parser<Iterator> gpvtg_;
};

} // namespace nmea::spirit

#endif
