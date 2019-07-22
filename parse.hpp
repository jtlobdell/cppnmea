#ifndef NMEA_PARSE_HPP
#define NMEA_PARSE_HPP

#include "nmea.hpp"
#include "parsers.hpp"

#include <string_view>
#include <functional>
#include <boost/fusion/container/map.hpp>
#include <boost/fusion/include/map.hpp>
#include <boost/fusion/container/map/map_fwd.hpp>
#include <boost/fusion/include/map_fwd.hpp>
#include <stdexcept>

namespace nmea::parse {

namespace {

/*

The Spirit parser returns a variant, but we want to abstract as much as possible. Iinstead
of requiring the user to learn Spirit, and variants, and everything else, we handle all of
the checking and converting in a nice wrapper function: parse. This way, an NME sentence
is parsed by simply calling it on a string_view (seamlessly constructed from a string):

std::string sentence = "...";
nmea::parse::parse(sentence);

That's easy enough. What to do with the parsed sentence depends on what the user wants to
do with it. So we associate the NMEA sentence types with user-defined callback functions.
This way the parsing function can take care of figuring out the variant's type, then it
simply calls the user-defined code for that type. Setting up a callback is easy as well:

nmea::parse::setCallback<nmea::gpgga>(
    [](const nmea::gpgga& gga) {
        print_gpgga(gga);
    }
);

Of course, this must be done before nmea::parse::parse() is called.

Templates are used along with boost::fusion and a bit of metaprogramming to avoid writing
the same code for each callback's declaration and setter method. Basically, the resulting
type (e.g. nmea::gpgga) is associated with a void function which takes the result as its
parameter (e.g. std::function<void(const nmea::gpgga&)>). Thus we want to store functions
in a map with types as its keys. boost::fusion::map does exactly that so we use it here.

We also need a callback for failed parses. That doesn't have a type, so we define an empty
struct so we have a key to store in the map (struct Parse_Failure). We pass a string_view
to this callback which contains the sentence that failed to parse.

 */


// We need a type with which to associate our parse failue callback.
struct Parse_Failure;

// For convenience and avoiding errors from typos.
template <typename T>
struct Callback
{
    typedef std::function<void(const T&)> Type;
};

// Also for convenience and avoiding errors from typos.
// The map key (T) should have the same type as the callback's arg.
template <typename T>
struct Callbacks_Entry
{
    typedef boost::fusion::pair<T, typename Callback<T>::Type> Type;
};

// Define everything in the map. Note the different structure for failures.
typedef boost::fusion::map<
    Callbacks_Entry<nmea::gpgga>::Type,
    Callbacks_Entry<nmea::gpgll>::Type,
    Callbacks_Entry<nmea::gpgsa>::Type,
    Callbacks_Entry<nmea::gpgsv>::Type,
    Callbacks_Entry<nmea::gprmc>::Type,
    Callbacks_Entry<nmea::gpvtg>::Type,
    boost::fusion::pair<Parse_Failure, std::function<void(std::string_view)>>
    > callbacks_map_type;

// Initialize the fusion::map with lambdas that do nothing.
// This prevents std::bad_function_call from being thrown due to calling an empty function.
// This way, the user can safely ignore any sentences they don't want to parse.
callbacks_map_type callbacks(
    boost::fusion::make_pair<nmea::gpgga>(Callback<nmea::gpgga>::Type([](const nmea::gpgga&){})),
    boost::fusion::make_pair<nmea::gpgll>(Callback<nmea::gpgll>::Type([](const nmea::gpgll&){})),
    boost::fusion::make_pair<nmea::gpgsa>(Callback<nmea::gpgsa>::Type([](const nmea::gpgsa&){})),
    boost::fusion::make_pair<nmea::gpgsv>(Callback<nmea::gpgsv>::Type([](const nmea::gpgsv&){})),
    boost::fusion::make_pair<nmea::gprmc>(Callback<nmea::gprmc>::Type([](const nmea::gprmc&){})),
    boost::fusion::make_pair<nmea::gpvtg>(Callback<nmea::gpvtg>::Type([](const nmea::gpvtg&){})),
    boost::fusion::make_pair<Parse_Failure>(std::function<void(std::string_view)>([](std::string_view){}))
);

} // anonymous namespace

template <typename T, typename Func>
void setCallback(Func func)
{
    boost::fusion::at_key<T>(callbacks) = func;
}

template <typename Func>
void setFailureCallback(Func func)
{
    boost::fusion::at_key<Parse_Failure>(callbacks) = func;
}

void parse(std::string_view str)
{
    typedef std::string_view::const_iterator iterator_type;
    typedef nmea::parse::nmea_parser<iterator_type> nmea_parser;
    nmea_parser p;
    nmea::nmea_sentence sentence;
    iterator_type iter = str.begin();
    iterator_type end = str.end();
    bool parsed = parse(iter, end, p, sentence);

    if (parsed && iter == end) {
        if (sentence.type() == typeid(nmea::gpgga)) {
            nmea::gpgga gga = boost::get<nmea::gpgga>(sentence);
            boost::fusion::at_key<nmea::gpgga>(callbacks)(gga);
        } else if (sentence.type() == typeid(nmea::gpgll)) {
            nmea::gpgll gll = boost::get<nmea::gpgll>(sentence);
            boost::fusion::at_key<nmea::gpgll>(callbacks)(gll);
        } else if (sentence.type() == typeid(nmea::gpgsa)) {
            nmea::gpgsa gsa = boost::get<nmea::gpgsa>(sentence);
            boost::fusion::at_key<nmea::gpgsa>(callbacks)(gsa);
        } else if (sentence.type() == typeid(nmea::gpgsv)) {
            nmea::gpgsv gsv = boost::get<nmea::gpgsv>(sentence);
            boost::fusion::at_key<nmea::gpgsv>(callbacks)(gsv);
        } else if (sentence.type() == typeid(nmea::gprmc)) {
            nmea::gprmc rmc = boost::get<nmea::gprmc>(sentence);
            boost::fusion::at_key<nmea::gprmc>(callbacks)(rmc);
        } else if (sentence.type() == typeid(nmea::gpvtg)) {
            nmea::gpvtg vtg = boost::get<nmea::gpvtg>(sentence);
            boost::fusion::at_key<nmea::gpvtg>(callbacks)(vtg);
        } else {
            // If this code is reached then some parsed type
            // is unaccounted for.
            throw std::domain_error("variant type is unhandled");
        }

    } else {
        boost::fusion::at_key<Parse_Failure>(callbacks)(str);
    }
}

} // namespace nmea::parse

#endif
