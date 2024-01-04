from os.path import join
from SCons.Script import (
    COMMAND_LINE_TARGETS,
    AlwaysBuild,
    Default,
    DefaultEnvironment,
    Flatten,
)

env = DefaultEnvironment()

PLATFORM_DIR = env.PioPlatform().get_dir()

# Remove generic C/C++ tools
for k in ("CC", "CXX"):
    if k in env:
        del env[k]

# Preserve C and C++ build flags
backup_cflags = env.get("CFLAGS", [])
backup_cxxflags = env.get("CXXFLAGS", [])

# Scan for GCC compiler
env.Tool("gcc")
env.Tool("g++")

# Reload "compilation_db" tool
if "compiledb" in COMMAND_LINE_TARGETS:
    env.Tool("compilation_db")

# Restore C/C++ build flags as they were overridden by env.Tool
env.Append(CFLAGS=backup_cflags, CXXFLAGS=backup_cxxflags)

# Build raylib
raylib_env = env.Clone(
    RAYLIB_SRC_PATH=join(PLATFORM_DIR, "lib/raylib/src"),
    RAYLIB_LIBTYPE="STATIC",
    RAYLIB_BUILD_MODE="RELEASE",
    RAYLIB_LIB_NAME="raylib",
    RAYLIB_RELEASE_PATH="$BUILD_DIR/raylib",
    RAYLIB_MODULE_AUDIO="FALSE",
    RAYLIB_MODULE_MODELS="FALSE",
    RAYLIB_MODULE_RAYGUI="FALSE",
)
raylib_target = raylib_env.Command(
    "$RAYLIB_RELEASE_PATH/lib${RAYLIB_LIB_NAME}.a",
    raylib_env.Dir("$RAYLIB_SRC_PATH"),
    "make -C $RAYLIB_SRC_PATH",
    ENV={
        "PLATFORM": "PLATFORM_DESKTOP",
        # SCons doesn't propagate construction environment to the
        # execution environment of external commands
        **{
            k: raylib_env.subst(v)
            for k, v in raylib_env.items()
            if k.startswith("RAYLIB_")
        },
    },
)

# Build Launchpad Pro simulator
lppsim_env = env.Clone(
    LPPSIM_SRC=join(PLATFORM_DIR, "src"),
    OBJPREFIX="$BUILD_DIR/simulator/",
    CPPPATH=[
        join(PLATFORM_DIR, "include"),
        join(PLATFORM_DIR, raylib_env["RAYLIB_SRC_PATH"]),
    ],
    CCFLAGS=["-O2", "-flto", "-Wall", "-Wextra", "-Wpedantic"],
)
lppsim_target = lppsim_env.Object(
    [join("$LPPSIM_SRC", filename) for filename in ("sim.c", "ext.c", "gui.c")]
)

# Target: Build executable program
env.Append(
    PIOBUILDFILES=lppsim_target,
    LIBS=[raylib_target],
    LINKFLAGS=[
        "-flto",
        ("-framework", "OpenGL"),
        ("-framework", "Cocoa"),
        ("-framework", "IOKit"),
        ("-framework", "CoreVideo"),
    ],
    CPPPATH=[join(PLATFORM_DIR, "include")],
)
env.Replace(PROGNAME="lppsim")
target_bin = env.BuildProgram()

# Target: Execute binary
exec_action = env.VerboseAction("$SOURCE $PROGRAM_ARGS", "Executing $SOURCE")
AlwaysBuild(env.Alias("exec", target_bin, exec_action))
AlwaysBuild(env.Alias("upload", target_bin, exec_action))

# Target: Print binary size
target_size = env.Alias(
    "size",
    target_bin,
    env.VerboseAction("$SIZEPRINTCMD", "Calculating size of $SOURCE"),
)
AlwaysBuild(target_size)

# Default targets
Default([target_bin])
