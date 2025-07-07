workspace "Transport-Sim"
   architecture "x64"
   startproject "Transport-Sim"
   configurations { "Debug", "Release" }

outputdir = "%{cfg.buildcfg}"

project "Transport-Sim"
   location "Transport-Sim"
   kind "ConsoleApp"
   language "C++"
   cppdialect "C++20"
   staticruntime "on"

   targetdir ("bin/" .. outputdir)
   objdir ("build/" .. outputdir)

   files {
      "src/**.cpp",
      "src/**.hpp",

      -- GLAD loader
      "vendor/glad/src/glad.c",

      -- ImGui core
      "vendor/imgui/imgui.cpp",
      "vendor/imgui/imgui_demo.cpp",
      "vendor/imgui/imgui_draw.cpp",
      "vendor/imgui/imgui_tables.cpp",
      "vendor/imgui/imgui_widgets.cpp",

      -- ImGui backends
      "vendor/imgui/backends/imgui_impl_glfw.cpp",
      "vendor/imgui/backends/imgui_impl_opengl3.cpp",

      -- GLFW (compiled manually)
      "vendor/glfw/src/context.c",
      "vendor/glfw/src/init.c",
      "vendor/glfw/src/input.c",
      "vendor/glfw/src/monitor.c",
      "vendor/glfw/src/vulkan.c",
      "vendor/glfw/src/window.c",
      "vendor/glfw/src/wgl_context.c",
      "vendor/glfw/src/win32_init.c",
      "vendor/glfw/src/win32_joystick.c",
      "vendor/glfw/src/win32_module.c",
      "vendor/glfw/src/win32_monitor.c",
      "vendor/glfw/src/win32_time.c",
      "vendor/glfw/src/win32_thread.c",
      "vendor/glfw/src/win32_window.c",
      "vendor/glfw/src/egl_context.c",         -- ✅ required for full context support
      "vendor/glfw/src/osmesa_context.c",      -- ✅ required if linking static
      "vendor/glfw/src/platform.c",
      "vendor/glfw/src/null_init.c",        -- (REQUIRED for null platform stub)
      "vendor/glfw/src/null_monitor.c",
      "vendor/glfw/src/null_window.c",
      "vendor/glfw/src/null_joystick.c"
   }

   includedirs {
      "vendor/glfw/include",
      "vendor/glfw/deps",                      -- required by GLFW for glad/glad.h
      "vendor/glad/include",
      "vendor/imgui",
      "vendor/imgui/backends",
      "src/"
   }

   links {
      "opengl32"
   }

   filter "system:windows"
      systemversion "latest"
      defines {
         "_GLFW_WIN32",
         "GLFW_BUILD_WIN32",
         "_CRT_SECURE_NO_WARNINGS"
      }

   filter "configurations:Debug"
      defines { "DEBUG" }
      symbols "On"

   filter "configurations:Release"
      defines { "NDEBUG" }
      optimize "On"
