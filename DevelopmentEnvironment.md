Linux 使用课程提供的虚拟机环境即可,参考作业0

### Windows配置

在windows系统中使用visual studio

#### 库下载

##### 	eigen库

​		[eigen库官网](https://eigen.tuxfamily.org/index.php?title=Main_Page) 右侧下载**latest stable release zip** 文件解压到本地

​		由于作业框架写的include路径是**<eigen3/Eigen/Eigen>**为了不频繁修改每个作业的路径

​		在eigen文件夹下创建文件夹**include**再创建子文件夹**eigen3**

​		把eigen文件夹下**Eigen**文件夹复制到刚刚创建的**include/eigen3**底下

##### 	opencv库

​		[opencv库官方下载](https://opencv.org/releases/) 下载windows安装包,打开选择安装位置解压

​		将**E:\Work\opencv\build\x64\vc16\bin** 添加到系统环境变量中

#### Visual Studio 配置

1. 新建项目,创建空项目
2. 点击顶部菜单栏视图->属性管理器
3. 属性管理器中项目->Debug x64->新建项目属性表 打开
   1. 配置属性->VC++ 目录->包含目录 双击编辑
      - **E:\Work\Eigen3\include**  (即刚刚在eigen库位置新建的include的文件夹)
      - **E:\Work\opencv\build\include**
   2. 配置属性->VC++ 目录->库目录  (根据opencv文件夹下复制,版本不一样vc数字不一样)
      - **E:\Work\opencv\build\x64\vc16\lib**  
   3. 连接器->输入->附加依赖项  (在opencv\build\x64\vc16\lib下找到复制文件名+后缀)
      - **opencv_world4100d.lib**   (Debug环境下是带d的)
4. 属性管理器中项目->配置属性->C/C++->所有属性->附加选项
   - 填入`/utf-8`	(防止中文注释导致编译失败)
5. 属性管理器中项目->配置属性->常规->C++语言标准 
   - 选择`ISO C++17 标准 (/std:c++17)`

回到解决方案资源管理器中将作业框架代码拖动进项目中完成~

#### VSCode配置

1. 安装[cmake](https://github.com/Kitware/CMake) 将bin添加到环境变量

2. 安装[mingw](https://github.com/niXman/mingw-builds-binaries/releases) 将bin添加到环境变量

   ​	进入mingw64\bin创建一个符号链接make.exe 链接到mingw32-make.exe 

   ​	`mklink mingw32-make.exe make.exe`

   ​	可以简化操作命令行直接打make就行,不用mingw32-make

3. 进入opencv/source,在此目录打开cmd

```cmake
mkdir build
cd build
cmake -G "MinGW Makefiles"
#会出现下载部分内容无法下载的提示, 打开CMakeDownloadLog.txt查看下载地址和存放地址, 下载后重新执行上面cmake指令后继续
make -j8
make install
```

​			把**opencv\sources\build\install\x64**下的**mingw**复制到**opencv\build\x64**下

​			将**E:\Work\opencv\build\x64\mingw\bin** 添加到系统环境变量中

4. 打开VSCode, `ctrl+,`打开设置修改一下项目可以搜索修改,

   ```
   "C_Cpp.default.compilerPath": "E:\\Work\\mingw64\\bin\\c++.exe",
   "C_Cpp.default.includePath": [
           "E:\\Work\\mingw64\\include",
           "E:\\Work\\Eigen3\\include",
           "E:\\Work\\opencv\\build\\include"
       ],
   ```

   

5. 在每个作业的CMakeLists中需要修改include路径

   ```cmake
   #opencv
   set(OpenCV_DIR "E:\\Work\\opencv\\build")
   find_package(OpenCV REQUIRED)
   include_directories(${OpenCV_INCLUDE_DIRS})
   target_link_libraries(${PROJECT_NAME} ${OpenCV_LIBS})
   
   #eigen
   #find_package(Eigen3 REQUIRED)#这行删除或者注释掉,因为我们没有make install等操作
   include_directories("E:\\Work\\Eigen3\\include")
   ```

示例:打开一个作业修改CMakeLists,打开terminal 输入

```
mkdir build
cd build
cmake .. -G "MinGW Makefiles"
make
-- 就可以运行make出来的程序
```

