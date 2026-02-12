dependencies = {
	basePath = "./deps"
}

function dependencies.load()
	dir = path.join(dependencies.basePath, "premake/*.lua")
	deps = os.matchfiles(dir)

	for i, dep in pairs(deps) do
		dep = dep:gsub(".lua", "")
		require(dep)
	end
end

function dependencies.imports()
	for i, proj in pairs(dependencies) do
		if type(i) == 'number' then
			proj.import()
		end
	end
end

function dependencies.projects()
	for i, proj in pairs(dependencies) do
		if type(i) == 'number' then
			proj.project()
		end
	end
end

dependencies.load()

workspace "nfsc-rtx"

	startproject "nfsc-rtx"
	location "./build"
	objdir "%{wks.location}/obj"
	targetdir "%{wks.location}/bin/%{cfg.buildcfg}"
	
    configurations { 
        "Debug", 
        "Release",
    }

	platforms "Win32"
	architecture "x86"

	cppdialect "C++20"
	systemversion "latest"
    symbols "On"
    staticruntime "On"

    disablewarnings {
		"4239",
		"4369",
		"4505",
		"4996",
		"5311",
		"6001",
		"6385",
		"6386",
		"26812"
	}

    defines { 
        "_SILENCE_ALL_CXX17_DEPRECATION_WARNINGS" 
    }

    filter "platforms:Win*"
		defines {
			"_WINDOWS", 
			"WIN32"
		}
	filter {}

	-- Release

	filter "configurations:Release"
		optimize "Full"

		buildoptions {
			"/GL"
		}

		defines {
			"NDEBUG"
		}
		
		flags { 
            "MultiProcessorCompile", 
            "LinkTimeOptimization", 
            "No64BitChecks",
			"FatalCompileWarnings"
        }
	filter {}

	-- Debug

	filter "configurations:Debug"
		optimize "Debug"

		defines { 
            "DEBUG", 
            "_DEBUG" 
        }

		flags { 
            "MultiProcessorCompile", 
            "No64BitChecks" 
        }
	filter {}


	-- Projects

	project "_shared"
		kind "StaticLib"
		language "C++"

		targetdir "bin/%{cfg.buildcfg}"
		objdir "obj/%{cfg.buildcfg}"
		
		pchheader "std_include.hpp"
		pchsource "src/shared/std_include.cpp"

		files {
			"./src/shared/**.hpp",
			"./src/shared/**.cpp",
		}

		includedirs {
			"%{prj.location}/src",
			"./src",
		}

		resincludedirs {
			"$(ProjectDir)src"
		}

        buildoptions { 
            "/Zm100 -Zm100" 
        }

        -- Specific configurations
		flags { 
			"UndefinedIdentifiers" 
		}

		warnings "Extra"
		dependencies.imports()

        group "Dependencies"
            dependencies.projects()
		group ""

	---------------------------

	project "nfsc-rtx"
	kind "SharedLib"
	language "C++"

	linkoptions {
		"/PDBCompress"
	}

	pchheader "std_include.hpp"
	pchsource "src/comp/std_include.cpp"

	files {
		"./src/comp/**.hpp",
		"./src/comp/**.cpp",
	}

	includedirs {
		"%{prj.location}/src",
		"./src",
	}

	links {
		"_shared"
	}

	resincludedirs {
		"$(ProjectDir)src"
	}

	buildoptions { 
		"/Zm100 -Zm100" 
	}

	filter "configurations:Debug or configurations:Release"
		if(os.getenv("NFSC_ROOT")) then
			print ("Setup paths using environment variable 'NFSC_ROOT' :: '" .. os.getenv("NFSC_ROOT") .. "'")
			targetdir(os.getenv("NFSC_ROOT") .. "/" .. "plugins")
			debugdir (os.getenv("NFSC_ROOT"))
			debugcommand (os.getenv("NFSC_ROOT") .. "/" .. "NFSC.exe")
		end
	filter {}

	-- Specific configurations
	flags { 
		"UndefinedIdentifiers" 
	}

	warnings "Extra"

	-- Post-build
	postbuildcommands {
		"MOVE /Y \"$(TargetDir)nfsc-rtx.dll\" \"$(TargetDir)nfsc-rtx.asi\"",
	}

	dependencies.imports()

	group "Dependencies"
		dependencies.projects()
	group ""
	