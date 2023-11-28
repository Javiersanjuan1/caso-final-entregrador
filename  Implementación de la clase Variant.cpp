#include <vector>
#include <string>
#include "json11.hpp"

enum variant_type { Symbol, Number, List, Proc, Lambda, Cadena };

struct Entorno;

class Variant {
public:
    typedef Variant (*proc_type)(const std::vector<Variant>&);
    typedef std::vector<Variant>::const_iterator iter;
    typedef std::map<std::string, Variant> map;

    variant_type type;
    std::string val;
    std::vector<Variant> list;
    proc_type proc;
    Entorno* env;

    Variant(variant_type type = Symbol) : type(type), env(nullptr), proc(nullptr) {}
    Variant(variant_type type, const std::string& val) : type(type), val(val), env(nullptr), proc(nullptr) {}
    Variant(proc_type proc) : type(Proc), proc(proc), env(nullptr) {}

    std::string to_string();
    std::string to_json_string();
    static Variant from_json_string(const std::string& json);
    static Variant parse_json(const json11::Json& job);
};

std::string Variant::to_string() {
    switch (type) {
        case Symbol:
        case Number:
        case Cadena:
            return val;
        case List: {
            std::string result = "[";
            for (const auto& element : list) {
                result += element.to_string() + ", ";
            }
            if (!list.empty()) {
                result.pop_back(); // Eliminar la coma adicional
                result.pop_back();
            }
            result += "]";
            return result;
        }
        default:
            return "Not Implemented";
    }
}

std::string Variant::to_json_string() {
    json11::Json jsonVal;
    switch (type) {
        case Symbol:
        case Number:
        case Cadena:
            jsonVal = val;
            break;
        case List: {
            std::vector<json11::Json> jsonList;
            for (const auto& element : list) {
                jsonList.push_back(json11::Json(element.to_json_string()));
            }
            jsonVal = jsonList;
            break;
        }
        default:
            break;
    }
    return jsonVal.dump();
}

Variant Variant::from_json_string(const std::string& sjson) {
    std::string err;
    json11::Json jsonVal = json11::Json::parse(sjson, err);
    if (!err.empty()) {
        return Variant();
    }

    // Agrega lógica para construir un objeto Variant desde el Json
    if (jsonVal.is_string()) {
        return Variant(Symbol, jsonVal.string_value());
    } else if (jsonVal.is_number()) {
        return Variant(Number, std::to_string(jsonVal.number_value()));
    } else if (jsonVal.is_array()) {
        Variant result(List);
        for (const auto& element : jsonVal.array_items()) {
            result.list.push_back(parse_json(element));
        }
        return result;
    }

    return Variant();
}

Variant Variant::parse_json(const json11::Json& job) {
    if (job.is_string()) {
        return Variant(Cadena, job.string_value());
    }
    // Agrega lógica para otros tipos si es necesario

    return Variant();
}

int main() {
    // Ejemplo de uso básico
    Variant symbol("Hello");
    Variant number(42);

    // Imprimir representación como cadena de texto
    std::string symbolString = symbol.to_string();
    std::string numberString = number.to_string();

    // Imprimir representación como cadena JSON
    std::string symbolJsonString = symbol.to_json_string();
    std::string numberJsonString = number.to_json_string();

    // Crear instancia de Variant desde cadena JSON
    ariant symbolFromJson = Variant::from_json_string(symbolJsonString);
    Variant numberFromJson = Variant::from_json_string(numberJsonString);

    return 0;
}
