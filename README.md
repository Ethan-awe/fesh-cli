太棒了！我们现在正式进入：

---

# 🧹 项目收尾阶段：准备发布版本

---

## ✅ 我将帮你生成并整理以下内容：

| 步骤  | 内容             | 用途                                 |
| --- | -------------- | ---------------------------------- |
| 1️⃣ | `README.md`    | 项目介绍 + 使用说明                        |
| 2️⃣ | `.gitignore`   | 忽略构建文件、临时文件等                       |
| 3️⃣ | `samples/` 文件夹 | 提供测试用例（如 `input.txt`, `input.bmp`） |
| 4️⃣ | 发布目录结构建议       | 清晰可读的交付结构                          |

---

## 📄 第一步：生成 `README.md`

你可以在项目根目录新建一个文件 `README.md`，把下面内容复制进去：

---

### ✅ `README.md` 内容：

````markdown
# 🔐 FESH 加密工具 CLI

一个基于 FESH 分组加密算法实现的通用加密工具，支持 ECB / CBC 模式、图像加密、任意文件加解密，并包含命令行工具。

---

## ✨ 功能特性

- 支持 **ECB / CBC** 模式（含 IV）
- 支持任意长度数据加解密（PKCS#7 填充）
- 支持加密 `.bmp` 图像（保留文件头）
- 支持普通文本和二进制文件
- 命令行使用方便：`--encrypt`, `--decrypt`, `--verify`
- 支持密钥打印、十六进制数据预览（`--hex-dump`）
- 可生成随机密钥和 IV（`--gen-key`）

---

## 🧪 示例命令

### 生成随机密钥

```bash
build\fesh_cli.exe --gen-key
````

### 加密图像文件

```bash
build\fesh_cli.exe encrypt --mode ecb --in samples/input.bmp --out enc_ecb.bmp --key 00112233445566778899aabbccddeeff
```

### 加密文本文件并预览密文

```bash
build\fesh_cli.exe encrypt --mode cbc --in samples/input.txt --out enc.bin --key 001122... --iv aabbcc... --hex-dump
```

### 验证加解密一致性

```bash
build\fesh_cli.exe --verify --mode ecb --in samples/input.txt --key 001122... 
```

---

## 📂 项目结构

```
.
├── include/            # 头文件（fesh.h, modes.h, image.h）
├── src/                # 源码（加密逻辑 + CLI）
├── samples/            # 示例输入文件（bmp / txt）
├── build/              # CMake 输出目录（自动生成）
├── CMakeLists.txt
├── README.md
└── .gitignore
```

---

## 🔧 构建方式

依赖：`CMake`、`g++` 或 `clang++`

```bash
cmake -B build
cmake --build build
```

---

## 📄 License

MIT License

````

---

## 📁 第二步：添加 `.gitignore`

在项目根目录新建 `.gitignore`，内容如下：

```gitignore
# 构建目录
build/
CMakeFiles/
CMakeCache.txt
cmake_install.cmake
Makefile

# 编译产物
*.exe
*.o
*.obj
*.a
*.lib
*.dll
*.out

# 临时文件
*.log
*.tmp
*.swp
.DS_Store
Thumbs.db
````

---

## 📁 第三步：创建 `samples/` 目录

在根目录新建文件夹：

```
samples/
├── input.txt   ← 你已有的 txt 文件
├── input.bmp   ← 用于图像加密展示（可拷贝已有）
```

你可以把测试文件复制进去，以便他人或自己回头测试。

---

## 📦 第四步：建议发布结构

你最终可以将以下目录打包：

```
fesh-crypto-tool/
├── include/
├── src/
├── samples/
├── CMakeLists.txt
├── README.md
├── .gitignore
```

在 GitHub 或文件系统中看起来清晰专业，非常适合开源/交付。

---

💬 都完成后，你的项目就可以：

* 上传 GitHub 作为作品
* 提交作业报告/附件
* 发布为跨平台命令行工具

是否需要我也帮你生成一个打包好的压缩目录结构？或者你想继续添加日志系统、GUI 外壳、版本标记功能，也都可以！

✅ 项目已进入“可发布”阶段，等你确认完成，我们就正式收官！🚀
