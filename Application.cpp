// example.cpp : �������̨Ӧ�ó������ڵ㡣
//
#include <Afxwin.h>
#include "stdafx.h"
#include "obj.h"
#include "msdk.h"

 
 
 
dmsoft* g_dm = NULL;

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

	// ������������һЩȫ���Ե�����,������ر����ܣ����ù����ֿ�ȵ�
	dmsoft& dm = *g_dm;

	long  intX, intY;

	CString result = g_dm->FindColorEx(0, 0, 2000, 2000, L"49A996-0B0D06", 0.8, 0);

	// ��鷵��ֵ��ȷ���Ƿ��ҵ�����ɫ��

	// ����Ҫ��ʾ����Ϣ�ַ���
	CString message;

	// ʹ��GetResultCount��ȡ�ҵ�����ɫ����
	long count = g_dm->GetResultCount(result);

	for (long index = 0; index < count; ++index) {
		long intX, intY;
		// ����ÿ���ҵ�����ɫ��ʹ��GetResultPos��ȡ������
		g_dm->GetResultPos(result, index, &intX, &intY);

		g_dm->MoveTo(intX, intY);


	}
	std::wstringstream ss;
	for (long index = 0; index < count; ++index) {
		ss << intX << L"," << intY << L" "; // ��������ӵ�stringstream
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
//	//ҵ������������ά��
//		//���Ҹ��ఴť ���
//	long intX = 0, intY = 0; // ��ʼ���������
//	long findMoreButton = dm.FindPic(0, 0, 2240, 1400, L"����.bmp", L"000000", 0.8, 2, &intX, &intY);
//
//	if (findMoreButton >= 0) {
//		// �ҵ�ͼƬ����������
//		std::cout << "�ҵ����ఴť������Ϊ: (" << intX << ", " << intY << ")" << std::endl;
//
//		//������ఴť
//		dm.MoveTo(intX, intY);
//
//		dm.LeftClick();
//
//		//�ȴ�ҳ��������
//
//	// ��ʱ��ʼ
//		clock_t start_time = clock();
//
//		// ��ʱʱ���趨Ϊ20��
//		const clock_t timeout = 40 * CLOCKS_PER_SEC; // CLOCKS_PER_SEC ��ÿ���ӵ�clock ticks
//
//		bool found = false; // ����Ƿ��ҵ�ͼƬ
//
//
//
//		while (true) {
//			// ������ͼ
//			long HisensePhoto = dm.FindPic(0, 0, 2240, 1400, L"���ż���.bmp", L"000000", 0.9, 2, &intX, &intY);
//			std::cout << "���س����ż��ţ�����Ϊ: (" << intX << ", " << intY << ")" << std::endl;
//			if (intX != -1 && intY != -1) {
//				found = true; // ͼƬ���ҵ�
//				break; // ����ѭ��
//			}
//
//			// ����Ƿ�ʱ
//			if (clock() - start_time > timeout) {
//				break; // �������20�뻹û�ҵ�ͼƬ������ֹѭ��
//			}
//
//			Sleep(1000); // ÿ�γ���֮��ȴ�1�루1000���룩������CPUʹ����
//		}
//
//		if (found) {
//			// �ҵ�ͼƬ������ִ�к�������
//
//			long FindComany = dm.FindPic(0, 0, 2240, 1400, L"������˾.bmp", L"000000 ", 0.9, 2, &intX, &intY);
//			std::cout << "�ҵ���������������Ϊ: (" << intX << ", " << intY << ")" << std::endl;
//			dm.MoveTo(intX + 50, intY);
//
//			dm.LeftClick();
//
//
//
//			//���빫˾
//			dm.KeyPressStr(companies[0], 20);
//			wprintf(L"��ǰ��˾Ϊ: %s\n", companies[0].GetString());
//			//���»س�
//			dm.KeyDown(13);
//
//			Sleep(2000); // �ȴ����� ���س���˾
//
//			//ѡ��˾����д��
//			dm.MoveTo(764, 420);
//
//			dm.LeftClick();
//			//����ҵ��ù�˾,ѡ�� ���δ�ҵ�  ��������
//
//			//��������ж������ǰҳ��δ�ر�,����������һ����˾,����ֵд���ı����Ǹ���˾��̨û��
//
//
//
//
//			Sleep(3000); // �ȴ�����
//
//			//��ɸѡ����,����Ҫά���Ļ�������
//			long selectNumberFilter = dm.FindPic(0, 0, 2240, 1400, L"ѡ��������.bmp", L"000000 ", 0.5, 2, &intX, &intY);
//			std::cout << "�ҵ�ɸѡ���ݰ�ť������Ϊ: (" << intX << ", " << intY << ")" << std::endl;
//			std::vector<int> numberPosition = { intX, intY };
//
//			if (selectNumberFilter > -1)
//			{
//				dm.MoveTo(intX + 35, intY + 5);
//				Sleep(2000); // �ȴ����� 
//				dm.LeftClick();
//
//				Sleep(2000); // �ȴ����� 
//
//				//�ȵ������  Ȼ�����´򿪹��� ��������
//				long retrunBunton = dm.FindPic(0, 0, 2240, 1400, L"����.bmp", L"000000 ", 0.7, 2, &intX, &intY);
//				dm.MoveTo(intX + 20, intY + 10);
//				Sleep(1000); // �ȴ����� 
//				dm.LeftClick();
//
//				Sleep(2000); // �ȴ����� 
//
//				dm.MoveTo(numberPosition[0] + 35, numberPosition[1] + 5);
//				Sleep(2000); // �ȴ����� 
//				dm.LeftClick();
//
//				Sleep(1000); // �ȴ����� 
//				long equalText = dm.FindPic(0, 0, 2240, 1400, L"����.bmp", L"000000 ", 0.8, 2, &intX, &intY);
//				std::cout << "�ҵ����ڣ�����Ϊ: (" << intX << ", " << intY << ")" << std::endl;
//
//				if (equalText >= 0)
//				{
//					dm.MoveTo(intX, intY);
//					Sleep(1000); // �ȴ����� 
//					dm.LeftClick();
//
//					long FilterContext = dm.FindPic(0, 0, 2240, 1400, L"�������ݿ�.bmp", L"000000 ", 0.8, 2, &intX, &intY);
//					std::cout << "�ҵ��������ݿ�����Ϊ: (" << intX << ", " << intY << ")" << std::endl;
//					dm.MoveTo(intX + 20, intY + 10);
//					Sleep(1000); // �ȴ����� 
//					dm.LeftClick();
//
//					//���빫˾
//					dm.KeyPressStr(AddDaties[0], 20);
//					wprintf(L"��ǰ���˱���Ϊ: %s\n", AddDaties[0].GetString());
//					//���»س�
//					dm.KeyDown(13);
//				}
//
//			}
//
//
//
//		}
//		else {
//			// ��ʱδ�ҵ�ͼƬ��������Ҫ��ֹ�������д�����
//			// ...
//		}
//
//	}
//	else {
//		// δ�ҵ�ͼƬ
//		std::cout << "δ�ҵ����ఴť��" << std::endl;
//	}
//
//
//
//	//��������� ���
//
//	//������˹�˾����,����س�
//
//	//��������ڹ�˾���� ������ִ��  	//ѡ���������Ĺ�˾
//
//	//����һ������
//	//int foobar  = dm.CreateFoobarRect(0, 200, 200, 500, 200);
//
//	/*while (true)
//	{
//		dm.FoobarUpdate(foobar);
//		dm.delay(2000);
//	}*/
//
//	//��ȡָ������
//
//	//ִ���ж� ����һϵ�в���
//
//	//int x1, y1, x2, y2; // ��������Խ�������ֵ
//
//	//// ���ú���
//	//CString str =  dm.EnumWindowByProcessId(14400, L"Chrome Legacy Window", L"Chrome_RenderWidgetHostHWND", 1+2);
//	//
// //
//	//std::wcout << L"Window Handles: " << (LPCTSTR)str << std::endl;
//}