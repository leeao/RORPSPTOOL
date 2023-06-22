#include "Path.h"
#include <io.h>
#include <vector>

std::string GetExePath()
{
    char szFilePath[MAX_PATH + 1] = { 0 };
    GetModuleFileNameA(NULL, szFilePath, MAX_PATH);
    /*
    strrchr:函数功能：查找一个字符c在另一个字符串str中末次出现的位置（也就是从str的右侧开始查找字符c首次出现的位置），
    并返回这个位置的地址。如果未能找到指定字符，那么函数将返回NULL。
    使用这个地址返回从最后一个字符c到str末尾的字符串。
    */
    (strrchr(szFilePath, '\\'))[0] = 0; // 删除文件名，只获得路径字串//
    std::string path = szFilePath;
    return path;
}

std::string GetDirectory(const std::string& path)
{
    size_t found = path.find_last_of("/\\");
    return(path.substr(0, found));
}
//获取文件名不带后缀
std::string GetFileNameWithoutExt(const std::string& path)
{
    size_t found = path.find_last_of("/\\") + 1;
    size_t foundDot = path.find_last_of(".");
    return(path.substr(found, foundDot - found));
}
//获取文件名+后缀
std::string GetFileNameWithExt(const std::string& path)
{
    size_t found = path.find_last_of("/\\") + 1;
    return(path.substr(found, path.length() - found));
}
//获取文件名的后缀
std::string GetExtName(const std::string& path)
{
    size_t found = path.find_last_of(".");
    if ((int)found != -1)
    {
        return(path.substr(found + 1, path.length() - found + 1));
    }
    else
    {
        return "";
    }
}
//获取文件名路径+文件名 不带后缀
std::string GetFileNameAddPathWithouExt(const std::string& path)
{
    size_t found = path.find_last_of(".");
    return(path.substr(0, found));
}
BOOL CheckFileReadable(const char* strPath)
{
    bool isExist = false;
    if (!_access(strPath, 4))
        isExist = true;
    else
        isExist = false;
    return isExist;
}
BOOL CheckFolderExist(const char* strPath)
{
    bool isExist = false;
    if (!_access(strPath, 0))
        isExist = true;
    else
        isExist = false;
    return isExist;
}

//获取指定目录下的所有指定类型文件（搜索子文件夹）
void getAllFiles(std::string path, std::vector<std::string>& files, const char* sType) 
{
    //文件句柄
    long hFile = 0;
    //文件信息
    struct _finddata_t fileinfo;
    std::string p;
    if ((hFile = _findfirst(p.assign(path).append("\\*").c_str(), &fileinfo)) != -1) {
        do {
            //比较文件类型是否是文件夹
            if ((fileinfo.attrib & _A_SUBDIR))
            {  // 跳过当前目录和父目录
                if (strcmp(fileinfo.name, ".") != 0 && strcmp(fileinfo.name, "..") != 0)
                {
                    //files.push_back(p.assign(path).append("\\").append(fileinfo.name));
                    char* pName = fileinfo.name;
                    char* pFind = strstr(pName, sType);
                    if (pFind != NULL)
                    {
                        files.push_back(p.assign(path).append("\\").append(fileinfo.name));
                    }
                    //递归搜索
                    getAllFiles(p.assign(path).append("\\").append(fileinfo.name), files, sType);
                }
            }
            else
            {
                //files.push_back(p.assign(path).append("\\").append(fileinfo.name));
                char* pName = fileinfo.name;
                char* pFind = strstr(pName, sType);
                if (pFind != NULL)
                {
                    files.push_back(p.assign(path).append("\\").append(fileinfo.name));
                }
            }
        } while (_findnext(hFile, &fileinfo) == 0);  //寻找下一个，成功返回0，否则-1
        _findclose(hFile);
    }
}