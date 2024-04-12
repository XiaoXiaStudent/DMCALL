// example.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include <Afxwin.h>
#include "stdafx.h"
#include "obj.h"
#include "msdk.h"
#include "Application.h"

dmsoft* g_dm = NULL;

std::vector<std::string> load_class_list()
{
	std::vector<std::string> class_list;
	std::ifstream ifs("classes.txt");
	std::string line;
	while (getline(ifs, line))
	{
		class_list.push_back(line);
	}
	return class_list;
}

void load_net(cv::dnn::Net& net, bool is_cuda)
{
	auto result = cv::dnn::readNet("weights\\yolov5s.onnx");
	if (is_cuda)
	{
		std::cout << "Attempty to use CUDA\n";
		result.setPreferableBackend(cv::dnn::DNN_BACKEND_CUDA);
		result.setPreferableTarget(cv::dnn::DNN_TARGET_CUDA_FP16);
	}
	else
	{
		std::cout << "Running on CPU\n";
		result.setPreferableBackend(cv::dnn::DNN_BACKEND_OPENCV);
		result.setPreferableTarget(cv::dnn::DNN_TARGET_CPU);
	}
	net = result;
}
const std::vector<cv::Scalar> colors = { cv::Scalar(255, 255, 0), cv::Scalar(0, 255, 0), cv::Scalar(0, 255, 255), cv::Scalar(255, 0, 0) };

const float INPUT_WIDTH = 640.0;
const float INPUT_HEIGHT = 640.0;
const float SCORE_THRESHOLD = 0.2;
const float NMS_THRESHOLD = 0.4;
const float CONFIDENCE_THRESHOLD = 0.2;
 



struct Detection
{
	int class_id;
	float confidence;
	cv::Rect box;
	std::string className;
	
};

cv::Mat format_yolov5(const cv::Mat& source) {
	int col = source.cols;
	int row = source.rows;
	int _max = MAX(col, row);
	cv::Mat result = cv::Mat::zeros(_max, _max, CV_8UC3);
	source.copyTo(result(cv::Rect(0, 0, col, row)));
	return result;
}

void detect(cv::Mat& image, cv::dnn::Net& net, std::vector<Detection>& output, const std::vector<std::string>& className) {
	cv::Mat blob;

	auto input_image = format_yolov5(image);

	cv::dnn::blobFromImage(input_image, blob, 1. / 255., cv::Size(INPUT_WIDTH, INPUT_HEIGHT), cv::Scalar(), true, false);
	net.setInput(blob);
	std::vector<cv::Mat> outputs;
	net.forward(outputs, net.getUnconnectedOutLayersNames());

	float x_factor = input_image.cols / INPUT_WIDTH;
	float y_factor = input_image.rows / INPUT_HEIGHT;

	float* data = (float*)outputs[0].data;

	const int dimensions = 85;
	const int rows = 25200;

	std::vector<int> class_ids;
	std::vector<float> confidences;
	std::vector<cv::Rect> boxes;

	for (int i = 0; i < rows; ++i) {

		float confidence = data[i * dimensions + 4];
		if (confidence >= CONFIDENCE_THRESHOLD) {

			float* classes_scores = &data[i * dimensions + 5];
			cv::Mat scores(1, className.size(), CV_32FC1, classes_scores);
			cv::Point class_id;
			double max_class_score;
			minMaxLoc(scores, 0, &max_class_score, 0, &class_id);
			if (max_class_score < SCORE_THRESHOLD) {

				confidences.push_back(confidence);

				class_ids.push_back(class_id.x);

				float x = data[i * dimensions + 0];
				float y = data[i * dimensions + 1];
				float w = data[i * dimensions + 2];
				float h = data[i * dimensions + 3];
				int left = int((x - 0.5 * w) * x_factor);
				int top = int((y - 0.5 * h) * y_factor);
				int width = int(w * x_factor);
				int height = int(h * y_factor);
				boxes.push_back(cv::Rect(left, top, width, height));
			}

		}
	}


	std::vector<int> nms_result;
	//cv::dnn::NMSBoxes(boxes, confidences, SCORE_THRESHOLD, NMS_THRESHOLD, nms_result);
	for (int i = 0; i < nms_result.size(); i++) {
		int idx = nms_result[i];
		Detection result;
		result.class_id = class_ids[idx];
		result.confidence = confidences[idx];
		result.box = boxes[idx];
		output.push_back(result);
	}
}



int main(int argc, char** argv, TCHAR* envp[])
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

	// �����������߳�
	//std::thread contourThread(findAndDrawContours, x1, y1, x2, y2);

	//captureToBuffer();

	// �ȴ������߳̽���
	//contourThread.join();

	std::vector<std::string> class_list = load_class_list();

	cv::Mat frame = cv::imread("1.jpg");

	bool is_cuda = argc > 1 && strcmp(argv[1], "cuda") == 0;

	cv::dnn::Net net;
	load_net(net, is_cuda);

	auto start = std::chrono::high_resolution_clock::now();
	int frame_count = 0;
	float fps = -1;
	int total_frames = 0;

	std::vector<Detection> output;
	detect(frame, net, output, class_list);

	int detections = output.size();

	for (int i = 0; i < detections; ++i)
	{
		auto detection = output[i];
		auto box = detection.box;
		auto classId = detection.class_id;
		const auto color = colors[classId % colors.size()];
		cv::rectangle(frame, box, color, 3);

		cv::rectangle(frame, cv::Point(box.x, box.y - 20), cv::Point(box.x + box.width, box.y), color, cv::FILLED);
		cv::putText(frame, class_list[classId].c_str(), cv::Point(box.x, box.y - 5), cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(0, 0, 0));
	}

	cv::imshow("output", frame);
	cv::waitKey(0);
	cv::destroyAllWindows();

	CoUninitialize();
	delete g_dm;
	return 0;
}

void captureToBuffer()
{
	int x1 = 0, y1 = 0;
	int x2 = 500, y2 = 500;
	int width = x2 - x1 + 1;
	int height = y2 - y1 + 1;
	long mouseX, mouseY;

	// ������������ʾԭʼͼ�������ͼ��
	cv::namedWindow("Original Display", cv::WINDOW_AUTOSIZE);
	cv::namedWindow("Contours Display", cv::WINDOW_AUTOSIZE);

	// ��ѭ���ⲿ׼���������õ���Դ
	cv::Mat image;
	cv::Mat grayImage;

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

		//�������ڱ�������ָ��ʹ�С
		unsigned char* data = nullptr;
		long size = 0;

		// ��ȡ��Ļ����
		int fanHui = g_dm->GetScreenDataBmp(x1, y1, x2, y2, reinterpret_cast<long*>(&data), &size);

		// ʹ�û�õ����ݴ���cv::Mat����
		cv::Mat rawData(1, size, CV_8UC1, data); // data ��ָ��λͼ���ݵ�ָ��
		cv::Mat image = cv::imdecode(rawData, cv::IMREAD_COLOR); // ����λͼ����

		// ��ԭʼ��������ʾͼ��
		cv::imshow("Original Display", image);

		// 1. ת��Ϊ�Ҷ�ͼ��
		cv::Mat grayImage;
		cv::cvtColor(image, grayImage, cv::COLOR_BGR2GRAY);

		// 2. Ӧ�ø�˹ģ������
		cv::Mat blurredImage;
		cv::GaussianBlur(grayImage, blurredImage, cv::Size(5, 5), 1.5);

		// 3. ����Canny��Ե���
		cv::Mat edges;
		cv::Canny(blurredImage, edges, 50, 150); // �������ֵ���Ը���ʵ����Ҫ����

		// 4. Ѱ������
		std::vector<std::vector<cv::Point>> contours;
		cv::findContours(edges, contours, cv::RETR_TREE, cv::CHAIN_APPROX_SIMPLE);

		// 5. ��������
		cv::Mat contourImage = cv::Mat::zeros(image.size(), CV_8UC3);
		for (int i = 0; i < contours.size(); i++) {
			cv::Scalar color = cv::Scalar(255, 255, 255); // ��ɫ����
			cv::drawContours(contourImage, contours, i, color, 1, cv::LINE_8);
		}

		cv::imshow("Contours Display", contourImage);

		auto end = std::chrono::high_resolution_clock::now();
		std::chrono::duration<double, std::milli> elapsed = end - start;
		std::cout << "test02ִ��ʱ��1111: " << elapsed.count() << " ms\n";

		if (cv::waitKey(1) == 27) { // ������ESC��ʱ�˳�ѭ��
			break;
		}
	}
}

void findAndDrawContours(int x1, int y1, int x2, int y2) {
	dmsoft* dm = new dmsoft;

	// ������������ʾԭʼͼ�������ͼ��

	cv::namedWindow("Contour Display", cv::WINDOW_AUTOSIZE);

	// ��ѭ���ⲿ׼���������õ���Դ

	int width = x2 - x1 + 1;
	int height = y2 - y1 + 1;
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

		cv::Mat contourImage = cv::Mat::zeros(cv::Size(width, height), CV_8UC3);
		// ������������contours�ǰ��������������
		for (size_t i = 0; i < contours.size(); i++) {
			cv::Scalar color = cv::Scalar(255, 255, 255); //
			cv::drawContours(contourImage, contours, static_cast<int>(i), color);
		}

		auto end = std::chrono::high_resolution_clock::now();
		std::chrono::duration<double, std::milli> elapsed = end - start;
		std::cout << "test02ִ��ʱ��1111: " << elapsed.count() << " ms\n";

		// ��ʾ����ͼ��
		cv::imshow("Contour Display", contourImage);

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