#include "Path.h"
#include <io.h>
#include <vector>

std::string GetExePath()
{
    char szFilePath[MAX_PATH + 1] = { 0 };
    GetModuleFileNameA(NULL, szFilePath, MAX_PATH);
    /*
    strrchr:�������ܣ�����һ���ַ�c����һ���ַ���str��ĩ�γ��ֵ�λ�ã�Ҳ���Ǵ�str���Ҳ࿪ʼ�����ַ�c�״γ��ֵ�λ�ã���
    ���������λ�õĵ�ַ�����δ���ҵ�ָ���ַ�����ô����������NULL��
    ʹ�������ַ���ش����һ���ַ�c��strĩβ���ַ�����
    */
    (strrchr(szFilePath, '\\'))[0] = 0; // ɾ���ļ�����ֻ���·���ִ�//
    std::string path = szFilePath;
    return path;
}

std::string GetDirectory(const std::string& path)
{
    size_t found = path.find_last_of("/\\");
    return(path.substr(0, found));
}
//��ȡ�ļ���������׺
std::string GetFileNameWithoutExt(const std::string& path)
{
    size_t found = path.find_last_of("/\\") + 1;
    size_t foundDot = path.find_last_of(".");
    return(path.substr(found, foundDot - found));
}
//��ȡ�ļ���+��׺
std::string GetFileNameWithExt(const std::string& path)
{
    size_t found = path.find_last_of("/\\") + 1;
    return(path.substr(found, path.length() - found));
}
//��ȡ�ļ����ĺ�׺
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
//��ȡ�ļ���·��+�ļ��� ������׺
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

//��ȡָ��Ŀ¼�µ�����ָ�������ļ����������ļ��У�
void getAllFiles(std::string path, std::vector<std::string>& files, const char* sType) 
{
    //�ļ����
    long hFile = 0;
    //�ļ���Ϣ
    struct _finddata_t fileinfo;
    std::string p;
    if ((hFile = _findfirst(p.assign(path).append("\\*").c_str(), &fileinfo)) != -1) {
        do {
            //�Ƚ��ļ������Ƿ����ļ���
            if ((fileinfo.attrib & _A_SUBDIR))
            {  // ������ǰĿ¼�͸�Ŀ¼
                if (strcmp(fileinfo.name, ".") != 0 && strcmp(fileinfo.name, "..") != 0)
                {
                    //files.push_back(p.assign(path).append("\\").append(fileinfo.name));
                    char* pName = fileinfo.name;
                    char* pFind = strstr(pName, sType);
                    if (pFind != NULL)
                    {
                        files.push_back(p.assign(path).append("\\").append(fileinfo.name));
                    }
                    //�ݹ�����
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
        } while (_findnext(hFile, &fileinfo) == 0);  //Ѱ����һ�����ɹ�����0������-1
        _findclose(hFile);
    }
}