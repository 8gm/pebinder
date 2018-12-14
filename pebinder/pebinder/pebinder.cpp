// pebinder.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#include "resource.h"
#include <Windows.h>

BOOL ReplaceICO(LPCTSTR lpszApp, LPCTSTR lpszICO)
{
	if( NULL == lpszApp )
		return 0;
	if( NULL == lpszICO )
		return 0;

    HANDLE    hICO;
    //��Ӳ���ϵ�ͼ���ļ�
    if (!(hICO = ::CreateFile(lpszICO, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL)))
    {
        return FALSE;
    }
    LONG    lOffset;    //��Դ���ļ��е�ƫ����
    DWORD    dwSize;        //���غ���Դ�Ĵ�С
    DWORD    dwReadByte;    //��ȡ�ļ�ʱʵ�ʶ�ȡ��С��APIǿ��Ҫ����ʵû�á���
    LPVOID    lpRes;        //ָ�����RT_ICON����Դ
    LPVOID    lpGIco;        //ָ�����RT_GROUP_ICON����Դ
    CONST BYTE byGSize = 0x18;    //RT_GROUP_ICON��Դ�Ĵ�С��Ϊ������
    //����RT_GROUP_ICON��Դ���ڴ档
    if (!(lpGIco = new BYTE[byGSize]))  {::CloseHandle(hICO); return FALSE;}
    memset(lpGIco, 0, byGSize);//����RT_GROUP_ICON��Դ�ڴ�
    ::ReadFile(hICO, lpGIco, 0x06, &dwReadByte, NULL);//���ļ��ж�ȡRT_GROUP_ICON��Դͷ
    ::ReadFile(hICO, (LPVOID)((BYTE*)lpGIco + 0x08), 0x0C, &dwReadByte, NULL);//���ļ��ж�ȡRT_GROUP_ICON��Դ
    //��ȡICO��Դ�Ĵ�С��
    memcpy((void*)&dwSize, (void*)((BYTE*)lpGIco + 0x10), sizeof(DWORD));
    //��ȡICO��Դ��ƫ������
    ::ReadFile(hICO, (void*)&lOffset, sizeof(LONG), &dwReadByte, NULL);
    //����ICO��Դ���ڴ档
    if (!(lpRes = new BYTE[dwSize])) {delete[] lpGIco; ::CloseHandle(hICO); return FALSE;}
    //ƫ���ļ�ָ��
    ::SetFilePointer(hICO, lOffset, NULL, FILE_BEGIN);
    //��ȡͼ����Դ���ڴ档
    ::ReadFile(hICO, lpRes, dwSize, &dwReadByte, NULL);
    ::CloseHandle(hICO);
    //��Ҫ����ͼ��Ŀ�ִ���ļ���
    HANDLE    hApp;
    if (NULL == (hApp = ::BeginUpdateResource(lpszApp, NULL)))
    {
        delete[] lpGIco;
        delete[] lpRes;
        return FALSE;
    }
    //��Ǹ�����Դ
    ::UpdateResource(hApp, RT_GROUP_ICON, MAKEINTRESOURCE(1), 0, lpGIco, byGSize);
    ::UpdateResource(hApp, RT_ICON, MAKEINTRESOURCE(1), 0, lpRes, dwSize);
    //д������Դ
    int error=0;
    if (!::EndUpdateResource(hApp, FALSE))
    {
        delete[] lpGIco;
        delete[] lpRes;
        error=GetLastError();
        return FALSE;
    }
    delete[] lpGIco;
    delete[] lpRes;
    return TRUE;
}


BOOL FreeResFile(DWORD dwResName, LPCSTR lpResType, LPCSTR lpFilePathName )  
{     
    HMODULE hInstance = ::GetModuleHandle(NULL);//�õ�����ʵ�����  
            
    HRSRC hResID = ::FindResource(hInstance,MAKEINTRESOURCE(dwResName),lpResType);//������Դ  
    HGLOBAL hRes = ::LoadResource(hInstance,hResID);//������Դ  
    LPVOID pRes = ::LockResource(hRes);//������Դ  
            
    if (pRes == NULL)//����ʧ��  
    {  
        return FALSE;  
    }  
    DWORD dwResSize = ::SizeofResource(hInstance,hResID);//�õ����ͷ���Դ�ļ���С  
    HANDLE hResFile = CreateFile(lpFilePathName,GENERIC_WRITE,0,NULL,CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL,NULL);//�����ļ�  
            
    if (INVALID_HANDLE_VALUE == hResFile)  
    {  
        //TRACE("�����ļ�ʧ�ܣ�");  
        return FALSE;  
    }  
        
    DWORD dwWritten = 0;//д���ļ��Ĵ�С     
    WriteFile(hResFile,pRes,dwResSize,&dwWritten,NULL);//д���ļ�  
    CloseHandle(hResFile);//�ر��ļ����  
            
    return (dwResSize == dwWritten);//��д���С�����ļ���С�����سɹ�������ʧ��  
}


BOOL ImportFile(char* sname,int resno , char* lst_name) 
{ 
	BOOL Bret = FALSE;
    HANDLE hFile; 
    DWORD dwFileSize,       
          dwBytesRead; 
    LPBYTE lpBuffer; 
     
    hFile = CreateFile(sname, GENERIC_READ,  
                       0, 
                       NULL, 
                       OPEN_EXISTING, 
                       FILE_ATTRIBUTE_NORMAL, 
                       NULL); 

    if (INVALID_HANDLE_VALUE != hFile) 
    { 
        dwFileSize = GetFileSize(hFile, NULL); 

        lpBuffer = new BYTE[dwFileSize]; 

        if (ReadFile(hFile, lpBuffer, dwFileSize, &dwBytesRead, NULL) != FALSE) 
        { 
            // do something with lpBuffer here //Updating the resource data 
            HANDLE hResource; 
            hResource = BeginUpdateResource(lst_name, FALSE); 
            if (NULL != hResource) 
            { 
                char* p = MAKEINTRESOURCE(resno); 
                if (UpdateResource(hResource,  
                    RT_RCDATA,  
                    MAKEINTRESOURCE(resno),  
                    MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),  //ʹ��ϵͳȱʡ���� 
                    (LPVOID) lpBuffer,  
                    dwFileSize) != FALSE) 
                { 
                    EndUpdateResource(hResource, FALSE); 
					Bret = TRUE;
					DWORD dwError=GetLastError();
					printf("ok\n");
                } 
            } 
        } 
        delete [] lpBuffer;           
        CloseHandle(hFile); 
    } 

	return Bret;
} 

void showinfo()
{
		printf("usage:\n");
		printf("pebinder.exe --file frist.exe secoend.exe new_file.exe\n");
		printf("pebinder.exe --ico  ico.ico new_file.exe\n");
}


void imfile(const char* one , const char* two , const char* three)
{
	if( NULL == one )
		return ;
	if( NULL == two )
		return ;
	if( NULL == three )
		return ;



		char onefile[1024] = {0};
		char twofile[1024] = {0};
		char lastfile[1024] = {0};
		memcpy(onefile,one,strlen(one));
		memcpy(twofile,two,strlen(two));
		memcpy(lastfile,three,strlen(three));

		/*
		char onefile[1024] = "d://test//PEiD_ch.exe";
		char twofile[1024] = "d://test//PEiD.exe";
		char lastfile[1024] = "d://test//last.exe";
		*/

		if( FreeResFile(IDR_RC_MYDATA1,"RC_MYDATA",lastfile) )
		{
			printf("relase lastfile success!!\n");

			if( ImportFile(onefile,104,lastfile) )
			{
				printf("frist file import success\n");
			}
			else
			{
				printf("frist file import error!!!\n");
			}

			if( ImportFile(twofile,105,lastfile) )
			{
				printf("secoend file import success\n");
			}
			else
			{
				printf("secoend file import error!!!\n");
			}
		}
		else
		{
			printf("relase lastfile error!!\n");
		}
}


int main(int argc, char** argv) 
{
	if( argc > 2 )
	{
		char str_Type[10] = {0x00};
		char szParam1[10] = {0x00};
		char szParam2[10] = {0x00};
		strcpy(szParam1,"--file");
		strcpy(szParam2,"--ico");
		strcpy(str_Type,argv[1]);

		if( 0 == memcmp(str_Type,szParam1 , strlen(szParam1)) )
		{
			imfile(argv[2],argv[3],argv[4]);
		}
		else if( 0 == memcmp(str_Type,szParam2 , strlen(szParam2)) )
		{ 
			char str_icon[1024] = {0};
			char str_app[1024] = {0};
			strcpy(str_app,argv[3]);
			strcpy(str_icon,argv[2]);

			ReplaceICO(str_app,str_icon);
		}
		else
		{
			showinfo();
		}
		
	}
	else
	{
		showinfo();
	}

	

    return 0; 
} 
