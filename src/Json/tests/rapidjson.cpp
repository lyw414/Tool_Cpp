#include <iostream>
#include <fstream>

#include "./../../utest/utest.hpp"

#include "rapidjson/document.h"
#include "rapidjson/istreamwrapper.h"
#include "rapidjson/ostreamwrapper.h"
#include "rapidjson/writer.h"



namespace {

    int32_t length = 10000;
    
    
    template<typename T>  void putPod(rapidjson::Document& doc, const std::string& key, const T& val) {
        rapidjson::Value node;
        node = val;
        
        rapidjson::Value name;
        name.SetString(key.c_str(), doc.GetAllocator());
        doc.AddMember(name, node, doc.GetAllocator());
    }
    
    void putValue(rapidjson::Document& doc, const std::string& key, const rapidjson::Value& val) {
        rapidjson::Value name;
        name.SetString(key.c_str(), doc.GetAllocator());
        doc.AddMember(name, rapidjson::Value().CopyFrom(val,
                      doc.GetAllocator()), doc.GetAllocator());
        return;
        
    }
    
    void pushBack(rapidjson::Document& doc,  const rapidjson::Value& val) {
        rapidjson::Value v;
        v.CopyFrom(val, doc.GetAllocator());
        doc.PushBack(v, doc.GetAllocator());
    }
    
    rapidjson::Document make_json2() {
        rapidjson::Document info;
        info.SetObject();
        
        putPod(info, "m_bool", true);
        putPod(info, "m_int8_t", 8);
        putPod(info, "m_int16_t", 16);
        putPod(info, "m_int32_t", 32);
        putPod(info, "m_int64_t", 64);
        putPod(info, "m_uint8_t", 8);
        putPod(info, "m_uint16_t", 16);
        putPod(info, "m_uint32_t", 32);
        putPod(info, "m_uin64_t", 64);
        putPod(info, "m_float", 1.0);
        putPod(info, "m_double", 2.22);
        
        {
            rapidjson::Value name;
            name.SetString("m_string", info.GetAllocator());
            rapidjson::Value node;
            node.SetString("string txt", info.GetAllocator());
            info.AddMember(name, node, info.GetAllocator());
        }
        
        return info;
    }
    
    rapidjson::Document make_json() {
        rapidjson::Document vec1;
        vec1.SetArray();
        
        {
            rapidjson::Value v;
            v.SetString("1", vec1.GetAllocator());
            vec1.PushBack(v, vec1.GetAllocator());
        }
        {
            rapidjson::Value v;
            v.SetString("2", vec1.GetAllocator());
            vec1.PushBack(v, vec1.GetAllocator());
        }
        {
            rapidjson::Value v;
            v.SetString("3", vec1.GetAllocator());
            vec1.PushBack(v, vec1.GetAllocator());
        }
        
        
        
        rapidjson::Document vec2;
        vec2.SetArray();
        {
            rapidjson::Value v;
            v.SetInt(4);
            vec2.PushBack(v, vec2.GetAllocator());
        }
        {
            rapidjson::Value v;
            v.SetInt(5);
            vec2.PushBack(v, vec2.GetAllocator());
        }
        {
            rapidjson::Value v;
            v.SetInt(6);
            vec2.PushBack(v, vec2.GetAllocator());
        }
        
        
        
        
        rapidjson::Document vec3;
        vec3.SetArray();
        {
            rapidjson::Document arr3;
            arr3.SetArray();
            {
                rapidjson::Value v;
                v.SetInt(7);
                arr3.PushBack(v, arr3.GetAllocator());
            }
            {
                rapidjson::Value v;
                v.SetInt(8);
                arr3.PushBack(v, arr3.GetAllocator());
            }
            {
                rapidjson::Value v;
                v.SetInt(9);
                arr3.PushBack(v, arr3.GetAllocator());
            }
            
            for (int i = 0; i < 3; ++i) {
                rapidjson::Value v;
                v.CopyFrom(arr3, vec3.GetAllocator());
                vec3.PushBack(v, vec3.GetAllocator());
            }
        }
        
        
        rapidjson::Document json;
        json.SetObject();
        
        {
            rapidjson::Value name;
            name.SetString("vec1", json.GetAllocator());
            json.AddMember(name, rapidjson::Value().CopyFrom(vec1, json.GetAllocator()), json.GetAllocator());
        }
        
        {
            rapidjson::Value name;
            name.SetString("vec2", json.GetAllocator());
            json.AddMember(name, rapidjson::Value().CopyFrom(vec2, json.GetAllocator()), json.GetAllocator());
        }
        
        {
            rapidjson::Value name;
            name.SetString("vec3", json.GetAllocator());
            json.AddMember(name, rapidjson::Value().CopyFrom(vec3, json.GetAllocator()), json.GetAllocator());
        }
        
        
        
        rapidjson::Document info;
        info.SetObject();
        
        putPod(info, "m_bool", true);
        putPod(info, "m_int8_t", 8);
        putPod(info, "m_int16_t", 16);
        putPod(info, "m_int32_t", 32);
        putPod(info, "m_int64_t", 64);
        putPod(info, "m_uint8_t", 8);
        putPod(info, "m_uint16_t", 16);
        putPod(info, "m_uint32_t", 32);
        putPod(info, "m_uin64_t", 64);
        putPod(info, "m_float", 1.0);
        putPod(info, "m_double", 2.22);
        
        {
            rapidjson::Value name;
            name.SetString("m_string", vec2.GetAllocator());
            rapidjson::Value node;
            node.SetString("string txt", vec2.GetAllocator());
            info.AddMember(name, node, info.GetAllocator());
        }
        
        
        
        
        putValue(json, "m_jsoninfo2", info);
        putValue(json, "m_jsoninfo3", info);
        
        
        rapidjson::Document jsoninfoclass;
        jsoninfoclass.SetObject();
        
        putValue(jsoninfoclass, "m_jsoninfo1", info);
        putValue(jsoninfoclass, "m_jsoninfo2", info);
        
        putValue(json, "m_jsoninfo2", jsoninfoclass);
        putValue(json, "m_jsoninfo3", jsoninfoclass);
        
        rapidjson::Document arr;
        arr.SetArray();
        pushBack(arr, jsoninfoclass);
        pushBack(arr, jsoninfoclass);
        pushBack(arr, jsoninfoclass);
        pushBack(arr, jsoninfoclass);
        
        putValue(json, "m_jsoninfo4", arr);
        putValue(json, "m_jsoninfo5", arr);
        
        rapidjson::Document arr2;
        arr2.SetArray();
        pushBack(arr2, arr);
        pushBack(arr2, arr);
        pushBack(arr2, arr);
        
        putValue(json, "m_jsoninfo6", arr2);
        
        return json;
    }
}

TEST(rapidjson_pfms, to_file) {
    rapidjson::Document json1 = make_json();
    rapidjson::Document arr;
    arr.SetArray();
    
    for (int i = 0; i < length ; ++i) {
        pushBack(arr, json1);
    }
    
    std::ofstream ofs("./bigstr.json", std::ios::out | std::ios::binary);
    rapidjson::OStreamWrapper osw(ofs);
    rapidjson::Writer<rapidjson::OStreamWrapper> writer(osw);
    arr.Accept(writer);
}

TEST(rapidjson_pfms, to_string) {
    rapidjson::Document json = make_json();
    std::size_t size = 0;
    
    for (int32_t i = 0; i < length; ++i) {
        std::stringstream ss;
        rapidjson::OStreamWrapper osw(ss);
        rapidjson::Writer< rapidjson::OStreamWrapper> writer(osw);
        json.Accept(writer);
        size += ss.str().size();
    }
}

TEST(rapidjson_pfms, from_string) {
    rapidjson::Document json = make_json();
    
    std::stringstream ss;
    rapidjson::OStreamWrapper osw(ss);
    rapidjson::Writer< rapidjson::OStreamWrapper> writer(osw);
    json.Accept(writer);
    std::string str = ss.str();
    
    std::size_t size = 0;
    
    
    for (int32_t i = 0; i < length; ++i) {
        rapidjson::Document doc;
        doc.Parse(str.c_str());
        
        
        /*   std::stringstream ss2(str);
           rapidjson::IStreamWrapper isw(ss2);
           doc.ParseStream(isw);
           */
        size += (str).size();
    }
    
    std::cout << size << std::endl;
}

TEST(rapidjson_pfms, from_string2) {
    rapidjson::Document json = make_json2();
    
    std::stringstream ss;
    rapidjson::OStreamWrapper osw(ss);
    rapidjson::Writer< rapidjson::OStreamWrapper> writer(osw);
    json.Accept(writer);
    std::string str = ss.str();
    
    std::size_t size = 0;
    
    
    for (int32_t i = 0; i < length; ++i) {
        rapidjson::Document doc;
        doc.Parse(str.c_str());
        
        
        /*   std::stringstream ss2(str);
        rapidjson::IStreamWrapper isw(ss2);
        doc.ParseStream(isw);
        */
        size += (str).size();
    }
    
    std::cout << size << std::endl;
}
TEST(rapidjson_pfms, from_file) {
    std::string filename = "./bigstr.json";
    std::ifstream ifs(filename, std::ios::in | std::ios::binary);
    rapidjson::Document doc;
    doc.SetObject();
    rapidjson::IStreamWrapper isw(ifs);
    doc.ParseStream(isw);
    
}
TEST(rapidjson_pfms, array_test) {
    rapidjson::Document doc;
    doc.SetArray();
    rapidjson::Value v;
    v.SetInt(555);
    
    for (int i = 0; i < length * 10; ++i) {
        rapidjson::Value node;
        node.CopyFrom(v, doc.GetAllocator());
        doc.PushBack(v, doc.GetAllocator());
    }
    
    for (int i = 0; i < length * 10; ++i) {
        doc[i].SetInt(i);
    }
}
