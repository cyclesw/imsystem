//
// Created by lang liu on 2024/11/20.
//

#ifndef CHANNEL_H
#define CHANNEL_H

#include <mutex>
#include <unordered_set>
#include <unordered_map>
#include <vector>


namespace brpc
{
  class Channel;
}

namespace ns_im {
class ServiceChannel {
public:
  using ChannelPtr = std::shared_ptr<brpc::Channel>;

  explicit ServiceChannel(std::string name);

  void Append(const std::string &host);

  void Remove(const std::string &host);

  ChannelPtr Choose();

private:
  std::mutex _mutex;
  size_t _index;
  std::string _serviceName;
  std::vector<ChannelPtr> _channels;
  std::unordered_map<std::string, ChannelPtr> _hosts;
};

class ServiceManager {
  using ServiceChannelPtr = std::shared_ptr<ServiceChannel>;

public:
  ServiceManager() = default;

  ServiceChannel::ChannelPtr Choose(const std::string &service_name);

  void Declared(const std::string &service_name);

  void OnServiceOnline(const std::string &service_instance,
                       const std::string &host);

  void OnServiceOffline(const std::string &service_instance,
                        const std::string &host);

private:
  static std::string GetServiceName(const std::string &service);

private:
  std::mutex _mutex;
  std::unordered_set<std::string> _follow_services;
  std::unordered_map<std::string, ServiceChannelPtr> _services;
};
} // namespace ns_im
#endif // CHANNEL_H
