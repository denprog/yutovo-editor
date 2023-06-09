#ifndef __CONFIG_H__
#define __CONFIG_H__

#include <string>
#include <boost/serialization/serialization.hpp>
#include "util.h"

namespace yutovo
{

typedef unsigned int uint;

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
    };

    RealResult real_result;

    struct IntegerResult
    {
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
        template <class Archive>
        void serialize(Archive& ar, const unsigned int version)
        {
            ar & fraction_form;
        }

        FractionForm fraction_form = FractionForm::PROPER;
    };

    RationalResult rational_result;

    struct ComplexResult
    {
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
        ElementType results_order[4] = {ElementType::REAL_RESULT, ElementType::INTEGER_RESULT, ElementType::RATIONAL_RESULT, ElementType::COMPLEX_RESULT};

        RealResult real_result;
        IntegerResult integer_result;
        RationalResult rational_result;
        ComplexResult complex_result;
    };

    AutoResult auto_result;
};

}

#endif
