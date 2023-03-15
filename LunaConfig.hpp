#include <fstream>
#include <string>
#include <unordered_map>

class LunaValue {
public:
    enum Type {
        Null,
        Bool,
        Int,
        Float,
        String,
        Array,
        Object
    };

    Type type;
    bool boolValue;
    int intValue;
    double floatValue;
    std::string stringValue;
    std::vector<LunaValue> arrayValue;
    std::unordered_map<std::string, LunaValue> objectValue;

    LunaValue() : type(Null), boolValue(false), intValue(0), floatValue(0.0), stringValue("") {}

    LunaValue(bool value) : type(Bool), boolValue(value), intValue(0), floatValue(0.0), stringValue("") {}

    LunaValue(int value) : type(Int), boolValue(false), intValue(value), floatValue(0.0), stringValue("") {}

    LunaValue(double value) : type(Float), boolValue(false), intValue(0), floatValue(value), stringValue("") {}

    LunaValue(const std::string& value) : type(String), boolValue(false), intValue(0), floatValue(0.0), stringValue(value) {}

    LunaValue(const std::vector<LunaValue>& value) : type(Array), boolValue(false), intValue(0), floatValue(0.0), stringValue(""), arrayValue(value) {}

    LunaValue(const std::unordered_map<std::string, LunaValue>& value) : type(Object), boolValue(false), intValue(0), floatValue(0.0), stringValue(""), objectValue(value) {}
};


class LunaParser {
public:
    LunaParser(const std::string& filename) : m_filename(filename) {}
    std::string m_filename; 

    LunaValue parse() {
        std::ifstream input(m_filename);
        LunaValue result;
        if (input.is_open()) {
            parseValue(input, result);
        }
        else {
            std::cerr << "Error opening file " << m_filename << std::endl;
        }
        return result;
    }

private:
    void parseValue(std::ifstream& input, LunaValue& result) {
        char c = input.peek();
        if (c == '$') {
            // Parse an object
            parseObject(input, result.objectValue);
            result.type = LunaValue::Object;
        }
        else if (c == '[') {
            // Parse an array
            parseArray(input, result.arrayValue);
            result.type = LunaValue::Array;
        }
        else if (c == '"') {
            // Parse a string
            parseString(input, result.stringValue);
            result.type = LunaValue::String;
        }
        else if (c == 't' || c == 'f') {
            // Parse a boolean
            parseBool(input, result.boolValue);
            result.type = LunaValue::Bool;
        }
        else {
            // Parse a number
            parseNumber(input, result.floatValue, result.intValue);
            if (result.floatValue == (double)result.intValue) {
                result.type = LunaValue::Int;
            }
            else {
                result.type = LunaValue::Float;
            }
        }
    }

    // Parse an object from the input stream
    void parseObject(std::ifstream& input, std::unordered_map<std::string, LunaValue>& result) {
        input.ignore();
        std::string key;
        while (std::getline(input, key, ':')) {
            LunaValue value;
            parseValue(input, value);
            result[key] = value;
            input.ignore();
            char c = input.peek();
            if (c == '$' || c == '[' || c == '"' || c == 't' || c == 'f' || (c >= '0' && c <= '9') || c == '-') {
                continue;
            }
            else {
                break;
            }
        }
    }

    // Parse an array from the input stream
    void parseArray(std::ifstream& input, std::vector<LunaValue>& result) {
        input.ignore();
        while (input.peek() != ']') {
            LunaValue value;
            parseValue(input, value);
            result.push_back(value);
            char c = input.peek();
            if (c == ',') {
                input.ignore();
            }
            else if (c == ']') {
                break;
            }
            else {
                std::cerr << "Error parsing array in file " << m_filename << std::endl;
                break;
            }
        }
        input.ignore();
    }

    // Parse a string from the input stream
    void parseString(std::ifstream& input, std::string& result) {
        input.ignore();
        std::getline(input, result, '"');
    }

    // Parse a boolean value from the input stream
    void parseBool(std::ifstream& input, bool& result) {
        std::string value;
        input >> value;
        if (value == "true") {
            result = true;
        }
        else if (value == "false") {
            result = false;
        }
        else {
            std::cerr << "Error parsing boolean in file " << m_filename << std::endl;
        }
    }

    // Parse a number from the input stream
    void parseNumber(std::ifstream& input, double& result, int& intValue) {
        if (!(input >> result)) {
            std::cerr << "Error parsing number in file " << m_filename << std::endl;
        }
        else if (input.peek() != EOF) {
            std::cerr << "Error parsing number in file " << m_filename << std::endl;
            input.setstate(std::ios::failbit);
        }
        else {
            intValue = static_cast<int>(result);
        }
    }


};