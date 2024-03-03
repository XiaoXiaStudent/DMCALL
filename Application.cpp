// example.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "obj.h"
#include "msdk.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

dmsoft* g_dm = NULL;

int main(int argc, TCHAR* argv[], TCHAR* envp[])
{
	int nRetCode = 0;

	// 初始化COM(mta)
	CoInitializeEx(NULL, 0);

	// 设置本地字符集为gbk
	setlocale(LC_ALL, "chs");

	// 注意：这里使用了 const wchar_t* 并且指定了 __stdcall 调用约定
	typedef void(__stdcall* SetDllPathWType)(const wchar_t*, int);

	// 加载DLL
	HMODULE hModule = LoadLibrary(TEXT("DmReg.dll"));

	HANDLE openFYL = M_Open(1);

	M_MoveTo2(openFYL, 200, 300);

	M_Close(openFYL);


 

	// 获取函数地址
	SetDllPathWType SetDllPathW = (SetDllPathWType)GetProcAddress(hModule, "SetDllPathW"); //init

	// 检查函数地址是否成功获取
	if (!SetDllPathW) {

		std::cerr << "获取函数地址失败!" << std::endl;
		FreeLibrary(hModule);
		return 1;
	}

	SetDllPathW(_T("dm.dll"), 0) ;
	FreeLibrary(hModule);

	// 创建对象
	g_dm = new dmsoft;
	if (g_dm == NULL)
	{
		return 1;
	}

	// 注册
	long dm_ret = g_dm->Reg(_T("mh84909b3bf80d45c618136887775ccc90d27d7"), _T("me03d3g0d976m27"));

	CString version = g_dm->Ver();
	if (dm_ret != 1)
	{
		std::cout << "注册失败:" << dm_ret << std::endl;
		std::wcout << L"版本:" << version.GetString() << std::endl;

		delete g_dm;
		return 1;
	}
	else
	{
		std::cout << "注册成功" << std::endl;
		std::wcout << L"版本:" << version.GetString() << std::endl;
	}

	// 接下来可以做一些全局性的设置,比如加载保护盾，设置共享字库等等
	dmsoft& dm = *g_dm;


	dm.Capture(0, 0, 2240, 1400, TEXT("deskphoto.bmp"));

	//dm.FindColor(0, 0, 2240, 1400, TEXT("ffffff-000000"), 1.);

	dm.MoveTo(300, 500);

	delete g_dm;

	return nRetCode;
}