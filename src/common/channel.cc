//
// Created by lang liu on 2024/11/20.
//

#include "channel.h"

#include "logger.h"
#include <brpc/channel.h>
#include <brpc/log.h>
#include <etcd/Client.hpp>

using namespace ns_im;

ServiceChannel::ServiceChannel(std::string name)
    : _index(0), _serviceName(std::move(name)) {}

void ServiceChannel::Append(const std::string &host) {
  auto channel = std::make_shared<brpc::Channel>();
  brpc::ChannelOptions options;
  options.connect_timeout_ms = -1;
  options.timeout_ms = -1;
  options.max_retry = 3;
  options.protocol = brpc::PROTOCOL_BAIDU_STD;

  int ret = channel->Init(host.c_str(), &options);
  if (ret == -1) {
    LOG_ERROR("初始化{}:{}信道失败", _serviceName, host);
    return;
  }

  std::lock_guard lock(_mutex);
  _hosts.emplace(host, channel);
  _channels.push_back(channel);
}

void ServiceChannel::Remove(const std::string &host) {
  std::lock_guard lock(_mutex);
  if (!_hosts.contains(host))
    return;

  const auto it = std::ranges::find_if(_channels, [&host, this](const ChannelPtr &channel) {
    return channel == _hosts.at(host);
  });

  if (it != _channels.end())
    _channels.erase(it);
}

ServiceChannel::ChannelPtr ServiceChannel::Choose() {
  std::lock_guard lock(_mutex);
  if (_channels.empty()) {
    LOG_ERROR("当前没有提供 {} 的节点", _serviceName);
    return {};
  }
  auto index = _index++ % _channels.size();
  return _channels[index];
}

ServiceChannel::ChannelPtr
ServiceManager::Choose(const std::string &service_name) {
  std::lock_guard lock(_mutex);

  const auto sit = _services.find(service_name);
  if (sit == _services.end()) {
    LOG_ERROR("当前没有能够提供 {} 服务的节点！", service_name);
    return {};
  }

  return sit->second->Choose();
}

void ServiceManager::Declared(const std::string &service_name) {
  std::lock_guard lock(_mutex);
  _follow_services.emplace(service_name);
  LOG_TRACE("定义了 {} 服务", service_name);
}

void ServiceManager::OnServiceOnline(const std::string &service_instance,
                                     const std::string &host) {
  std::string service_name = GetServiceName(service_instance);
  ServiceChannelPtr service;
  {
    std::lock_guard lock(_mutex);
    if (!_follow_services.contains(service_name)) {
      LOG_DEBUG("{}-{} 服务上线了，但是当前并不关心！", service_name, host);
      return;
    }

    auto it = _services.find(service_name);
    if (it == _services.end()) {
      service = std::make_shared<ServiceChannel>(service_name);
      _services.emplace(service_name, service);
    } else
      service = it->second;
  }

  if (!service) {
    LOG_ERROR("新增 {} 服务管理节点失败！", service_name);
    return;
  }

  service->Append(host);
  LOG_DEBUG("{}-{} 服务上线新节点，进行添加管理！", service_name, host);
}

void ServiceManager::OnServiceOffline(const std::string &service_instance,
                                      const std::string &host) {
  std::string service_name = GetServiceName(service_name);
  ServiceChannelPtr service;
  {
    std::lock_guard lock(_mutex);
    if (!_follow_services.contains(service_name)) {
      LOG_DEBUG("{}-{} 服务下线了，但是当前并不关心！", service_name, host);
      return;
    }

    if (!_services.contains(host)) {
      LOG_WARN("删除{}服务节点时，没有找到管理对象", service_name);
      return;
    }
    service = _services[service_name];
  }

  service->Remove(host);
  LOG_TRACE("{}-{} 服务下线节点，进行删除管理！", service_name, host);
}

std::string ServiceManager::GetServiceName(const std::string &service) {
  auto pos = service.find_last_of('/');
  if (pos == std::string::npos)
    return service;
  return service.substr(0, pos);
}
