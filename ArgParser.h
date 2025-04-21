#pragma once
#include <string>
#include <vector>
#include <cstdint>

namespace ArgumentParser {
    bool IsNumber(const std::string& argument);
    std::vector<std::string>FlagUnGlue(const std::vector<std::string>& options);

    class Argument {
    public:
        char short_name_;
        std::string long_name_;
        std::string description_;

        uint64_t min_args_amount = 0;
        bool is_multi_value = false;
        bool is_positional = false;
        bool is_store_value = false;
        bool is_default = false;

        virtual ~Argument() = default;
    };

    class FlagArgument : public Argument {
    public:
        bool value_;
        bool* store_value_;
        FlagArgument& Default(bool value);
        FlagArgument& StoreValue(bool& value);
    };

    class IntArgument : public Argument {
    public:
        int value_;
        int* store_value_;

        std::vector<int> values_;
        std::vector<int>* store_multi_values_;

        IntArgument& Default(int value);
        IntArgument& StoreValue(int& value);
        IntArgument& StoreValues(std::vector<int>& value);
        IntArgument& MultiValue();
        IntArgument& MultiValue(uint64_t min_number);
        IntArgument& Positional();
    };

    class StringArgument : public Argument {
    public:
        std::string value_;
        std::string* store_value_;

        std::vector<std::string> values_;
        std::vector<std::string>* store_multi_values_;

        StringArgument& Default(const std::string& value);
        StringArgument& StoreValue(std::string& value);
        StringArgument& StoreValues(std::vector<std::string>& value);
        StringArgument& MultiValue();
        StringArgument& MultiValue(uint64_t min_number);
        StringArgument& Positional();
    };

    class HelpArgument : public Argument {};

    class ArgParser {
    private:
        std::string name_;
        std::vector<Argument*> arguments_;
        bool is_help_ = false;
        std::string help_description_;

    public:
        explicit ArgParser(std::string str)
            : name_(std::move(str)) {}

        ~ArgParser() {
            for (int i = 0; i < arguments_.size(); ++i) {
                delete arguments_[i];
            }
        }

        bool Parse(const std::vector<std::string>& arguments);
        bool Parse(int argc, char* argv[]);

        FlagArgument& AddFlag(char short_name, const char* long_name);
        FlagArgument& AddFlag(const char* long_name);
        FlagArgument& AddFlag(char short_name, const char* long_name, const char* description);
        FlagArgument& AddFlag(const char* long_name, const char* description);

        bool GetFlag(const std::string& name);

        IntArgument& AddIntArgument(char short_name, const char* long_name);
        IntArgument& AddIntArgument(const char* long_name);
        IntArgument& AddIntArgument(const char* long_name, const char* description);
        IntArgument& AddIntArgument(char short_name, const char* long_name, const char* description);

        int64_t GetIntValue(const std::string& name);
        int64_t GetIntValue(std::string name, uint64_t number);

        StringArgument& AddStringArgument(char short_name, const char* long_name);
        StringArgument& AddStringArgument(const char* long_name);
        StringArgument& AddStringArgument(const char* long_name, const char* description);
        StringArgument& AddStringArgument(char short_name, const char* long_name, const char* description);

        std::string GetStringValue(const std::string& name);
        std::string GetStringValue(const std::string& name, uint64_t number);

        HelpArgument& AddHelp(char short_name, const char* long_name, const char* description);
        HelpArgument& AddHelp(const char* long_name, const char* description);
        bool Help();
        std::string HelpDescription();
    };
}
