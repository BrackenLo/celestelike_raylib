add_rules("mode.debug", "mode.release")

add_requires("raylib", "entt")
add_requires("cereal", "magic_enum")
add_requires("imgui", "raygui")

target("celestelike_raylib")
  set_kind("binary")
  set_languages("c++20")

  add_packages("raylib", "entt")
  add_packages("cereal", "magic_enum")
  add_packages("imgui", "raygui")  

  -- Imgui Raylib Package
  add_includedirs("external/rl-imgui")
  add_files("external/rl-imgui/rlImGui.cpp")

  add_includedirs("include")
  add_includedirs("include/engine")
  add_includedirs("include/game")

  add_files("src/**.cpp")  

  set_configdir("$(buildir)/$(plat)/$(arch)/$(mode)")  
  add_configfiles("src/resources/*", {onlycopy = true})

  after_build(function(target)
      import("core.base.task")
      task.run("project", {kind = "compile_commands", outputdir = "./"})
  end)
