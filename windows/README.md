
# Windows Network Remote Framework

# Development Environment Setup

As described in the main project [`README`](/README.md), a C++ 20 compiler and CMake are required. A known, minimal working environment is using Visual Studio 2022 Build Tools which provides all of the necessary development requirements. Note that the build tools do not provide the Visual Studio IDE, so if this is desired, the Visual Studio product of choice may be installed instead. Instructions for setting up this environment are provided below.

## 1. Install the tools

Download the latest version of [Visual Studio 2022 Build Tools](https://visualstudio.microsoft.com/downloads/#build-tools-for-visual-studio-2022) or [the full Visual Studio IDE](https://visualstudio.microsoft.com/) and run the installer. Select the following 'Workloads':

* ASP.NET and web development
* Desktop development with C++
* .NET desktop development
* Windows Universal Platform development

Select the 'Individual Components' tab at the top, and select the following required components:

* C++ CMake tools for Windows
* MSVC v143 - VS 2022 C++ x64/x86 build tools (Latest)
* MSVC v143 - VS 2022 C++ x64/x86 Spectre-mitigated libs (Latest)
* Windows 11 SDK (10.0.22621.0)
* .NET 7.0 Runtime

Additionally if you are using the Visual Studio 2022 Build Tools install the followng under 'Individual Components'

* C++ Build Tools core features
* C++ core features

The following components are optional, but strongly recommended for a more complete development experience:

* C++ AddressSanitizer
* C++ Clang Compiler for Windows (15.0.1)
* C++ Clang-cl for v143 build tools (x64/x86)

Once installed, the project can be built using CMake as described in the main [`README`](/README.md#cmake).

## 2. Configure Commit signing (optional)
 
Prior to following the generic instructions for configuring a git signing key [here](/README.md#commit-signing), the gpg tools must be installed:

1. Download the latest version of gpg4win from [https://www.gpg4win.org/](https://www.gpg4win.org/).
2. Run the installer, deselect all optional components, leaving only `GnuPG` selected. Accept all other defaults and complete the installation. 
3. Configure git with the location of the Windows gpg tools:

```Shell
git config --global gpg.program "C:\Program Files (x86)\GnuPG\bin\gpg.exe"
```
