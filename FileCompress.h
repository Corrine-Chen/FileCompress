#pragma once
#include <string>
#include "HuffmanTree.hpp"
#include <iostream>
using namespace std;


struct FileInfo 
{
	FileInfo(long long appearCount = 0)
		: _appearCount(appearCount)
	{}

	char _ch;           // �ַ�
	long long _appearCount;   // �ַ����ֵĴ���
	std::string _strCode;     // �ַ���huffman����
};

bool operator<(const FileInfo& left, const FileInfo& right)
{
	return left._appearCount < right._appearCount;
}

FileInfo operator+(const FileInfo& left, const FileInfo& right)
{
	return FileInfo(left._appearCount + right._appearCount);
}

bool operator==(const FileInfo& left, const FileInfo& right)
{
	return left._appearCount != right._appearCount;
}

bool operator!=(const FileInfo& left, const FileInfo& right)
{
	return left._appearCount != right._appearCount;
}

class FileCompress
{
public:
	FileCompress()
	{
		// ��ʼ��_fileInfo�ṹ�е��ַ���Ϣ
		for (size_t idx = 0; idx < 256; idx++)
			_fileInfo[idx]._ch = idx;
	}

	void CompressFile(std::string _strFilePath)
	{
		// 1��ͳ���ļ�����ͬ�ַ����ֵĴ���
		FILE* fIn = fopen(_strFilePath.c_str(), "r");
		if(NULL == fIn)
		{
			cout<<"Դ�ļ�·���Ƿ�"<<endl;
			return;
		}

		// ��ȡ��ͬ�ַ����ֵĴ���
		char* pReadBuff = new char[1024];
		while(true)
		{
			size_t readSize = fread(pReadBuff, 1, 1024, fIn);
			if(0 == readSize)
				break;

			for (size_t idx = 0; idx < readSize; ++idx)
				_fileInfo[pReadBuff[idx]]._appearCount++;
		}

		// ͨ��ÿ���ַ����ֵĴ�������huffman��
		HuffmanTree<FileInfo> ht(_fileInfo, 256, FileInfo(0));

		// ��ȡhuffman����
		_GenerateHuffmanCode(ht.GetRoot());

		FILE* fOut = fopen("1.hzp", "w");
		assert(fOut);

		// ����ѹ����Ϣ
		// ��ȡԴ�ļ��ĺ�׺
		std::string strFileHead;
		strFileHead = GetFilePostFix(_strFilePath);

		// ��ȡ�������Ϣ
		std::string strCode;
		char szAppearCount[32];
		size_t lineCount = 0;
		for (size_t idx = 0; idx < 256; idx++)
		{
			if(_fileInfo[idx]._appearCount)
			{
				strCode += _fileInfo[idx]._ch;
				strCode += ',';
				itoa(_fileInfo[idx]._appearCount, szAppearCount, 10);
				strCode += szAppearCount;
				strCode += '\n';
				lineCount++;
			}
		}

		itoa(lineCount, szAppearCount, 10);
		strFileHead += '\n';
		strFileHead += szAppearCount;
		strFileHead += '\n';
		strFileHead += strCode;
		
		fwrite(strFileHead.c_str(), 1, strFileHead.length(), fOut);

		fseek(fIn, 0, SEEK_SET);
		// ���¸�д�ļ�
		char* pWriteBuff = new char[1024];
		size_t writeSize = 0;
		char value = 0;
		int pos = 0;
		while(true)
		{
			size_t readSize = fread(pReadBuff, 1, 1024, fIn);
			if(0 == readSize)
				break;

			for (size_t idx = 0; idx < readSize; ++idx)
			{
				std::string& strCode = _fileInfo[pReadBuff[idx]]._strCode;

				// "100" "101" "101"
				for (size_t i = 0; i < strCode.length(); ++i)
				{
					value <<= 1;
					if('1' == strCode[i])
						value |= 1;

					if(++pos == 8)
					{
						pWriteBuff[writeSize++] = value;
						if(1024 == writeSize)
						{
							fwrite(pWriteBuff, 1, 1024, fOut);
							writeSize = 0;
						}

						value = 0;
						pos = 0;
					}
				}
			}
		}

		if(pos)
		{
			value <<= (8-pos);
			pWriteBuff[writeSize++] = value;
		}

		if(writeSize)
			fwrite(pWriteBuff, 1, writeSize, fOut);

		delete[] pReadBuff;
		delete[] pWriteBuff;

		fclose(fIn);
		fclose(fOut);
	}

	void UnCompressFile(const std::string& strFilePath)
	{
		FILE* fIn = fopen(strFilePath.c_str(), "r");
		assert(fIn);

		std::string filePostFix;
		ReadLine(fIn, filePostFix);

		std::string strLineCount;
		ReadLine(fIn, strLineCount);
		size_t lineCount = atoi(strLineCount.c_str());

		std::string strCode;
		//?????????????????////
		for (size_t idx = 0; idx < lineCount; ++idx)
		{
			ReadLine(fIn, strCode); // A,1
			_fileInfo[strCode[0]]._appearCount = atoi(strCode.substr(2, strCode.length()-2).c_str());
			strCode = "";
		}

		HuffmanTree<FileInfo> ht(_fileInfo, sizeof(_fileInfo)/sizeof(_fileInfo[0]), FileInfo(0));
		HuffmanTreeNode<FileInfo>* pCur = ht.GetRoot();
		// ��ѹ��

		FILE* fOut = fopen("2.txt", "w");
		assert(fOut);

		char* pReadBuff = new char[1024];
		char* pWriteBuff = new char[1024];
		size_t writeSize = 0;
		size_t fileSize = pCur->_weight._appearCount;
		int pos = 7;
		while(true)
		{
			size_t readSize = fread(pReadBuff, 1, 1024, fIn);
			if(0 == readSize)
				break;

			for (size_t idx = 0; idx < readSize;)
			{
				if(pReadBuff[idx]& (1 << pos))
					pCur = pCur->_pRight;
				else
					pCur = pCur->_pLeft;

				if(NULL == pCur->_pLeft && NULL == pCur->_pRight)
				{
					pWriteBuff[writeSize++] = pCur->_weight._ch;
					if(1024 == writeSize)
					{
						fwrite(pWriteBuff, 1, 1024, fOut);
						writeSize = 0;
					}

					fileSize--;
					if(0 == fileSize)
						break;

					pCur = ht.GetRoot();
				}

				pos--;
				// ��ǰ�ֽ������еı���Ϊ�Ѿ�ȫ������
				if(pos < 0)
				{
					pos = 7;
					idx++;
				}
			}
		}

		if(writeSize)
			fwrite(pWriteBuff, 1, writeSize, fOut);

		delete[] pReadBuff;
		delete[] pWriteBuff;

		fclose(fIn);
		fclose(fOut);
	}

private:
	void _GenerateHuffmanCode(HuffmanTreeNode<FileInfo>* pRoot)
	{
		if(NULL == pRoot)
			return;

		//if(pRoot->_pLeft)
			_GenerateHuffmanCode(pRoot->_pLeft);

		//if(pRoot->_pRight)
			_GenerateHuffmanCode(pRoot->_pRight);

		if(NULL == pRoot->_pLeft && NULL == pRoot->_pRight)
		{
			string& strCode = _fileInfo[pRoot->_weight._ch]._strCode;
			HuffmanTreeNode<FileInfo>* pCur = pRoot;
			HuffmanTreeNode<FileInfo>* parent = pCur->_pParent;

			while(parent)
			{
				if(pCur == parent->_pLeft)
					strCode += '0';
				else
					strCode += '1';

				pCur = parent;
				parent = parent->_pParent;
			}
			
			//[)
			reverse(strCode.begin(), strCode.end());
		}
	}

	void ReadLine(FILE* fp, std::string& strRet)
	{
		if(feof(fp))
			return;

		char ch = fgetc(fp);
		while('\n' != ch)
		{
			strRet += ch;

			if(feof(fp))
				return;

			ch = fgetc(fp);
		}
	}

	std::string GetFilePostFix(const std::string& strFilePath)
	{
		size_t pos = strFilePath.find_last_of('.');
		return strFilePath.substr(pos, strFilePath.length() - pos);
	}

private:
	FileInfo _fileInfo[256];
};

void Test()
{
	FileCompress file;
	//file.CompressFile("1.txt");
	file.UnCompressFile("1.hzp");
}