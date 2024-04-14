// example.cpp : �������̨Ӧ�ó������ڵ㡣
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

	// ��ʼ��COM(mta)
	CoInitializeEx(NULL, 0);

	// ���ñ����ַ���Ϊgbk
	setlocale(LC_ALL, "chs");

	/* // ע�⣺����ʹ���� const wchar_t* ����ָ���� __stdcall ����Լ��
	typedef void(__stdcall* SetDllPathWType)(const wchar_t*, int);

	 ʹ��MFC��AfxLoadLibrary����DLL
	HMODULE hModule = LoadLibrary(TEXT("DmReg.dll"));
	if (hModule == NULL) {
		std::cerr << "����DLLʧ��!" << std::endl;
		return 1;
	}

	 ��ȡ������ַ
	SetDllPathWType SetDllPathW = (SetDllPathWType)GetProcAddress(hModule, "SetDllPathW");
	if (!SetDllPathW) {
		std::cerr << "��ȡ������ַʧ��!" << std::endl;
		FreeLibrary(hModule);
		return 1;
	}

		//// ���ú���
	//SetDllPathW(_T("dm.dll"), 0);

	//FreeLibrary(hModule);
	*/

	//��������������ƶ��߼�
	/*HANDLE openFYL = M_Open(1);

	M_MoveTo2(openFYL, 200, 300);

	M_Close(openFYL)

		HANDLE openFYL = Ap5isNXMcaWr(1);

	n8MDw8SN3t(openFYL, 200, 300);

	Dfd8mbZxDc0f(openFYL);

	;*/

	// ��������
	g_dm = new dmsoft();
	if (g_dm == NULL)
	{
		return 1;
	}

	// ע��
	long dm_ret = g_dm->Reg(_T("mh84909b3bf80d45c618136887775ccc90d27d7"), _T("me03d3g0d976m27"));

	CString version = g_dm->Ver();
	if (dm_ret != 1)
	{
		std::cout << "ע��ʧ��:" << dm_ret << std::endl;
		std::wcout << L"�汾:" << version.GetString() << std::endl;
		//FreeLibrary(hModule);
		delete g_dm;
		return 1;
	}
	else
	{
		std::cout << "ע��ɹ�" << std::endl;
		std::wcout << L"�汾:" << version.GetString() << std::endl;
	}

	// ������������һЩȫ���Ե�����,������ر����ܣ����ù����ֿ��
 // ��Ļ��������


	

	std::mutex mtx; // ȫ�ֻ�����
	// ��ѭ���ⲿ׼���������õ���Դ
	cv::Mat image;


	std::thread captureThread([&image,&mtx]() {
			captureToBuffer(image, mtx);
		});

	std::thread detectionThread([&image,&mtx]() {

		//���������ɫ
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
			mtx.lock(); // ��ȡ��
			if (image.empty()) {
				
				std::cout << "�޷����ڴ����ݼ���ͼƬ" << std::endl;
				mtx.unlock();
				break; // �˳�ѭ��
			
			}
			auto start = std::chrono::high_resolution_clock::now();
			std::vector<Output> result;
			if (test.Detect(image, net, result)) {
				test.drawPred(image, result, color);
			}
			else {
				std::cout << "Detect Failed!" << std::endl;
				mtx.unlock(); // �ͷ���
			}

			auto end = std::chrono::high_resolution_clock::now();
			std::chrono::duration<double, std::milli> elapsed = end - start;
			std::cout << "ʶ��: " << elapsed.count() << " ms\n";

			if (!result.empty()) {
				mtx.unlock(); // �ͷ���
			}
			
		}
		});



	// �ȴ��߳̽���
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

	// ������������ʾԭʼͼ�������ͼ��
	cv::namedWindow("Original Display", cv::WINDOW_AUTOSIZE);

	while (true)

	{
		auto start = std::chrono::high_resolution_clock::now();

		//�������ڱ�������ָ��ʹ�С
		unsigned char* data = nullptr;
		long size = 0;

		// ��ȡ��Ļ����
		int fanHui = g_dm->GetScreenDataBmp(x1, y1, x2, y2, reinterpret_cast<long*>(&data), &size);
	 
		// ʹ�û�õ����ݴ���cv::Mat����
		cv::Mat rawData(1, size, CV_8UC1, data); // data ��ָ��λͼ���ݵ�ָ��

		image = cv::imdecode(rawData, cv::IMREAD_COLOR); // ����λͼ����
		
		 
		if (image.empty()) {
			std::cout << "�޷����ڴ����ݼ���ͼƬ" << std::endl;
			break; // �˳�ѭ��
		}
		// ��ԭʼ��������ʾͼ��
		cv::imshow("Original Display", image);


		auto end = std::chrono::high_resolution_clock::now();
		std::chrono::duration<double, std::milli> elapsed = end - start;
		//std::cout << "����: " << elapsed.count() << " ms\n";


		if (cv::waitKey(1) == 27) { // ������ESC��ʱ�˳�ѭ��
			break;
		}
	
	}
}

/*
void findAndDrawContours() {
	dmsoft* dm= new dmsoft;

	// ������������ʾԭʼͼ�������ͼ��

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
		std::cout << "�󶨳ɹ�" << std::endl;
	}
	long width1, height1;

	dm->GetClientSize(197080, &width1, &height1);

	std::cout << "width1:" << width1 << "height1:" << height1 << std::endl;

	while (true)
	{
		auto start = std::chrono::high_resolution_clock::now();

		//����������ɫ����
		std::vector<std::vector<cv::Point>> contours;
		CString result = dm->FindMultiColorEx(x1, y1, x2, y2, L"D047C7-000000", L"29312B", 0.8, 0);
		//���� FindMultiColorE �Ѿ����ã������������ַ���

		long count = dm->GetResultCount(result);

		// �������������Ѿ���ȷִ�У�count�ȱ����Ѿ�����ֵ
		long intX, intY;
		for (long index = 0; index < count; ++index) {
			// ����ÿ���ҵ�����ɫ��ʹ��GetResultPos��ȡ������
			dm->GetResultPos(result, index, &intX, &intY);

			// ������ӵ�һ���µ�������
			std::vector<cv::Point> newContour;
			newContour.push_back(cv::Point(intX, intY)); // ������뵽������

			// �����������뵽contours��
			contours.push_back(newContour);
		}

		cv::Mat contourImage(height, width, CV_8UC3, cv::Scalar(0, 0, 0));
		// ������������contours�ǰ��������������
		for (size_t i = 0; i < contours.size(); i++) {
			cv::Scalar color = cv::Scalar(255, 255, 255); //
			cv::drawContours(contourImage, contours, static_cast<int>(i), color);
		}

		// ��ʾ����ͼ��
		cv::imshow("Contour Display", contourImage);

	/*	auto end = std::chrono::high_resolution_clock::now();
		std::chrono::duration<double, std::milli> elapsed = end - start;
		std::cout << "test02ִ��ʱ��1111: " << elapsed.count() << " ms\n";

if (cv::waitKey(1) == 27) { // ������ESC��ʱ�˳�ѭ��
	break;
}
	}
}

*/

/*

		CString result = g_dm->FindMultiColorEx(x1, y1, x2, y2, L"D047C7-000000", L"29312B", 0.8, 0);

		long count = g_dm->GetResultCount(result);

		// �������������Ѿ���ȷִ�У�count�ȱ����Ѿ�����ֵ
		long intX, intY;
		for (long index = 0; index < count; ++index) {
			// ����ÿ���ҵ�����ɫ��ʹ��GetResultPos��ȡ������
			g_dm->GetResultPos(result, index, &intX, &intY);

			// ������ӵ�һ���µ�������
			std::vector<cv::Point> newContour;
			newContour.push_back(cv::Point(intX, intY)); // ������뵽������

			// �����������뵽contours��
			contours.push_back(newContour);
		}
*/

//E525E2-060505
//fanHui = g_dm->GetScreenDataBmp(x1, y1, x2, y2, &data, &size);

		//CString result = g_dm->FindMultiColorEx(x1, y1, x2, y2, L"D047C7-000000", L"29312B", 0.8, 0);

		//long count = g_dm->GetResultCount(result);

		//// �������������Ѿ���ȷִ�У�count�ȱ����Ѿ�����ֵ
		//long intX, intY;
		//for (long index = 0; index < count; ++index) {
		//	// ����ÿ���ҵ�����ɫ��ʹ��GetResultPos��ȡ������
		//	g_dm->GetResultPos(result, index, &intX, &intY);

		//	// ������ӵ�һ���µ�������
		//	std::vector<cv::Point> newContour;
		//	newContour.push_back(cv::Point(intX, intY)); // ������뵽������

		//	// �����������뵽contours��
		//	contours.push_back(newContour);
		//}

//// ת��BGRͼ��HSV��ɫ�ռ�
		//cv::cvtColor(image, hsvImage, cv::COLOR_BGR2HSV);

		//// ʹ��HSV��ɫ��Χ��ԭʼͼ���ϴ�������
		//cv::inRange(hsvImage, hsvLowerBound, hsvUpperBound, mask);

		//// ��������и�˹ģ������������
		//cv::GaussianBlur(mask, mask, cv::Size(5, 5), 0);

		//cv::Canny(mask, edges, 100, 200);

		//// ����һ��5x5�ĽṹԪ��
		//cv::Mat kernel = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(2, 2));

		//// �Ա�Եͼ����б�����
		//cv::morphologyEx(edges, edges, cv::MORPH_CLOSE, kernel);

		//// ��ʾ��ֵ������ͼ��
		//cv::imshow("Binary Contour Display", edges);

		//// �ڱ�Եͼ���ϲ�������
		//std::vector<std::vector<cv::Point>> contours;
		//cv::findContours(edges, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

		//// �ҵ��������������͹��
		//double maxArea = 0;
		//std::vector<cv::Point> largestHull;
		//for (size_t i = 0; i < contours.size(); i++) {
		//	// ���������м�
		//	std::vector<cv::Point> approx;
		//	cv::approxPolyDP(contours[i], approx, 1.0, true);

		//	// ����͹��
		//	std::vector<cv::Point> hull;
		//	cv::convexHull(approx, hull);

		//	// ����Ƿ�Ϊ��������͹��
		//	double area = cv::contourArea(hull);
		//	if (area > maxArea) {
		//		maxArea = area;
		//		largestHull = hull;
		//	}
		//}

		//// �������͹������
		//cv::Mat contourImage = cv::Mat::zeros(image.size(), CV_8UC3);
		//if (!largestHull.empty()) {
		//	std::vector<std::vector<cv::Point>> hullsToDraw;
		//	hullsToDraw.push_back(largestHull);
		//	cv::drawContours(contourImage, hullsToDraw, -1, cv::Scalar(255, 255, 255), 1);
		//}