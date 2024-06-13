add_rules("mode.debug", "mode.release")

add_requires("raylib")

target("celestelike_raylib")
  set_kind("binary")
  add_files("src/*.cpp")
  add_files("src/engine/*.cpp")
  add_files("src/game/*.cpp")
  add_packages("raylib")
  set_languages("c++20")
