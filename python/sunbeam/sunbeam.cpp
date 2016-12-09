#include <boost/python.hpp>
#include <boost/python/suite/indexing/vector_indexing_suite.hpp>

#include <opm/parser/eclipse/EclipseState/EclipseState.hpp>
#include <opm/parser/eclipse/EclipseState/Schedule/Well.hpp>
#include <opm/parser/eclipse/Parser/Parser.hpp>

namespace py = boost::python;
using namespace Opm;

namespace {

std::vector< Well > get_wells( const Schedule& sch ) {
    std::vector< Well > wells;
    for( const auto& w : sch.getWells() )
        wells.push_back( *w );

    return wells;
}

/* alias some of boost's long names and operations */
using ref = py::return_internal_reference<>;

template< typename F >
auto mkref( F f ) -> decltype( py::make_function( f, ref() ) ) {
    return py::make_function( f, ref() );
}

}

BOOST_PYTHON_MODULE(libsunbeam) {

EclipseState (*parse_file)( const std::string&, const ParseContext& ) = &Parser::parse;
py::def( "parse", parse_file );

py::class_< EclipseState >( "EclipseState", py::no_init )
    .add_property( "title", &EclipseState::getTitle )
    .def( "_schedule", &EclipseState::getSchedule, ref() )
    ;

int    (Well::*headI)() const = &Well::getHeadI;
int    (Well::*headJ)() const = &Well::getHeadI;
double (Well::*refD)()  const = &Well::getRefDepth;

int    (Well::*headIts)(size_t) const = &Well::getHeadI;
int    (Well::*headJts)(size_t) const = &Well::getHeadI;
double (Well::*refDts)(size_t)  const = &Well::getRefDepth;

py::class_< Well >( "Well", py::no_init )
    .def( "I",   headI )
    .def( "I",   headIts )
    .def( "J",   headJ )
    .def( "J",   headJts )
    .def( "ref", refD )
    .def( "ref", refDts )
    ;

py::class_< std::vector< Well > >( "WellList", py::no_init )
    .def( py::vector_indexing_suite< std::vector< Well > >() )
    ;

py::class_< Schedule >( "Schedule", py::no_init )
    .add_property( "wells", get_wells )
    ;

void (ParseContext::*ctx_update)(const std::string&, InputError::Action) = &ParseContext::update;
py::class_< ParseContext >( "ParseContext" )
    .def( "update", ctx_update )
    ;

py::enum_< InputError::Action >( "action" )
    .value( "throw",  InputError::Action::THROW_EXCEPTION )
    .value( "warn",   InputError::Action::WARN )
    .value( "ignore", InputError::Action::IGNORE )
    ;
}