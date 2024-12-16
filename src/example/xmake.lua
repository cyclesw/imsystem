---
--- Created by langliu.
--- DateTime: 2024/12/6 11:40
---

target("passby")
    set_kind("binary")
    add_files("passby.cc")

target("icsearchtest")
    set_kind("binary")
    add_deps("until")
    add_files("es/*.cc")
    add_packages("elasticlient", "spdlog", "jsoncpp")


 target("mq-client")
     set_kind("binary")
     add_files("rabbitmq/consume.cc")
     add_packages("libev", "amqpcpp", "openssl")

 target("mq-publish")
     set_kind("binary")
     add_files("rabbitmq/publish.cc")
     add_packages("libev", "amqpcpp", "openssl")