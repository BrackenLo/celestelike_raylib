add_rules("mode.debug", "mode.release")

add_requires("raylib", "raygui", "cereal", "magic_enum", "entt")

target("celestelike_raylib")
  set_kind("binary")
  set_languages("c++20")
  add_packages("raylib", "raygui", "entt", "cereal", "magic_enum")

  add_includedirs("include")
  add_includedirs("include/engine")
  add_includedirs("include/game")
  
  add_files("src/*.cpp")
  add_files("src/engine/*.cpp")
  add_files("src/game/*.cpp")

  set_configdir("$(buildir)/$(plat)/$(arch)/$(mode)")  
  add_configfiles("src/resources/*", {onlycopy = true})

  after_build(function(target)
      import("core.base.task")
      task.run("project", {kind = "compile_commands", outputdir = "./"})
  end)
