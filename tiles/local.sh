#!/bin/bash

set -euxo pipefail

cd "$(dirname "$0")/.."

bazel build --config linux //tiles:local

bazel-bin/tiles/local \
  ../shoveler-assets/tilesets/field/spring.png \
  7 \
  2 \
  ../shoveler-assets/characters/circle/player1.png \
  ../shoveler-assets/characters/circle/player2.png \
  ../shoveler-assets/characters/circle/player3.png \
  ../shoveler-assets/characters/circle/player4.png \
  1
