import json
import os
import platform

# Determine the default executable name based on the platform
EXECUTABLE_NAME = "hello_methodverse"

# Load and parse the CMakePresets.json file
def load_presets(presets_path):
    with open(presets_path, "r") as f:
        return json.load(f)

# Resolve the binary directory from the CMake preset by replacing ${sourceDir}
def resolve_binary_path(binary_dir_template, source_dir):
    if "${sourceDir}" in binary_dir_template:
        return os.path.normpath(binary_dir_template.replace("${sourceDir}", source_dir))
    return os.path.normpath(binary_dir_template)

# Look up the build configuration (e.g., Debug or Release) for a given build preset
def get_configuration_for_preset(presets, name):
    for b in presets.get("buildPresets", []):
        if b.get("name") == name:
            return b.get("configuration", "Debug")  # Default to Debug
    return "Debug"

# Determine whether the preset uses a multi-config generator (like Visual Studio or Xcode)
def is_multiconfig_generator(generator_name):
    return any(g in generator_name.lower() for g in ["visual studio", "xcode"])

# Identify current OS platform as 'windows', 'linux', or 'unknown'
def get_current_platform():
    if os.name == "nt":
        return "windows"
    elif platform.system().lower() == "linux":
        return "linux"
    return "unknown"

# Generate tasks.json content based on presets and current platform
def generate_tasks(presets, source_dir, current_platform):
    tasks = []
    configure_presets = presets.get("configurePresets", [])
    build_presets = {b["name"]: b for b in presets.get("buildPresets", [])}

    is_windows = current_platform == "windows"
    print(f"🧠 Detected OS: {platform.system()}")
    print(f"🔧 Using shell: {'cmd' if is_windows else 'bash'}")

    for preset in configure_presets:
        name = preset["name"]
        platform_tag = preset.get("vendor", {}).get("platform", "")
        # Skip if the preset doesn't match the current platform
        if platform_tag != current_platform:
            continue

        if name not in build_presets:
            print(f"⚠️  Skipping task for '{name}' (no matching buildPreset)")
            continue

        # Use platform-specific shell and arguments
        shell_command = "cmd" if is_windows else "bash"
        shell_args = ["/c", f"cmake --preset {name} && cmake --build --preset {name}"] if is_windows else \
                     ["-c", f"cmake --preset {name} && cmake --build --preset {name}"]

        tasks.append({
            "label": f"build-{name}",
            "type": "shell",
            "command": shell_command,
            "args": shell_args,
            "problemMatcher": [],
            "group": {
                "kind": "build",
                "isDefault": name == "x64-debug"
            }
        })

    return {
        "version": "2.0.0",
        "tasks": tasks
    }

# Generate launch.json content based on presets and current platform
def generate_launch(presets, source_dir, current_platform):
    launches = []
    is_windows = current_platform == "windows"

    for preset in presets.get("configurePresets", []):
        name = preset["name"]
        platform_tag = preset.get("vendor", {}).get("platform", "")
        if platform_tag != current_platform:
            continue

        generator = preset.get("generator", "")
        binary_dir_template = preset.get("binaryDir")
        if not binary_dir_template:
            continue

        # Use workspace folder in launch.json for compatibility
		# For multiple config generator, it will add a sub-folder (such as /Debug) in build dir for the target name.
        # Example: assume "config-x" is a build configuration defined in CMakePresets.json
        # - For single-config generators (e.g., Ninja), the build target is placed directly in /out/config-x
        # - For multi-config generators (e.g., Visual Studio), the build target is placed in:
        #     /out/config-x/Debug    for Debug builds, or
        #     /out/config-x/Release  for Release builds,
        #   depending on the selected configuration
        resolved_binary_dir = resolve_binary_path(binary_dir_template, "${workspaceFolder}")
        is_multiconfig = is_multiconfig_generator(generator)
        config_name = get_configuration_for_preset(presets, name)

        if is_multiconfig:
            launch_dir = os.path.join(resolved_binary_dir, config_name)
        else:
            launch_dir = resolved_binary_dir

        if platform_tag == "windows":
            executable_path = os.path.join(launch_dir, EXECUTABLE_NAME + ".exe")
        elif platform_tag in ("android", "linux"):
            executable_path = os.path.join(launch_dir, EXECUTABLE_NAME)
        else:
            executable_path = os.path.join(launch_dir, EXECUTABLE_NAME)
			
        config = {
            "name": f"Launch {name}",
            # we use "cppdbg" for both Windows and Linux for consistency
            "type": "cppdbg" if is_windows else "cppdbg",
            "request": "launch",
            "program": executable_path,
            "args": [],
            "stopAtEntry": True,
            "cwd": launch_dir,
            "environment": [],
            "externalConsole": False,
            "preLaunchTask": f"build-{name}"
        }

        if not is_windows:
            config.update({
                "MIMode": "gdb",
                "miDebuggerPath": "/usr/bin/gdb",
                "setupCommands": [
                    {
                        "description": "Enable pretty-printing for gdb",
                        "text": "-enable-pretty-printing",
                        "ignoreFailures": True
                    }
                ]
            })

        launches.append(config)

    return {
        "version": "0.2.0",
        "configurations": launches
    }

def write_json(output_path, data, label):
    os.makedirs(os.path.dirname(output_path), exist_ok=True)
    with open(output_path, "w") as f:
        json.dump(data, f, indent=2)
    print(f"✅ {label} written to {output_path}")

def main():
    script_dir = os.path.abspath(os.path.dirname(__file__))
    root_dir = os.path.abspath(os.path.join(script_dir, ".."))
    presets_path = os.path.join(root_dir, "CMakePresets.json")

    vscode_dir = os.path.join(root_dir, ".vscode")
    tasks_path = os.path.join(vscode_dir, "tasks.json")
    launch_path = os.path.join(vscode_dir, "launch.json")

    try:
        presets = load_presets(presets_path)
        current_platform = get_current_platform()
        tasks = generate_tasks(presets, root_dir, current_platform)
        launch = generate_launch(presets, root_dir, current_platform)

        write_json(tasks_path, tasks, "tasks.json")
        write_json(launch_path, launch, "launch.json")
    except FileNotFoundError:
        print(f"❌ Error: Could not find {presets_path}")
    except Exception as e:
        print(f"❌ Unexpected error: {e}")

if __name__ == "__main__":
    main()

# Generated by ChatGPT.
# This script generates VSCode tasks and launch configurations based on CMake presets.
# Usage: From your project root:
# > python tools/generate_tasks_json.py
# It will then create .vscode/tasks.json and .vscode/launch.json
# based on the CMakePresets.json file.
# Make sure to have CMakePresets.json in the root directory of your project.