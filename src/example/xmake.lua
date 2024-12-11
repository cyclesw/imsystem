---
--- Created by langliu.
--- DateTime: 2024/12/6 11:40
---

target ("passby")
    add_files("passby.cc")

target ("icsearchtest")
    add_deps("until")
    add_files("es/*.cc")
    add_packages("elasticlient", "spdlog", "jsoncpp")
