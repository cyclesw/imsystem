//
// Created by lang liu on 2024/12/3.
//

#ifndef ETCD_H
#define ETCD_H

#include <functional>



namespace etcd
{
    class KeepAlive;
    class Watcher;
    class Client;
    class Response;
}

namespace ns_im
{
    class ServiceRegistry
    {
    public:
        using ServiceRegistryPtr = std::shared_ptr<ServiceRegistry>;

        explicit ServiceRegistry(std::string& host, std::string const& username = "", std::string const& password = "");

        ~ServiceRegistry();

        [[nodiscard]] bool Registry(const std::string& key, const std::string& value) const;
    private:
        std::shared_ptr<etcd::Client> _client;
        std::shared_ptr<etcd::KeepAlive> _keepalive;
        uint64_t _leaseId;
    };

    class ServiceDiscovery
    {
    public:
        using ServiceDiscoveryPtr = std::shared_ptr<ServiceDiscovery>;
        using NotifyCallback = std::function<void(std::string, std::string)>;

        ServiceDiscovery(const std::string& host,
                    const std::string& username,
                    const std::string& password,
                    const std::string& basedir,
                    NotifyCallback put_callback,
                    NotifyCallback delete_callback);

        ~ServiceDiscovery();

    private:
        void Callback(const etcd::Response& response) const;

    private:
        NotifyCallback _put_callback, _delete_callback;
        std::shared_ptr<etcd::Client> _client;
        std::shared_ptr<etcd::Watcher> _watcher;
    };
}


#endif //ETCD_H
