﻿#include "INIBufferManager.h"

void INIBufferManager::ClearBuffer(EventSystem* sender, Event e, void* args)
{
	// 释放INIFileBuffer
	for (auto fileBuffer : s_File)
	{
		fileBuffer->ClearBuffer();
	}
	s_File.clear();
	// 释放LinkedBuffer
	for (auto linkedBuffer : s_LinkedBuffer)
	{
		//linkedBuffer.second->Expired();
		GameDelete(linkedBuffer.second);
	}
	s_LinkedBuffer.clear();
}

INIFileBuffer* INIBufferManager::FindFile(std::string fileName)
{
	for (auto buffer : s_File)
	{
		if (buffer->FileName == fileName)
		{
			return buffer;
		}
	}
	// 为每个iniFile创建一个存储对象
	// INIFileBuffer在构建时会读取KV对，以字符串形式缓存
	std::string f = fileName.data();
	INIFileBuffer* buffer = GameCreate<INIFileBuffer>(f);
	s_File.push_back(buffer);
	return buffer;
}

INIBuffer* INIBufferManager::FindBuffer(std::string fileName, std::string section)
{
	// ini文件按顺序储存，在查找时先读取Map.ini，GameMode.ini，最后读取Rules.ini
	return FindFile(fileName)->GetSection(section);
}

INILinkedBuffer* INIBufferManager::FindLinkedBuffer(std::vector<std::string> dependency, std::string section)
{
	const LinkedBufferKey key{ dependency, section };
	auto it = s_LinkedBuffer.find(key);
	if (it != s_LinkedBuffer.end())
	{
		return it->second;
	}
	INILinkedBuffer* linkedBuffer{};
	// 缓存的是最后一个对象，所以是和Dependency的顺序相反，要倒着构建
	std::vector<std::string>::reverse_iterator rit;
	for(rit = dependency.rbegin(); rit != dependency.rend(); rit++)
	{
		std::string iniFileName = *rit;
		INIBuffer* buffer = FindBuffer(iniFileName, section);
		linkedBuffer = GameCreate<INILinkedBuffer>( buffer, linkedBuffer);
	}
	s_LinkedBuffer[key] = linkedBuffer;

	return linkedBuffer;
}

std::vector<INIFileBuffer*> INIBufferManager::s_File = {};
std::map<INIBufferManager::LinkedBufferKey, INILinkedBuffer*> INIBufferManager::s_LinkedBuffer = {};