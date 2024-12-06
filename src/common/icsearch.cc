//
// Created by lang liu on 2024/12/3.
//

#include <json/json.h>
#include <cpr/cpr.h>
#include "elasticlient/client.h"

#include "icsearch.h"
#include "logger.h"


namespace
{
    bool Serialize(const Json::Value &val, std::stringstream& ss)
    {
        Json::StreamWriterBuilder builder;
        builder.settings_["emitUTF8"] = true;

        std::unique_ptr<Json::StreamWriter> writer(builder.newStreamWriter());

        int ret = writer->write(val, &ss);
        if (ret != 0)
            return false;

        return true;
    }
}

ns_im::ESIndex::ESIndex(std::shared_ptr<elasticlient::Client> &client,
    std::string name,
    std::string type)
        :_name(std::move(name))
        ,_type(std::move(type))
        ,_client(client)
{
    Json::Value analysis;
    Json::Value analyzer;
    Json::Value ik;
    Json::Value tokenizer;
    tokenizer["tokenizer"] = "ik_max_word";
    ik["ik"] = tokenizer;
    analyzer["analyzer"] = ik;
    analysis["analysis"] = analyzer;
    _index["settings"] = analysis;
}

ns_im::ESIndex & ns_im::ESIndex::Append(const std::string &key, const std::string &type, const std::string &analyzer, bool enable)
{
    Json::Value field;
    field["type"] = type;
    field["analyzer"] = analyzer;

    if (!enable) field["enabled"] = enable;
    _properties[key] = field;

    return *this;
}

bool ns_im::ESIndex::Create(const std::string &indexId)
{
    Json::Value mappings;
    mappings["dynamic"] = true;
    mappings["properties"] = _properties;
    _index["mappings"] = mappings;

    std::stringstream body;

    bool ret = Serialize(_index, body);
    if (!ret)
    {
        LOG_ERROR("索引序列化失败!");
        return false;
    }

    LOG_DEBUG(body.str());

    try
    {
        auto resp = _client->index(_name, _type, indexId, body.str());
        if (resp.status_code < 200 || resp.status_code >= 300)
        {
            LOG_ERROR("创建ES索引 {} 失败, 响应状态码异常: {}", _name, resp.status_code);
            return false;
        }
    }
    catch(const std::exception& e)
    {
        LOG_ERROR("创建ES索引 {} 失败: {}", _name, e.what());
        return false;
    }

    return true;
}

bool ns_im::ESInsert::Insert(const std::string &id)
{
    std::stringstream body;
    bool ret = Serialize(_item, body);
    if (!ret)
    {
        LOG_ERROR("索引序列化失败!");
        return false;
    }
    LOG_DEBUG(body.str());

    try
    {
        auto resp = _client->index(_name, _type, id, body.str());
        if (resp.status_code < 200 || resp.status_code >= 300)
        {
            LOG_ERROR("新增数据 {} 失败， 响应状态码异常: {}", body.str(), resp.status_code);
            return false;
        }
    }
    catch (std::exception& e)
    {
        LOG_ERROR("新增数据 {} 失败: ", body.str(), e.what());
        return false;
    }

    return true;
}
