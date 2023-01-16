#ifndef JSON_H
#define JSON_H

#include <iostream>
#include <cstring>
#include <initializer_list>
#include <unordered_map>
#include <memory>
//#define ERRORLOG
#include "ErrorLog/ErrorLog.h"
#include <cassert>
namespace hzd {

    enum class json_type : uint8_t {
        JSON_NULL,
        JSON_INT,
        JSON_DOUBLE,
        JSON_BOOL,
        JSON_STRING,
        JSON_ARRAY,
    };

    struct json_val {
        int destoryed;
        json_type type{json_type::JSON_NULL};

        union object {
            int32_t v_int{};
            double v_double;
            bool v_bool;
            struct string {
                char* data{nullptr};
                size_t size;
                string()
                {
                    data = nullptr;
                    size = 0;
                }
            } v_string;
            struct array {
                json_val *data{nullptr};
                size_t size;
                array()
                {
                    data = nullptr;
                    size = 0;
                }
            } v_array;

            object() {
                v_array.size = 0;
                v_string.size = 0;
                v_array.data = nullptr;
                v_string.data = nullptr;
            };

            object(int _int) : v_int(_int) {}

            object(double _double) : v_double(_double) {}

            object(bool _bool) : v_bool(_bool) {}

            object(const char *str) {
                v_string.data = new char[strlen(str) + 1];
                memcpy(v_string.data, str,strlen(str) +1);
                v_string.size = strlen(v_string.data);
            }

            object(::std::initializer_list<json_val> vals) {

            }

            ~object() = default;

        } obj;

        void clear() {
            if(type == json_type::JSON_ARRAY)
            {
                if(obj.v_array.data && destoryed == 1)
                    delete []obj.v_array.data;
                obj.v_array.size = 0;
                obj.v_array.data = nullptr;
                type = json_type::JSON_NULL;
            }
            else if(type == json_type::JSON_STRING)
            {
                if(obj.v_string.data && destoryed == 1)
                    delete obj.v_string.data;
                obj.v_string.size = 0;
                obj.v_string.data = nullptr;
                type = json_type::JSON_NULL;
            }
        }

        json_val() = default;

        ~json_val() {clear();};


        json_val(const json_val &val)
        {
            type = val.type;
            switch (type) {
                case json_type::JSON_STRING : {
                    obj.v_string.size = val.obj.v_string.size;
                    destoryed = 1;
                    obj.v_string.data = new char[obj.v_string.size + 1];
                    memcpy(obj.v_string.data, val.obj.v_string.data,obj.v_string.size+1);
                    break;
                }
                case json_type::JSON_ARRAY : {
                    obj.v_array.size = val.obj.v_array.size;
                    destoryed = 1;
                    obj.v_array.data = new json_val[obj.v_array.size];
                    for (int i = 0; i < obj.v_array.size; i++) {
//                        obj.v_array.data[i].type = val.obj.v_array.data[i].type;
//                        obj.v_array.data[i].obj = val.obj.v_array.data[i].obj;
//                        obj.v_array.data[i] = json_val(val.obj.v_array.data[i]);
                    }
                    break;
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
        }

//        json_val(json_val &&val) noexcept {
//            type = val.type;
//            switch (type) {
//                case json_type::JSON_STRING : {
//                    obj.v_string.size = val.obj.v_string.size;
//                    obj.v_string.data = val.obj.v_string.data;
//                    val.obj.v_string.data = nullptr;
//                    break;
//                }
//                case json_type::JSON_ARRAY : {
//                    obj.v_array.size = val.obj.v_array.size;
//                    obj.v_array.data = val.obj.v_array.data;
//                    val.obj.v_array.data = nullptr;
//                    break;
//                }
//                case json_type::JSON_INT : {
//                    obj.v_int = val.obj.v_int;
//                    break;
//                }
//                case json_type::JSON_DOUBLE : {
//                    obj.v_double = val.obj.v_double;
//                    break;
//                }
//                case json_type::JSON_BOOL : {
//                    obj.v_bool = val.obj.v_bool;
//                    break;
//                }
//                case json_type::JSON_NULL : {
//                    break;
//                }
//            }
//        }

        json_val(int32_t int_) : obj(int_) { type = json_type::JSON_INT; }

        json_val(double double_) : obj(double_) { type = json_type::JSON_DOUBLE; }

        json_val(bool bool_) : obj(bool_) { type = json_type::JSON_BOOL; }

        json_val(const char *str) : obj(str) { type = json_type::JSON_STRING; }

        json_val(::std::initializer_list<json_val> vals)
        {
            type = json_type::JSON_ARRAY;
            obj.v_array.data = new json_val[vals.size()];
            obj.v_array.size = 0;
            for(const json_val val : vals)
            {
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
            memcpy(obj.v_string.data, str,strlen(str) + 1);
            obj.v_string.size = strlen(str);
            return *this;
        }

        json_val &operator=(::std::initializer_list<json_val> vals) {
            clear();
            type = json_type::JSON_ARRAY;
            obj.v_array.data = new json_val[vals.size()];
            obj.v_array.size = 0;
            for(auto val : vals)
            {

            }
            return *this;
        }

        json_val &operator=(std::nullptr_t)
        {
            clear();
            type = json_type::JSON_NULL;
            return *this;
        }

        operator int() const
        {
            assert(type == json_type::JSON_INT);
            return obj.v_int;
        }
        operator double() const
        {
            assert(type == json_type::JSON_DOUBLE);
            return obj.v_double;
        }
        operator const char*() const
        {
            assert(type == json_type::JSON_STRING);
            return obj.v_string.data;
        }
        operator ::std::string() const
        {
            assert(type == json_type::JSON_STRING);
            return obj.v_string.data;
        }
        operator bool() const
        {
            assert(type == json_type::JSON_BOOL);
            return obj.v_bool;
        }
        friend ::std::ostream &operator<<(::std::ostream &out, json_val& val) {
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
                    out << val.obj.v_bool;
                    break;
                }
                case json_type::JSON_STRING: {
                    out << val.obj.v_string.data;
                    break;
                }
                case json_type::JSON_ARRAY: {
                    int i;
                    out << "[";
                    for (i = 0; i < val.obj.v_array.size-1; i++)
                        out << *(val.obj.v_array.data + i) << ",";
                    out << *(val.obj.v_array.data + i);
                    out << "]";
                    break;
                }
                default:break;
            }
            return out;
        }
    };

    using json_key = std::string;

    class json{
        std::unordered_map<json_key,json_val> json_data;
    public:
        std::string dump()
        {

        }

        bool load()
        {

        }

        json_val& operator[](const json_key& key)
        {
            assert(json_data.find(key) != json_data.end());
            return json_data[key];
        }

        json() = default;

        json(std::initializer_list<std::pair<json_key,json_val>> kvs)
        {
            json_data.clear();
            for(auto& kv : kvs)
            {
                json_data.insert(kv);
            }
        }
    };

}

#endif
