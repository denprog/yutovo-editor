#ifndef __CONFIG_H__
#define __CONFIG_H__

#include <string>
#include <boost/serialization/serialization.hpp>
#include "util.h"
#include <yutovo_calculator/unit.h>

namespace yutovo
{

typedef unsigned int uint;

using namespace yutovo_service;

struct Config
{
    //solver
    std::string service_ip = "localhost";
    uint service_port = 8010;
    uint service_timeout = 10; //seconds
    uint solve_delay = 2000; //milliseconds

    //caret
    uint caret_blink_delay = 500; //milliseconds

    //undo
    uint undo_size = 100; //count of undo groups

    //calculator

    struct RealResult
    {
        bool operator==(const RealResult& other)
        {
            return precision == other.precision && exp == other.exp && default_angle_measure == other.default_angle_measure &&
                result_angle_measure == other.result_angle_measure && show_angle_measure == other.show_angle_measure;
        }

        template <class Archive>
        void serialize(Archive& ar, const unsigned int version)
        {
            ar & precision;
            ar & exp;
            ar & default_angle_measure;
            ar & result_angle_measure;
            ar & show_angle_measure;
        }

        uint precision = 3;
        uint exp = 10;
        AngleMeasure default_angle_measure = AngleMeasure::RADIAN;
        AngleMeasure result_angle_measure = AngleMeasure::RADIAN;
        bool show_angle_measure = true;
        yutovo_calculator::Unit unit;
    };

    RealResult real_result;

    struct IntegerResult
    {
        bool operator==(const IntegerResult& other)
        {
            return result_notation == other.result_notation && show_notation == other.show_notation;
        }

        template <class Archive>
        void serialize(Archive& ar, const unsigned int version)
        {
            ar & result_notation;
            ar & show_notation;
        }

        Notation result_notation = Notation::DECIMAL;
        bool show_notation = true;
    };

    IntegerResult integer_result;

    struct RationalResult
    {
        bool operator==(const RationalResult& other)
        {
            return fraction_form == other.fraction_form;
        }

        template <class Archive>
        void serialize(Archive& ar, const unsigned int version)
        {
            ar & fraction_form;
        }

        FractionForm fraction_form = FractionForm::PROPER;
        yutovo_calculator::Unit unit;
    };

    RationalResult rational_result;

    struct ComplexResult
    {
        bool operator==(const ComplexResult& other)
        {
            return precision == other.precision && exp == other.exp && default_angle_measure == other.default_angle_measure && 
                result_angle_measure == other.result_angle_measure && show_angle_measure == other.show_angle_measure && 
                form == other.form && max_count == other.max_count;
        }

        template <class Archive>
        void serialize(Archive& ar, const unsigned int version)
        {
            ar & precision;
            ar & exp;
            ar & default_angle_measure;
            ar & result_angle_measure;
            ar & show_angle_measure;
            ar & form;
            ar & max_count;
        }

        uint precision = 3;
        uint exp = 10;
        AngleMeasure default_angle_measure = AngleMeasure::RADIAN;
        AngleMeasure result_angle_measure = AngleMeasure::RADIAN;
        bool show_angle_measure = true;
        ComplexForm form = ComplexForm::ARITHMETIC;
        uint max_count = 10;
    };

    ComplexResult complex_result;

    struct AutoResult
    {
        bool operator==(const AutoResult& other)
        {
            return result_auto_advance == other.result_auto_advance && results_order == other.results_order && 
                real_result == other.real_result && integer_result == other.integer_result && 
                rational_result == other.rational_result && complex_result == other.complex_result;
        }

        template <class Archive>
        void serialize(Archive& ar, const unsigned int version)
        {
            ar & result_auto_advance;
            ar & results_order;
            ar & real_result;
            ar & integer_result;
            ar & rational_result;
            ar & complex_result;
        }

        bool result_auto_advance = true;
        yutovo_service::ResultType results_order[4] = {ResultType::REAL, ResultType::INTEGER, ResultType::RATIONAL, ResultType::COMPLEX};

        RealResult real_result;
        IntegerResult integer_result;
        RationalResult rational_result;
        ComplexResult complex_result;
    };

    AutoResult auto_result;
};

}

#endif
