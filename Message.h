#pragma once

#include <string>
#include <iostream>
#include <vector>
#include <map>
#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"

using std::string;
using std::vector;
class Message
{
public:
    Message();
    void fromStr(const char*);
    string toJsonStr();
    void displayMsg();
    Message& operator<<(const std::map<std::string, int> &map);

    int flag;
    string msg;
    string name;
    string target;
    int count;
    vector<string> names;
};

