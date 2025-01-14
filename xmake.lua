add_rules("mode.debug", "mode.release")

add_requires("raylib", "raygui", "cereal", "magic_enum")

target("celestelike_raylib")
  set_kind("binary")
  add_files("src/*.cpp")
  add_files("src/engine/*.cpp")
  add_files("src/game/*.cpp")
  add_packages("raylib", "raygui", "cereal", "magic_enum")
  set_languages("c++20")

  set_configdir("$(buildir)/$(plat)/$(arch)/$(mode)")  
  add_configfiles("src/resources/*", {onlycopy = true})

  after_build(function(target)
      import("core.base.task")
      task.run("project", {kind = "compile_commands", outputdir = "./"})
  end)
