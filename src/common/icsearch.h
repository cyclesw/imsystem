//
// Created by lang liu on 2024/12/3.
//

#ifndef ICSEARCH_H
#define ICSEARCH_H

#include <string>
#include <json/json.h>

namespace elasticlient
{
    class Client;
}


namespace ns_im {

    class ESIndex
    {
    public:
        ESIndex(std::shared_ptr<elasticlient::Client> &client,
            std::string  name,
            std::string  type = "_doc" );

        ESIndex &Append(const std::string &key,
                        const std::string &type = "text",
                        const std::string &analyzer = "ik_max_word",
                        bool enable = true);

        bool Create(const std::string &indexId = "default_index_id");
    private:
        std::string _name;
        std::string _type;
        Json::Value _properties;
        Json::Value _index;
        std::shared_ptr<elasticlient::Client> _client;
    };

    class ESInsert
    {
    public:
        ESInsert(const std::shared_ptr<elasticlient::Client>& client,
            std::string  name,
            std::string  type = "_doc"
            );

        template <class T>
        ESInsert& Append(const std::string& key, const T& val)
        {
            _item[key] = val;
            return *this;
        }

        bool Insert(const std::string& id = "");

    private:
        std::shared_ptr<elasticlient::Client> _client;
        Json::Value _item;
        std::string _name;
        std::string _type;
    };

    class ESRemove {
    public:
        ESRemove(const std::shared_ptr<elasticlient::Client> &client,
            std::string name,
            std::string type = "_doc");

        bool Remove(const std::string &id);

    private:
        std::string _name;
        std::string _type;
        std::shared_ptr<elasticlient::Client> _client;
    };

    class ESSearch
    {
    public:
        ESSearch(std::shared_ptr<elasticlient::Client> &client,
            std::string name,
            std::string type = "");

        ESSearch& AppendMustNotTerms(const std::string &key, const std::vector<std::string> &vals);

        ESSearch& AppendShouldMatch(const std::string &key, const std::string &val);

        ESSearch& AppendMustTerm(const std::string &key, const std::string &val);

        ESSearch& AppendMustMatch(const std::string &key, const std::string &val);

        Json::Value Search();
    private:
        std::string _name;
        std::string _type;
        Json::Value _must_not;
        Json::Value _should;
        Json::Value _must;
        std::shared_ptr<elasticlient::Client> _client;
    };

}


#endif //ICSEARCH_H
