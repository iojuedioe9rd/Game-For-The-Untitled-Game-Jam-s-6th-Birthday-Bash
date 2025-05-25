import os

# Top-level source directories to scan
source_dirs = [
    "../Game/src",
    "../Engine/src",
    "../Engine/Platform/WebGL",
    "../Engine/vendor/box2d/src",
]

# Additional source files not in the source_dirs (like vendor C files)
source_files = [
    "../Engine/vendor/FelineLoad/feline_load.c",
    "../Engine/vendor/stb/stb_image.c",
    "../Engine/vendor/miniaudio/miniaudio.c",
    "../Engine/vendor/ImGui/imgui.cpp",
    "../Engine/vendor/ImGui/imgui_draw.cpp",
    "../Engine/vendor/ImGui/imgui_tables.cpp",
    "../Engine/vendor/ImGui/imgui_widgets.cpp",
    "../Engine/vendor/ImGui/imgui_demo.cpp",
    "../Engine/vendor/ImGui/backends/imgui_impl_opengl3.cpp",
	"../Engine/vendor/ImGui/backends/imgui_impl_sdl3.cpp",
]

# Compiler flags for emcc
args = [
    "-O3",
    "-msimd128",
    "-msse",
    "-flto",
    ""
    "-msse2",
    "-msse3",
    "-mssse3",
    "-msse4.1",
    "-msse4.2",
    "-s", "WASM=1",
    "-s", "ALLOW_MEMORY_GROWTH=1",
    "-s", "USE_WEBGL2=1",
    "-s", "FULL_ES3=1",
    "-Wall",
    "-Wunused-command-line-argument",
    "-sALLOW_TABLE_GROWTH=1",
    "-sASYNCIFY=1"
]

# Include directories
include_dirs = [
    "../Engine/src",
    "../Engine/vendor/SDL3/include",
    "../Engine",
    "../Engine/vendor/FelineLoad",
    "../Engine/vendor/stb",
    "../Engine/vendor/glm",
    "../Engine/vendor/ecs/include",
    "../Engine/vendor/box2d/include",
    "../Engine/vendor/box2d/src",
    "../Engine/vendor/miniaudio",
    "../Engine/vendor/json/single_include",
    "../Engine/vendor/imgui"
]

# Library directories
lib_dirs = [
    "../Engine/vendor/SDL3/Web",
]

# Library names (without `lib` prefix and `.a`/`.so` suffix)
libs = [
    "-lSDL3",
]

# Convert source path (.cpp or .c) to output object path relative to current dir
def relative_object_path(source_path):
    source_path = os.path.normpath(source_path)
    if source_path.startswith(".." + os.sep):
        source_path = source_path[3:]
    if source_path.endswith(".cpp"):
        obj_path = source_path[:-4] + ".o"
    elif source_path.endswith(".c"):
        obj_path = source_path[:-2] + ".o"
    else:
        raise RuntimeError(f"Unknown source file extension: {source_path}")
    return os.path.join("build", obj_path).replace("\\", "/")

# Find all .cpp and .c files recursively in source_dirs
def find_source_files():
    files = []
    for src_dir in source_dirs:
        for root, dirs, fs in os.walk(src_dir):
            for file in fs:
                if file.endswith(".cpp") or file.endswith(".c"):
                    full_path = os.path.join(root, file).replace("\\", "/")
                    files.append(full_path)
    files.extend(source_files)
    return list(set(files))

# Generate build.ninja
def generate_ninja():
    source_files_list = find_source_files()

    include_flags = " ".join(f"-I{inc}" for inc in include_dirs)
    lib_dir_flags = " ".join(f"-L{lib}" for lib in lib_dirs)
    lib_flags = " ".join(libs)
    compiler_args = " ".join(args)
    all_objs = [relative_object_path(s) for s in source_files_list]

    with open("build.ninja", "w") as f:
        f.write("# Generated build.ninja\n\n")

        # Compile rules
        f.write("rule compile_cpp\n")
        f.write(f"  command = cmd /c emcc {compiler_args} {include_flags} -std=c++17 -c $in -o $out\n\n")

        f.write("rule compile_c\n")
        f.write(f"  command = cmd /c emcc {compiler_args} {include_flags} -x c -c $in -o $out\n\n")

        # Link rule with libraries and --preload-file assets
        f.write("rule link\n")
        f.write(f"  command = cmd /c emcc {lib_dir_flags} $in -o dist/index.html {lib_flags} {compiler_args} --shell-file shell.html --preload-file assets\n\n")

        # Build object files
        for src_file in source_files_list:
            obj_file = relative_object_path(src_file)
            if src_file.endswith(".cpp"):
                f.write(f"build {obj_file}: compile_cpp {src_file}\n")
            else:
                f.write(f"build {obj_file}: compile_c {src_file}\n")

        # Link all objects into dist/index.html
        f.write(f"\nbuild dist/index.html: link {' '.join(all_objs)}\n")

        # Default target
        f.write("\ndefault dist/index.html\n")

if __name__ == "__main__":
    os.makedirs("build", exist_ok=True)
    os.makedirs("dist", exist_ok=True)
    generate_ninja()
