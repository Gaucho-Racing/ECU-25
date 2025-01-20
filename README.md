see [git wiki page](https://wiki.gauchoracing.com/books/onboarding/page/git) for a primer on git
# Required stuff
## applications
Mac and Unix download using package managers (besides arm-toolchain), windows use links below
* [CMAKE](https://cmake.org/download/)  
* [ARM toolchain (arm-none-eabi), CHECK OS VERSION](https://developer.arm.com/downloads/-/arm-gnu-toolchain-downloads)  
* [openocd](https://github.com/openocd-org/openocd/releases/tag/v0.12.0)  
* [ninja](https://github.com/ninja-build/ninja/releases)  
## vscode extensions
* CMAKE  
* Cortex Debug
### Optional Vscode extensions
* Serial Monitor

# How 2 Run
1. Download Required stuff and add to PATH
2. Open VSCode  
3. If the CMake Extension prompts you to set up the configuration, choose DEBUG and proceed to step 5
4. Generate the build stuff with `cmake --preset <target>`. With target in development being `Debug` typically.  
5. Run and Debug, Debug using the provided Debug config, Cortex Debug  
Cry as nothing works

# Debugging
If something doesn't work chances are one of the tools wasn't added to path correctly, run each of the tools independently  
* CMAKE - `cmake --version`
* ARM toolchain - `arm-none-eabi-gcc --version`
* openocd - `openocd --version`
* ninja - `ninja --version`
