// example.cpp : 定义控制台应用程序的入口点。
//
#include <Afxwin.h>
#include "stdafx.h"
#include "obj.h"
#include "msdk.h"

dmsoft* g_dm = NULL;

void test02();

void test03();
void test04();

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
 
	// 接下来可以做一些全局性的设置,比如加载保护盾，设置共享字库等

	int x1 = 10, y1 = 10;
	int x2 = 600, y2 = 600;
	int width = x2 - x1 + 1;
	int height = y2 - y1 + 1;

	// 创建窗口以显示原始图像和轮廓图像
	cv::namedWindow("Original Display", cv::WINDOW_AUTOSIZE);
	cv::namedWindow("Contour Display", cv::WINDOW_AUTOSIZE);

	while (true) {
		long data ;
		long size;
		std::cout << "调用 g_dm->GetScreenDataBmp 前" << std::endl;
		int fanHui = g_dm->GetScreenDataBmp(x1, y1, x2, y2, &data, &size);
		std::cout << "调用 g_dm->GetScreenDataBmp 后" << std::endl;

		// 读取指针到数组
		std::vector<unsigned char> genePic2(size);
		for (int i = 0; i < size; i++) {
			genePic2[i] = *reinterpret_cast<unsigned char*>(data + i);
		}

		std::cout << "调用 cv::imdecode 前" << std::endl;
		cv::Mat image = cv::imdecode(cv::Mat(genePic2), cv::IMREAD_COLOR);
		std::cout << "调用 cv::imdecode 后" << std::endl;
 
		if (image.empty()) {
			std::cout << "无法从内存数据加载图片" << std::endl;
			break; // 退出循环
		}

		// 在原始窗口中显示图像
		cv::imshow("Original Display", image);
		std::vector<std::vector<cv::Point>> contours;
		// 将图像转换为灰度图
		cv::Mat grayImage;
		cv::cvtColor(image, grayImage, cv::COLOR_BGR2GRAY);


		auto start = std::chrono::high_resolution_clock::now();

		// 应用Canny边缘检测
		cv::Mat edges;
		cv::Canny(grayImage, edges, 1000, 200); // 这里的阈值50和150可以根据你的需求调整

		// 查找轮廓
		std::cout << "调用 cv::findContours 前" << std::endl;
		cv::findContours(edges, contours, cv::RETR_LIST, cv::CHAIN_APPROX_SIMPLE);
		std::cout << "调用 cv::findContours 后" << std::endl;


		//CString result = g_dm->FindMultiColorEx(x1, y1, x2, y2, L"D047C7-000000", L"29312B", 0.8, 0);

		//long count = g_dm->GetResultCount(result);

		//// 假设其他代码已经正确执行，count等变量已经被赋值
		//long intX, intY;
		//for (long index = 0; index < count; ++index) {
		//	// 对于每个找到的颜色，使用GetResultPos获取其坐标
		//	g_dm->GetResultPos(result, index, &intX, &intY);

		//	// 将点添加到一个新的轮廓中
		//	std::vector<cv::Point> newContour;
		//	newContour.push_back(cv::Point(intX, intY)); // 将点加入到轮廓中

		//	// 将新轮廓加入到contours中
		//	contours.push_back(newContour);
		//}
		auto end = std::chrono::high_resolution_clock::now();
		std::chrono::duration<double, std::milli> elapsed = end - start;
		std::cout << "test02执行时间1111: " << elapsed.count() << " ms\n";
 

		// 绘制轮廓
		cv::Mat contourImage = cv::Mat::zeros(image.size(), CV_8UC3);
		for (size_t i = 0; i < contours.size(); i++) {
			cv::Scalar color = cv::Scalar(255, 0, 0); // 轮廓颜色：蓝色
			cv::drawContours(contourImage, contours, static_cast<int>(i), color, 1, cv::LINE_8);
		}

		// 在轮廓窗口中显示轮廓图像
		std::cout << "调用 cv::imshow 前" << std::endl;
		cv::imshow("Original Display", image);
		std::cout << "调用 cv::imshow 后" << std::endl;


		std::cout << "调用 cv::waitKey 前" << std::endl;
		if (cv::waitKey(1) == 27) {
			break; // 退出循环
		}
		std::cout << "调用 cv::waitKey 后" << std::endl;


		std::cout << "调用 image.release() 前" << std::endl;
		image.release();
		std::cout << "调用 image.release() 后" << std::endl;

		std::cout << "调用 contours.clear() 前" << std::endl;
		contours.clear();
		std::cout << "调用 contours.clear() 后" << std::endl;
	}
 

	delete g_dm;
 

	return 0;
}





void test02() {

	
}

//E525E2-060505
//fanHui = g_dm->GetScreenDataBmp(x1, y1, x2, y2, &data, &size);
