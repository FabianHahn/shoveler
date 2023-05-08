#!/bin/bash

set -euxo pipefail

cd "$(dirname "$0")/../.."

bazel build --config linux //examples/client_tiles

bazel-bin/examples/client_tiles/client_tiles \
  ../shoveler-assets/tilesets/field/spring.png \
  7 \
  2 \
  ../shoveler-assets/characters/circle/player1.png \
  ../shoveler-assets/characters/circle/player2.png \
  ../shoveler-assets/characters/circle/player3.png \
  ../shoveler-assets/characters/circle/player4.png \
  1
