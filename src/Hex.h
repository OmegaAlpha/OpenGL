#pragma once
#include <iomanip>
#include <limits>

template <typename T>
struct Hex
{
    // C++11:
    // static constexpr int Width = (std::numeric_limits<T>::digits + 1) / 4;
    // Otherwise:
    enum { Width = (std::numeric_limits<T>::digits + 1) / 4 };
    const T& value;
    const int width;

    Hex(const T& value, int width = Width)
        : value(value), width(width)
    {}

    void write(std::ostream& stream) const {
        if (std::numeric_limits<T>::radix != 2) stream << value;
        else {
            std::ios_base::fmtflags flags = stream.setf(
                std::ios_base::hex, std::ios_base::basefield);
            char fill = stream.fill('0');
            stream << "0x" << std::setw(width) << value;
            stream.fill(fill);
            stream.setf(flags, std::ios_base::basefield);
        }
    }
};

template <typename T>
inline Hex<T> hex(const T& value, int width = Hex<T>::Width) {
    return Hex<T>(value, width);
}

template <typename T>
inline std::ostream& operator << (std::ostream& stream, const Hex<T>& value) {
    value.write(stream);
    return stream;
}
