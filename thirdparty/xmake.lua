add_rules("mode.debug", "mode.release")

target("elasticlient")
    set_kind("static")
    add_files("elasticlient/src/*.cc")
    add_includedirs("elasticlient/include/", { public = true})
    add_packages("cpr", "jsoncpp")