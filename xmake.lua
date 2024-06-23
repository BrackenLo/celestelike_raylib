add_rules("mode.debug", "mode.release")

add_requires("raylib", "raygui")

target("celestelike_raylib")
  set_kind("binary")
  add_files("src/*.cpp")
  add_files("src/engine/*.cpp")
  add_files("src/game/*.cpp")
  -- add_configfiles("src/resources/*.h")
  add_packages("raylib", "raygui")
  set_languages("c++20")

  set_configdir("$(buildir)/$(plat)/$(arch)/$(mode)")  
  add_configfiles("src/resources/*", {onlycopy = true})

  after_build(function(target)
      import("core.base.task")
      task.run("project", {kind = "compile_commands", outputdir = "./"})
  end)

  -- on_install(function(target)
  --     os.cp(target:targetfile(), path.join(target:installdir(), "xxx"))
  -- end)

  -- after_build(function(target)
      
  -- end)
