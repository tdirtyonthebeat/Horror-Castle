#!/bin/bash
set -euo pipefail

ROOT="$(cd "$(dirname "$0")" && pwd)"
BUILD="$ROOT/build"
CONFIG="Release"

printf '\n🏰 HORROR CASTLE v1.2 — STONE & SHADOW\n'
printf 'Project: %s\n\n' "$ROOT"

if ! command -v cmake >/dev/null 2>&1; then
    echo "ERROR: CMake is not installed."
    echo "Install it with Homebrew: brew install cmake"
    exit 1
fi

if ! xcode-select -p >/dev/null 2>&1; then
    echo "ERROR: Apple command-line developer tools are missing."
    echo "Run: xcode-select --install"
    exit 1
fi

CLEAN=0
RUN=0
TEST=0
RENDER=0
QA=0
STATIC=0
for arg in "$@"; do
    [[ "$arg" == "clean" ]] && CLEAN=1
    [[ "$arg" == "run" ]] && RUN=1
    [[ "$arg" == "test" ]] && TEST=1
    [[ "$arg" == "render" ]] && RENDER=1
    [[ "$arg" == "qa" ]] && QA=1
    [[ "$arg" == "static" ]] && STATIC=1
done

if [[ "$STATIC" == "1" ]]; then
    python3 "$ROOT/tools/static_validate.py"
    exit $?
fi

if [[ "$CLEAN" == "1" ]]; then
    echo "Removing previous build folder..."
    rm -rf "$BUILD"
fi

ARCH="$(uname -m)"
echo "Configuring Horror Castle for $ARCH..."
cmake -S "$ROOT" -B "$BUILD" \
    -DCMAKE_BUILD_TYPE="$CONFIG" \
    -DCMAKE_OSX_ARCHITECTURES="$ARCH"

echo
echo "Building VST3, AU and Standalone..."
cmake --build "$BUILD" --config "$CONFIG" \
    --target HorrorCastle_VST3 HorrorCastle_AU HorrorCastle_Standalone \
    -j "$(sysctl -n hw.logicalcpu 2>/dev/null || echo 4)"

echo
echo "✅ Horror Castle v1.2 build finished."
find "$BUILD" -type d \( -name "*.vst3" -o -name "*.component" -o -name "Horror Castle.app" \) -print 2>/dev/null || true

if [[ "$RUN" == "1" ]]; then
    APP="$(find "$BUILD" -type d -name 'Horror Castle.app' -print -quit 2>/dev/null || true)"
    if [[ -n "$APP" ]]; then
        echo "Launching: $APP"
        open "$APP"
    else
        echo "WARNING: Standalone app was not found automatically."
    fi
fi


if [[ "$TEST" == "1" || "$RENDER" == "1" || "$QA" == "1" ]]; then
    echo
    echo "Building signature regression tool..."
    cmake --build "$BUILD" --config "$CONFIG" --target HorrorCastleSignatureCheck \
        -j "$(sysctl -n hw.logicalcpu 2>/dev/null || echo 4)"

    CHECKER="$(find "$BUILD" -type f -perm -111 -name 'Horror Castle Signature Check' -print -quit 2>/dev/null || true)"
    if [[ -z "$CHECKER" ]]; then
        CHECKER="$(find "$BUILD" -type f -perm -111 -name 'HorrorCastleSignatureCheck' -print -quit 2>/dev/null || true)"
    fi

    if [[ -z "$CHECKER" ]]; then
        echo "ERROR: Signature checker executable was not found."
        exit 1
    fi

    if [[ "$RENDER" == "1" || "$QA" == "1" ]]; then
        RENDER_DIR="$ROOT/SignatureRenders"
        mkdir -p "$RENDER_DIR"
        "$CHECKER" --render-dir "$RENDER_DIR"
        echo "Signature WAVs: $RENDER_DIR"
    else
        "$CHECKER"
    fi
fi


if [[ "$QA" == "1" ]]; then
    echo
    echo "Running macOS Audio Unit validation when available..."
    if command -v auval >/dev/null 2>&1; then
        if auval -v aumu Hcst Slfs; then
            echo "✅ AU validation passed."
        else
            echo "⚠️  AU validation reported a failure. Review the auval output before release."
            exit 2
        fi
    else
        echo "auval not found; skipping AU validation."
    fi
fi
