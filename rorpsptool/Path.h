#pragma once
#include <string>
#include <Windows.h>
#include <vector>
//创建目录
void createDirectory(char* fileName);

//获取exe文件所在目录
std::string GetExePath();

//从字符串种获取路径目录
std::string GetDirectory(const std::string& path);

//检查文件名是否可读
BOOL CheckFileReadable(const char* strPath);

//检查文件名或目录是否存在
BOOL CheckFolderExist(const char* strPath);

//获取文件名不带后缀
std::string GetFileNameWithoutExt(const std::string& path);

//获取文件名+后缀
std::string GetFileNameWithExt(const std::string& path);

//获取文件名的后缀
std::string GetExtName(const std::string& path);

//获取文件名路径+文件名 不带后缀
std::string GetFileNameAddPathWithouExt(const std::string& path);

//获取指定目录下的所有指定类型文件（搜索子文件夹）
void getAllFiles(std::string path, std::vector<std::string>& files, const char* sType);