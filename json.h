
#ifndef _MSC_VER
#pragma clang diagnostic push
#pragma ide diagnostic ignored "DanglingPointer"
#pragma ide diagnostic ignored "misc-no-recursion"
#else
#pragma warning(disable:4996)
#endif

#ifndef JSON_H
#define JSON_H

/**
 * @author hzd
 * @brief json类
 */

#include <iostream>
#include <fstream>
#include <streambuf>
#include <sstream>
#include <cstring>
#include <initializer_list>
#include <unordered_map>
#include <vector>
#include <cassert>

namespace hzd {

    class json;
    using json_key = std::string;
    enum json_type{
        JSON_NULL,
        JSON_INT,
        JSON_DOUBLE,
        JSON_BOOL,
        JSON_STRING,
        JSON_ARRAY,
        JSON_JSON,
    };
    enum json_ret {
        JSON_INVALID_VALUE = 0,
        JSON_OK,
        JSON_PARSE_STRING_0,
        JSON_MISS_SQUARE,
        JSON_PARSE_EXPECT_VALUE,
        JSON_MISS_KEY,
        JSON_MISS_COLON,
        JSON_MISS_CURLY,
        JSON_PARSE_INVALID_UNICODE_SURROGATE,
        JSON_PARSE_INVALID_UNICODE_HEX,
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
                    size_t capacity;
                    string() {
                        data = nullptr;
                        size = 0;
                        capacity = 0;
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

#define JSON_ARRAY_REALLOC do{                  \
    if(capacity == 0) capacity = 1;             \
    if(size + 1 >= capacity)                    \
    {                                           \
        if(data){                               \
            json_val* last = data;              \
            data = new json_val[capacity * 2];  \
            for(size_t t = 0;t < size;t++)      \
            {                                   \
                data[t] = last[t];              \
            }                                   \
            delete []last;                      \
            capacity *= 2;                      \
        }                                       \
        else                                    \
        {                                       \
            data = new json_val[capacity * 2];  \
        }                                       \
    }                                           \
}while(0)

                    void push_back(json_val& val)
                    {
                        JSON_ARRAY_REALLOC;
                        data[size++] = val;
                    }

                    void push_back(json_val&& val)
                    {
                        JSON_ARRAY_REALLOC;
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
                    type = json_type::JSON_NULL;
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
                obj.v_array.capacity = obj.v_array.size;            \
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
            /**
             * @brief json_val's constructors
             * @note multiple construct function for different data type
             * @param json_val&,int32_t,double,bool,const char*,const json&,initializer_list
             * @retval None
             */

            json_val(const json_val &val) { JSON_VAL_COPY; }
            json_val(int32_t int_) noexcept : obj(int_) { type = json_type::JSON_INT; }
            json_val(double double_) noexcept : obj(double_) { type = json_type::JSON_DOUBLE; }
            json_val(bool bool_) noexcept : obj(bool_) { type = json_type::JSON_BOOL; }
            json_val(const char *str) noexcept : obj(str) { type = json_type::JSON_STRING; }
            json_val(const json& json_ref) {
                type = json_type::JSON_JSON;
                obj.v_json = new json();
                *obj.v_json = json_ref;
            }
            json_val(::std::initializer_list<json_val> vals) {
                type = json_type::JSON_ARRAY;
                obj.v_array.data = new json_val[vals.size()];
                obj.v_array.capacity = vals.size();
                obj.v_array.size = 0;
                for (const json_val &val: vals) {
                    obj.v_array.data[obj.v_array.size++] = val;
                }
            }
            json_val(::std::nullptr_t)
            {
                clear();
                type = json_type::JSON_NULL;
            }

            /**
             * @brief operator = overload functions
             * @note overload operator = for different data types
             * @param int32_t,double,bool,const char*,string&,initializer_list<json_val>,nullptr,json_val&&,const json_val&
             * @retval None
             */

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
                obj.v_array.capacity = vals.size();
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
                return *this;
            }
            json_val &operator=(const json_val &val) {
                clear();
                JSON_VAL_COPY;
                return *this;
            }

            /**
             * @brief operator different types cast function
             * @note overload cast for different data types
             * @param None
             * @retval None
             */

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
                assert(obj.v_string.data);
                return obj.v_string.data;
            }
            operator ::std::string() const {
                assert(type == json_type::JSON_STRING);
                assert(obj.v_string.data);
                return obj.v_string.data;
            }
            operator bool() const {
                assert(type == json_type::JSON_BOOL);
                return obj.v_bool;
            }

            /**
             * @brief overload [] for different data type
             * @note overload [] for different data type
             * @param int,size_t,const string,const char*
             * @retval json_val&
             */

            json_val& operator [](int index) const
            {
                assert(type == JSON_ARRAY);
                if(obj.v_array.size <= index) {
                    std::cerr << "Out of bound [" << __func__ << "  文件:"<< __FILE__ << "  行号:" << __LINE__ << "]" << std::endl;
                    throw std::exception();
                }
                return obj.v_array.data[index];
            }
            json_val& operator [](size_t index) const
            {
                assert(type == JSON_ARRAY);
                if(obj.v_array.size <= index) {
                    std::cerr << "Out of bound [" << __func__ << "  文件:"<< __FILE__ << "  行号:" << __LINE__ << "]" << std::endl;
                    throw std::exception();
                }
                return obj.v_array.data[index];
            }
            json_val& operator[](const std::string& key) const
            {
                assert(type == JSON_JSON);
                return (*obj.v_json)[key];
            }
            json_val& operator[](const char* key) const
            {
                assert(type == JSON_JSON);
                return (*obj.v_json)[key];
            }

            /**
             * @brief overload ostream's << for json_val
             * @note overload ostream's << for json_val
             * @param ostream& out,json_val& val
             * @retval ostream&
             */
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
                        out << "\"" << val.obj.v_string.data << "\"";
                        break;
                    }
                    case json_type::JSON_ARRAY: {
                        int i;
                        out << "[";
                        if(val.obj.v_array.size > 0)
                        {
                            for (i = 0; i < val.obj.v_array.size - 1; i++)
                            {
                                out << *(val.obj.v_array.data + i) << ",";
                            }
                            if (i == val.obj.v_array.size - 1)
                            {
                                out << *(val.obj.v_array.data + i);
                            }
                        }
                        out << "]";
                        break;
                    }
                    case json_type::JSON_JSON : {
                        out << "{";
                        if(!val.obj.v_json->json_data.empty())
                        {
                            size_t size = val.obj.v_json->json_data.size();
                            for(auto& x : val.obj.v_json->json_data)
                            {
                                out << "\"" << x.first << "\" : " << x.second;
                                if(size-- > 1) out << ",";
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

            /**
             * @brief json_val's to_string function
             * @note to_string function
             * @param None
             * @retval string
             */
            std::string to_string() const
            {
                switch (type) {
                    case json_type::JSON_NULL: {
                        return "null";
                    }
                    case json_type::JSON_INT: {
                        std::ostringstream  stream;
                        stream << obj.v_int;
                        return stream.str();
//                        return std::to_string(obj.v_int);
                    }
                    case json_type::JSON_DOUBLE: {
                        std::ostringstream  stream;
                        stream << obj.v_double;
                        return stream.str();
//                        return std::to_string(obj.v_double);
                    }
                    case json_type::JSON_BOOL: {
                        if (obj.v_bool)
                            return "true";
                        else
                            return "false";
                    }
                    case json_type::JSON_STRING: {
                        std::string s;
                        s += "\"";
                        s += obj.v_string.data;
                        s += "\"";
                        return s;
                    }
                    case json_type::JSON_ARRAY: {
                        int i;
                        std::string s;
                        s += "[";
                        if(obj.v_array.size > 0)
                        {
                            for (i = 0; i < obj.v_array.size - 1; i++) {
                                s += (*(obj.v_array.data + i)).to_string();
                                s += ",";
                            }
                            if (i == obj.v_array.size - 1) {
                                s += (*(obj.v_array.data + i)).to_string();
                            }
                        }
                        s += "]";
                        return s;
                    }
                    case json_type::JSON_JSON : {
                        std::string s;
                        s += "{";
                        if(!obj.v_json->json_data.empty())
                        {
                            size_t size = obj.v_json->json_data.size();
                            for(auto& x : obj.v_json->json_data)
                            {
                                s += "\"";
                                s += x.first;
                                s += "\" : ";
                                s += x.second.to_string();
                                if(size-- > 1) s += ",";
                            }
                        }
                        s += "}";
                        return s;
                    }
                    default:
                        break;
                }
                std::cerr << "hzd::json -> 错误的to_string调用 [" << __func__ << "  文件:"<< __FILE__ << "  行号:" << __LINE__ << "]" << std::endl;
                throw std::exception();
            }

            /**
             * @brief type JSON_ARRAY can push the data to back of its data list
             * @note push_back
             * @param json_val& val_ref
             * @retval None
             */
            void push_back(json_val& val_ref)
            {
                assert(type == JSON_ARRAY);
                obj.v_array.push_back(val_ref);
            }

            /**
             * @brief type JSON_ARRAY can push the data to back of its data list
             * @note use the right reference of its data
             * @param json_val&& val_ref
             * @retval None
             */
            void push_back(json_val&& val_right_ref)
            {
                assert(type == JSON_ARRAY);
                obj.v_array.push_back(std::forward<json_val&&>(val_right_ref));
            }

            /**
             * @brief return size of json_val
             * @note None
             * @param None
             * @retval size_t
             */
            size_t size() const
            {
                if(type == JSON_ARRAY)
                {
                    return obj.v_array.size;
                }
                if(type == JSON_STRING)
                {
                    return obj.v_string.size;
                }
                return 0;
            }

            /**
             * @brief return capacity of json_val
             * @note None
             * @param None
             * @retval size_t
             */
            size_t capacity() const
            {
                if(type == JSON_ARRAY)
                {
                    return obj.v_array.capacity;
                }
                if(type == JSON_STRING)
                {
                    return obj.v_string.capacity;
                }
                return 0;
            }
        };
    public:std::unordered_map<json_key, json_val> json_data;
    private:std::string::iterator context;

        /**
         * @brief parse the white space such as \n \t ' ' \r
         * @note use for skip the white space when parse json string
         * @param json& json_ref
         * @retval void
         */

        static void parse_white_space(json &json_ref) {
            while (*json_ref.context == '\r'
                   || *json_ref.context == '\n'
                   || *json_ref.context == ' '
                   || *json_ref.context == '\t') {
                json_ref.context++;
            }
        }
#define PARSE_WHITE_SPACE parse_white_space(json_ref)
        /**
         * @brief parse many types for json data
         * @note parse many types for json data
         * @param json& json_ref,json_val& val
         * @retval json_ret
         */

        static json_ret parse_null(json &json_ref, json_val &val) {
            if (*json_ref.context != 'n')
            {
                std::cerr << "hzd::json -> 非法字符 [" << __func__ << "  文件:"<< __FILE__ << "  行号:" << __LINE__ << "]" <<  std::endl;
                return JSON_INVALID_VALUE;
            }
            if (*(json_ref.context + 1) != 'u'
                || *(json_ref.context + 2) != 'l'
                || *(json_ref.context + 3) != 'l') {
                std::cerr << "hzd::json -> 非法字符 [" << __func__ << "  文件:"<< __FILE__ << "  行号:" << __LINE__ << "]" << std::endl;
                return JSON_INVALID_VALUE;
            }
            json_ref.context += 4;
            PARSE_WHITE_SPACE;
            val = nullptr;
            return JSON_OK;
        }
        static json_ret parse_bool(json &json_ref, json_val &val) {
            if (*json_ref.context != 't' && *json_ref.context != 'f') {
                std::cerr << "hzd::json -> 非法字符 [" << __func__ << "  文件:"<< __FILE__ << "  行号:" << __LINE__ << "]" << std::endl;
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
                    std::cerr << "hzd::json -> 非法字符 [" << __func__ << "  文件:"<< __FILE__ << "  行号:" << __LINE__ << "]" << std::endl;
                    return JSON_INVALID_VALUE;
                }
                val = false;
                json_ref.context += 5;
            }
            PARSE_WHITE_SPACE;
            return JSON_OK;
        }
        static json_ret parse_number(json &json_ref, json_val &val) {
            if(*(json_ref.context) > '9' || *(json_ref.context) < '0')
            {
                std::cerr << "hzd::json -> 非法字符 [" << __func__ << "  文件:"<< __FILE__ << "  行号:" << __LINE__ << "]" << std::endl;
                return JSON_INVALID_VALUE;
            }
            char *end;
            double tmp;
            #ifdef _MSC_VER
            tmp = strtod(json_ref.context._Ptr,&end);
            #else
            tmp = strtod(json_ref.context.base(), &end);
            #endif
            if (
                    #ifdef _MSC_VER
                    json_ref.context._Ptr == end
                    #else
                    json_ref.context.base() == end
                    #endif
                    ) {
                std::cerr << "hzd::json -> 非法字符 [" << __func__ << "  文件:"<< __FILE__ << "  行号:" << __LINE__ << "]" << std::endl;
                return JSON_INVALID_VALUE;
            }
            if (int(tmp) == tmp) {
                val = int(tmp);
            } else {
                val = tmp;
            }
            #ifdef _MSC_VER
            json_ref.context += (end - json_ref.context._Ptr);
            #else
            json_ref.context += (end - json_ref.context.base());
            #endif
            PARSE_WHITE_SPACE;
            return JSON_OK;
        }

        static void* parse_hex4(json& json_ref,unsigned* u)
        {
            int i;
            *u = 0;
            for (i = 0; i < 4; i++) {
                char ch = *(json_ref.context++);
                *u <<= 4;
                if      (ch >= '0' && ch <= '9')  *u |= ch - '0';
                else if (ch >= 'A' && ch <= 'F')  *u |= ch - ('A' - 10);
                else if (ch >= 'a' && ch <= 'f')  *u |= ch - ('a' - 10);
                else
                {
                    return NULL;
                }
            }
            return &json_ref.context;
        }
        static json_ret parse_string(json& json_ref,json_val &val) {
            if(*json_ref.context != '\"')
            {
                std::cerr << "hzd::json -> 非法字符 [" << __func__ << "  文件:"<< __FILE__ << "  行号:" << __LINE__ << "]" << std::endl;
                return JSON_INVALID_VALUE;
            }
            json_ref.context += 1;
            std::string cur;
            unsigned u,u2;
            while(true) {
                char c = *(json_ref.context++);
                switch (c) {
                    case '\"' : {
                        val = cur;
                        PARSE_WHITE_SPACE;
                        return JSON_OK;
                    }
                    case '\\' : {
                        switch(*(json_ref.context++))
                        {
                            case '\"' : { cur.push_back('\"'); break; }
                            case '\\' : { cur.push_back('\\'); break; }
                            case '/' : { cur.push_back('/'); break; }
                            case 'b' : { cur.push_back('\b'); break; }
                            case 'f' : { cur.push_back('\f'); break; }
                            case 'n' : { cur.push_back('\n'); break; }
                            case 'r' : { cur.push_back('\r'); break; }
                            case 't' : { cur.push_back('\t'); break; }
                            case 'u' : {
                                if(!parse_hex4(json_ref,&u))
                                {
                                    std::cerr << "hzd::json -> parse_hex4(u) 失败 [" << __func__ << "  文件:"<< __FILE__ << "  行号:" << __LINE__ << "]" <<std:: endl;
                                    return JSON_PARSE_INVALID_UNICODE_SURROGATE;
                                }
                                if(u >= 0xD800 && u<= 0xDBFF)
                                {
                                    if(*(json_ref.context++) != '\\')
                                    {
                                        std::cerr << "hzd::json -> json_ref.context != \\ [" << __func__ << "  文件:"<< __FILE__ << "  行号:" << __LINE__ << "]" << std::endl;
                                        return JSON_PARSE_INVALID_UNICODE_SURROGATE;
                                    }
                                    if(*(json_ref.context++) != 'u')
                                    {
                                        std::cerr << "hzd::json -> json_ref.context != u [" << __func__ << "  文件:"<< __FILE__ << "  行号:" << __LINE__ << "]" <<std::endl;
                                        return JSON_PARSE_INVALID_UNICODE_SURROGATE;
                                    }
                                    if(!parse_hex4(json_ref,&u2))
                                    {
                                        std::cerr << "hzd::json -> parse_hex4(u2) 失败 [" << __func__ << "  文件:"<< __FILE__ << "  行号:" << __LINE__ << "]" << std::endl;
                                        return JSON_PARSE_INVALID_UNICODE_HEX;
                                    }
                                    if(u2 < 0xDC00 || u2 > 0xDFFF)
                                    {
                                        std::cerr << "hzd::json -> u2 error [" << __func__ << "  文件:"<< __FILE__ << "  行号:" << __LINE__ << "]" << std::endl;
                                        return JSON_PARSE_INVALID_UNICODE_SURROGATE;
                                    }
                                    u = (((u - 0xD800) << 10) | (u2 - 0xDC00)) + 0x10000;
                                }
                                if(u < 0x7F)
                                {
                                    cur.push_back(u&0xFF);
                                }
                                else if(u <= 0xFF)
                                {
                                    cur.push_back(0xC0 | ((u >> 6) & 0xFF));
                                    cur.push_back(0x80 | ( u       & 0x3F));
                                }
                                else if(u <= 0xFFFF)
                                {
                                    cur.push_back(0xE0 | ((u >> 12) & 0xFF));
                                    cur.push_back(0x80 | ((u >>  6) & 0x3F));
                                    cur.push_back(0x80 | ( u        & 0x3F));
                                }
                                else
                                {
                                    assert(u <= 0x10FFFF);
                                    cur.push_back(0xF0 | ((u >> 18) & 0xFF));
                                    cur.push_back(0x80 | ((u >> 12) & 0x3F));
                                    cur.push_back(0x80 | ((u >>  6) & 0x3F));
                                    cur.push_back(0x80 | ( u        & 0x3F));
                                }
                                break;
                            }
                            default : {
                                std::cout << "hzd::json -> parse_string_\\ default" <<std::endl;
                                return JSON_PARSE_INVALID_UNICODE_SURROGATE;
                            }
                        }
                        break;
                    }
                    case '\0' : {
                        std::cerr << "hzd::json -> 检测到 \\0 [" << __func__ << "  文件:"<< __FILE__ << "  行号:" << __LINE__ << "]" << std::endl;
                        return JSON_PARSE_STRING_0;
                    }
                    default : {
                        cur.push_back(c);
                    }
                }
            }
        }
        static json_ret parse_array(json& json_ref,json_val &val) {
            if(*(json_ref.context) != '[')
            {
                std::cerr << "hzd::json -> 非法字符 [" << __func__ << "  文件:"<< __FILE__ << "  行号:" << __LINE__ << "]" << std::endl;
                return JSON_INVALID_VALUE;
            }
            json_ref.context += 1;
            PARSE_WHITE_SPACE;
            if(*(json_ref.context) == ']') {
                val.type = json_type::JSON_ARRAY;
                val.obj.v_array.data = nullptr;
                val.obj.v_array.size = 0;
                json_ref.context += 1;
                PARSE_WHITE_SPACE;
                return JSON_OK;
            }
            while(true)
            {
                json_val tmp_val;
                json_ret ret;
                PARSE_WHITE_SPACE;
                if((ret = parse_value(json_ref,tmp_val)) != JSON_OK)
                    return ret;
                val.type = JSON_ARRAY;
                val.push_back(tmp_val);
                PARSE_WHITE_SPACE;
                if(*json_ref.context == ',')
                {
                    json_ref.context += 1;
                    PARSE_WHITE_SPACE;
                }
                else if(*json_ref.context == ']')
                {
                    json_ref.context += 1;
                    val.type = json_type::JSON_ARRAY;
                    PARSE_WHITE_SPACE;
                    return JSON_OK;
                }
                else {
                    PARSE_WHITE_SPACE;
                    std::cerr << "hzd::json -> 缺少 , 或 ] [" << __func__ << "  文件:"<< __FILE__ << "  行号:" << __LINE__ << "]" << std::endl;
                    return JSON_MISS_SQUARE;
                }
            }
        }
        static json_ret parse_string_raw(json& json_ref,std::string& str)
        {
            if(*json_ref.context != '\"')
            {
                std::cerr << "hzd::json -> 非法字符 [" << __func__ << "  文件:"<< __FILE__ << "  行号:" << __LINE__ << "]" << std::endl;
                return JSON_INVALID_VALUE;
            }
            json_ref.context += 1;
            while(true) {
                char c = *(json_ref.context++);
                switch (c) {
                    case '\"' : {
                        PARSE_WHITE_SPACE;
                        return JSON_OK;
                    }
                    case '\0' : {
                        std::cerr << "hzd::json -> 检测到 \\0 [" << __func__ << "  文件:"<< __FILE__ << "  行号:" << __LINE__ << "]" << std::endl;
                        return JSON_PARSE_STRING_0;
                    }
                    default : {
                        str.push_back(c);
                    }
                }
            }
        }
        static json_ret parse_json(json& json_ref,json_val& val) {
            if(*(json_ref.context) != '{')
            {
                std::cerr << "hzd::json -> 非法字符 [" << __func__ << "  文件:"<< __FILE__ << "  行号:" << __LINE__ << "]" << std::endl;
                return JSON_INVALID_VALUE;
            }
            json_ret ret;
            json_ref.context += 1;
            PARSE_WHITE_SPACE;
            json* json_new = new json();
            if(*(json_ref.context) == '}') {
                json_ref.context += 1;
                val.type = json_type::JSON_JSON;
                val.obj.v_json = json_new;
                return JSON_OK;
            }
            while(true) {
                if(*(json_ref.context) != '\"') {
                    std::cerr << "hzd::json -> 缺少 键 [" << __func__ << "  文件:"<< __FILE__ << "  行号:" << __LINE__ << "]" << std::endl;
                    ret = JSON_MISS_KEY;
                    break;
                }
                std::string cur;
                if((ret = parse_string_raw(json_ref,cur)) != JSON_OK)
                    break;
                PARSE_WHITE_SPACE;
                if(*json_ref.context != ':') {
                    std::cerr << "hzd::json -> 缺少 : [" << __func__ << "  文件:"<< __FILE__ << "  行号:" << __LINE__ << "]" << std::endl;
                    ret = JSON_MISS_COLON;
                    break;
                }
                json_ref.context += 1;
                PARSE_WHITE_SPACE;
                json_val tmp;
                if((ret = parse_value(json_ref,tmp)) != JSON_OK) {
                    break;
                }
                json_new->json_data.insert(std::pair<std::string,json_val>(cur,tmp));
                PARSE_WHITE_SPACE;
                if(*json_ref.context == ',') {
                    json_ref.context += 1;
                    PARSE_WHITE_SPACE;
                }
                else if(*json_ref.context == '}') {
                    json_ref.context += 1;
                    val.type = json_type::JSON_JSON;
                    val.obj.v_json = json_new;
                    PARSE_WHITE_SPACE;
                    return JSON_OK;
                }
                else
                {
                    std::cerr << "hzd::json -> 缺少 , 或 } [" << __func__ << "  文件:"<< __FILE__ << "  行号:" << __LINE__ << "]" << std::endl;
                    ret = JSON_MISS_CURLY;
                    PARSE_WHITE_SPACE;
                    break;
                }
            }
            json_new->json_data.clear();
            delete json_new;
            return ret;
        }
        static json_ret parse_value(json& json_ref,json_val& val) {
            switch(*json_ref.context) {
                case '\n':
                case '\t':
                case '\r':{ PARSE_WHITE_SPACE; }
                case 't' :
                case 'f' : { return parse_bool(json_ref,val);}
                case 'n' : { return parse_null(json_ref,val);}
                case '\"' : { return parse_string(json_ref,val);}
                case '[' : { return parse_array(json_ref,val);}
                case '{' : { return parse_json(json_ref,val);}
                case '\0' :
                {
                    std::cerr << "hzd::json -> 缺少解析所需的值 [" << __func__ << "  文件:"<< __FILE__ << "  行号:" << __LINE__ << "]" << std::endl;
                    return JSON_PARSE_EXPECT_VALUE;
                }
                default  : { return parse_number(json_ref,val);}
            }
        }
        static inline std::string get_level_str(int level)
        {
            std::string levelStr;
            for (int i = 0; i < level; i++)
            {
                levelStr += "\t";
            }
            return levelStr;
        }
    public:
        /**
         * @brief dump the json data to a string
         * @note dump json data
         * @param None
         * @retval string
         */
        std::string dump() {
            if(json_data.empty()) return "{}";
            std::string json_string;
            size_t size = json_data.size();
            json_string += "{";
            for(auto& kv : json_data)
            {
                json_string += "\"";
                json_string += kv.first;
                json_string += "\"";
                json_string += " : ";
                switch (kv.second.type)
                {
                    case json_type::JSON_NULL:
                    {
                        json_string += "null";
                        break;
                    }
                    case json_type::JSON_INT:
                    case json_type::JSON_DOUBLE:
                    {
                        json_string += kv.second.to_string();
                        break;
                    }
                    case json_type::JSON_BOOL:
                    {
                        if (kv.second.obj.v_bool)
                            json_string += "true";
                        else
                            json_string += "false";
                        break;
                    }
                    case json_type::JSON_STRING:
                    {
                        json_string +="\"";
                        json_string +=kv.second.obj.v_string.data;
                        json_string += "\"";
                        break;
                    }
                    case json_type::JSON_ARRAY:
                    {
                        int i;
                        json_string += "[";
                        for (i = 0; i < kv.second.obj.v_array.size - 1; i++)
                        {
                            json_string += (*(kv.second.obj.v_array.data + i)).to_string();
                            json_string += ",";
                        }
                        if (i == kv.second.obj.v_array.size - 1)
                        {
                            json_string += (*(kv.second.obj.v_array.data + i)).to_string();
                        }
                        json_string += "]";
                        break;
                    }
                    case json_type::JSON_JSON :
                    {
                        json_string += "{";
                        if (!kv.second.obj.v_json->json_data.empty())
                        {
                            size_t size_tmp = kv.second.obj.v_json->json_data.size();
                            for (auto &x: kv.second.obj.v_json->json_data)
                            {
                                json_string += "\"";
                                json_string += x.first;
                                json_string += "\" : ";
                                json_string += x.second.to_string();
                                if (size_tmp-- > 1) json_string += ",";
                            }
                        }
                        json_string += "}";
                        break;
                    }
                    default:
                        break;
                }
                if(size-- > 1)
                {
                    json_string += ",";
                }
            }
            json_string += "}";
            return json_string;
        }

        std::string format()
        {
            std::string s = dump();
            std::string result;
            int level = 0;
            for(std::string::size_type index = 0;index < s.size();index++)
            {
                char c = s[index];
                if(level > 0 && '\n' == s[s.size() - 1])
                {
                    result += get_level_str(level);
                }
                switch(c)
                {
                    case '[':
                    case '{':{
                        result += c;
                        result += '\n';
                        level++;
                        result += get_level_str(level);
                        break;
                    }
                    case ',':{
                        result += c;
                        result += '\n';
                        result += get_level_str(level);
                        break;
                    }
                    case ']':
                    case '}':{
                        result += '\n';
                        level--;
                        result += get_level_str(level);
                        result += c;
                        break;
                    }
                    default : {
                        result += c;
                        break;
                    }
                }
            }
            return result;
        }

        /**
         * @brief load json by parse the json string
         * @note load json by parse the json string
         * @param string& str
         * @retval bool
         */
        bool load(std::string &str) {
            clear();
            context = str.begin();
            json_val val;

            parse_value(*this,val);
            if(val.type == JSON_JSON)
            {
                *this = std::move(*val.obj.v_json);
                return true;
            }
            std::cerr << "hzd::json -> 解析失败 [" << __func__ << "  文件:"<< __FILE__ << "  行号:" << __LINE__ << "]" << std::endl;
            return false;
        }

        /**
         * @brief load json from in file stream
         * @note load json from in file stream
         * @param std::ifstream&
         * @retval bool
         */
        bool load(std::ifstream &in)
        {
            if(!in.is_open())
            {
                std::cerr << "hzd::json -> 文件未打开 [" << __func__ << "  文件:"<< __FILE__ << "  行号:" << __LINE__ << "]" << std::endl;
                return false;
            }
            std::string json_string((std::istreambuf_iterator<char>(in)),
                                    std::istreambuf_iterator<char>());
            return load(json_string);
        }

        /**
         * @brief load json by file name
         * @note load json by file name
         * @param std::string file name
         * @retval bool
         */
        bool load_by_file_name(std::string file_name)
        {
            std::ifstream in(file_name,std::ios::in);
            if(!in.is_open())
            {
                std::cerr << "hzd::json -> 文件没有找到或无法打开 [" << __func__ << "  文件:"<< __FILE__ << "  行号:" << __LINE__ << "]" << std::endl;
                return false;
            }
            bool flag = load(in);
            in.close();
            return flag;
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
                return json_data[key];
        }

        json& operator=(const json& cp_json)
        {
            for (auto &p: cp_json.json_data) {
                json_data.insert(p);
            }
            return *this;
        }

        json& operator=(json&& cp_json_right_ref) noexcept
        {
            for (auto &&p: cp_json_right_ref.json_data) {
                json_data.insert(std::move(p));
            }
            return *this;
        }

        friend std::ostream& operator<<(std::ostream& out,json& json_ref)
        {
            out << json_ref.format();
            return out;
        }

        ~json() {
            json_data.clear();
        }

        /**
         * @brief find key from json_data if find return true else return false
         * @note None
         * @param std::string key
         * @retval bool
         */
        bool has_key(std::string key) const
        {
            if(json_data.find(key) != json_data.end())
            {
                return true;
            }
            else
            {
                return false;
            }
        }

        /**
         * @brief show keys in json
         * @note None
         * @param void
         * @retval std::vector<std::string>
         */
         std::vector<std::string> keys()
        {
             std::vector<std::string> key;
             for(auto it = json_data.begin();it != json_data.end();it++)
             {
                 key.emplace_back(it->first);
             }
             return key;
        }

        /**
         * @brief clear data
         * @note None
         * @param void
         * @retval void
         */
        void clear()
        {
            json_data.clear();
        }
    };

    using json_val = json::json_val;
}

#endif

#ifndef _MSC_VER
#pragma clang diagnostic pop
#endif