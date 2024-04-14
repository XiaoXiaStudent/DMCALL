// example.cpp : 定义控制台应用程序的入口点。
//

#include <Afxwin.h>
#include "stdafx.h"
#include "obj.h"
#include "msdk.h"
#include "Application.h"

dmsoft* g_dm = NULL;

int main(int argc, TCHAR* argv[], TCHAR* envp[])
{
#ifdef _DEBUG
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif

	int nRetCode = 0;

	// 初始化COM(mta)
	CoInitializeEx(NULL, 0);

	// 设置本地字符集为gbk
	setlocale(LC_ALL, "chs");

	/* // 注意：这里使用了 const wchar_t* 并且指定了 __stdcall 调用约定
	typedef void(__stdcall* SetDllPathWType)(const wchar_t*, int);

	 使用MFC的AfxLoadLibrary加载DLL
	HMODULE hModule = LoadLibrary(TEXT("DmReg.dll"));
	if (hModule == NULL) {
		std::cerr << "加载DLL失败!" << std::endl;
		return 1;
	}

	 获取函数地址
	SetDllPathWType SetDllPathW = (SetDllPathWType)GetProcAddress(hModule, "SetDllPathW");
	if (!SetDllPathW) {
		std::cerr << "获取函数地址失败!" << std::endl;
		FreeLibrary(hModule);
		return 1;
	}

		//// 调用函数
	//SetDllPathW(_T("dm.dll"), 0);

	//FreeLibrary(hModule);
	*/

	//飞易来调用鼠标移动逻辑
	/*HANDLE openFYL = M_Open(1);

	M_MoveTo2(openFYL, 200, 300);

	M_Close(openFYL)

		HANDLE openFYL = Ap5isNXMcaWr(1);

	n8MDw8SN3t(openFYL, 200, 300);

	Dfd8mbZxDc0f(openFYL);

	;*/

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
		//FreeLibrary(hModule);
		delete g_dm;
		return 1;
	}
	else
	{
		std::cout << "注册成功" << std::endl;
		std::wcout << L"版本:" << version.GetString() << std::endl;
	}

	// 接下来可以做一些全局性的设置,比如加载保护盾，设置共享字库等
 // 屏幕区域坐标


	

	std::mutex mtx; // 全局互斥锁
	// 在循环外部准备可能重用的资源
	cv::Mat image;


	std::thread captureThread([&image,&mtx]() {
			captureToBuffer(image, mtx);
		});

	std::thread detectionThread([&image,&mtx]() {

		//生成随机颜色
		std::vector<cv::Scalar> color;
		srand(time(0));


		std::vector<Output> result;

		std::string model_path = "yolov5s.onnx";

		Yolov5 test;

		cv::dnn::Net net;
		if (test.readModel(net, model_path, true)) {
			std::cout << "read net ok!" << std::endl;
		}
		else {
			std::cout << "read net failed!" << std::endl;
		}

		for (int i = 0; i < 80; i++) {
			int b = rand() % 256;
			int g = rand() % 256;
			int r = rand() % 256;
			color.push_back(cv::Scalar(b, g, r));
		}

		while (true) {
			mtx.lock(); // 获取锁
			if (image.empty()) {
				
				std::cout << "无法从内存数据加载图片" << std::endl;
				mtx.unlock();
				break; // 退出循环
			
			}
			auto start = std::chrono::high_resolution_clock::now();
			std::vector<Output> result;
			if (test.Detect(image, net, result)) {
				test.drawPred(image, result, color);
			}
			else {
				std::cout << "Detect Failed!" << std::endl;
				mtx.unlock(); // 释放锁
			}

			auto end = std::chrono::high_resolution_clock::now();
			std::chrono::duration<double, std::milli> elapsed = end - start;
			std::cout << "识别: " << elapsed.count() << " ms\n";

			if (!result.empty()) {
				mtx.unlock(); // 释放锁
			}
			
		}
		});



	// 等待线程结束
	captureThread.join();
	detectionThread.join();
	CoUninitialize();
	delete g_dm;
	return 0;
}

void captureToBuffer(cv::Mat& image, std::mutex&  mtx)
{
	int x1 = 250, y1 = 250;
	int x2 = 750, y2 = 750;
	int width = x2 - x1 + 1;
	int height = y2 - y1 + 1;

	long mouseX, mouseY;

	// 创建窗口以显示原始图像和轮廓图像
	cv::namedWindow("Original Display", cv::WINDOW_AUTOSIZE);

	while (true)

	{
		auto start = std::chrono::high_resolution_clock::now();

		//变量用于保存数据指针和大小
		unsigned char* data = nullptr;
		long size = 0;

		// 获取屏幕数据
		int fanHui = g_dm->GetScreenDataBmp(x1, y1, x2, y2, reinterpret_cast<long*>(&data), &size);
	 
		// 使用获得的数据创建cv::Mat对象
		cv::Mat rawData(1, size, CV_8UC1, data); // data 是指向位图数据的指针

		image = cv::imdecode(rawData, cv::IMREAD_COLOR); // 解码位图数据
		
		 
		if (image.empty()) {
			std::cout << "无法从内存数据加载图片" << std::endl;
			break; // 退出循环
		}
		// 在原始窗口中显示图像
		cv::imshow("Original Display", image);


		auto end = std::chrono::high_resolution_clock::now();
		std::chrono::duration<double, std::milli> elapsed = end - start;
		//std::cout << "捕获: " << elapsed.count() << " ms\n";


		if (cv::waitKey(1) == 27) { // 当按下ESC键时退出循环
			break;
		}
	
	}
}

/*
void findAndDrawContours() {
	dmsoft* dm= new dmsoft;

	// 创建窗口以显示原始图像和轮廓图像

	cv::namedWindow("Contour Display", cv::WINDOW_AUTOSIZE);

	int x1 = 400 ,y1 = 0, x2 = 1200, y2 = 800;

	int width = x2 - x1 + 1;
	int height = y2 - y1 + 1;

	long dm_ret = dm->BindWindowEx(197080, L"normal", L"normal", L"normal", L"", 0);

	// Check if the binding was successful
	if (dm_ret != 1) {
		std::cerr << "Failed to bind the window!" << std::endl;
	}
	else
	{
		std::cout << "绑定成功" << std::endl;
	}
	long width1, height1;

	dm->GetClientSize(197080, &width1, &height1);

	std::cout << "width1:" << width1 << "height1:" << height1 << std::endl;

	while (true)
	{
		auto start = std::chrono::high_resolution_clock::now();

		//查找人物颜色坐标
		std::vector<std::vector<cv::Point>> contours;
		CString result = dm->FindMultiColorEx(x1, y1, x2, y2, L"D047C7-000000", L"29312B", 0.8, 0);
		//假设 FindMultiColorE 已经调用，返回了坐标字符串

		long count = dm->GetResultCount(result);

		// 假设其他代码已经正确执行，count等变量已经被赋值
		long intX, intY;
		for (long index = 0; index < count; ++index) {
			// 对于每个找到的颜色，使用GetResultPos获取其坐标
			dm->GetResultPos(result, index, &intX, &intY);

			// 将点添加到一个新的轮廓中
			std::vector<cv::Point> newContour;
			newContour.push_back(cv::Point(intX, intY)); // 将点加入到轮廓中

			// 将新轮廓加入到contours中
			contours.push_back(newContour);
		}

		cv::Mat contourImage(height, width, CV_8UC3, cv::Scalar(0, 0, 0));
		// 画轮廓，其中contours是包含轮廓点的数组
		for (size_t i = 0; i < contours.size(); i++) {
			cv::Scalar color = cv::Scalar(255, 255, 255); //
			cv::drawContours(contourImage, contours, static_cast<int>(i), color);
		}

		// 显示轮廓图像
		cv::imshow("Contour Display", contourImage);

	/*	auto end = std::chrono::high_resolution_clock::now();
		std::chrono::duration<double, std::milli> elapsed = end - start;
		std::cout << "test02执行时间1111: " << elapsed.count() << " ms\n";

if (cv::waitKey(1) == 27) { // 当按下ESC键时退出循环
	break;
}
	}
}

*/

/*

		CString result = g_dm->FindMultiColorEx(x1, y1, x2, y2, L"D047C7-000000", L"29312B", 0.8, 0);

		long count = g_dm->GetResultCount(result);

		// 假设其他代码已经正确执行，count等变量已经被赋值
		long intX, intY;
		for (long index = 0; index < count; ++index) {
			// 对于每个找到的颜色，使用GetResultPos获取其坐标
			g_dm->GetResultPos(result, index, &intX, &intY);

			// 将点添加到一个新的轮廓中
			std::vector<cv::Point> newContour;
			newContour.push_back(cv::Point(intX, intY)); // 将点加入到轮廓中

			// 将新轮廓加入到contours中
			contours.push_back(newContour);
		}
*/

//E525E2-060505
//fanHui = g_dm->GetScreenDataBmp(x1, y1, x2, y2, &data, &size);

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

//// 转换BGR图像到HSV颜色空间
		//cv::cvtColor(image, hsvImage, cv::COLOR_BGR2HSV);

		//// 使用HSV颜色范围在原始图像上创建掩码
		//cv::inRange(hsvImage, hsvLowerBound, hsvUpperBound, mask);

		//// 对掩码进行高斯模糊，减少噪声
		//cv::GaussianBlur(mask, mask, cv::Size(5, 5), 0);

		//cv::Canny(mask, edges, 100, 200);

		//// 创建一个5x5的结构元素
		//cv::Mat kernel = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(2, 2));

		//// 对边缘图像进行闭运算
		//cv::morphologyEx(edges, edges, cv::MORPH_CLOSE, kernel);

		//// 显示二值化轮廓图像
		//cv::imshow("Binary Contour Display", edges);

		//// 在边缘图像上查找轮廓
		//std::vector<std::vector<cv::Point>> contours;
		//cv::findContours(edges, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

		//// 找到面积最大的轮廓的凸包
		//double maxArea = 0;
		//std::vector<cv::Point> largestHull;
		//for (size_t i = 0; i < contours.size(); i++) {
		//	// 对轮廓进行简化
		//	std::vector<cv::Point> approx;
		//	cv::approxPolyDP(contours[i], approx, 1.0, true);

		//	// 计算凸包
		//	std::vector<cv::Point> hull;
		//	cv::convexHull(approx, hull);

		//	// 检查是否为最大面积的凸包
		//	double area = cv::contourArea(hull);
		//	if (area > maxArea) {
		//		maxArea = area;
		//		largestHull = hull;
		//	}
		//}

		//// 绘制最大凸包轮廓
		//cv::Mat contourImage = cv::Mat::zeros(image.size(), CV_8UC3);
		//if (!largestHull.empty()) {
		//	std::vector<std::vector<cv::Point>> hullsToDraw;
		//	hullsToDraw.push_back(largestHull);
		//	cv::drawContours(contourImage, hullsToDraw, -1, cv::Scalar(255, 255, 255), 1);
		//}