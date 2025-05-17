#pragma once
#include <string>
#include <vector>
#include <cstdint>

namespace fesh::image {

// 读取 BMP 文件像素数据（跳过文件头）
bool LoadBMP(const std::string& path,
             std::vector<uint8_t>& header,
             std::vector<uint8_t>& data);

// 保存 BMP 文件：头部 + 加密后像素
bool SaveBMP(const std::string& path,
             const std::vector<uint8_t>& header,
             const std::vector<uint8_t>& data);

}
