#ifndef CPPNMEA_HPP
#define CPPNMEA_HPP

#include <cppnmea/cppnmea.hpp>
#include <cppnmea/types.hpp>
#include <cppnmea/parsers.hpp>

#include <string_view>
#include <functional>
#include <boost/fusion/container/map.hpp>
#include <boost/fusion/include/map.hpp>
#include <boost/fusion/container/map/map_fwd.hpp>
#include <boost/fusion/include/map_fwd.hpp>
#include <stdexcept>

namespace nmea {

/*

The Spirit parser returns a variant, but we want to abstract as much as possible. Instead
of requiring the user to learn Spirit, and variants, and everything else, we handle all of
the checking and converting in a nice wrapper function: parse. This way, an NMEA sentence
is parsed by simply calling it on a string_view (seamlessly constructed from a string):

std::string sentence = "...";
nmea::Parser p;
p.parse(sentence);

That's easy enough. What to do with the parsed sentence depends on what the user wants to
do with it. So we associate the NMEA sentence types with user-defined callback functions.
This way the parsing function can take care of figuring out the variant's type, then it
simply calls the user-defined code for that type. Setting up a callback is easy as well.
For example, a lambda:

p.setCallback<nmea::gpgga>(
    [](const nmea::gpgga& gga) {
        print_gpgga(gga);
    }
);

or a function pointer:
p.setCallback<nmea::gpgsa>(print_gpgsa);

Of course, this must be done before nmea::parse::parse() is called.

Templates are used along with boost::fusion and a bit of metaprogramming to avoid writing
the same code for each callback's declaration and setter method. Basically, the resulting
type (e.g. nmea::gpgga) is associated with a void function which takes the result as its
parameter (e.g. std::function<void(const nmea::gpgga&)>). Thus we want to store functions
in a map with types as its keys. boost::fusion::map does exactly that so we use it here.

We also need a callback for failed parses. That doesn't have a type, so we define a struct
so we have a key to store in the map (struct Parse_Failure). We pass a string_view to this
callback which contains the sentence that failed to parse.

*/

namespace detail {

struct Parse_Failure
{
    typedef std::function<void(std::string_view)> Func_Type;
};

template <typename T>
struct Callback
{
    typedef std::function<void(const T&)> Type;
};

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
    boost::fusion::pair<Parse_Failure, Parse_Failure::Func_Type>
    > callbacks_map_type;

typedef std::string_view::const_iterator iterator_type;
typedef nmea::spirit::nmea_parser<iterator_type> nmea_parser;
typedef std::string_view::const_iterator iterator_type;

} // namespace detail

class Parser
{
public:

    Parser()
    {
        // Initialize the fusion::map with lambdas that do nothing.
        // This prevents std::bad_function_call from being thrown due to calling an empty function.
        // This way, the user can safely ignore any sentences they don't want to parse.
        callbacks = detail::callbacks_map_type(
            boost::fusion::make_pair<nmea::gpgga>(detail::Callback<nmea::gpgga>::Type([](const nmea::gpgga&){})),
            boost::fusion::make_pair<nmea::gpgll>(detail::Callback<nmea::gpgll>::Type([](const nmea::gpgll&){})),
            boost::fusion::make_pair<nmea::gpgsa>(detail::Callback<nmea::gpgsa>::Type([](const nmea::gpgsa&){})),
            boost::fusion::make_pair<nmea::gpgsv>(detail::Callback<nmea::gpgsv>::Type([](const nmea::gpgsv&){})),
            boost::fusion::make_pair<nmea::gprmc>(detail::Callback<nmea::gprmc>::Type([](const nmea::gprmc&){})),
            boost::fusion::make_pair<nmea::gpvtg>(detail::Callback<nmea::gpvtg>::Type([](const nmea::gpvtg&){})),
            boost::fusion::make_pair<detail::Parse_Failure>(detail::Parse_Failure::Func_Type([](std::string_view){}))
        );
    }
    
    /// 
    /// \brief Set the callback function for a parsed sentence type
    /// 
    template <typename T>
    void setCallback(typename detail::Callback<T>::Type func)
    {
        boost::fusion::at_key<T>(callbacks) = func;
    }

    /// 
    /// \brief Set the callback function for handling failed parses
    /// 
    void setFailureCallback(detail::Parse_Failure::Func_Type func)
    {
        boost::fusion::at_key<detail::Parse_Failure>(callbacks) = func;
    }

    /// 
    /// \brief Parse an NMEA sentence and call any associated callback funcions
    /// 
    void parse(std::string_view str)
    {
        
        detail::iterator_type iter = str.begin();
        detail::iterator_type end = str.end();
        bool parsed = boost::spirit::qi::parse(iter, end, p, sentence);

        if (parsed && iter == end) {
            if (std::holds_alternative<nmea::gpgga>(sentence)) {
                nmea::gpgga gga = std::get<nmea::gpgga>(sentence);
                boost::fusion::at_key<nmea::gpgga>(callbacks)(gga);
            } else if (std::holds_alternative<nmea::gpgll>(sentence)) {
                nmea::gpgll gll = std::get<nmea::gpgll>(sentence);
                boost::fusion::at_key<nmea::gpgll>(callbacks)(gll);
            } else if (std::holds_alternative<nmea::gpgsa>(sentence)) {
                nmea::gpgsa gsa = std::get<nmea::gpgsa>(sentence);
                boost::fusion::at_key<nmea::gpgsa>(callbacks)(gsa);
            } else if (std::holds_alternative<nmea::gpgsv>(sentence)) {
                nmea::gpgsv gsv = std::get<nmea::gpgsv>(sentence);
                boost::fusion::at_key<nmea::gpgsv>(callbacks)(gsv);
            } else if (std::holds_alternative<nmea::gprmc>(sentence)) {
                nmea::gprmc rmc = std::get<nmea::gprmc>(sentence);
                boost::fusion::at_key<nmea::gprmc>(callbacks)(rmc);
            } else if (std::holds_alternative<nmea::gpvtg>(sentence)) {
                nmea::gpvtg vtg = std::get<nmea::gpvtg>(sentence);
                boost::fusion::at_key<nmea::gpvtg>(callbacks)(vtg);
            } else {
                // If this code is reached then some parsed type
                // is unaccounted for.
                throw std::domain_error("variant type is unhandled. sentence: "
                                        + std::string(str));
            }
        } else {
            boost::fusion::at_key<detail::Parse_Failure>(callbacks)(str);
        }
    }

protected:    
    detail::callbacks_map_type callbacks;
    detail::nmea_parser p;
    nmea::nmea_sentence sentence;
    
}; // class Parser

} // namespace nmea

#endif
