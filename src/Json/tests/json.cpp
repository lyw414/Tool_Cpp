#include <iostream>



#include "./../json.hpp"
#include "./../../utest/utest.hpp"
#include <fstream>
#include <array>
#include <unordered_map>
#include <thread>


namespace {

    int32_t length = 10000;
    amo::json make_json2() {
        amo::json info;
        info.put("m_bool", true);
        //info.put("m_char", 'c');
        info.put("m_int8_t", 8);
        info.put("m_int16_t", 16);
        info.put("m_int32_t", 32);
        info.put("m_int64_t", 64);
        info.put("m_uint8_t", 8u);
        info.put("m_uint16_t", 16u);
        info.put("m_uint32_t", 32u);
        info.put("m_uin64_t", 64u);
        info.put("m_float", 1.0);
        info.put("m_double", 2.22);
        info.put("m_string", "string txt");
        return info;
    }
    amo::json make_json() {
    
    
    
        amo::json vec1(amo::json_array);
        vec1.push_back("1");
        vec1.push_back("2");
        vec1.push_back("3");
        
        amo::json vec2(amo::json_array);
        vec2.push_back(4);
        vec2.push_back(5);
        vec2.push_back(6);
        
        amo::json vec3(amo::json_array);
        
        {
            amo::json arr3(amo::json_array);
            arr3.push_back(7);
            arr3.push_back(8);
            arr3.push_back(9);
            
            vec3.push_back(arr3);
            vec3.push_back(arr3);
            vec3.push_back(arr3);
        }
        
        
        amo::json json;
        json.put("vec1", vec1);
        json.put("vec2", vec2);
        json.put("vec3", vec3);
        
        
        amo::json info;
        info.put("m_bool", true);
        //info.put("m_char", 'c');
        info.put("m_int8_t", 8);
        info.put("m_int16_t", 16);
        info.put("m_int32_t", 32);
        info.put("m_int64_t", 64);
        info.put("m_uint8_t", 8u);
        info.put("m_uint16_t", 16u);
        info.put("m_uint32_t", 32u);
        info.put("m_uin64_t", 64u);
        info.put("m_float", 1.0);
        info.put("m_double", 2.22);
        info.put("m_string", "string txt");
        
        json.put("m_jsoninfo2", info);
        json.put("m_jsoninfo3", info);
        
        
        amo::json jsoninfoclass;
        jsoninfoclass.put("m_jsoninfo1", info);
        jsoninfoclass.put("m_jsoninfo2", info);
        
        json.put("m_jsoninfo2", jsoninfoclass);
        json.put("m_jsoninfo3", jsoninfoclass);
        
        amo::json arr(amo::json_array);
        arr.push_back(jsoninfoclass);
        arr.push_back(jsoninfoclass);
        arr.push_back(jsoninfoclass);
        arr.push_back(jsoninfoclass);
        
        json.put("m_jsoninfo4", arr);
        json.put("m_jsoninfo5", arr);
        
        amo::json arr2(amo::json_array);
        arr2.push_back(arr);
        arr2.push_back(arr);
        arr2.push_back(arr);
        json.put("m_jsoninfo6", arr2);
        
        return json;
    }
}

namespace amo {
    namespace JSON_IMPL {
        /* template<> inline std::string to_string(const amo::date_time& val) {
        return val.to_string();
        }
        
        template<> inline amo::date_time lexical_cast(const std::string& val) {
        return amo::date_time::from_string(val);
        }
        
        template<> inline std::string to_string(const amo::uuid& val) {
        return val.to_string();
        }
        
        template<> inline amo::uuid lexical_cast(const std::string& val) {
        return amo::uuid::from_string(val);
        }*/
    }
    
    // 自定义类型
    template<> struct json_value_type<amo::ObjectData::value_type > {
        typedef amo::ObjectData::value_type type;
        static const int32_t value = 2;
        json_value_type(const Data& d_) : d(*(Data*) & d_) {
        }
        type get_value()  const {
            return *d.o.elements;
        }
        void set_value(const type& val) {
            d.ss.flag = json_object;
            d.o.elements = new ObjectData::value_type();
            
            for (auto& p : val) {
                d.o.elements->insert({ p.first, p.second.clone() });
            }
        }
        void to_string(std::ostream& stream) const {
            Object object;
            object.d = d;
            return object.to_string(stream);
        };
        Data& d;
    };
    
    // 自定义类型
    template<> struct json_value_type<std::vector<Object> > {
        typedef std::vector<Object> type;
        static const int32_t value = 3;
        json_value_type(const Data& d_) : d(*(Data*) & d_) {
        }
        type get_value()  const {
            return *d.a.elements;
        }
        void set_value(const type& val) {
            d.ss.flag = json_array;
            d.a.elements = new ArrayData::value_type();
            
            for (auto& p : val) {
                d.a.elements->push_back(p.clone());
            }
        }
        void to_string(std::ostream& stream) const {
            Object object;
            object.d = d;
            return object.to_string(stream);
        };
        Data& d;
    };
    
    
}


TEST(json, type) {
    std::string str =   "{\"aaa\":3, \"bbb\":\"3.0\", \"ccc\":true, \"ddd\":null }";
    amo::json json(str);
    json["eee"] = 3;
    json["fff"] = "3.0";
    json["ggg"] = true;
    json["hhh"] = nullptr;
    
    EXPECT_EQ(json["aaa"].type(), amo::value_t::json_integer);
    EXPECT_EQ(json["bbb"].type(), amo::value_t::json_string);
    EXPECT_EQ(json["ccc"].type(), amo::value_t::json_boolean);
    EXPECT_EQ(json["ddd"].type(), amo::value_t::json_null);
    
    EXPECT_EQ(json["eee"].type(), amo::value_t::json_integer);
    EXPECT_EQ(json["fff"].type(), amo::value_t::json_string);
    EXPECT_EQ(json["ggg"].type(), amo::value_t::json_boolean);
    EXPECT_EQ(json["hhh"].type(), amo::value_t::json_null);
    
    
    int32_t a = json["aaa"];
    std::string b = json["bbb"];
    bool c = json["ccc"];
    std::nullptr_t d = (std::nullptr_t)json["ddd"].get<std::nullptr_t>();
    
    int32_t e = 	json["eee"];
    std::string f = json["fff"];
    bool g = json["ggg"];
    std::nullptr_t h =  json["hhh"].get<std::nullptr_t>();
    
    EXPECT_TRUE((int32_t)json["aaa"] == 3);
    EXPECT_EQ(b, "3.0");
    EXPECT_EQ(c, true);
    EXPECT_EQ(json["hhh"].to_string(), "null");
    EXPECT_EQ(e, 3);
    EXPECT_EQ(f, "3.0");
    EXPECT_EQ(g, true);
    EXPECT_TRUE(h == nullptr);
}



TEST(json, misc) {

    amo::json jso2;
    jso2 = 1.0;
    std::string sssss = jso2.to_string();
    std::string str = make_json().to_string();// "{\"aaa\":3, \"bbb\":\"3.0\", \"ccc\":true, \"ddd\":null }";
    amo::json json(str);
    json["eee"] = 3;
    json["fff"] = "3.0";
    json["ggg"] = true;
    json["hhh"] = nullptr;
    EXPECT_EQ(json["aaa"].type(), amo::value_t::empty_type);
    EXPECT_EQ(json["bbb"].type(), amo::value_t::empty_type);
    EXPECT_EQ(json["ccc"].type(), amo::value_t::empty_type);
    EXPECT_EQ(json["ddd"].type(), amo::value_t::empty_type);
    
    EXPECT_EQ(json["eee"].type(), amo::value_t::json_integer);
    EXPECT_EQ(json["fff"].type(), amo::value_t::json_string);
    EXPECT_EQ(json["ggg"].type(), amo::value_t::json_boolean);
    EXPECT_EQ(json["hhh"].type(), amo::value_t::json_null);
    
    
    EXPECT_EQ(json["vec1"].type(), amo::value_t::json_array);
    EXPECT_EQ(json["vec3"].type(), amo::value_t::json_array);
    EXPECT_EQ(json["vec1"][0].type(), amo::value_t::json_string);
    EXPECT_EQ(json["vec2"][0].type(), amo::value_t::json_integer);
    
    amo::json m_jsoninfo1 = json["m_jsoninfo2"]["m_jsoninfo1"];
    
    EXPECT_EQ(m_jsoninfo1["m_bool"].type(), amo::value_t::json_boolean);
    //info.put("m_char", 'c');
    EXPECT_EQ(m_jsoninfo1["m_int8_t"].type(), amo::value_t::json_integer);
    EXPECT_EQ(m_jsoninfo1["m_int16_t"].type(), amo::value_t::json_integer);
    EXPECT_EQ(m_jsoninfo1["m_int32_t"].type(), amo::value_t::json_integer);
    EXPECT_EQ(m_jsoninfo1["m_int64_t"].type(), amo::value_t::json_integer);
    EXPECT_EQ(m_jsoninfo1["m_uint8_t"].type(), amo::value_t::json_integer);
    EXPECT_EQ(m_jsoninfo1["m_uint16_t"].type(), amo::value_t::json_integer);
    EXPECT_EQ(m_jsoninfo1["m_uint32_t"].type(), amo::value_t::json_integer);
    EXPECT_EQ(m_jsoninfo1["m_uin64_t"].type(), amo::value_t::json_integer);
    EXPECT_EQ(m_jsoninfo1["m_float"].type(), amo::value_t::json_float);
    EXPECT_EQ(m_jsoninfo1["m_double"].type(), amo::value_t::json_float);
    EXPECT_EQ(m_jsoninfo1["m_string"].type(), amo::value_t::json_string);
    
    
    
    int32_t a = json["aaa"];
    std::string b = json["bbb"];
    bool c = json["ccc"];
    std::nullptr_t d = (std::nullptr_t)json["ddd"].get<std::nullptr_t>();
    
    int32_t e = json["eee"];
    std::string f = json["fff"];
    bool g = json["ggg"];
    std::nullptr_t h = json["hhh"].get<std::nullptr_t>();
    std::nullptr_t  t = std::nullptr_t();
    std::nullptr_t t2 = t;
    
    if (t == nullptr) {
        std::cout << "null" << std::endl;
    }
}
TEST(json, construct) {

    std::stringstream stream;
    stream << "3";
    amo::json json1;
    amo::json json2(amo::json_array);
    amo::json json3("\"3\"");
    amo::json json4("{\"aa\":1}");
    //amo::json json5({ {"aa", 1}, {"bb", "2"} });
    amo::json json6 = (json4);
    amo::json json7(stream);
    
    std::ifstream ifs("test.json", std::ios::in | std::ios::binary);
    
    amo::json json8(ifs);
    amo::json json9 = json8;
    EXPECT_EQ(json1.type(), amo::json_object);
    EXPECT_EQ(json2.type(), amo::json_array);
    EXPECT_EQ(json3.type(), amo::json_string);
    EXPECT_EQ(json4.type(), amo::json_object);
    //EXPECT_EQ(json5.is_object(), true);
    EXPECT_EQ(json6.type(), amo::json_object);
    EXPECT_EQ(json7.type(), amo::json_integer);
    EXPECT_EQ(json8.is_valid(), false);
    EXPECT_EQ(json9.is_valid(), false);
    
}

TEST(json, join) {
    amo::json json1;
    json1.put("aa", 1);
    json1.put("bb", "2");
    amo::json json2;
    json2.put("aa", 3);
    
    json1.join(json2);
    EXPECT_EQ(json1["aa"].get<int32_t>(), 3);
    
    json2["dd"] = 75;
    amo::json json3 = json1;
    json3["ccc"] = json1;
    json3["ccc"].join(json2);
    EXPECT_EQ(json3["ccc"]["dd"].get<int32_t>(), 75);
}

TEST(json, put_child) {

    amo::json json1;
    json1.put("aa", 1);
    json1.put("bb", "2");
    amo::json json2;
    json2.put("aa", 3);
    json1["cc"] = json2;
    EXPECT_EQ(json1["cc"]["aa"].get<int32_t>(), 3);
    
}


TEST(json, contains_key) {

    amo::json json1;
    json1.put("aa", 1);
    json1.put("bb", "2");
    amo::json json2;
    json2.put("aa", 3);
    json1["cc"] = json2;
    EXPECT_EQ(json1["cc"].contains_key("aa"), true);
    EXPECT_EQ(json1.contains_key("cc"), true);
    EXPECT_EQ(json1.contains_key("dd"), false);
}

TEST(json, keys) {
    amo::json json1;
    json1.put("aa", 1);
    json1.put("bb", "2");
    
    EXPECT_EQ(json1.keys().size(), 2);
}

TEST(json, is) {

    amo::json jso2;
    jso2 = 1.0;
    std::string sssss = jso2.to_string();
    std::string str = make_json().to_string();// "{\"aaa\":3, \"bbb\":\"3.0\", \"ccc\":true, \"ddd\":null }";
    amo::json json(str);
    json["eee"] = 3;
    json["fff"] = "3.0";
    json["ggg"] = true;
    json["hhh"] = nullptr;
    EXPECT_EQ(json["aaa"].is<amo::invalid_t>(), true);
    EXPECT_EQ(json["bbb"].is<amo::invalid_t>(), true);
    EXPECT_EQ(json["ccc"].is<amo::invalid_t>(), true);
    EXPECT_EQ(json["ddd"].is<amo::invalid_t>(), true);
    
    EXPECT_EQ(json["eee"].is<int32_t>(), true);
    EXPECT_EQ(json["fff"].is<std::string>(), true);
    EXPECT_EQ(json["ggg"].is<bool>(), true);
    EXPECT_EQ(json["hhh"].is<std::nullptr_t>(), true);
    
    EXPECT_EQ(json["vec1"].is<amo::json>(), true);
    EXPECT_EQ(json["vec3"].is<amo::json>(), true);
    
    EXPECT_EQ(json["vec1"][0].is<std::string>(), true);
    EXPECT_EQ(json["vec2"][0].is<int32_t>(), true);
    
    amo::json m_jsoninfo1 = json["m_jsoninfo2"]["m_jsoninfo1"];
    
    EXPECT_EQ(m_jsoninfo1["m_bool"].is(amo::value_t::json_boolean), true);
    EXPECT_EQ(m_jsoninfo1["m_int8_t"].is(amo::value_t::json_integer), true);
    EXPECT_EQ(m_jsoninfo1["m_int16_t"].is(amo::value_t::json_integer), true);
    EXPECT_EQ(m_jsoninfo1["m_int32_t"].is(amo::value_t::json_integer), true);
    EXPECT_EQ(m_jsoninfo1["m_int64_t"].is(amo::value_t::json_integer), true);
    EXPECT_EQ(m_jsoninfo1["m_uint8_t"].is(amo::value_t::json_integer), true);
    EXPECT_EQ(m_jsoninfo1["m_uint16_t"].is(amo::value_t::json_integer), true);
    EXPECT_EQ(m_jsoninfo1["m_uint32_t"].is(amo::value_t::json_integer), true);
    EXPECT_EQ(m_jsoninfo1["m_uin64_t"].is(amo::value_t::json_integer), true);
    EXPECT_EQ(m_jsoninfo1["m_float"].is(amo::value_t::json_float), true);
    EXPECT_EQ(m_jsoninfo1["m_double"].is(amo::value_t::json_float), true);
    EXPECT_EQ(m_jsoninfo1["m_string"].is(amo::value_t::json_string), true);
    
}

class aaaa {
public:
    std::string to_string() const {
        return "123123";
    }
};

namespace amo {
    namespace JSON_IMPL {
        template<> inline aaaa from_string(const std::string& val) {
            return aaaa();
        }
        template<> inline std::string to_string<aaaa>(const aaaa& val)   {
            return "123123";
        }
    }
}
TEST(json, custom) {
    amo::json json1;
    json1.put("aa", 1);
    json1.put("bb", "2");
    json1 = aaaa();
    aaaa a = json1;
    
    amo::ObjectData::value_type mp = { {"aa", "char"}, {"bb", 32} };
    json1 = mp;
    std::string str = json1.to_string();
    std::cout << str << std::endl;
    EXPECT_EQ(json1.keys().size(), 2);
    std::vector<amo::Object> vec{ 12, "ccc", 3423, "123", true, 2.34, std::nullptr_t() };
    json1 = vec;
    std::cout << json1.to_string() << std::endl;
    
    json1 = amo::json(json1.to_string());
    std::cout << json1.to_string() << std::endl;
}



//
//TEST(json, put_get) {
//    {
//        amo::uuid id;
//        amo::date_time now;
//        amo::json json1;
//        json1.put("aa", 1);
//        json1.put("bb", "2");
//        json1.put("cc", true);
//        json1.put("dd", 1.1);
//        json1.put("ee", id);
//        json1.put("ff", now);
//        json1.put("gg", amo::json());
//        std::cout << json1.to_string() << std::endl;
//
//        EXPECT_EQ(json1["ff"].get<amo::date_time>().to_string(), now.to_string());
//        EXPECT_EQ(json1["ee"].get<amo::uuid>().to_string(), id.to_string());
//    }
//
//    {
//        amo::uuid id;
//        amo::date_time now;
//        amo::json json1;
//        json1 = (1);
//        json1 = ("2");
//        json1 = (true);
//        json1 = (1.1);
//
//        json1 = (now);
//
//
//
//        EXPECT_EQ(json1.get<amo::date_time>().to_string(), now.to_string());
//        json1 = (id);
//        EXPECT_EQ(json1.get<amo::uuid>().to_string(), id.to_string());
//        json1 = (amo::json());
//    }
//
//    {
//        amo::uuid id;
//        amo::date_time now;
//        amo::json json1;
//        json1.put("aa", 1);
//        json1.put("bb", "2");
//        json1.put("cc", true);
//        json1.put("dd", 1.1);
//        json1.put("ee", id);
//        json1.put("ff", now);
//        json1.put("gg", amo::json());
//        json1.put(u8"中文KEY", u8"中文值");
//        std::cout << amo::u8string(json1.to_string(), true).to_locale() << std::endl;
//
//        EXPECT_EQ(json1["ff"].get<amo::date_time>().to_string(), now.to_string());
//        EXPECT_EQ(json1["ee"].get<amo::uuid>().to_string(), id.to_string());
//        EXPECT_EQ(json1[u8"中文KEY"].get<std::string>(), u8"中文值");
//    }
//
//    {
//        amo::uuid id;
//        amo::date_time now;
//        amo::json json1;
//        json1 = (1);
//        json1 = ("2");
//        json1 = (true);
//        json1 = (1.1);
//        json1 = (now);
//
//
//
//        EXPECT_EQ(json1.get<amo::date_time>().to_string(), now.to_string());
//        json1 = (id);
//        EXPECT_EQ(json1.get<amo::uuid>().to_string(), id.to_string());
//        json1 = (u8"中文值");
//        EXPECT_EQ(json1.get<std::string>(), u8"中文值");
//
//    }
//}

TEST(json, array_option) {
    amo::json vec(amo::json_array);
    vec.push_back(1);
    vec.push_back("1");
    vec.push_back(amo::json());
    
    EXPECT_EQ(vec.size(), 3);
    EXPECT_EQ(vec[0].get<int32_t>(), 1);
    EXPECT_EQ(vec[1].get<std::string>(), "1");
    EXPECT_EQ(vec[2].type(), amo::json_object);
}

TEST(json, to_string) {
    amo::json json1(amo::json_array);
    json1.push_back(1);
    json1.push_back("1");
    json1.push_back(amo::json());
}

TEST(json, format) {
    amo::json json1(amo::json_array);
    //json1.set_array();
    json1.push_back(1);
    json1.push_back("1");
    amo::json json2;
    json2.put("aa", 3);
    json2.put("bb", true);
    json2.put("cc", "dddd");
    json1.push_back(json2);
    json1.push_back(json2);
    json1.push_back(json2);
    
    std::fstream ifs("./format.json", std::ios::in);
    amo::json json3(ifs);
}


//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////

TEST(json_pfms, to_file) {
    amo::json json1 = ::make_json();
    amo::json arr(amo::json_array);
    
    for (int i = 0; i < length / 10; ++i) {
        arr.push_back(json1);
    }
    
    std::shared_ptr< std::ostream> ofs(new std::fstream("./bigstr.json", std::ios::out | std::ios::binary));
    arr.to_string(ofs);
}

TEST(json_pfms, to_string) {
    amo::json json = make_json();
    std::size_t size = 0;
    
    for (int32_t i = 0; i < length / 10; ++i) {
        size += json.to_string(0).size();
    }
}

TEST(json_pfms, from_string) {




    amo::json json = make_json();
	 
    std::string str = json.to_string();
	 
    std::string str2 = str;
    std::size_t size = 0;
    
    //    for (int32_t i = 0; i < length; ++i) {
    //        amo::json json2(str);
    //        size += str.size();
    //    }
    //
    typedef const char* stream_type;
    int32_t count = 1;
    
    for (int32_t i = 0; i < length; ++i) {
        amo::json json2(str);
    }
    
    //typedef std::stringstream stream_type;
    //int32_t count = 0;
    //
    //for (int32_t i = 0; i < length; ++i) {
    //    stream_type val(str.c_str());
    //    amo::char_reader<stream_type> reader(&val);
    //    amo::tokenizer<stream_type> tokenizr(&reader);
    //    amo::parser<stream_type> parser(&tokenizr);
    //    const amo::Object& token = tokenizr.next();
    //
    //    if (!tokenizr.has_more()) {
    //        break;
    //    }
    //
    //    /*while (true) {
    //    	const amo::Object& token = tokenizr.next();
    //    	++count;
    //
    //    	if (!tokenizr.has_more()) {
    //    		break;
    //    	}
    //    }*/
    //}
    //
    //std::cout << "count: " << count << std::endl;
}



TEST(json_pfms, from_string2) {




    amo::json json = make_json2();
    std::string str = json.to_string();
    std::string str2 = str;
    std::size_t size = 0;
    
    
    typedef const char* stream_type;
    int32_t count = 1;
    
    for (int32_t i = 0; i < length; ++i) {
        amo::json json1(str);
    }
    
    
    //std::cout << "count: " << count << std::endl;
}

TEST(json_pfms, from_file) {

    std::ifstream ifs("./bigstr.json", std::ios::in | std::ios::binary);
    amo::json json(ifs);
}

TEST(json_pfms, array_test) {
    amo::json arr(amo::json_array);
    
    for (int i = 0; i < length * 10; ++i) {
        arr.push_back(555);
    }
    
    for (int i = 0; i < length * 10; ++i) {
        arr[i] = i;
    }
}


TEST(json_pfms, vec_test) {

    std::vector<amo::Object> vec;
    
    for (int32_t i = 0; i < length; ++i) {
        vec.emplace_back(amo::Object());
    }
}

TEST(json_pfms, vec_pair_test) {

    std::vector<std::pair<std::string, amo::Object> > vec;
    
    for (int32_t i = 0; i < length; ++i) {
        vec.emplace_back(std::make_pair(std::to_string(i), amo::Object()));
    }
    
    
    /* for (int32_t i = 0; i < length2; ++i) {
         std::string str = std::to_string(i);
    
         for (auto&p : vec) {
             if (str == p.first) {
                 break;
             }
         }
     }*/
}

TEST(json_pfms, map_test) {
    amo::ObjectData::value_type vec;
    
    for (int32_t i = 0; i < length; ++i) {
        //std::cout << std::hash<std::string>()(std::to_string(i)) << std::endl;
        vec.insert({ std::to_string(i), amo::Object() });
    }
    
    for (int32_t i = 0; i < 1000; ++i) {
        std::string str = std::to_string(i);
        auto iter = vec.find(str);
        
        if (iter != vec.end()) {
            continue;
        }
    }
}

TEST(json_pfms, unordered_map_test) {

    std::unordered_map<std::string, amo::Object> vec;
    
    for (int32_t i = 0; i < length; ++i) {
        vec.insert({ std::to_string(i), amo::Object() });
    }
    
    for (int32_t i = 0; i < length; ++i) {
        std::string str = std::to_string(i);
        auto iter = vec.find(str);
        
        if (iter != vec.end()) {
            continue;
        }
    }
}



//
//TEST(json_pfms, new_vec_pair_test) {
//
//    for (int i = 0; i < length; ++i) {
//        std::vector<std::pair<std::string, amo::Object> >* vec = new std::vector<std::pair<std::string, amo::Object>>();
//
//        for (int32_t i = 0; i < 100; ++i) {
//            vec->emplace_back(std::make_pair("", amo::Object()));
//        }
//
//    }
//}
//
//TEST(json_pfms, new_map_test) {
//    for (int i = 0; i < length; ++i) {
//        std::map<std::string, amo::Object>* vec = new std::map<std::string, amo::Object>();
//
//        for (int32_t i = 0; i < 100; ++i) {
//            vec->insert({ std::to_string(i), amo::Object() });
//        }
//    }
//
//
//}
//
//TEST(json_pfms, new_vec_object_test2) {
//    int32_t count = 0;
//
//    for (int i = 0; i < length; ++i) {
//        std::vector<FastObject > vec ;
//
//        for (int32_t i = 0; i < 100; ++i) {
//            //count += std::to_string(i).size();
//            vec.emplace_back(FastObject());
//        }
//
//    }
//}
//
//TEST(json_pfms, new_vec_object_test) {
//    int32_t count = 0;
//
//    for (int i = 0; i < length; ++i) {
//        std::vector<FastObject >* vec = new std::vector<FastObject>();
//
//        for (int32_t i = 0; i < 100; ++i) {
//            count +=   std::to_string(i).size();
//            vec->emplace_back(FastObject());
//        }
//
//    }
//}

//
//TEST(json_pfms, new__emap_test) {
//    for (int32_t i = 0; i < length; ++i) {
//        std::map<std::string, amo::Object> vec;
//        vec.insert({ std::to_string(i), amo::Object() });
//
//        for (int32_t i = 0; i < 100; ++i) {
//            vec.insert({ std::to_string(i), amo::Object() });
//        }
//    }
//}
//
//
//TEST(json_pfms, new_eunordered_map_test) {
//
//
//    for (int32_t i = 0; i < length; ++i) {
//        std::unordered_map<std::string, amo::Object> vec;
//
//        vec.insert({ std::to_string(i), amo::Object() });
//
//        for (int32_t i = 0; i < 100; ++i) {
//
//            vec.insert({ std::to_string(i), amo::Object() });
//        }
//    }
//
//
//}
//
//
//
//TEST(json_pfms, new_unordered_map_test) {
//
//    for (int i = 0; i < length; ++i) {
//        std::unordered_map<std::string, amo::Object>* vec = new std::unordered_map<std::string, amo::Object>();
//
//        for (int32_t i = 0; i < 100; ++i) {
//
//            vec->insert({ std::to_string(i), amo::Object() });
//        }
//    }
//}