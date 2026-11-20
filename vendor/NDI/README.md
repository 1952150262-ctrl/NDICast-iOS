把 NDI SDK for iOS 放到这个目录（一次性）：

1. 到 https://ndi.video/for-developers/ndi-sdk/ 下载 "NDI SDK for iOS"
2. 解压后，确保目录结构是：

   vendor/NDI/include/Processing.NDI.Lib.h     ← 头文件（整个 include 目录拷过来）
   vendor/NDI/libndi_ios.a                     ← 静态库（arm64）

3. SDK 里通常长这样：
   <NDI SDK>/include/*.h
   <NDI SDK>/lib/libndi_ios.a
   把 include 和 libndi_ios.a 按上面结构放好即可。

注意：NDI SDK 的许可只允许你自己编译使用，不要把 SDK 文件公开分发。
如果 GitHub 仓库是公开的，建议把本目录加入 .gitignore，改用 Actions 的
repository secret（base64 打包）再解出来，或者干脆用私有仓库。
