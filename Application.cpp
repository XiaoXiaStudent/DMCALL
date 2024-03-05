// example.cpp : �������̨Ӧ�ó������ڵ㡣
//
#include <Afxwin.h>
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

	CString companies[] = { _T("1600C") };

	//ҵ������������ά��
		//���Ҹ��ఴť ���
	long intX = 0, intY = 0; // ��ʼ���������
	long findMoreButton = dm.FindPic(0, 0, 2240, 1400, L"����.bmp", L"00aaa6-000000", 0.8, 2, &intX, &intY);

	if (findMoreButton >= 0) {
		// �ҵ�ͼƬ����������
		std::cout << "�ҵ����ఴť������Ϊ: (" << intX << ", " << intY << ")" << std::endl;
	}
	else {
		// δ�ҵ�ͼƬ
		std::cout << "δ�ҵ����ఴť��" << std::endl;
	}

	/*dm.MoveTo(intX, intY);

	dm.LeftClick();*/

	//�ȴ�ҳ��������

	// ��ʱ��ʼ
	clock_t start_time = clock();

	// ��ʱʱ���趨Ϊ20��
	const clock_t timeout = 20 * CLOCKS_PER_SEC; // CLOCKS_PER_SEC ��ÿ���ӵ�clock ticks

	bool found = false; // ����Ƿ��ҵ�ͼƬ

	long HisensePhoto = dm.FindPic(0, 0, 2240, 1400, L"���ż���.bmp", L"8cded9-000000|35beba-000000|00aaa6-000000|8cded9-000000|2abab5-000000|a7e8e3-000000", 0.9, 2, &intX, &intY);
	std::cout << "���س����ż��ţ�����Ϊ: (" << intX << ", " << intY << ")" << std::endl;

		long FindComany = dm.FindPic(0, 0, 2240, 1400, L"������˾.bmp", L"ffffff-000000", 0.9, 2, &intX, &intY);
	std::cout << "�ҵ���������������Ϊ: (" << intX << ", " << intY << ")" << std::endl;

	//while (true) {
	//	// ������ͼ
	//	long HisensePhoto = dm.FindPic(0, 0, 2240, 1400, L"���ż���.bmp", L"e6fffa-000000", 0.9, 2, &intX, &intY);
	//	std::cout << "���س����ż��ţ�����Ϊ: (" << intX << ", " << intY << ")" << std::endl;
	//	if (intX != -1 && intY != -1) {
	//		found = true; // ͼƬ���ҵ�
	//		break; // ����ѭ��
	//	}

	//	// ����Ƿ�ʱ
	//	if (clock() - start_time > timeout) {
	//		break; // �������20�뻹û�ҵ�ͼƬ������ֹѭ��
	//	}

	//	Sleep(1000); // ÿ�γ���֮��ȴ�1�루1000���룩������CPUʹ����
	//}

	//if (found) {
	//	// �ҵ�ͼƬ������ִ�к�������
	//	long FindComany = dm.FindPic(0, 0, 2240, 1400, L"������˾.bmp", L"ffffff-000000", 0.9, 2, &intX, &intY);
	//	std::cout << "�ҵ���������������Ϊ: (" << intX << ", " << intY << ")" << std::endl;
	//	dm.MoveTo(intX+50, intY);

	//	//dm.LeftClick();
	//}
	//else {
	//	// ��ʱδ�ҵ�ͼƬ��������Ҫ��ֹ�������д�����
	//	// ...
	//}

	//��������� ���

	//������˹�˾����,����س�

	//��������ڹ�˾���� ������ִ��  	//ѡ���������Ĺ�˾

	//����һ������
	//int foobar  = dm.CreateFoobarRect(0, 200, 200, 500, 200);

	/*while (true)
	{
		dm.FoobarUpdate(foobar);
		dm.delay(2000);
	}*/

	//��ȡָ������

	//ִ���ж� ����һϵ�в���

	//int x1, y1, x2, y2; // ��������Խ�������ֵ

	//// ���ú���
	//CString str =  dm.EnumWindowByProcessId(14400, L"Chrome Legacy Window", L"Chrome_RenderWidgetHostHWND", 1+2);
	//
 //
	//std::wcout << L"Window Handles: " << (LPCTSTR)str << std::endl;

	//dm.Capture(0, 0, 2240, 1400, TEXT("deskphoto.bmp"));

	//dm.FindColor(0, 0, 2240, 1400, TEXT("ffffff-000000"), 1.);

	//dm.MoveTo(300, 500);

	delete g_dm;

	return nRetCode;
}