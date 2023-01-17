#ifndef JSON_H
#define JSON_H

#include <iostream>
#include <cstring>
#include <initializer_list>
#include <unordered_map>
#include <cassert>

namespace hzd {

    class json;

    enum class json_type : uint8_t {
        JSON_NULL,
        JSON_INT,
        JSON_DOUBLE,
        JSON_BOOL,
        JSON_STRING,
        JSON_ARRAY,
        JSON_JSON,
    };



    using json_key = std::string;
    enum json_ret {
        JSON_INVALID_VALUE = 0,
        JSON_OK,
        JSON_PARSE_STRING_0,
        JSON_MISS_SQUARE,
        JSON_PARSE_EXPECT_VALUE,
        JSON_MISS_KEY,
        JSON_MISS_COLON,
        JSON_MISS_CURLY,
    };

    class json {
    public:struct json_val {
            json_type type{json_type::JSON_NULL};
            union object {
                int32_t v_int{};
                double v_double;
                bool v_bool;
                json* v_json;
                struct string {
                    char *data;
                    size_t size;
                    size_t capcity;
                    string() {
                        data = nullptr;
                        size = 0;
                        capcity = 0;
                    }
                } v_string;

                struct array {
                    json_val *data;
                    size_t size;
                    size_t capacity;
                    array() {
                        data = nullptr;
                        size = 0;
                        capacity = 0;
                    }

                    void push_back(json_val& val)
                    {
                        if(size + 1 > capacity)
                        {
                            data = (json_val*) realloc(data,capacity * 1.5);
                            capacity *= 1.5;
                        }
                        data[size++] = val;
                    }
                } v_array;

                object() {
                    memset(this, 0, sizeof(object));
                }

                explicit object(int _int) : v_int(_int) {}

                explicit object(double _double) : v_double(_double) {}

                explicit object(bool _bool) : v_bool(_bool) {}

                explicit object(const char *str) {
                    v_string.data = new char[strlen(str) + 1];
                    memcpy(v_string.data, str, strlen(str) + 1);
                    v_string.size = strlen(v_string.data);
                }
                ~object() = default;
            } obj;

        private:
            void clear() {
                if (type == json_type::JSON_STRING) {
                    delete[]obj.v_string.data;
                    obj.v_string.size = 0;
                    type = json_type::JSON_NULL;
                }
                if (type == json_type::JSON_ARRAY) {
                    delete[]obj.v_array.data;
                    obj.v_array.size = 0;
                    type = json_type::JSON_NULL;
                }
                if(type == json_type::JSON_JSON)
                {
                    delete obj.v_json;
                }
            }

        public:
            json_val() = default;

            ~json_val() {
                if (type == json_type::JSON_STRING) {
                    delete[]obj.v_string.data;
                }
                if (type == json_type::JSON_ARRAY) {
                    delete[]obj.v_array.data;
                }
                if(type == json_type::JSON_JSON)
                {
                    delete obj.v_json;
                }
            }

#define JSON_VAL_COPY do {                                          \
        type = val.type;                                            \
        switch (type) {                                             \
            case json_type::JSON_STRING : {                         \
                obj.v_string.size = val.obj.v_string.size;          \
                obj.v_string.data = new char[obj.v_string.size + 1];\
                strcpy(obj.v_string.data,val.obj.v_string.data);    \
                break;                                              \
            }                                                       \
            case json_type::JSON_ARRAY : {                          \
                obj.v_array.size = val.obj.v_array.size;            \
                obj.v_array.data = new json_val[obj.v_array.size];  \
                for(int i=0;i<obj.v_array.size;i++)                 \
                {                                                   \
                    obj.v_array.data[i] = val.obj.v_array.data[i];  \
                }                                                   \
                break;                                              \
            }                                                       \
            case json_type::JSON_JSON : {                           \
                obj.v_json = new json();                            \
                *obj.v_json = *val.obj.v_json;                      \
                break;                                              \
            }                                                       \
            case json_type::JSON_INT : {                            \
                obj.v_int = val.obj.v_int;                          \
                break;                                              \
            }                                                       \
            case json_type::JSON_DOUBLE : {                         \
                obj.v_double = val.obj.v_double;                    \
                break;                                              \
            }                                                       \
            case json_type::JSON_BOOL : {                           \
                obj.v_bool = val.obj.v_bool;                        \
                break;                                              \
            }                                                       \
            case json_type::JSON_NULL : {                           \
                break;                                              \
            }                                                       \
        }                                                           \
        }while(0)

            json_val(const json_val &val) {
                JSON_VAL_COPY;
            }

            json_val(int32_t int_) : obj(int_) { type = json_type::JSON_INT; }

            json_val(double double_) : obj(double_) { type = json_type::JSON_DOUBLE; }

            json_val(bool bool_) : obj(bool_) { type = json_type::JSON_BOOL; }

            json_val(const char *str) : obj(str) { type = json_type::JSON_STRING; }

            json_val(const json& json_ref) {
                type = json_type::JSON_JSON;
                obj.v_json = new json();
                *obj.v_json = json_ref;
            }

            json_val(::std::initializer_list<json_val> vals) {
                type = json_type::JSON_ARRAY;

                obj.v_array.data = new json_val[vals.size()];
                obj.v_array.size = 0;
                for (const json_val &val: vals) {
                    obj.v_array.data[obj.v_array.size++] = val;
                }
            }

            json_val &operator=(int32_t int_) {
                clear();
                obj.v_int = int_;
                type = json_type::JSON_INT;
                return *this;
            }

            json_val &operator=(double double_) {
                clear();
                obj.v_double = double_;
                type = json_type::JSON_DOUBLE;
                return *this;
            }

            json_val &operator=(bool bool_) {
                clear();
                obj.v_bool = bool_;
                type = json_type::JSON_BOOL;
                return *this;
            }

            json_val &operator=(const char *str) {
                clear();
                type = json_type::JSON_STRING;
                obj.v_string.data = new char[strlen(str) + 1];
                memcpy(obj.v_string.data, str, strlen(str) + 1);
                obj.v_string.size = strlen(str);
                return *this;
            }

            json_val &operator=(std::string& str)
            {
                clear();
                type = json_type::JSON_STRING;
                obj.v_string.data = new char[str.size() + 1];
                memcpy(obj.v_string.data,str.c_str(),str.size()+1);
                obj.v_string.size = str.size();
                return *this;
            }

            json_val &operator=(::std::initializer_list<json_val> vals) {
                clear();
                type = json_type::JSON_ARRAY;
                obj.v_array.data = new json_val[vals.size()];
                obj.v_array.size = 0;
                for (const json_val &val: vals) {
                    obj.v_array.data[obj.v_array.size++] = val;
                }
                return *this;
            }

            json_val &operator=(std::nullptr_t) {
                clear();
                type = json_type::JSON_NULL;
                return *this;
            }

            json_val &operator=(json_val &&val) noexcept {
                clear();
                type = val.type;
                switch (type) {
                    case json_type::JSON_STRING : {
                        obj.v_string.size = val.obj.v_string.size;
                        obj.v_string.data = val.obj.v_string.data;
                        val.obj.v_string.data = nullptr;
                        break;
                    }
                    case json_type::JSON_ARRAY : {
                        obj.v_array.size = val.obj.v_array.size;
                        obj.v_array.data = val.obj.v_array.data;
                        val.obj.v_array.data = nullptr;
                        break;
                    }
                    case json_type::JSON_JSON : {
                        obj.v_json = val.obj.v_json;
                        val.obj.v_json = nullptr;
                    }
                    case json_type::JSON_INT : {
                        obj.v_int = val.obj.v_int;
                        break;
                    }
                    case json_type::JSON_DOUBLE : {
                        obj.v_double = val.obj.v_double;
                        break;
                    }
                    case json_type::JSON_BOOL : {
                        obj.v_bool = val.obj.v_bool;
                        break;
                    }
                    case json_type::JSON_NULL : {
                        break;
                    }
                }
                return *this;
            }

            json_val &operator=(const json_val &val) {
                clear();
                JSON_VAL_COPY;
                return *this;
            }

            operator int32_t() const {
                assert(type == json_type::JSON_INT);
                return obj.v_int;
            }

            operator double() const {
                assert(type == json_type::JSON_DOUBLE);
                return obj.v_double;
            }

            operator const char *() const {
                assert(type == json_type::JSON_STRING);
                return obj.v_string.data;
            }

            operator ::std::string() const {
                assert(type == json_type::JSON_STRING);
                return obj.v_string.data;
            }

            operator bool() const {
                assert(type == json_type::JSON_BOOL);
                return obj.v_bool;
            }

            friend ::std::ostream &operator<<(::std::ostream &out, json_val &val) {
                switch (val.type) {

                    case json_type::JSON_NULL: {
                        out << "null";
                        break;
                    }
                    case json_type::JSON_INT: {
                        out << val.obj.v_int;
                        break;
                    }
                    case json_type::JSON_DOUBLE: {
                        out << val.obj.v_double;
                        break;
                    }
                    case json_type::JSON_BOOL: {
                        if (val.obj.v_bool)
                            out << "true";
                        else
                            out << "false";
                        break;
                    }
                    case json_type::JSON_STRING: {
                        out << val.obj.v_string.data;
                        break;
                    }
                    case json_type::JSON_ARRAY: {
                        int i;
                        out << "[";
                        for (i = 0; i < val.obj.v_array.size - 1; i++) {
                            out << *(val.obj.v_array.data + i) << ",";
                        }
                        if (i == val.obj.v_array.size - 1) {
                            out << *(val.obj.v_array.data + i);
                        }
                        out << "]";
                        break;
                    }
                    case json_type::JSON_JSON : {
                        out << "{";
                        if(val.obj.v_json->json_data.size() > 0)
                        {
                            for(auto& x : val.obj.v_json->json_data)
                            {
                                out << "\"" << x.first << "\" : ";
                                out << x.second << " ,";
                            }
                        }
                        out << "}";
                        break;
                    }
                    default:
                        break;
                }
                return out;
            }
        };

    public:std::unordered_map<json_key, json_val> json_data;
    private:std::string::iterator context;

        static void parse_white_space(json &json_ref) {
            while (*json_ref.context == '\r'
                   || *json_ref.context == '\n'
                   || *json_ref.context == ' '
                   || *json_ref.context == '\t') {
                json_ref.context++;
            }
        }
        static json_ret parse_null(json &json_ref, json_val &val) {
            if (*json_ref.context != 'n') return JSON_INVALID_VALUE;
            if (*(json_ref.context + 1) != 'u'
                || *(json_ref.context + 2) != 'l'
                || *(json_ref.context + 3) != 'l') {
                return JSON_INVALID_VALUE;
            }
            json_ref.context += 3;
            val = nullptr;
            return JSON_OK;
        }
        static json_ret parse_bool(json &json_ref, json_val &val) {
            if (*json_ref.context != 't' && *json_ref.context != 'f') {
                return JSON_INVALID_VALUE;
            }
            if (*json_ref.context == 't') {
                if (*(json_ref.context + 1) != 'r'
                    || *(json_ref.context + 2) != 'u'
                    || *(json_ref.context + 3) != 'e') {
                    return JSON_INVALID_VALUE;
                }
                val = true;
                json_ref.context += 4;
            }
            if (*json_ref.context == 'f') {
                if (*(json_ref.context + 1) != 'a'
                    || *(json_ref.context + 2) != 'l'
                    || *(json_ref.context + 3) != 's'
                    || *(json_ref.context + 4) != 'e') {
                    return JSON_INVALID_VALUE;
                }
                val = false;
                json_ref.context += 5;
            }
            return JSON_OK;
        }
        static json_ret parse_number(json &json_ref, json_val &val) {
            if(*(json_ref.context) > '9' || *(json_ref.context) < '0') return JSON_INVALID_VALUE;
            char *end;
            double tmp;
            tmp = strtod(json_ref.context.base(), &end);
            if (json_ref.context.base() == end) {
                return JSON_INVALID_VALUE;
            }
            if (int(tmp) == tmp) {
                val = int(tmp);
            } else {
                val = tmp;
            }

            json_ref.context += (end - json_ref.context.base());
            return JSON_OK;
        }
        static json_ret parse_string(json& json_ref,json_val &val) {
            if(*json_ref.context != '\"') return JSON_INVALID_VALUE;
            json_ref.context += 1;
            std::string cur;
            while(1) {
                char c = *(json_ref.context++);
                switch (c) {
                    case '\"' : {
                        val = cur;
                        return JSON_OK;
                    }
                    case '\0' : {
                        return JSON_PARSE_STRING_0;
                    }
                    default : {
                        cur.push_back(c);
                    }
                }
            }
        }
        static json_ret parse_array(json& json_ref,json_val &val) {
            if(*(json_ref.context) != '[') return JSON_INVALID_VALUE;
            json_ref.context += 1;
            if(*(json_ref.context) == ']') {
                val.type = json_type::JSON_ARRAY;
                val.obj.v_array.data = nullptr;
                val.obj.v_array.size = 0;
                json_ref.context += 1;
                return JSON_OK;
            }
            while(1)
            {
                json_val tmp_val;
                json_ret ret;
                if((ret = parse_value(json_ref,tmp_val)) != JSON_OK)
                    return ret;
                val.obj.v_array.push_back(tmp_val);
                parse_white_space(json_ref);
                if(*json_ref.context == ',')
                {
                    json_ref.context += 1;
                    parse_white_space(json_ref);
                }
                else if(*json_ref.context == ']')
                {
                    json_ref.context += 1;
                    val.type = json_type::JSON_ARRAY;
                    return JSON_OK;
                }
                else {
                    return JSON_MISS_SQUARE;
                }
            }
        }
        static json_ret parse_string_raw(json& json_ref,std::string& str)
        {
            if(*json_ref.context != '\"') return JSON_INVALID_VALUE;
            json_ref.context += 1;
            while(1) {
                char c = *(json_ref.context++);
                switch (c) {
                    case '\"' : {
                        return JSON_OK;
                    }
                    case '\0' : {
                        return JSON_PARSE_STRING_0;
                    }
                    default : {
                        str.push_back(c);
                    }
                }
            }
        }
        static json_ret parse_json(json& json_ref,json_val& val) {
            if(*(json_ref.context) != '{') return JSON_INVALID_VALUE;
            json_ret ret;
            json_ref.context += 1;
            parse_white_space(json_ref);
            json* json_new = new json();
            if(*(json_ref.context) == '}') {
                json_ref.context += 1;
                val.type = json_type::JSON_JSON;
                val.obj.v_json = json_new;
                return JSON_OK;
            }
            while(1) {
                if(*(json_ref.context) != '\"') {
                    ret = JSON_MISS_KEY;
                    break;
                }
                std::string cur;
                if((ret = parse_string_raw(json_ref,cur)) != JSON_OK)
                    break;
                parse_white_space(json_ref);
                if(*json_ref.context != ':') {
                    ret = JSON_MISS_COLON;
                    break;
                }
                json_ref.context += 1;
                parse_white_space(json_ref);
                json_val tmp;
                if((ret = parse_value(json_ref,tmp)) != JSON_OK) {
                    break;
                }
                json_new->json_data.insert(std::pair<std::string,json_val>(cur,tmp));
                parse_white_space(json_ref);
                if(*json_ref.context == ',') {
                    json_ref.context += 1;
                    parse_white_space(json_ref);
                }
                else if(*json_ref.context == '}') {
                    json_ref.context += 1;
                    val.type = json_type::JSON_JSON;
                    val.obj.v_json = json_new;
                    return JSON_OK;
                }
                else
                {
                    ret = JSON_MISS_CURLY;
                    break;
                }
            }
            json_new->json_data.clear();
            delete json_new;
            return ret;
        }
        static json_ret parse_value(json& json_ref,json_val& val) {
            switch(*json_ref.context) {
                case 't' : { return parse_bool(json_ref,val);}
                case 'f' : { return parse_bool(json_ref,val);}
                case 'n' : { return parse_null(json_ref,val);}
                default  : { return parse_number(json_ref,val);}
                case '\"' : { return parse_string(json_ref,val);}
                case '[' : { return parse_array(json_ref,val);}
                case '{' : { return parse_json(json_ref,val);}
                case '\0' : { return JSON_PARSE_EXPECT_VALUE;}

            }
        }
    public:
        std::string dump() {
            return nullptr;
        }

        bool load(std::string &str) {
            context = str.begin();
            return false;
        }

        json() = default;

        json(const json& cp_json) {
            for (auto &p: cp_json.json_data) {
                json_data.insert(p);
            }
        }

        json(std::initializer_list<std::pair<json_key, json_val>> kvs) {
            json_data.clear();
            for (auto &kv: kvs) {
                json_data.insert(kv);
            }
        }

        json_val &operator[](const json_key &key) {
            assert(json_data.find(key) != json_data.end());
            return json_data[key];
        }

        ~json() {
            json_data.clear();
        }
    };

    using json_val = json::json_val;
}

#endif
