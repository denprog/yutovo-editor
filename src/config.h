#ifndef __CONFIG_H__
#define __CONFIG_H__

#include <string>
#include "util.h"
#include <yutovo_calculator/unit.h>
#include <yutovo_calculator/math_helper.h>

namespace yutovo
{

typedef unsigned int uint;

using namespace yutovo_service;
using namespace yutovo_calculator;

struct Config
{
    //solver
    std::string service_ip = "localhost";
    uint service_port = 8010;
    uint service_timeout = 10; //seconds
    uint reconnect_timeout = 10; //seconds
    uint solve_delay = 2000; //milliseconds

    //document
    bool with_border = true;
    bool formula_border = true;
    bool pretty_json = false; //for debug purposes
    std::string language = "en";

    //caret
    bool caret_visible = true;
    uint caret_blink_delay = 500; //milliseconds

    //undo
    uint undo_size = 100; //count of undo groups

    //colors
    Color code_block_border_color = Color::Blue();

    //calculator

    struct RealResultConfig
    {
        bool operator==(const RealResultConfig& other) const
        {
            return precision == other.precision && exp == other.exp && default_angle_measure == other.default_angle_measure &&
                result_angle_measure == other.result_angle_measure && show_angle_measure == other.show_angle_measure;
        }

        void ToJson(rapidjson::Value& value, rapidjson::Document::AllocatorType& alloc);
        void FromJson(rapidjson::Value& value, rapidjson::Document::AllocatorType& alloc);

        uint precision = 3;
        uint exp = 10;
        AngleMeasure default_angle_measure = AngleMeasure::Radian;
        AngleMeasure result_angle_measure = AngleMeasure::Radian;
        bool show_angle_measure = true;
        yutovo_calculator::Unit unit;
    };

    RealResultConfig real_result;

    struct IntegerResultConfig
    {
        bool operator==(const IntegerResultConfig& other) const
        {
            return default_notation == other.default_notation && result_notation == other.result_notation && show_notation == other.show_notation;
        }

        void ToJson(rapidjson::Value& value, rapidjson::Document::AllocatorType& alloc);
        void FromJson(rapidjson::Value& value, rapidjson::Document::AllocatorType& alloc);

        Notation result_notation = Notation::Decimal;
        Notation default_notation = Notation::Decimal;
        bool show_notation = true;
    };

    IntegerResultConfig integer_result;

    struct RationalResultConfig
    {
        bool operator==(const RationalResultConfig& other) const
        {
            return fraction_form == other.fraction_form;
        }

        void ToJson(rapidjson::Value& value, rapidjson::Document::AllocatorType& alloc);
        void FromJson(rapidjson::Value& value, rapidjson::Document::AllocatorType& alloc);

        FractionForm fraction_form = FractionForm::Proper;
        yutovo_calculator::Unit unit;
    };

    RationalResultConfig rational_result;

    struct ComplexResultConfig
    {
        bool operator==(const ComplexResultConfig& other) const
        {
            return precision == other.precision && exp == other.exp && default_angle_measure == other.default_angle_measure && 
                result_angle_measure == other.result_angle_measure && show_angle_measure == other.show_angle_measure && 
                form == other.form && max_count == other.max_count;
        }

        void ToJson(rapidjson::Value& value, rapidjson::Document::AllocatorType& alloc);
        void FromJson(rapidjson::Value& value, rapidjson::Document::AllocatorType& alloc);

        uint precision = 3;
        uint exp = 10;
        AngleMeasure default_angle_measure = AngleMeasure::Radian;
        AngleMeasure result_angle_measure = AngleMeasure::Radian;
        bool show_angle_measure = true;
        ComplexForm form = ComplexForm::Arithmetic;
        uint max_count = 10;
    };

    ComplexResultConfig complex_result;

    struct AutoResultConfig
    {
        bool operator==(const AutoResultConfig& other) const
        {
            return result_auto_advance == other.result_auto_advance && 
                std::equal(std::begin(results_order), std::end(results_order), std::begin(other.results_order)) && 
                real_result == other.real_result && integer_result == other.integer_result && 
                rational_result == other.rational_result && complex_result == other.complex_result;
        }

        void ToJson(rapidjson::Value& value, rapidjson::Document::AllocatorType& alloc);
        void FromJson(rapidjson::Value& value, rapidjson::Document::AllocatorType& alloc);

        bool result_auto_advance = true;
        yutovo_service::ResultType results_order[4] = {ResultType::REAL, ResultType::INTEGER, ResultType::RATIONAL, ResultType::COMPLEX};

        RealResultConfig real_result;
        IntegerResultConfig integer_result;
        RationalResultConfig rational_result;
        ComplexResultConfig complex_result;
    };

    AutoResultConfig auto_result;
};

}

#endif
