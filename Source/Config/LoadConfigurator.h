#pragma once

#include <JsonForward.h>
#include <nlohmann/json.hpp>

template <typename T>
constexpr bool jsonValueTypeMatchesType(json::value_t valueType) noexcept
{
    if constexpr (std::is_same_v<T, bool>) {
        return valueType == json::value_t::boolean;
    } else if constexpr (std::is_same_v<T, int>) {
        return valueType == json::value_t::number_integer || valueType == json::value_t::number_unsigned;
    } else {
        static_assert(!std::is_same_v<T, T>, "Unsupported type!");
    }
}

template <typename T>
struct LoadHandler {
    LoadHandler(const char* name, T& variable, const json& j)
        : name{ name }, variable{ variable }, j{ j }
    {
    }

    void def([[maybe_unused]] const T& defaultValue) const noexcept
    {
    }

    ~LoadHandler() noexcept
    {
        if (const auto it = j.find(name); it != j.end() && jsonValueTypeMatchesType<T>(it->type()))
            it->get_to(variable);
    }

private:
    const char* name;
    T& variable;
    const json& j;
    bool variableHasDefaultValue = false;
};

struct LoadConfigurator {
    explicit LoadConfigurator(const json& j)
        : j{ j }
    {
    }

    template <typename T>
    auto operator()(const char* name, T& variable)
    {
        return LoadHandler<T>{ name, variable, j };
    }

private:
    const json& j;
};
