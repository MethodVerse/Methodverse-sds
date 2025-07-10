#!/usr/bin/env bash
# -----------------------------------------------------------------------------
# Script Purpose:
#   This script is intended to be run by a CI pipeline (e.g., GitHub Actions)
#   to configure, build, and verify Android-native binaries using CMake presets.
#   It is designed to be executed on the host machine while an Android emulator
#   is running. The script builds each Android preset, pushes the resulting
#   binary to the emulator, executes it inside the emulator, and checks its output.
#
# Rationale:
#   - Automates the process of building and testing native Android binaries in CI.
#   - Ensures that the built binaries run correctly on an actual Android emulator.
#   - Provides clear logging and output verification for robust CI feedback.
# -----------------------------------------------------------------------------
set -e

expected="Hello, MethodVerse!"
source_dir="$(pwd)"
echo "Current working directory: $source_dir"

jq -r '.configurePresets[] | select(.vendor.platform=="android") | [.name, .generator, .binaryDir] | @tsv' CMakePresets.json | while IFS=$'\t' read name generator binaryDir; do
    echo "🔧 Configuring and building preset: $name"
    cmake --preset "$name"
    cmake --build --preset "$name"

    config=$(jq -r --arg name "$name" '.buildPresets[] | select(.name == $name) | .configuration // "Debug"' CMakePresets.json)

    # Strip literal ${sourceDir}/ using sed
    relative_dir=$(echo "$binaryDir" | sed 's#\${sourceDir}/##')

    exe_path="$source_dir/$relative_dir"
    [[ "$generator" =~ [Vv]isual ]] && exe_path="$exe_path/$config"
    exe_path="$exe_path/hello_methodverse"
    exe_path="${exe_path//\\//}"  # normalize slashes
    exe_path="${exe_path// /\\ }" # escape spaces for adb push
    echo "Executable full path: $exe_path"
	
    echo "📤 Pushing binary to Android emulator: $exe_path"
    adb root
    adb remount
    adb push "$exe_path" /data/local/tmp/hello_methodverse
    adb shell chmod +x /data/local/tmp/hello_methodverse
    echo "🚀 Running binary inside emulator"
    output=$(adb shell /data/local/tmp/hello_methodverse | tr -d '\r')
    if [[ "$output" != "$expected" ]]; then
        echo "❌ Output from emulator: '$output' (expected: '$expected')"
        exit 1
    else
    echo "✅ Output verified: $output"
    fi

    # --- Run unit tests with CTest if test binary exists ---
    test_bin="$source_dir/$relative_dir/tst/hello_methodverse_test"
    test_bin="${test_bin//\\//}"
    test_bin="${test_bin// /\\ }"
    if [[ -f "$test_bin" ]]; then
        echo "📤 Pushing test binary to Android emulator: $test_bin"
        adb push "$test_bin" /data/local/tmp/hello_methodverse_test
        adb shell chmod +x /data/local/tmp/hello_methodverse_test
        echo "🧪 Running unit tests inside emulator"
        adb shell /data/local/tmp/hello_methodverse_test --gtest_output=plain
    else
        echo "ℹ️ No test binary found at $test_bin, skipping unit tests."
    fi    
done
