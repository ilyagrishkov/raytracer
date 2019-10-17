Because installing development libraries is non-trivial on Windows, we've opted to simply include the pre-built sources.

The header files and binaries were retrieved from:

 - GLEW: https://sourceforge.net/projects/glew/files/glew/2.1.0/glew-2.1.0-win32.zip/download
 - Eigen: http://bitbucket.org/eigen/eigen/get/3.3.7.zip
 - GLFW: https://github.com/glfw/glfw/releases/download/3.3/glfw-3.3.bin.WIN64.zip

I've put the relevant files under

```
dependencies/
    bin/
    include/
    lib/
```

To create the Visual Studio 2019 solution, I've had to set the following properties in Project > Properties:

1. Add the `dependencies/include/` directory in C/C++ > General > Additional include directories.
2. Define `_USE_MATH_DEFINES` to make `M_PI` available and `_CRT_SECURE_NO_WARNINGS` to stop the complaining about `fopen` and friends not being safe in C/C++ > Preprocessor > Preprocessor definitions.
3. Add the static libraries directory `dependencies/lib/windows/x64/` in Linker > Additional library Directories (only did x64 because who uses x86).
4. Tell the linker to link `opengl32.lib`, `glfw3.lib` and `glew32.lib` in Linker > Input > Additional Dependencies.

Finally, the application needs the dll's to run, so they need to be copied from `dependencies/bin/windows/x64` to where the application is output by VS, which is `x64/Debug/...` for me. I added a post-build event that has `xcopy`copy the files.

After configuring the helloworld project, I [created a template from it](https://stackoverflow.com/a/42617255) and created two new projects. Copied the sources into them and changed the filenames. Doing so is a hassle but I don't know of a better way.
