#!/usr/bin/env bash

# This file is heavily inspired from the upstream generation script that can be found at:
# https://github.com/grailbio/bazel-compilation-database/blob/master/generate.sh

set -e -u -o -x pipefail

cd "$(dirname "$0")/../"

function isLinux() {
  [[ "$(uname -s)" == "Linux" ]]
}

function isMacOS() {
  [[ "$(uname -s)" == "Darwin" ]]
}

function isWindows() {
  ! (isLinux || isMacOS)
}

readonly compilation_db_file="compile_commands.json"
readonly aspects_file="@bazel_compilation_database//:aspects.bzl"
readonly output_groups="compdb_files"

readonly exec_root="$(bazel info execution_root)"
readonly workspace_root="$(bazel info workspace)"
readonly quiet_options=(
  "--noshow_progress"
  "--noshow_loading_progress"
)

targets=()
while read -r target; do
  targets+=("${target}")
done < <(bazel query "${quiet_options[@]}" 'kind("cc_(library|binary|test|inc_library|proto_library)", //...)' | tr -d '\r')

# Clean any previously generated files.
if [[ -e "${exec_root}" ]]; then
  find "${exec_root}" -name '*.compile_commands.json' -delete
fi

# shellcheck disable=SC2046
# NOTE: We need to set the 'MSYS_ARG_CONV_EXCL' environment variable because otherwise our Windows shell
# does weird things with Bazel target paths (see http://www.mingw.org/wiki/Posix_path_conversion).
MSYS2_ARG_CONV_EXCL="*" bazel build \
  "${quiet_options[@]}" \
  --aspects="${aspects_file}"%compilation_database_aspect \
  --output_groups="${output_groups}" \
  "${targets[@]}" >/dev/null

find "${exec_root}" -name '*.compile_commands.json' -exec cat "{}" ';' >"${compilation_db_file}"
sed -i -e 's/,*\r*$//' "${compilation_db_file}" # Strip trailing commas.

if isWindows; then
  # The basic idea here is that we are going to use the workspace root instead of the real bazel execroot so IDEs don't
  # get confused about our source files locations. For external repository references to work, we symlink external into
  # the execroot.
  #
  # Note: must have "Developer Mode" enabled in Windows 10 for symlinks to work!
  sed -i -e "s|__EXEC_ROOT__|${workspace_root}|g;" "${compilation_db_file}" # Replace exec_root marker
  if [[ -e "external" ]]; then
    # Gotta be careful here, we don't want to recursively delete all contents of this directory but many other commands
    # risk doing that on Windows. However, when rmdir is executed through cmd only the symlink is removed.
    # Must execute as "cmd.exe" and not just as "cmd" from bash, otherwise will open an interactive prompt!
    # Must set the MSYS argument or the /c will be magically interpreted as the C:\ drive and converted...
    MSYS2_ARG_CONV_EXCL="*" cmd.exe /c "rmdir external"
  fi
  # ln -s doesn't always work on Windows so it's safer to use mklink through cmd.
  # For whatever crazy reason we cannot escape quotes when executing cmd from msys2 bash (it works from git bash!).
  # If we pass the path without quotes, that also doesn't work because the forward slashes will be interpreted as CLI
  # options (yes, really). So we need to both *not* pass any quotes that would need to be escaped, and avoid any slashes
  # because either would break the cmd execution. Luckily, we can convert the forward slashes to backslashes to pull
  # this off.
  # Must execute as "cmd.exe" and not just as "cmd" from bash, otherwise will open an interactive prompt!
  # Must set the MSYS argument or the /c will be magically interpreted as the C:\ drive and converted...
  external_exec_root_bs="${exec_root//\//\\}\\external" # replace / by \, append external
  MSYS2_ARG_CONV_EXCL="*" cmd.exe /c "mklink /D external ${external_exec_root_bs}"

  # The compilation database at this point will contain lots of gcc or clang specific flags that we need to clean up so
  # MSVC's cl.exe can understand them.
  sed -i -e "s| \-D| /D|g;" "${compilation_db_file}"    # fix gcc style defines
  sed -i -e "s| -x c++ -c||g;" "${compilation_db_file}" # remove more gcc/clang specific stuff

  # Change -isystem and -iquote into /external:I which is the MSVC analogous option.
  # However, we also need to pass /external:W for the previous options to work.
  # We also strip the compiler path here because CLion struggles with spaces in "Program Files" and it finds cl.exe
  # just fine by itself.
  #
  # see: https://devblogs.microsoft.com/cppblog/broken-warnings-theory/
  sed -i -e "s|\"command\"\:\".*cl.exe|\"command\"\:\"cl.exe /external:W0|g;" "${compilation_db_file}"
  sed -i -e "s| \-iquote| /external:I|g;" "${compilation_db_file}"
  sed -i -e "s| \-isystem| /external:I|g;" "${compilation_db_file}"
else
  sed -i -e "s|__EXEC_ROOT__|${exec_root}|g; s|iquote|isystem|g" "${compilation_db_file}" # Replace exec_root marker
  sed -i -e "s|external/local_config_cc/wrapped_clang|$(which clang)|g" "${compilation_db_file}"
fi

jq --slurp . "${compilation_db_file}" >"${compilation_db_file}.new"
mv -f "${compilation_db_file}.new" "${compilation_db_file}"
