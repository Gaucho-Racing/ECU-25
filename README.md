See [git wiki page](https://wiki.gauchoracing.com/books/onboarding/page/git) for a primer on `git`.
# Required Stuff
## Applications
Mac and Unix download using your package managers (Mac may need the ARM toolchain), Windows should use the links below:
* [CMAKE](https://cmake.org/download/)
* [ARM toolchain (arm-none-eabi), CHECK OS VERSION](https://developer.arm.com/downloads/-/arm-gnu-toolchain-downloads)
* [openocd](https://github.com/openocd-org/openocd/releases/tag/v0.12.0)
* [ninja](https://github.com/ninja-build/ninja/releases)
## VS Code Extensions
* CMAKE
* Cortex Debug
### Optional Vscode Extensions
* Serial Monitor

# How To Run
Download "Required Stuff" and add to your PATH.
## VS Code
1. Open VS Code.
2. If the CMake Extension prompts you to set up the configuration, choose DEBUG and proceed to step 4.
3. Generate the build stuff with `cmake --preset <target>`. With target in development being `Debug` typically.
4. Run and Debug, Debug using the provided Debug config, Cortex Debug.
## Shell Script
1. Execute the `debug.sh` script (does not require VS Code).
2. If you are not trying to flash the chip run `debug.sh build` instead.

# Debugging
Chances are something will not work right the first try, likely one of the tools wasn't added to your PATH correctly.

Run each of the tools independently:
* CMAKE - `cmake --version`
* ARM toolchain - `arm-none-eabi-gcc --version`
* openocd - `openocd --version`
* ninja - `ninja --version`
