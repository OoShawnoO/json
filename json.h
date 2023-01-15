#ifndef JSON_H
#define JSON_H

#include <iostream>
#include <cstring>
#include <initializer_list>
namespace hzd {

    enum class json_type : uint8_t
    {
        JSON_NULL,
        JSON_INT,
        JSON_DOUBLE,
        JSON_BOOL,
        JSON_STRING,
        JSON_ARRAY,
    };
    struct json_val {
        json_type type;

        union object {
            int32_t v_int;
            double v_double;
            bool v_bool;
            struct string {
                char *data;
                size_t size;
            } v_string;
            struct array {
                json_val *data;
                size_t size;
            } v_array;

            object() = default;
            object(int _int) : v_int(_int) {}
//            object(int&& _int) : v_int(_int) {}
            object(double _double) : v_double(_double) {}
//            object(double&& _double) : v_double(_double) {}
            object(bool _bool) : v_bool(_bool) {}
//            object(bool&& _bool) : v_bool(_bool) {}
            object(const char* str) {
                v_string.data = new char[strlen(str)+1];
                memcpy(v_string.data,str,strlen(str)+1);
                v_string.size = strlen(v_string.data);
            }
            object(::std::initializer_list<json_val> vals)
            {
                v_array.data = new json_val[vals.size()];
                for(auto val : vals)
                {
                    memcpy(v_array.data + (v_array.size++),&val,sizeof(json_val));
                }
            }

        } obj;
        json_val() = default;
        json_val(int32_t int_) : obj(int_) { type = json_type::JSON_INT;}
        json_val(double double_) : obj(double_) { type = json_type::JSON_DOUBLE;}
        json_val(bool bool_) : obj(bool_) {type = json_type::JSON_BOOL;}
        json_val(const char* str) : obj(str) {type = json_type::JSON_STRING;}
        json_val(::std::initializer_list<json_val> vals) : obj(vals) {type = json_type::JSON_ARRAY;}

        friend ::std::ostream& operator<<(::std::ostream& out,json_val val)
        {
            switch (val.type) {

                case json_type::JSON_NULL: {
                    break;
                }
                case json_type::JSON_INT: {
                    out << val.obj.v_int << " ";
                    break;
                }
                case json_type::JSON_DOUBLE: {
                    out << val.obj.v_double << " ";
                    break;
                }
                case json_type::JSON_BOOL: {
                    out << val.obj.v_bool << " ";
                    break;
                }
                case json_type::JSON_STRING: {
                    out << val.obj.v_string.data << " ";
                    break;
                }
                case json_type::JSON_ARRAY: {
                    out << "[";
                    for(int i=0;i<val.obj.v_array.size;i++)
                        out << *(val.obj.v_array.data + i) << " ";
                    out << "]";
                    break;
                }
            }
            return out;
        }
    };
}

#endif
