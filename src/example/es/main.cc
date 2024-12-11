//
// Created by lang liu on 2024/12/10.
//

#include <iostream>
#include <json/json.h>

#include "../../common/logger.h"
#include <elasticlient/client.h>

#include "../../common/icsearch.h"


/*!
 * @brief
 * • 在 8.0 版本之前，Elasticsearch 支持基于类型的 API 调用，
 *   例如   <target>/<type>/_search  。
 *   从 8.0 版本开始，Elasticsearch 完全移除了基于类型的 API，转而使用无类型的 API。
 *   这意味着所有的搜索请求都应该使用   <target>/_search   而不是   <target>/<type>/_search  。
 * @return 0
 */
int main()
{
    using namespace ns_im;

    ns_log::Logger::InitLogger(false, ns_log::LogLevel::trace);

    std::vector<std::string> hosts = {"http://127.0.0.1:9200/"};
    auto client = std::make_shared<elasticlient::Client>(hosts);

    bool ret = ESIndex(client, "test_user")
    .Append("nickname")
    .Append("phone", "keyword", "standard", true)
    .Create();
    if (ret == false) {
        LOG_INFO("索引创建失败!");
        return -1;
    }
    LOG_INFO("索引创建成功!");

    //数据的新增
    ret = ESInsert(client, "test_user")
        .Append("nickname", "张三")
        .Append("phone", "15566667777")
        .Insert("00001");
    if (ret == false) {
        LOG_ERROR("数据插入失败!");
        return -1;
    }
    LOG_INFO("数据新增成功!");

    //数据的修改
    ret = ESInsert(client, "test_user")
        .Append("nickname", "张三")
        .Append("phone", "13344445555")
        .Insert("00001");
    if (ret == false) {
        LOG_ERROR("数据更新失败!");
        return -1;
    }
    LOG_INFO("数据更新成功!");

    Json::Value user = ESSearch(client, "test_user")
        .AppendShouldMatch("phone.keyword", "13344445555")
        // .AppendMustNotTerms("nickname.keyword", {"张三"})
        .Search();
    if (user.empty() || user.isArray() == false) {
        LOG_ERROR("结果为空，或者结果不是数组类型");
        return -1;
    } else {
        LOG_INFO("数据检索成功!");
    }

    int sz = user.size();
    LOG_DEBUG("检索结果条目数量：{}", sz);
    for (int i = 0; i < sz; i++) {
        LOG_INFO("nickname: {}", user[i]["_source"]["nickname"].asString());
    }

    ret = ESRemove(client, "test_user").Remove("00001");
    if (ret == false) {
        LOG_ERROR("删除数据失败");
        return -1;
    }
    LOG_INFO("数据删除成功!");

    return 0;
}
