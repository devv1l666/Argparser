#include "ArgParser.h"

#include <string>
#include <iostream>

using namespace ArgumentParser;

bool ArgumentParser::IsNumber(const std::string& argument) {
    size_t size = argument.size();
    for (size_t i = 0; i < size; ++i) {
        if (!std::isdigit(argument[i])) {

            return false;
        }
    }

    return true;
}

std::vector<std::string>ArgumentParser::FlagUnGlue(const std::vector<std::string>& options) {
    std::vector<std::string> result;
    result.push_back(options[0]);
    for (size_t i = 1; i < options.size(); ++i) {
        std::string current_option = options[i];
        std::string opt_name;
        if (current_option.find("-") != std::string::npos) {
            if (current_option.find("--") == std::string::npos) {
                if (current_option.find("=") != std::string::npos) {
                    result.push_back(current_option);
                    continue;
                }
                while (current_option.size() > 2) {
                    std::string last_letter = current_option.substr(current_option.size() - 1, 1);
                    current_option = current_option.substr(0, current_option.size() - 1);
                    std::string new_option = "-" + last_letter;
                    result.push_back(new_option);
                }
            }
        }

        result.push_back(current_option);
    }

    return result;
}

bool ArgParser::Parse(const std::vector<std::string>& original_options) {
    std::vector<std::string>options = FlagUnGlue(original_options);
    if (options.size() < 2) {
        if (arguments_.empty()) {

            return true;
        }
        for (size_t i = 0; i < arguments_.size(); ++i) {
            auto argument = arguments_[i];
            if (argument->is_default) {

                return true;
            }
        }

        return false;
    }
    for (size_t i = 1; i < options.size(); ++i) {
        std::string current_option = options[i];
        std::string full_current_option = current_option;
        std::string opt_name;
        bool equal_opt = false;
        if (current_option.find("-") != std::string::npos) {
            if (current_option.find("--") != std::string::npos) {
                current_option = current_option.substr(2, std::string::npos);
            } else {
                current_option = current_option.substr(1, std::string::npos);
            }
            if (current_option.find("=") != std::string::npos) {
                equal_opt = true;
                opt_name = current_option.substr(0, current_option.find("="));
            } else {
                opt_name = current_option;
            }
            bool found = false;
            for (size_t j = 0; j < arguments_.size(); ++j) {
                auto* current_arg = arguments_[j];
                if (HelpArgument* argument = dynamic_cast<HelpArgument*>(current_arg)) {
                    std::string argument_short_name = {argument->short_name_};
                    if (argument->long_name_ == opt_name || argument_short_name == opt_name) {
                        is_help_ = true;
                        found = true;
                        break;
                    }
                } else if (FlagArgument* argument = dynamic_cast<FlagArgument*>(current_arg)) {
                    std::string argument_short_name = {argument->short_name_};
                    if (argument->long_name_ == opt_name || argument_short_name == opt_name) {
                        if (argument->is_store_value) {
                           *argument->store_value_ = true;
                        }
                        argument->value_ = true;
                        found = true;
                        break;
                    }
                } else if (IntArgument* argument = dynamic_cast<IntArgument*>(current_arg)) {
                    std::string argument_short_name = {argument->short_name_};
                    if (argument->long_name_ == opt_name || argument_short_name == opt_name) {
                        std::string str_value;
                        int64_t int_value;
                        if (equal_opt) {
                            str_value = current_option.substr(current_option.find("=") + 1, std::string::npos);
                        } else {
                            if (i >= options.size() - 1) {

                                return false;
                            }
                            std::string next_option = options[i + 1];
                            if (next_option.find("-") != std::string::npos) {

                                return false;
                            }
                            str_value = next_option;
                            ++i;
                        }
                        if (!IsNumber(str_value)) {

                            return false;
                        }
                        int_value = std::stoll(str_value);
                        if (argument->is_multi_value) {
                            if (argument->is_store_value) {
                                argument->store_multi_values_->push_back(int_value);
                            }
                            argument->values_.push_back(int_value);
                        } else {
                            if (argument->is_store_value) {
                                *argument->store_value_ = int_value;
                            }
                            argument->value_ = int_value;
                        }
                        found = true;
                        break;
                    }
                } else if (StringArgument* argument = dynamic_cast<StringArgument*>(current_arg)) {
                    std::string argument_short_name = {argument->short_name_};
                    if (argument->long_name_ == opt_name || argument_short_name == opt_name) {
                        std::string result_value;
                        if (equal_opt) {
                            result_value = current_option.substr(current_option.find("=") + 1, std::string::npos);
                        } else {
                            if (i >= options.size() - 1) {

                                return false;
                            }
                            std::string next_option = options[i + 1];
                            if (next_option.find("-") != std::string::npos) {

                                return false;
                            }
                            result_value = next_option;
                            ++i;
                        }
                        if (argument->is_multi_value) {
                            if (argument->is_store_value) {
                                argument->store_multi_values_->push_back(result_value);
                            }
                            argument->values_.push_back(result_value);
                        } else {
                            if (argument->is_store_value) {
                                *argument->store_value_ = result_value;
                            }
                            argument->value_ = result_value;
                        }
                        found = true;
                        break;
                    }
                }
            }
            if (!found) {
                return false;
            }
        } else {
            if (IsNumber(current_option)) {
                int64_t new_value = stoll(current_option);
                size_t int_count = 0;
                for (size_t x = 0; x < arguments_.size(); ++x) {
                    auto* current_arg = arguments_[x];
                    if (IntArgument* argument = dynamic_cast<IntArgument*>(current_arg)) {
                        if (argument->is_positional) {
                            ++int_count;
                            if (int_count > 1) {

                                return false;
                            }
                            if (argument->is_multi_value) {
                                if (argument->is_store_value) {
                                    argument->store_multi_values_->push_back(new_value);
                                }
                                argument->values_.push_back(new_value);
                            } else {
                                if (argument->is_store_value) {
                                    *argument->store_value_ = new_value;
                                }
                                argument->value_ = new_value;
                            }
                        }
                    }
                }
            } else {
                size_t string_count = 0;
                for (size_t x = 0; x < arguments_.size(); ++x) {
                    auto* current_arg = arguments_[x];
                    if (StringArgument* argument = dynamic_cast<StringArgument*>(current_arg)) {
                        if (argument->is_positional) {
                            ++string_count;
                            if (string_count > 1) {

                                return false;
                            }
                            if (argument->is_multi_value) {
                                if (argument->is_store_value) {
                                    argument->store_multi_values_->push_back(current_option);
                                }
                                argument->values_.push_back(current_option);
                            } else {
                                if (argument->is_store_value) {
                                    *argument->store_value_ = current_option;
                                }
                                argument->value_ = current_option;
                            }
                        }
                    }
                }
            }
        }
    }
    for (size_t i = 0; i < arguments_.size(); ++i) {
        auto* current_arg = arguments_[i];
        if (IntArgument* argument = dynamic_cast<IntArgument*>(current_arg)) {
            if (argument->is_multi_value) {
                int64_t size;
                if (argument->is_store_value) {
                    size = argument->store_multi_values_->size();
                } else {
                    size = argument->values_.size();
                }
                if (argument->min_args_amount > size) {

                    return false;
                }
            }
        }
    }

    return true;
}

bool ArgParser::Parse(int argc, char* argv[]) {
    std::vector<std::string> args;
    for (int i = 0; i < argc; ++i) {
        args.emplace_back(argv[i]);
    }

    return Parse(args);
}


FlagArgument& ArgParser::AddFlag(char short_name, const char* long_name) {
    FlagArgument* arg = new FlagArgument();
    arg->short_name_ = short_name;
    arg->long_name_ = long_name;
    arguments_.push_back(arg);

    return *arg;
}

FlagArgument& ArgParser::AddFlag(const char* long_name) {
    FlagArgument* arg = new FlagArgument();
    arg->long_name_ = long_name;
    arguments_.push_back(arg);

    return *arg;
}

FlagArgument& ArgParser::AddFlag(char short_name, const char* long_name, const char* description) {
    FlagArgument* arg = new FlagArgument();
    arg->short_name_ = short_name;
    arg->long_name_ = long_name;
    arg->description_ = description;
    arguments_.push_back(arg);

    return *arg;
}

FlagArgument& ArgParser::AddFlag(const char* long_name, const char* description) {
    FlagArgument* arg = new FlagArgument();
    arg->long_name_ = long_name;
    arg->description_ = description;
    arguments_.push_back(arg);

    return *arg;
}

FlagArgument& FlagArgument::Default(bool value) {
    value_ = value;
    if (is_store_value) {
        *store_value_ = value;
    }
    is_default = true;

    return *this;
}

FlagArgument& FlagArgument::StoreValue(bool& value) {
    is_store_value = true;
    store_value_ = &value;

    return *this;
}

bool ArgParser::GetFlag(const std::string& name) {
    for (size_t i = 0; i < arguments_.size(); ++i) {
        auto* current_arg = arguments_[i];
        if (auto* bool_arg = dynamic_cast<FlagArgument*>(current_arg)) {
            if (bool_arg->long_name_ == name) {

                return bool_arg->value_;
            }
        }
    }

    return false;
}


IntArgument& ArgParser::AddIntArgument(char short_name, const char* long_name) {
    IntArgument* arg = new IntArgument();
    arg->short_name_ = short_name;
    arg->long_name_ = long_name;
    arguments_.push_back(arg);

    return *arg;
}

IntArgument& ArgParser::AddIntArgument(const char* long_name) {
    IntArgument* arg = new IntArgument();
    arg->long_name_ = long_name;
    arguments_.push_back(arg);

    return *arg;
}

IntArgument& ArgParser::AddIntArgument(const char* long_name, const char* description) {
    IntArgument* arg = new IntArgument();
    arg->long_name_ = long_name;
    arg->description_ = description;
    arguments_.push_back(arg);

    return *arg;
}

IntArgument& ArgParser::AddIntArgument(char short_name, const char* long_name, const char* description) {
    IntArgument* arg = new IntArgument();
    arg->short_name_ = short_name;
    arg->long_name_ = long_name;
    arg->description_ = description;
    arguments_.push_back(arg);

    return *arg;
}

IntArgument& IntArgument::Default(int value) {
    value_ = value;
    is_default = true;

    return *this;
}

IntArgument& IntArgument::StoreValue(int& value) {
    store_value_ = &value;
    is_store_value = true;

    return *this;
}

IntArgument& IntArgument::StoreValues(std::vector<int>& value) {
    store_multi_values_ = &value;
    is_store_value = true;
    is_multi_value = true;

    return *this;
}

IntArgument& IntArgument::MultiValue() {
    is_multi_value = true;

    return *this;
}

IntArgument& IntArgument::MultiValue(uint64_t min_number) {
    is_multi_value = true;
    min_args_amount = min_number;

    return *this;
}

IntArgument& IntArgument::Positional() {
    is_positional = true;

    return *this;
}


int64_t ArgParser::GetIntValue(const std::string& name) {
    for (size_t i = 0; i < arguments_.size(); ++i) {
        auto* current_arg = arguments_[i];
        if (IntArgument* int_arg = dynamic_cast<IntArgument*>(current_arg)) {
            if (int_arg->long_name_ == name) {

                return int_arg->value_;
            }
        }
    }

    return 0;
}

int64_t ArgParser::GetIntValue(std::string name, uint64_t number) {
    for (size_t i = 0; i < arguments_.size(); ++i) {
        auto* current_arg = arguments_[i];
        if (IntArgument* int_arg = dynamic_cast<IntArgument*>(current_arg)) {
            if (int_arg->long_name_ == name) {
                if (int_arg->is_store_value) {
                    std::vector<int> search = *int_arg->store_multi_values_;

                    return search[number];
                }

                return int_arg->values_[number];
            }
        }
    }

    return 0;
}


StringArgument& ArgParser::AddStringArgument(char short_name, const char* long_name) {
    StringArgument* arg = new StringArgument();
    arg->short_name_ = short_name;
    arg->long_name_ = long_name;
    arguments_.push_back(arg);

    return *arg;
}

StringArgument& ArgParser::AddStringArgument(const char* long_name) {
    StringArgument* arg = new StringArgument();
    arg->long_name_ = long_name;
    arguments_.push_back(arg);

    return *arg;
}

StringArgument& ArgParser::AddStringArgument(const char* long_name, const char* description) {
    StringArgument* arg = new StringArgument();
    arg->long_name_ = long_name;
    arg->description_ = description;
    arguments_.push_back(arg);

    return *arg;
}

StringArgument& ArgParser::AddStringArgument(char short_name, const char* long_name, const char* description) {
    StringArgument* arg = new StringArgument();
    arg->short_name_ = short_name;
    arg->long_name_ = long_name;
    arg->description_ = description;
    arguments_.push_back(arg);

    return *arg;
}

StringArgument& StringArgument::Default(const std::string& value) {
    value_ = value;
    is_default = true;

    return *this;
}

StringArgument& StringArgument::StoreValue(std::string& value) {
    store_value_ = &value;
    is_store_value = true;

    return *this;
}

StringArgument& StringArgument::StoreValues(std::vector<std::string>& value) {
    store_multi_values_ = &value;
    is_store_value = true;
    is_multi_value = true;

    return *this;
}

StringArgument& StringArgument::MultiValue() {
    is_multi_value = true;

    return *this;
}

StringArgument& StringArgument::MultiValue(uint64_t min_number) {
    is_multi_value = true;
    min_args_amount = min_number;

    return *this;
}

StringArgument& StringArgument::Positional() {
    is_positional = true;

    return *this;
}

std::string ArgParser::GetStringValue(const std::string& name) {
    for (size_t i = 0; i < arguments_.size(); ++i) {
        auto* current_argument = arguments_[i];
        if (auto* str_arg = dynamic_cast<StringArgument*>(current_argument)) {
            if (str_arg->long_name_ == name) {

                return str_arg->value_;
            }
        }
    }

    return "";
}

std::string ArgParser::GetStringValue(const std::string& name, uint64_t number) {
    for (size_t i = 0; i < arguments_.size(); ++i) {
        auto* current_argument = arguments_[i];
        if (auto* str_arg = dynamic_cast<StringArgument*>(current_argument)) {
            if (str_arg->long_name_ == name) {
                std::vector<std::string> result = *str_arg->store_multi_values_;

                return result[number];
            }
        }
    }

    return "";
}


HelpArgument& ArgParser::AddHelp(const char* long_name, const char* description) {
    HelpArgument* arg = new HelpArgument();
    arg->long_name_ = long_name;
    help_description_ = description;
    arguments_.push_back(arg);

    return *arg;
}

HelpArgument& ArgParser::AddHelp(char short_name, const char* long_name, const char* description) {
    HelpArgument* arg = new HelpArgument();
    arg->short_name_ = short_name;
    arg->long_name_ = long_name;
    help_description_ = description;
    arguments_.push_back(arg);

    return *arg;
}

bool ArgParser::Help() {

    return is_help_;
}

std::string ArgParser::HelpDescription() {
    std::string result = help_description_ + "\n";
    std::string plus_help = "";
    for (int i = 0; i < arguments_.size(); ++i) {
        auto* current_argument = arguments_[i];
        if (IntArgument* arg = dynamic_cast<IntArgument*>(current_argument)) {
            result += "Int argument: ";
            if (arg->short_name_) {
                result += "-";
                result += arg->short_name_;
                result += "  ";
            }
            if (arg->long_name_ != "") {
                result += "--";
                result += arg->long_name_ + "  ";
            }
            if (arg->description_ != "") {
                result += arg->description_;
            }
            result += "\n";
        } else if (FlagArgument* arg = dynamic_cast<FlagArgument*>(current_argument)) {
            result += "Flag argument: ";
            if (arg->short_name_) {
                result += "-";
                result += arg->short_name_;
                result += "  ";
            }
            if (arg->long_name_ != "") {
                result += "--";
                result += arg->long_name_ + "  ";
            }
            if (arg->description_ != "") {
                result += arg->description_;
            }
            result += "\n";
        } else if (StringArgument* arg = dynamic_cast<StringArgument*>(current_argument)) {
            result += "String argument: ";
            if (arg->short_name_) {
                result += "-";
                result += arg->short_name_;
                result += "  ";
            }
            if (arg->long_name_ != "") {
                result += "--";
                result += arg->long_name_ + "  ";
            }
            if (arg->description_ != "") {
                result += arg->description_;
            }
            result += "\n";
        } else if (HelpArgument* arg = dynamic_cast<HelpArgument*>(current_argument)) {
            plus_help += "Help argument: ";
            if (arg->short_name_) {
                plus_help += "-";
                plus_help += arg->short_name_;
                plus_help += "  ";
            }
            if (arg->long_name_ != "") {
                plus_help += "--";
                plus_help += arg->long_name_ + "  ";
            }
            plus_help += "Display this help and exit\n";
        }
    }
    result += plus_help;

    return result;
}
