#### CMake

1. 写CMakeLists.txt

2. 执行`cmake PATH`

3. 使用`make`进行编译

   

语法

注释：`#`

块注释使用：`#[[ ....]]`

引用变量：`${<variable>}`

常用命令

1. 指定需要最小的cmake版本

   ```cmake
   cmake_minimum_required(VERSION 3.4.1)
   ```

2. 查找目录下源文件并保存到相应的变量中

   ```cmake
   aux_source_directory(. SRC_LIST)
   ```

3. 添加库

   ```cmake
   add_library(<name> [STATIC|SHARED|MODULE] [EXCLUDE_FROM_ALL] source1 source2 ... sourceN)
   ```

   添加一个名为`<name>`的库文件

   指定`STATIC, SHARED, MODULE`参数来指定要创建的库的类型, `STATIC`对应的静态库(.a)，`SHARED`对应共享动态库(.so)

   `[EXCLUDE_FROM_ALL]`, 如果指定了这一属性，对应的一些属性会在目标被创建时被设置(**指明此目录和子目录中所有的目标，是否应当从默认构建中排除, 子目录的IDE工程文件/Makefile将从顶级IDE工程文件/Makefile中排除**)

   `source1 source2 ... sourceN`用来指定源文件

4. 导入已有库

   ```cmake
   add_library(<name> [STATIC | SHARED | MODULE | UNKNOWN] IMPORTED)
   ```

   导入了一个已存在的`<name>`库文件，导入库一般配合`set_target_properties`使用，这个命令用来指定导入库的路径,比如：

   ```cmake
   add_library(test SHARED IMPORTED)
   set_target_properties(  test #指定目标库名称
                           PROPERTIES IMPORTED_LOCATION #指明要设置的参数
                           libs/src/${ANDROID_ABI}/libtest.so #设定导入库的路径)
   ```

5. 设置CMake变量`set`

   ```cmake
   # 设置可执行文件的输出路径(EXCUTABLE_OUTPUT_PATH是全局变量)
   set(EXECUTABLE_OUTPUT_PATH [output_path])
   ```

   ```cmake
   # 设置库文件的输出路径(LIBRARY_OUTPUT_PATH是全局变量)
   set(LIBRARY_OUTPUT_PATH [output_path])
   ```

   ```cmake
   # 设置C++编译参数(CMAKE_CXX_FLAGS是全局变量)
   set(CMAKE_CXX_FLAGS "-Wall std=c++11")
   ```
   ```cmake
   # 设置源文件集合(SOURCE_FILES是本地变量即自定义变量)
   set(SOURCE_FILES main.cpp test.cpp ...)
   ```
   设置变量：`set(Name value)`

   取消设置变量：`unset(Name value)`

   设置列表`set(List a b c)`、`set(List a;b;c)`、`set(List "a;b;c")`

6. 设置头文件位置`include_directorie`

   ```cmake
   # 可以用相对货绝对路径，也可以用自定义的变量值
   include_directories(./include ${MY_INCLUDE})
   ```

7. 添加可执行文件`add_executable`

   ```cmake
   add_executable(<name> ${SRC_LIST})
   ```

8. 将若干库链接到目标库文件`target_link_libraries`

   ```cmake
   target_link_libraries(<name> lib1 lib2 lib3)
   ```


9. 添加子目录`add_subdirectory`

   ```cmake
   # sub_dir指定包含CMakeLists.txt和源码文件的子目录位置
   # binary_dir是输出路径， 一般可以不指定
   add_subdirecroty(sub_dir [binary_dir])
   ```

   如果当前目录下还有子目录时可以使用`add_subdirectory`，子目录中也需要包含有`CMakeLists.txt`


cmake交叉编译
```cmake
set(CMAKE_SYSTEM_NAME Linux)

set(TOOLCHAIN_PATH /OPT/gcc-arm-linux-gnueabi)
set(CMAKE_C_COMPILER ${tools}/bin/arm-linux-gnueabi-gcc)
set(CMAKE_CXX_COMPILER ${tools}/bin/arm-linux-gnueabi-g++)
```

`./configure`交叉编译

```cmake
./configure --build --host --target
```

build：执行代码编译的主机，正常的话就是你的主机系统。这个参数一般由config.guess来猜就可以。当然自己指定也可以。

**host：**编译出来的二进制程序所执行的主机，因为绝大多数是本机编译，本机执行，所以这个值就等于build。只有交叉编译的时候（也就是本机编译，其他系统机器执行）才会build和host不同。用host指定运行主机。

target：这个选项只有在建立交叉编译环境的时候用到，正常编译和交叉编译都不会用到。他用build主机上的编译器，编译一个新的编译器（binutils, gcc,gdb等），这个新的编译器将来编译出来的其他程序将运行在target指定的系统上。

总的来说，只有host !=build的时候编译才是交叉编译。否则就是正常编译。