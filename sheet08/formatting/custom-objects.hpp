#include <format>
#include <string_view>

class ObfuscatedText
{
public:
    explicit ObfuscatedText(std::string_view view) : text(view) {}

private:
    template <typename, typename>
    friend struct std::formatter;
    std::string_view text;
};

class ProgressBar
{
public:
    explicit ProgressBar(double p) : progress(p) {}

private:
    template <typename, typename>
    friend struct std::formatter;
    double progress;
};