#include "Message.h"

using namespace rapidjson;

Message::Message() : flag(0), count(0) {}

void Message::fromStr(const char* jsonStr) {
    Document doc;
    doc.Parse(jsonStr);

    if (!doc.IsObject()) {
        std::cerr << "Invalid JSON format" << std::endl;
        return;
    }

    if (doc.HasMember("flag") && doc["flag"].IsInt())
        flag = doc["flag"].GetInt();

    if (doc.HasMember("msg") && doc["msg"].IsString())
        msg = doc["msg"].GetString();

    if (doc.HasMember("name") && doc["name"].IsString())
        name = doc["name"].GetString();

    if (doc.HasMember("target") && doc["target"].IsString())
        target = doc["target"].GetString();

  /*  if (doc.HasMember("ChatInfo") && doc["ChatInfo"].IsObject()) 
    {
        const Value& chatInfo = doc["ChatInfo"];
        if (chatInfo.HasMember("total") && chatInfo["total"].IsInt())
            count = chatInfo["total"].GetInt();

        if (chatInfo.HasMember("names") && chatInfo["names"].IsArray()) {
            const Value& namesArray = chatInfo["names"];
            for (SizeType i = 0; i < namesArray.Size(); i++) {
                if (namesArray[i].IsString())
                    names.push_back(namesArray[i].GetString());
            }
        }
    }*/
}

string Message::toJsonStr() {
    StringBuffer buffer;
    Writer<StringBuffer> writer(buffer);

    writer.StartObject();
    writer.Key("flag");
    writer.Int(flag);

    writer.Key("msg");
    writer.String(msg.c_str());

    writer.Key("name");
    writer.String(name.c_str());

    writer.Key("target");
    writer.String(target.c_str());

    writer.Key("ChatInfo");
    writer.StartObject();
    writer.Key("total");
    writer.Int(count);

    writer.Key("names");
    writer.StartArray();
    for (const auto& name : names) {
        writer.String(name.c_str());
    }
    writer.EndArray();

    writer.EndObject();
    writer.EndObject();

    return buffer.GetString();
}

void Message::displayMsg() {
    std::cout << "flag: " << flag << std::endl;
    std::cout << "msg: " << msg << std::endl;
    std::cout << "name: " << name << std::endl;
    std::cout << "target: " << target << std::endl;
    std::cout << "total: " << count << std::endl;
    std::cout << "names: ";
    for (const auto& name : names) {
        std::cout << name << " ";
    }
    std::cout << std::endl;
}

Message& Message::operator<<(const std::map<std::string, int>& map)
{
    names.clear();
    for (auto  &it : map)
    {
        names.push_back(it.first);
    }
    return *this;
}
