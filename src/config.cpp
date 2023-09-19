#include "config.h"

namespace yutovo
{

//Config::RealResultConfig

void Config::RealResultConfig::ToJson(rapidjson::Value& value, rapidjson::Document::AllocatorType& alloc)
{
    value.AddMember("precision", precision, alloc);
    value.AddMember("exp", exp, alloc);
    value.AddMember("default_angle_measure", (int)default_angle_measure, alloc);
    value.AddMember("result_angle_measure", (int)result_angle_measure, alloc);
    value.AddMember("show_angle_measure", show_angle_measure, alloc);
}

void Config::RealResultConfig::FromJson(rapidjson::Value& value, rapidjson::Document::AllocatorType& alloc)
{
    if (value.HasMember("precision") && value["precision"].IsInt())
        precision = value["precision"].GetInt();
    if (value.HasMember("exp") && value["exp"].IsInt())
        exp = value["exp"].GetInt();
    if (value.HasMember("default_angle_measure") && value["default_angle_measure"].IsInt())
        default_angle_measure = (AngleMeasure)value["default_angle_measure"].GetInt();
    if (value.HasMember("result_angle_measure") && value["result_angle_measure"].IsInt())
        result_angle_measure = (AngleMeasure)value["default_angle_measure"].GetInt();
    if (value.HasMember("show_angle_measure") && value["show_angle_measure"].IsBool())
        show_angle_measure = value["show_angle_measure"].GetBool();
}

//Config::IntegerResultConfig

void Config::IntegerResultConfig::ToJson(rapidjson::Value& value, rapidjson::Document::AllocatorType& alloc)
{
    value.AddMember("result_notation", (int)result_notation, alloc);
    value.AddMember("default_notation", (int)default_notation, alloc);
    value.AddMember("show_notation", show_notation, alloc);
}

void Config::IntegerResultConfig::FromJson(rapidjson::Value& value, rapidjson::Document::AllocatorType& alloc)
{
    if (value.HasMember("result_notation") && value["result_notation"].IsInt())
        result_notation = (Notation)value["result_notation"].GetInt();
    if (value.HasMember("default_notation") && value["default_notation"].IsInt())
        default_notation = (Notation)value["default_notation"].GetInt();
    if (value.HasMember("show_notation") && value["show_notation"].IsBool())
        show_notation = value["show_notation"].GetBool();
}

//Config::RationalResultConfig

void Config::RationalResultConfig::ToJson(rapidjson::Value& value, rapidjson::Document::AllocatorType& alloc)
{
    value.AddMember("fraction_form", (int)fraction_form, alloc);
}

void Config::RationalResultConfig::FromJson(rapidjson::Value& value, rapidjson::Document::AllocatorType& alloc)
{
    if (value.HasMember("fraction_form") && value["fraction_form"].IsInt())
        fraction_form = (FractionForm)value["fraction_form"].GetInt();
}

//Config::ComplexResultConfig

void Config::ComplexResultConfig::ToJson(rapidjson::Value& value, rapidjson::Document::AllocatorType& alloc)
{
    value.AddMember("precision", precision, alloc);
    value.AddMember("exp", exp, alloc);
    value.AddMember("default_angle_measure", (int)default_angle_measure, alloc);
    value.AddMember("result_angle_measure", (int)result_angle_measure, alloc);
    value.AddMember("show_angle_measure", show_angle_measure, alloc);
    value.AddMember("form", (int)form, alloc);
    value.AddMember("max_count", max_count, alloc);
}

void Config::ComplexResultConfig::FromJson(rapidjson::Value& value, rapidjson::Document::AllocatorType& alloc)
{
    if (value.HasMember("precision") && value["precision"].IsInt())
        precision = value["precision"].GetInt();
    if (value.HasMember("exp") && value["exp"].IsInt())
        exp = value["exp"].GetInt();
    if (value.HasMember("default_angle_measure") && value["default_angle_measure"].IsInt())
        default_angle_measure = (AngleMeasure)value["default_angle_measure"].GetInt();
    if (value.HasMember("result_angle_measure") && value["result_angle_measure"].IsInt())
        result_angle_measure = (AngleMeasure)value["default_angle_measure"].GetInt();
    if (value.HasMember("show_angle_measure") && value["show_angle_measure"].IsBool())
        show_angle_measure = value["show_angle_measure"].GetBool();
    if (value.HasMember("form") && value["form"].IsInt())
        form = (ComplexForm)value["form"].GetInt();
    if (value.HasMember("max_count") && value["max_count"].IsInt())
        max_count = value["max_count"].GetInt();
}

//Config::AutoResultConfig

void Config::AutoResultConfig::ToJson(rapidjson::Value& value, rapidjson::Document::AllocatorType& alloc)
{
    value.AddMember("result_auto_advance", result_auto_advance, alloc);

    rapidjson::Value arr(rapidjson::kArrayType);
    for (size_t i = 0; i < std::size(results_order); ++i)
        arr.PushBack((int)results_order[i], alloc);
    value.AddMember("results_order", arr, alloc);

    rapidjson::Value real_config(rapidjson::kObjectType);
    real_result.ToJson(real_config, alloc);
    value.AddMember("real_config", real_config, alloc);

    rapidjson::Value integer_config(rapidjson::kObjectType);
    integer_result.ToJson(integer_config, alloc);
    value.AddMember("integer_config", integer_config, alloc);

    rapidjson::Value rational_config(rapidjson::kObjectType);
    rational_result.ToJson(rational_config, alloc);
    value.AddMember("rational_config", rational_config, alloc);

    rapidjson::Value complex_config(rapidjson::kObjectType);
    complex_result.ToJson(complex_config, alloc);
    value.AddMember("complex_config", complex_config, alloc);
}

void Config::AutoResultConfig::FromJson(rapidjson::Value& value, rapidjson::Document::AllocatorType& alloc)
{
    if (value.HasMember("result_auto_advance") && value["result_auto_advance"].IsBool())
        result_auto_advance = value["result_auto_advance"].GetBool();
    if (value.HasMember("results_order") && value["results_order"].IsArray())
    {
        const auto& arr = value["results_order"].GetArray();
        for (rapidjson::SizeType i = 0; i < arr.Size() && i < std::size(results_order); ++i)
        {
            if (!arr[i].IsInt())
                return;
            results_order[i] = (yutovo_service::ResultType)arr[i].GetInt();
        }
    }

    if (value.HasMember("real_config") && value["real_config"].IsObject())
    {
        rapidjson::Value obj = value["real_config"].GetObject();
        real_result.FromJson(obj, alloc);
    }
    if (value.HasMember("integer_config") && value["integer_config"].IsObject())
    {
        rapidjson::Value obj = value["integer_config"].GetObject();
        integer_result.FromJson(obj, alloc);
    }
    if (value.HasMember("rational_config") && value["rational_config"].IsObject())
    {
        rapidjson::Value obj = value["rational_config"].GetObject();
        rational_result.FromJson(obj, alloc);
    }
    if (value.HasMember("complex_config") && value["complex_config"].IsObject())
    {
        rapidjson::Value obj = value["complex_config"].GetObject();
        complex_result.FromJson(obj, alloc);
    }
}

}
