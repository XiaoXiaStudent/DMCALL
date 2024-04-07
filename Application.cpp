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

	// ע�⣺����ʹ���� const wchar_t* ����ָ���� __stdcall ����Լ��
	typedef void(__stdcall* SetDllPathWType)(const wchar_t*, int);

	// ʹ��MFC��AfxLoadLibrary����DLL
	HMODULE hModule = LoadLibrary(TEXT("DmReg.dll"));
	if (hModule == NULL) {
		std::cerr << "����DLLʧ��!" << std::endl;
		return 1;
	}

	//��������������ƶ��߼�
	/*HANDLE openFYL = M_Open(1);

	M_MoveTo2(openFYL, 200, 300);

	M_Close(openFYL);*/

	/*HANDLE openFYL = Ap5isNXMcaWr(1);

	n8MDw8SN3t(openFYL, 200, 300);

	Dfd8mbZxDc0f(openFYL);*/

	// ��ȡ������ַ
	SetDllPathWType SetDllPathW = (SetDllPathWType)GetProcAddress(hModule, "SetDllPathW");
	if (!SetDllPathW) {
		std::cerr << "��ȡ������ַʧ��!" << std::endl;
		FreeLibrary(hModule);
		return 1;
	}

	// ���ú���
	SetDllPathW(_T("dm.dll"), 0);

	FreeLibrary(hModule);

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
		FreeLibrary(hModule);
		delete g_dm;
		return 1;
	}
	else
	{
		std::cout << "ע��ɹ�" << std::endl;
		std::wcout << L"�汾:" << version.GetString() << std::endl;
	}

	// ������������һЩȫ���Ե�����,������ر����ܣ����ù����ֿ��
	captureToBuffer();

	CoUninitialize();
	delete g_dm;
	return 0;
}

void captureToBuffer()
{
	int x1 = 0, y1 = 0;
	int x2 = 600, y2 = 600;
	int width = x2 - x1 + 1;
	int height = y2 - y1 + 1;
	long mouseX, mouseY;

	// ������������ʾԭʼͼ�������ͼ��
	cv::namedWindow("Original Display", cv::WINDOW_AUTOSIZE);
	cv::namedWindow("Contour Display", cv::WINDOW_AUTOSIZE);
	cv::namedWindow("Binary Contour Display", cv::WINDOW_AUTOSIZE);

 

	// ��ѭ���ⲿ׼���������õ���Դ
	cv::Mat hsvImage, mask, edges;
	std::vector<unsigned char> genePic2;
	cv::Mat image;

	int B = 199, G = 71, R = 208;
	int deltaB = 35, deltaG = 35, deltaR = 35;

	// ����BGR�ռ��е���ɫ��Χ
	cv::Scalar bgrLowerBound(B - deltaB, G - deltaG, R - deltaR);
	cv::Scalar bgrUpperBound(B + deltaB, G + deltaG, R + deltaR);

		// ����RGB��ɫ�ռ��е���ɫ��Χ�����ɫ��
  // ���ɫ��HSV��ɫ�ռ䷶Χ
	cv::Scalar hsvLowerBound(139, 96, 129);
	cv::Scalar hsvUpperBound(169, 225, 225);

	while (true)

	{
		auto start = std::chrono::high_resolution_clock::now();

		long data;
		long size;
		int fanHui = g_dm->GetScreenDataBmp(x1, y1, x2, y2, &data, &size);

		// ��ȡָ�뵽����
		genePic2.clear();
		genePic2.resize(size);
		for (int i = 0; i < size; i++) {
			genePic2[i] = *reinterpret_cast<unsigned char*>(data + i);
		}

		image = cv::imdecode(cv::Mat(genePic2), cv::IMREAD_COLOR);

		if (image.empty()) {
			std::cout << "�޷����ڴ����ݼ���ͼƬ" << std::endl;
			//break; // �˳�ѭ��
		}

		// ��ԭʼ��������ʾͼ��
		cv::imshow("Original Display", image);

		//��ȡ��ǰ���λ��
		g_dm->GetCursorPos(&mouseX, &mouseY);

		// ת��BGRͼ��HSV��ɫ�ռ�
		cv::cvtColor(image, hsvImage, cv::COLOR_BGR2HSV);

		// ʹ��HSV��ɫ��Χ��ԭʼͼ���ϴ�������
		cv::inRange(hsvImage, hsvLowerBound, hsvUpperBound, mask);


		// ��������и�˹ģ������������
		cv::GaussianBlur(mask, mask, cv::Size(5, 5), 0);
	
		cv::Canny(mask, edges, 100, 200);

	
		// ����һ��5x5�ĽṹԪ��
		cv::Mat kernel = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(2, 2));

		// �Ա�Եͼ����б�����
		cv::morphologyEx(edges, edges, cv::MORPH_CLOSE, kernel);

		// ��ʾ��ֵ������ͼ��
		cv::imshow("Binary Contour Display", edges);

		// �ڱ�Եͼ���ϲ�������
		std::vector<std::vector<cv::Point>> contours;
		cv::findContours(edges, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);


		// �ҵ��������������͹��
		double maxArea = 0;
		std::vector<cv::Point> largestHull;
		for (size_t i = 0; i < contours.size(); i++) {
			// ���������м�
			std::vector<cv::Point> approx;
			cv::approxPolyDP(contours[i], approx, 1.0, true);

			// ����͹��
			std::vector<cv::Point> hull;
			cv::convexHull(approx, hull);

			// ����Ƿ�Ϊ��������͹��
			double area = cv::contourArea(hull);
			if (area > maxArea) {
				maxArea = area;
				largestHull = hull;
			}
		}

		// �������͹������
		cv::Mat contourImage = cv::Mat::zeros(image.size(), CV_8UC3);
		if (!largestHull.empty()) {
			std::vector<std::vector<cv::Point>> hullsToDraw;
			hullsToDraw.push_back(largestHull);
			cv::drawContours(contourImage, hullsToDraw, -1, cv::Scalar(255, 255, 255), 1);
		}
	 
 
		// ��ʾ����ͼ��
		cv::imshow("Contour Display", contourImage);
 

		/*auto end = std::chrono::high_resolution_clock::now();
		std::chrono::duration<double, std::milli> elapsed = end - start;
		std::cout << "test02ִ��ʱ��1111: " << elapsed.count() << " ms\n";*/

		if (cv::waitKey(1) == 27) { // ������ESC��ʱ�˳�ѭ��
			break;
		}
	}
}

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