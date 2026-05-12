#!/bin/bash
# Build script for NetRemote NuGet package (Linux/macOS)
# This script builds and packs the Microsoft.Net.Remote.Client NuGet package

set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(cd "$SCRIPT_DIR/../.." && pwd)"
OUTPUT_DIR="${PROJECT_ROOT}/out/nuget"

# Default values
CONFIGURATION="Release"
VERSION=""
SKIP_RESTORE=false
SKIP_BUILD=false
CLEAN=false

# Parse command line arguments
while [[ $# -gt 0 ]]; do
    case $1 in
        -c|--configuration)
            CONFIGURATION="$2"
            shift 2
            ;;
        -v|--version)
            VERSION="$2"
            shift 2
            ;;
        -o|--output)
            OUTPUT_DIR="$2"
            shift 2
            ;;
        --skip-restore)
            SKIP_RESTORE=true
            shift
            ;;
        --skip-build)
            SKIP_BUILD=true
            shift
            ;;
        --clean)
            CLEAN=true
            shift
            ;;
        -h|--help)
            echo "Usage: $0 [options]"
            echo ""
            echo "Options:"
            echo "  -c, --configuration <config>  Build configuration (Debug|Release). Default: Release"
            echo "  -v, --version <version>       Package version (e.g., 1.0.0). Default: from git tag or 0.5.2"
            echo "  -o, --output <dir>            Output directory for the NuGet package"
            echo "      --skip-restore            Skip package restore"
            echo "      --skip-build              Skip build (pack only)"
            echo "      --clean                   Clean build artifacts before building"
            echo "  -h, --help                    Show this help message"
            exit 0
            ;;
        *)
            echo "Unknown option: $1"
            exit 1
            ;;
    esac
done

# Check for dotnet CLI
if ! command -v dotnet &> /dev/null; then
    echo "Error: dotnet CLI not found. Please install .NET SDK."
    echo "Visit: https://dotnet.microsoft.com/download"
    exit 1
fi

echo "=== NetRemote NuGet Package Build ==="
echo "Configuration: $CONFIGURATION"
echo "Output: $OUTPUT_DIR"

# Determine version if not specified
if [ -z "$VERSION" ]; then
    # Try to get version from git tag
    if command -v git &> /dev/null && [ -d "$PROJECT_ROOT/.git" ]; then
        LATEST_TAG=$(git -C "$PROJECT_ROOT" tag -l --sort=-v:refname | head -n1)
        if [[ "$LATEST_TAG" =~ ^v([0-9]+)\.([0-9]+)\.([0-9]+) ]]; then
            VERSION="${BASH_REMATCH[1]}.${BASH_REMATCH[2]}.${BASH_REMATCH[3]}"
        fi
    fi

    # Fallback to default version
    if [ -z "$VERSION" ]; then
        VERSION="0.5.2"
    fi
fi

echo "Version: $VERSION"

cd "$SCRIPT_DIR"

# Clean if requested
if [ "$CLEAN" = true ]; then
    echo ""
    echo "Cleaning build artifacts..."
    dotnet clean NetRemoteClient.csproj -c "$CONFIGURATION" || true
    rm -rf bin obj
fi

# Restore packages
if [ "$SKIP_RESTORE" = false ]; then
    echo ""
    echo "Restoring packages..."
    dotnet restore NetRemoteClient.csproj
fi

# Build
if [ "$SKIP_BUILD" = false ]; then
    echo ""
    echo "Building..."
    dotnet build NetRemoteClient.csproj -c "$CONFIGURATION" --no-restore
fi

# Create output directory
mkdir -p "$OUTPUT_DIR"

# Pack
echo ""
echo "Creating NuGet package..."
dotnet pack NetRemoteClient.csproj \
    -c "$CONFIGURATION" \
    -o "$OUTPUT_DIR" \
    -p:Version="$VERSION" \
    --no-build

echo ""
echo "=== Build Complete ==="
echo "NuGet package created at: $OUTPUT_DIR"
ls -la "$OUTPUT_DIR"/*.nupkg 2>/dev/null || echo "No .nupkg files found"
