#pragma once

#include <string>

enum class PropertyType
{
	STRING = 0,
	NUMBER,
};

class IProperty
{
public:
    virtual PropertyType GetPropertyType() = 0;
    virtual ~IProperty() = default;
};

class StringProperty : public IProperty
{
public:
    explicit StringProperty(std::string&& value)
        : m_Value(std::move(value))
    { }

    PropertyType GetPropertyType() override
    {
        return PropertyType::STRING;
    }

    inline std::string GetValue() const
    {
        return m_Value;
    }
private:
    std::string m_Value;
};

class NumberProperty : public IProperty
{
public:
    explicit NumberProperty(double value)
        : m_Value(value)
    { }

    PropertyType GetPropertyType() override
    {
        return PropertyType::NUMBER;
    }

    inline double GetValue() const
    {
        return m_Value;
    }
private:
    double m_Value;
};