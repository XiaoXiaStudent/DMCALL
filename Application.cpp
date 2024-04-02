// example.cpp : �������̨Ӧ�ó������ڵ㡣
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

	int x1 = 10, y1 = 10;
	int x2 = 600, y2 = 600;
	int width = x2 - x1 + 1;
	int height = y2 - y1 + 1;

	// ������������ʾԭʼͼ�������ͼ��
	cv::namedWindow("Original Display", cv::WINDOW_AUTOSIZE);
	cv::namedWindow("Contour Display", cv::WINDOW_AUTOSIZE);

	while (true) {
		long data ;
		long size;
		std::cout << "���� g_dm->GetScreenDataBmp ǰ" << std::endl;
		int fanHui = g_dm->GetScreenDataBmp(x1, y1, x2, y2, &data, &size);
		std::cout << "���� g_dm->GetScreenDataBmp ��" << std::endl;

		// ��ȡָ�뵽����
		std::vector<unsigned char> genePic2(size);
		for (int i = 0; i < size; i++) {
			genePic2[i] = *reinterpret_cast<unsigned char*>(data + i);
		}

		std::cout << "���� cv::imdecode ǰ" << std::endl;
		cv::Mat image = cv::imdecode(cv::Mat(genePic2), cv::IMREAD_COLOR);
		std::cout << "���� cv::imdecode ��" << std::endl;
 
		if (image.empty()) {
			std::cout << "�޷����ڴ����ݼ���ͼƬ" << std::endl;
			break; // �˳�ѭ��
		}

		// ��ԭʼ��������ʾͼ��
		cv::imshow("Original Display", image);
		std::vector<std::vector<cv::Point>> contours;
		// ��ͼ��ת��Ϊ�Ҷ�ͼ
		cv::Mat grayImage;
		cv::cvtColor(image, grayImage, cv::COLOR_BGR2GRAY);


		auto start = std::chrono::high_resolution_clock::now();

		// Ӧ��Canny��Ե���
		cv::Mat edges;
		cv::Canny(grayImage, edges, 1000, 200); // �������ֵ50��150���Ը�������������

		// ��������
		std::cout << "���� cv::findContours ǰ" << std::endl;
		cv::findContours(edges, contours, cv::RETR_LIST, cv::CHAIN_APPROX_SIMPLE);
		std::cout << "���� cv::findContours ��" << std::endl;


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
		auto end = std::chrono::high_resolution_clock::now();
		std::chrono::duration<double, std::milli> elapsed = end - start;
		std::cout << "test02ִ��ʱ��1111: " << elapsed.count() << " ms\n";
 

		// ��������
		cv::Mat contourImage = cv::Mat::zeros(image.size(), CV_8UC3);
		for (size_t i = 0; i < contours.size(); i++) {
			cv::Scalar color = cv::Scalar(255, 0, 0); // ������ɫ����ɫ
			cv::drawContours(contourImage, contours, static_cast<int>(i), color, 1, cv::LINE_8);
		}

		// ��������������ʾ����ͼ��
		std::cout << "���� cv::imshow ǰ" << std::endl;
		cv::imshow("Original Display", image);
		std::cout << "���� cv::imshow ��" << std::endl;


		std::cout << "���� cv::waitKey ǰ" << std::endl;
		if (cv::waitKey(1) == 27) {
			break; // �˳�ѭ��
		}
		std::cout << "���� cv::waitKey ��" << std::endl;


		std::cout << "���� image.release() ǰ" << std::endl;
		image.release();
		std::cout << "���� image.release() ��" << std::endl;

		std::cout << "���� contours.clear() ǰ" << std::endl;
		contours.clear();
		std::cout << "���� contours.clear() ��" << std::endl;
	}
 

	delete g_dm;
 

	return 0;
}





void test02() {

	
}

//E525E2-060505
//fanHui = g_dm->GetScreenDataBmp(x1, y1, x2, y2, &data, &size);
