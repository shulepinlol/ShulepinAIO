#ifndef LOGGING_UTILS_H
#define LOGGING_UTILS_H

#include <string>
#include <sstream>

#define LOG(value) g_sdk->log_console(log_value_to_string(value).c_str())

inline std::string bool_to_string(const bool value)
{
    return value ? "TRUE" : "FALSE";
}

inline std::string log_value_to_string(const char* value)
{
    return std::string{ value };
}

inline std::string log_value_to_string(const std::string& value)
{
    return value;
}

inline std::string log_value_to_string(const bool value)
{
    return bool_to_string(value);
}

template<typename T>
std::string log_value_to_string(T value) {
    std::ostringstream stream;
    stream << value;
    return stream.str();
}

#endif // LOGGING_UTILS_H