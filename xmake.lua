add_rules("mode.debug", "mode.release")

add_repositories("liteldev-repo https://github.com/LiteLDev/xmake-repo.git")

-- LeviLamina
add_requires("levilamina 1.0.0", {configs = {target_type = "server"}})
add_requires("levibuildscript")

if not has_config("vs_runtime") then
    set_runtimes("MD")
end

target("StarrySkyForm")
    add_rules("@levibuildscript/linkrule")
    add_rules("@levibuildscript/modpacker")
    add_rules("plugin.compile_commands.autoupdate")
    set_exceptions("none")
    set_kind("shared")
    set_languages("c++20")
    set_symbols("debug")
    add_cxflags(
        "/EHa",
        "/utf-8",
        "/W4",
        "/w44265",
        "/w44289",
        "/w44296",
        "/w45263",
        "/w44738",
        "/w45204"
    )
    add_defines(
        "NOMINMAX",
        "UNICODE",
        "LDAPI_EXPORT"
    )
    add_includedirs("src")
    add_files("src/**.cpp", "src/**.cc")
    add_headerfiles("src/(starryskyform/**.h)")

    add_packages(
        "levilamina"
    )

    add_configfiles("src/BuildInfo.h.in")
    set_configdir("src/starryskyform")

    if is_mode("debug") then
        add_defines("DEBUG")
    end

    after_build(function (target)
        local bindir = path.join(os.projectdir(), "bin")
        local outputdir = path.join(bindir, target:name())

        local assetsdir = path.join(os.projectdir(), "assets")
        if os.isdir(assetsdir) then
            os.cp(assetsdir, outputdir)
        end
    end)
