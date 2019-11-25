#include <iostream>
#include <fstream>

#include "./../json.hpp"
#include "./../../utest/utest.hpp"

#include "nlohmann/json.hpp"
#include "rapidjson/document.h"
#include "rapidjson/istreamwrapper.h"

namespace {

    int32_t length = 10000;
    nlohmann::json make_json() {
        nlohmann::json vec1;
        vec1.push_back("1");
        vec1.push_back("2");
        vec1.push_back("3");
        
        nlohmann::json vec2;
        vec2.push_back(4);
        vec2.push_back(5);
        vec2.push_back(6);
        
        nlohmann::json vec3;
        
        {
            nlohmann::json arr3;
            arr3.push_back(7);
            arr3.push_back(8);
            arr3.push_back(9);
            
            vec3.push_back(arr3);
            vec3.push_back(arr3);
            vec3.push_back(arr3);
        }
        
        
        nlohmann::json json;
        json  ["vec1"] = (vec1);
        json  ["vec2"] = (vec2);
        json  ["vec3"] = (vec3);
        
        
        nlohmann::json info;
        info["m_bool"] = (true);
        //info=["m_char", 'c');
        info["m_int8_t"] = (8);
        info["m_int16_t"] = (16);
        info["m_int32_t"] = (32);
        info["m_int64_t"] = (64);
        info["m_uint8_t"] = (8);
        info["m_uint16_t"] = (16);
        info["m_uint32_t"] = (32);
        info["m_uin64_t"] = (64);
        info["m_float"] = (1.0);
        info["m_double"] = (2.22);
        info["m_string"] = ("string txt");
        
        json  ["m_jsoninfo2"] = (info);
        json  ["m_jsoninfo3"] = (info);
        
        
        nlohmann::json jsoninfoclass;
        jsoninfoclass  ["m_jsoninfo1"] = (info);
        jsoninfoclass  ["m_jsoninfo2"] = (info);
        
        json  ["m_jsoninfo2"] = (jsoninfoclass);
        json  ["m_jsoninfo3"] = (jsoninfoclass);
        
        nlohmann::json arr;
        arr.push_back(jsoninfoclass);
        arr.push_back(jsoninfoclass);
        arr.push_back(jsoninfoclass);
        arr.push_back(jsoninfoclass);
        
        json  ["m_jsoninfo4"] = (arr);
        json  ["m_jsoninfo5"] = (arr);
        
        nlohmann::json arr2;
        arr2.push_back(arr);
        arr2.push_back(arr);
        arr2.push_back(arr);
        json  ["m_jsoninfo6"] = (arr2);
        
        return json;
    }
}
TEST(nlohmann_pfms, to_file) {
    nlohmann::json json1 = make_json();
    nlohmann::json arr;
    
    for (int i = 0; i < length; ++i) {
        arr.push_back(json1);
    }
    
    std::fstream ofs("./bigstr.json", std::ios::out | std::ios::binary);
    ofs << arr;
}

TEST(nlohmann_pfms, to_string) {
    nlohmann::json json = make_json();
    std::size_t size = 0;
    
    for (int32_t i = 0; i < length; ++i) {
        size += json.dump(0).size();
    }
}

TEST(nlohmann_pfms, from_string) {
    nlohmann::json json = make_json();
    std::string str = json.dump();
    uint32_t size = 0;
    
    for (int32_t i = 0; i < length; ++i) {
        size = nlohmann::json::parse(str).size();
    }
    
    std::cout << size << std::endl;
}

TEST(nlohmann_pfms, from_file) {
    std::string filename = "./bigstr.json";
    std::ifstream ifs(filename, std::ios::in | std::ios::binary);
    nlohmann::json j;
    ifs >> j;
}


TEST(nlohmann_pfms, array_test) {
    nlohmann::json json1;
    
    for (int i = 0; i < length * 10; ++i) {
        json1.push_back(555);
    }
    
    for (int i = 0; i < length * 10; ++i) {
        json1[i] = (i);
    }
}