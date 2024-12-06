//
// Created by lang liu on 2024/12/3.
//

#include "etcd.h"

#include <etcd/Client.hpp>
#include <etcd/KeepAlive.hpp>
#include <etcd/Response.hpp>
#include <etcd/Watcher.hpp>

#include "logger.h"

using namespace ns_im;

ServiceRegistry::ServiceRegistry(std::string &host, std::string const& username, std::string const& password)
    : _client(std::make_shared<etcd::Client>(host, username, password))
    , _keepalive(_client->leasekeepalive(3).get())
    , _leaseId(_keepalive->Lease())
{}

bool ServiceRegistry::Registry(const std::string &key, const std::string &value) const
{
    auto resp = _client->put(key, value).get();
    if (!resp.is_ok())
    {
        LOG_ERROR("注册服务数据失败: {}", resp.error_message());
        return false;
    }
    return true;
}

ServiceRegistry::~ServiceRegistry()
{
    _keepalive->Cancel();
}

ServiceDiscovery::ServiceDiscovery(const std::string &host, const std::string &username, const std::string &password, const std::string &basedir, NotifyCallback put_callback, NotifyCallback delete_callback)
    :_put_callback(std::move(put_callback))
    ,_delete_callback(std::move(delete_callback))
    ,_client(std::make_shared<etcd::Client>(host, username, password))
{
    auto resp = _client->ls(basedir).get();
    if (!resp.is_ok())
    {
        LOG_ERROR("获取服务数据失败: {}", resp.error_message());
        throw std::runtime_error(resp.error_message());
    }

    size_t sz = resp.keys().size();
    for (size_t i = 0; i < sz; ++i)
    {
        if (_put_callback)
            _put_callback(resp.key(i), resp.value(i).as_string());
    }
    _watcher = std::make_shared<etcd::Watcher>(*_client, basedir,
        [this](const etcd::Response & response) { Callback(response); }, true);
}

ServiceDiscovery::~ServiceDiscovery()
{
    _watcher->Cancel();
}

void ServiceDiscovery::Callback(const etcd::Response &response) const
{
    if (!response.is_ok())
    {
        LOG_ERROR("收到了一个错误事件通知: {}", response.error_message());
        return;
    }

    for (const auto& ev : response.events())
    {
        auto& key = ev.kv().key();
        auto& value = ev.kv().as_string();

        if (ev.event_type() == etcd::Event::EventType::PUT)
        {
            if (_put_callback)
                _put_callback(key, value);
            LOG_TRACE("新增服务: {}:{}", key, value);
        }
        if (ev.event_type() == etcd::Event::EventType::DELETE_)
        {
            if (_delete_callback)
                _delete_callback(key, value);
            LOG_TRACE("删除服务: {}:{}", key, value);
        }
    }
}
