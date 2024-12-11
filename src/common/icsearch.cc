//
// Created by lang liu on 2024/12/3.
//

#include "icsearch.h"
#include "logger.h"
#include "elasticlient/client.h"

#include <json/json.h>
#include <cpr/cpr.h>




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

    bool Deserialize(const std::string& src, Json::Value &val)
    {
        Json::CharReaderBuilder builder;
        std::unique_ptr<Json::CharReader> reader(builder.newCharReader());
        std::string err;
        bool ret = reader->parse(src.c_str(), src.c_str() + src.size(), &val, &err);

        return ret;
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
        if (auto resp = _client->index(_name, _type, indexId, body.str()); resp.status_code < 200 || resp.status_code >= 300)
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

ns_im::ESInsert::ESInsert(const std::shared_ptr<elasticlient::Client> &client,
    std::string name,
    std::string type)
        :_client(client), _name(std::move(name)), _type(std::move(type))
{
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
    LOG_TRACE(body.str());

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

ns_im::ESRemove::ESRemove(const std::shared_ptr<elasticlient::Client> &client, std::string name, std::string type)
        :_name(std::move(name))
        ,_type(std::move(type))
        ,_client(client)
{
}


bool ns_im::ESRemove::Remove(const std::string &id)
{
    try
    {
        auto resp = _client->remove(_name, _type, id);
        if  (resp.status_code < 200 || resp.status_code >= 300)
        {
            LOG_ERROR("删除数据 {} 失败, 响应状态码异常: {}", id, resp.status_code);
            return false;
        }
    } catch(const std::exception& e)
    {
        LOG_ERROR("删除数据 {} 失败: {}", id, e.what());
        return false;
    }

    return true;
}

ns_im::ESSearch::ESSearch(std::shared_ptr<elasticlient::Client> &client, std::string name, std::string type)
    :_name(std::move(name)), _type(std::move(type)), _client(client)
{
}

ns_im::ESSearch & ns_im::ESSearch::AppendMustNotTerms(const std::string &key, const std::vector<std::string> &vals)
{
    Json::Value fields;
    for (const auto& val: vals)
    {
        fields[key].append(val);
    }

    Json::Value terms;
    terms["terms"] = fields;
    _must_not.append(terms);
    return *this;
}

ns_im::ESSearch & ns_im::ESSearch::AppendShouldMatch(const std::string &key, const std::string &val)
{
    Json::Value fields;
    fields[key] = val;
    Json::Value match;
    match["match"] = fields;
    _should.append(match);
    return *this;
}

ns_im::ESSearch & ns_im::ESSearch::AppendMustTerm(const std::string &key, const std::string &val)
{
    Json::Value fields;
    fields[key] = val;
    Json::Value term    ;
    term["term"] = fields;
    _must.append(term);
    return *this;
}

ns_im::ESSearch & ns_im::ESSearch::AppendMustMatch(const std::string &key, const std::string &val)
{
    Json::Value fields;
    fields[key] = val;
    Json::Value match;
    match["match"] = fields;
    _must.append(match);
    return *this;
}

Json::Value ns_im::ESSearch::Search()
{
    Json::Value cond;
    if (!_must_not.empty()) cond["must_not"] = _must_not;
    if (!_should.empty()) cond["should"] = _should;
    if (!_must.empty()) cond["must"] = _must;

    Json::Value query;
    query["bool"] = cond;
    Json::Value root;
    root["query"] = query;

    std::stringstream body;
    bool ret = Serialize(root, body);

    if (ret == false)
    {
        LOG_ERROR("索引序列化失败!");
        return Json::nullValue;
    }
    LOG_TRACE(body.str());

    cpr::Response resp;
    try
    {
        resp = _client->search(_name, _type, body.str());
        if (resp.status_code < 200 || resp.status_code >= 300)
        {
            LOG_ERROR("检索数据 {} 失败, 响应状态码异常: {}", body.str(), resp.status_code);
            return Json::nullValue;
        }
    }
    catch(const Json::Exception& e)
    {
        LOG_ERROR("检索数据 {} 失败: {}", body.str(), e.what());
        return Json::nullValue;
    }

    LOG_DEBUG("检索响应正文: [{}]", resp.text);
    Json::Value res;

    ret = Deserialize(resp.text, res);
    if (ret == false)
    {
        LOG_ERROR("检索数据 {} 结果反序列化失败", resp.text);
        return Json::nullValue;
    }

    return res["hits"]["hits"];
}

