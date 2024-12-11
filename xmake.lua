---@diagnostic disable: undefined-global
add_imports("moudle")
add_rules("mode.debug", "mode.release")

set_languages("c++20")

if is_mode("debug") then
    add_defines("_DEBUG")
    set_symbols("debug")
    set_optimize("none")
end
-- TODO:
-- 1. 从包管理器获取，不要让用户自己手动构建依赖。
-- 2. 使用静态库
-- 3. 运行 bootstrap-vcpkg.sh

add_requires("spdlog")
add_requires("jsoncpp")
add_requires("cpr")
add_requires("cpp-httplib")
add_requires("brpc", {system = true})
add_requires("protobuf", {system = true, alias = "protobuf-cpp"})
add_requires("vcpkg::etcd-cpp-apiv3",{ alias = "etcd-app-api"} )

includes("thirdparty")

option("enalbe_example")
    set_default(true)
    set_description("一些用法测试")
    set_showmenu(true)


if is_config("enalbe_example", true) then
    includes("src/example")
end


target("until")
    set_kind("static")
    add_files("src/common/*.cc")
    add_deps("elasticlient")
    add_packages("spdlog","protobuf-cpp", "brpc", "etcd-cpp-api", "jsoncpp", "cpr")
    set_policy("build.merge_archive", true)



target("ChatSystem")
    set_kind("binary")
    add_files("src/*.cc")
    add_deps("until")
    add_packages("spdlog")


includes("src/example")

--
-- If you want to known more usage about xmake, please see https://xmake.io
--
-- ## FAQ
--
-- You can enter the project directory firstly before building project.
--
--   $ cd projectdir
--
-- 1. How to build project?
--
--   $ xmake
--
-- 2. How to configure project?
--
--   $ xmake f -p [macosx|linux|iphoneos ..] -a [x86_64|i386|arm64 ..] -m [debug|release]
--
-- 3. Where is the build output directory?
--
--   The default output directory is `./build` and you can configure the output directory.
--
--   $ xmake f -o outputdir
--   $ xmake
--
-- 4. How to run and debug target after building project?
--
--   $ xmake run [targetname]
--   $ xmake run -d [targetname]
--
-- 5. How to install target to the system directory or other output directory?
--
--   $ xmake install
--   $ xmake install -o installdir
--
-- 6. Add some frequently-used compilation flags in xmake.lua
--
-- @code
--    -- add debug and release modes
--    add_rules("mode.debug", "mode.release")
--
--    -- add macro definition
--    add_defines("NDEBUG", "_GNU_SOURCE=1")
--
--    -- set warning all as error
--    set_warnings("all", "error")
--
--    -- set language: c99, c++11
--    set_languages("c99", "c++11")
--
--    -- set optimization: none, faster, fastest, smallest
--    set_optimize("fastest")
--
--    -- add include search directories
--    add_includedirs("/usr/include", "/usr/local/include")
--
--    -- add link libraries and search directories
--    add_links("tbox")
--    add_linkdirs("/usr/local/lib", "/usr/lib")
--
--    -- add system link libraries
--    add_syslinks("z", "pthread")
--
--    -- add compilation and link flags
--    add_cxflags("-stdnolib", "-fno-strict-aliasing")
--    add_ldflags("-L/usr/local/lib", "-lpthread", {force = true})
--
-- @endcode
--
