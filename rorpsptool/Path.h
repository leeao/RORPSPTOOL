#pragma once
#include <string>
#include <Windows.h>
#include <vector>
//����Ŀ¼
void createDirectory(char* fileName);

//��ȡexe�ļ�����Ŀ¼
std::string GetExePath();

//���ַ����ֻ�ȡ·��Ŀ¼
std::string GetDirectory(const std::string& path);

//����ļ����Ƿ�ɶ�
BOOL CheckFileReadable(const char* strPath);

//����ļ�����Ŀ¼�Ƿ����
BOOL CheckFolderExist(const char* strPath);

//��ȡ�ļ���������׺
std::string GetFileNameWithoutExt(const std::string& path);

//��ȡ�ļ���+��׺
std::string GetFileNameWithExt(const std::string& path);

//��ȡ�ļ����ĺ�׺
std::string GetExtName(const std::string& path);

//��ȡ�ļ���·��+�ļ��� ������׺
std::string GetFileNameAddPathWithouExt(const std::string& path);

//��ȡָ��Ŀ¼�µ�����ָ�������ļ����������ļ��У�
void getAllFiles(std::string path, std::vector<std::string>& files, const char* sType);