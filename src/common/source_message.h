//
// Created by lang liu on 2024/12/5.
//

#ifndef SOURCE_MESSAGE_H
#define SOURCE_MESSAGE_H

struct SourceLocation
{
public:
    constexpr SourceLocation(const char *fileName = __builtin_FILE(), const char *funcName = __builtin_FUNCTION(),
                             std::uint32_t lineNum = __builtin_LINE()) noexcept
        : _fileName(fileName), _funcName(funcName), _lineNum(lineNum)
    {
    }

    [[nodiscard]] constexpr const char *FileName() const noexcept
    {
        return _fileName;
    }

    [[nodiscard]] constexpr const char *FuncName() const noexcept
    {
        return _funcName;
    }

    [[nodiscard]] constexpr std::uint32_t LineNum() const noexcept
    {
        return _lineNum;
    }

private:
    const char         *_fileName;
    const char         *_funcName;
    const std::uint32_t _lineNum;
};

constexpr auto GetLogSourceLocation(const SourceLocation &location = {})
{
    return spdlog::source_loc{location.FileName(), static_cast<int>(location.LineNum()), location.FuncName()};
}

#endif //SOURCE_MESSAGE_H
