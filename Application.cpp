// example.cpp : 定义控制台应用程序的入口点。
//
#include <Afxwin.h>
#include "stdafx.h"
#include "obj.h"
#include "msdk.h"

 
 
 
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

	// 使用MFC的AfxLoadLibrary加载DLL
	HMODULE hModule = LoadLibrary(TEXT("DmReg.dll"));
	if (hModule == NULL) {
		std::cerr << "加载DLL失败!" << std::endl;
		return 1;
	}

	//飞易来调用鼠标移动逻辑
	/*HANDLE openFYL = M_Open(1);

	M_MoveTo2(openFYL, 200, 300);

	M_Close(openFYL);*/

	/*HANDLE openFYL = Ap5isNXMcaWr(1);

	n8MDw8SN3t(openFYL, 200, 300);

	Dfd8mbZxDc0f(openFYL);*/

	// 获取函数地址
	SetDllPathWType SetDllPathW = (SetDllPathWType)GetProcAddress(hModule, "SetDllPathW");
	if (!SetDllPathW) {
		std::cerr << "获取函数地址失败!" << std::endl;
		FreeLibrary(hModule);
		return 1;
	}

	// 调用函数
	SetDllPathW(_T("dm.dll"), 0);

	FreeLibrary(hModule);

	// 创建对象
	g_dm = new dmsoft();
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
		FreeLibrary(hModule);
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

	long  intX, intY;

	CString result = g_dm->FindColorEx(0, 0, 2000, 2000, L"49A996-0B0D06", 0.8, 0);

	// 检查返回值，确认是否找到了颜色块

	// 构建要显示的消息字符串
	CString message;

	// 使用GetResultCount获取找到的颜色数量
	long count = g_dm->GetResultCount(result);

	for (long index = 0; index < count; ++index) {
		long intX, intY;
		// 对于每个找到的颜色，使用GetResultPos获取其坐标
		g_dm->GetResultPos(result, index, &intX, &intY);

		g_dm->MoveTo(intX, intY);


	}
	std::wstringstream ss;
	for (long index = 0; index < count; ++index) {
		ss << intX << L"," << intY << L" "; // 将坐标添加到stringstream
	}
	

	//dm.Capture(0, 0, 2240, 1400, TEXT("deskphoto.bmp"));

	//dm.FindColor(0, 0, 2240, 1400, TEXT("ffffff-000000"), 1.);

	//dm.MoveTo(300, 500);

	std::cin.get();

	delete g_dm;

	return nRetCode;
}



//void testCode() {
//
//	CString companies[] = { _T("1600") };
//
//	CString AddDaties[] = { _T("tea.0206.002"), _T("tea.0206.007") };
//
//
//	//业务分类基础资料维护
//		//查找更多按钮 点击
//	long intX = 0, intY = 0; // 初始化坐标变量
//	long findMoreButton = dm.FindPic(0, 0, 2240, 1400, L"更多.bmp", L"000000", 0.8, 2, &intX, &intY);
//
//	if (findMoreButton >= 0) {
//		// 找到图片，处理坐标
//		std::cout << "找到更多按钮，坐标为: (" << intX << ", " << intY << ")" << std::endl;
//
//		//点击更多按钮
//		dm.MoveTo(intX, intY);
//
//		dm.LeftClick();
//
//		//等待页面加载完毕
//
//	// 计时开始
//		clock_t start_time = clock();
//
//		// 超时时间设定为20秒
//		const clock_t timeout = 40 * CLOCKS_PER_SEC; // CLOCKS_PER_SEC 是每秒钟的clock ticks
//
//		bool found = false; // 标记是否找到图片
//
//
//
//		while (true) {
//			// 尝试找图
//			long HisensePhoto = dm.FindPic(0, 0, 2240, 1400, L"海信集团.bmp", L"000000", 0.9, 2, &intX, &intY);
//			std::cout << "加载出海信集团，坐标为: (" << intX << ", " << intY << ")" << std::endl;
//			if (intX != -1 && intY != -1) {
//				found = true; // 图片被找到
//				break; // 跳出循环
//			}
//
//			// 检查是否超时
//			if (clock() - start_time > timeout) {
//				break; // 如果超过20秒还没找到图片，则终止循环
//			}
//
//			Sleep(1000); // 每次尝试之间等待1秒（1000毫秒），减少CPU使用率
//		}
//
//		if (found) {
//			// 找到图片，继续执行后续操作
//
//			long FindComany = dm.FindPic(0, 0, 2240, 1400, L"搜索公司.bmp", L"000000 ", 0.9, 2, &intX, &intY);
//			std::cout << "找到海信搜索，坐标为: (" << intX << ", " << intY << ")" << std::endl;
//			dm.MoveTo(intX + 50, intY);
//
//			dm.LeftClick();
//
//
//
//			//输入公司
//			dm.KeyPressStr(companies[0], 20);
//			wprintf(L"当前公司为: %s\n", companies[0].GetString());
//			//按下回车
//			dm.KeyDown(13);
//
//			Sleep(2000); // 等待两秒 加载出公司
//
//			//选择公司这里写死
//			dm.MoveTo(764, 420);
//
//			dm.LeftClick();
//			//如果找到该公司,选择 如果未找到  继续查找
//
//			//这里继续判断如果当前页面未关闭,重新搜索下一个公司,并赋值写导文本中那个公司中台没有
//
//
//
//
//			Sleep(3000); // 等待两秒
//
//			//打开筛选内容,输入要维护的基础资料
//			long selectNumberFilter = dm.FindPic(0, 0, 2240, 1400, L"选择编码过滤.bmp", L"000000 ", 0.5, 2, &intX, &intY);
//			std::cout << "找到筛选内容按钮，坐标为: (" << intX << ", " << intY << ")" << std::endl;
//			std::vector<int> numberPosition = { intX, intY };
//
//			if (selectNumberFilter > -1)
//			{
//				dm.MoveTo(intX + 35, intY + 5);
//				Sleep(2000); // 等待两秒 
//				dm.LeftClick();
//
//				Sleep(2000); // 等待两秒 
//
//				//先点击重置  然后重新打开过滤 输入内容
//				long retrunBunton = dm.FindPic(0, 0, 2240, 1400, L"重置.bmp", L"000000 ", 0.7, 2, &intX, &intY);
//				dm.MoveTo(intX + 20, intY + 10);
//				Sleep(1000); // 等待两秒 
//				dm.LeftClick();
//
//				Sleep(2000); // 等待两秒 
//
//				dm.MoveTo(numberPosition[0] + 35, numberPosition[1] + 5);
//				Sleep(2000); // 等待两秒 
//				dm.LeftClick();
//
//				Sleep(1000); // 等待两秒 
//				long equalText = dm.FindPic(0, 0, 2240, 1400, L"等于.bmp", L"000000 ", 0.8, 2, &intX, &intY);
//				std::cout << "找到等于，坐标为: (" << intX << ", " << intY << ")" << std::endl;
//
//				if (equalText >= 0)
//				{
//					dm.MoveTo(intX, intY);
//					Sleep(1000); // 等待两秒 
//					dm.LeftClick();
//
//					long FilterContext = dm.FindPic(0, 0, 2240, 1400, L"过滤内容框.bmp", L"000000 ", 0.8, 2, &intX, &intY);
//					std::cout << "找到过滤内容框，坐标为: (" << intX << ", " << intY << ")" << std::endl;
//					dm.MoveTo(intX + 20, intY + 10);
//					Sleep(1000); // 等待两秒 
//					dm.LeftClick();
//
//					//输入公司
//					dm.KeyPressStr(AddDaties[0], 20);
//					wprintf(L"当前过滤编码为: %s\n", AddDaties[0].GetString());
//					//按下回车
//					dm.KeyDown(13);
//				}
//
//			}
//
//
//
//		}
//		else {
//			// 超时未找到图片，可能需要终止程序或进行错误处理
//			// ...
//		}
//
//	}
//	else {
//		// 未找到图片
//		std::cout << "未找到更多按钮。" << std::endl;
//	}
//
//
//
//	//查找输入框 点击
//
//	//输入记账公司编码,点击回车
//
//	//如果不存在公司编码 则跳过执行  	//选择输入编码的公司
//
//	//创建一个窗口
//	//int foobar  = dm.CreateFoobarRect(0, 200, 200, 500, 200);
//
//	/*while (true)
//	{
//		dm.FoobarUpdate(foobar);
//		dm.delay(2000);
//	}*/
//
//	//获取指定窗口
//
//	//执行判断 按键一系列操作
//
//	//int x1, y1, x2, y2; // 定义变量以接收坐标值
//
//	//// 调用函数
//	//CString str =  dm.EnumWindowByProcessId(14400, L"Chrome Legacy Window", L"Chrome_RenderWidgetHostHWND", 1+2);
//	//
// //
//	//std::wcout << L"Window Handles: " << (LPCTSTR)str << std::endl;
//}