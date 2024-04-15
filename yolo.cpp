#include "stdafx.h"

void Yolov5::LetterBox(const cv::Mat& image, cv::Mat& outImage, cv::Vec4d& params, const cv::Size& newShape,
	bool autoShape, bool scaleFill, bool scaleUp, int stride, const cv::Scalar& color)
{
	cv::Size shape = image.size();
	float r = std::min((float)newShape.width / (float)shape.width,
		(float)newShape.height / (float)shape.height); // ѡ����С�����űȣ�����ᳬ��

	float ratio[2]{ r, r };

	int new_up_pad[2] = { (int)round((float)shape.width * r),
						(int)round((float)shape.height * r) }; // ���ź���Ŀ�곤����ܻ���һ��

	auto dw = (float)(newShape.width - new_up_pad[0]);// �����Ŀ�곤������
	auto dh = (float)(newShape.height - new_up_pad[1]);
	dw /= 2.0f;
	dh /= 2.0f;

	if (shape.width != new_up_pad[0] && shape.height != new_up_pad[1])//�ȱ�������
	{
		resize(image, outImage, cv::Size(new_up_pad[0], new_up_pad[1]));
	}
	else {
		outImage = image.clone();
	}

	int top = int(round(dh - 0.1f)); // ������0�����
	int bottom = int(round(dh + 0.1f));
	int left = int(round(dw - 0.1f));
	int right = int(round(dw + 0.1f));
	params[0] = ratio[0];
	params[1] = ratio[1];
	params[2] = left;
	params[3] = top;
	copyMakeBorder(outImage, outImage, top, bottom, left, right, cv::BORDER_CONSTANT, color);
}

bool Yolov5::readModel(cv::dnn::Net& net, std::string& netPath, bool isCuda = false) {
	try {
		net = cv::dnn::readNet(netPath);
#if CV_VERSION_MAJOR==4 &&CV_VERSION_MINOR==7&&CV_VERSION_REVISION==0
		net.enableWinograd(false);  //bug of opencv4.7.x in AVX only platform ,https://github.com/opencv/opencv/pull/23112 and https://github.com/opencv/opencv/issues/23080
		//net.enableWinograd(true);		//If your CPU supports AVX2, you can set it true to speed up
#endif
	}
	catch (const std::exception&) {
		return false;
	}
	//cuda
	if (isCuda) {
		net.setPreferableBackend(cv::dnn::DNN_BACKEND_CUDA);
		net.setPreferableTarget(cv::dnn::DNN_TARGET_CUDA);
	}
	//cpu
	else {
		net.setPreferableBackend(cv::dnn::DNN_BACKEND_DEFAULT);
		net.setPreferableTarget(cv::dnn::DNN_TARGET_CPU);
	}
	return true;
}

//cv::Mat format_yolov5(const cv::Mat& source) {
//	int col = source.cols;
//	int row = source.rows;
//	int _max = MAX(col, row);
//	cv::Mat result = cv::Mat::zeros(_max, _max, CV_8UC3);
//	source.copyTo(result(cv::Rect(0, 0, col, row)));
//	return result;
//}

bool Yolov5::Detect(cv::Mat& SrcImg, cv::dnn::Net& net, std::vector<Output>& output) {
	cv::Mat blob;

	int col = SrcImg.cols;
	int row = SrcImg.rows;
	int maxLen = MAX(col, row);
	cv::Mat netInputImg = SrcImg.clone();
	cv::Vec4d params;

	//auto input_image = format_yolov5(netInputImg);
	LetterBox(SrcImg, netInputImg, params, cv::Size(_netWidth, _netHeight), true, true, false, 32, cv::Scalar());
	SrcImg = netInputImg.clone();
	cv::dnn::blobFromImage(netInputImg, blob, 1. / 255., cv::Size(_netWidth, _netHeight), cv::Scalar(), true, false);
	//�������������û�����������µ��ǽ��ƫ��ܴ󣬿��Գ������������������
	 //cv::dnn::blobFromImage(netInputImg, blob, 1 / 255.0, cv::Size(_netWidth, _netHeight), cv::Scalar(104, 117, 123), true, false);
	//cv::dnn::blobFromImage(netInputImg, blob, 1 / 255.0, cv::Size(_netWidth, _netHeight), cv::Scalar(114, 114,114), true, false);
	net.setInput(blob);

	std::vector<cv::Mat> netOutputImg;
	//vector<string> outputLayerName{"345","403", "461","output" };
	//net.forward(netOutputImg, outputLayerName[3]); //��ȡoutput�����
	net.forward(netOutputImg, net.getUnconnectedOutLayersNames());

	std::vector<int> classIds;//���id����
	std::vector<float> confidences;//���ÿ��id��Ӧ���Ŷ�����
	std::vector<cv::Rect> boxes;//ÿ��id���ο�
	int net_width = _className.size() + 5;  //������������������+5
	int net_out_width = netOutputImg[0].size[2];
	assert(net_out_width == net_width, "Error Wrong number of _className");  //ģ�������Ŀ����
	float* pdata = (float*)netOutputImg[0].data;
	int net_height = netOutputImg[0].size[1];

	for (int r = 0; r < net_height; ++r) {
		float box_score = pdata[4];
		if (box_score >= _classThreshold) {
			cv::Mat scores(1, _className.size(), CV_32FC1, pdata + 5);
			cv::Point classIdPoint;
			double max_class_score;
			minMaxLoc(scores, 0, &max_class_score, 0, &classIdPoint);
			max_class_score = max_class_score * box_score;
			if (max_class_score >= _classThreshold) {
				// ������ͼ��Ŀ�Ⱥ͸߶�������һ������
				int centerX = int(pdata[0]);
				int centerY = int(pdata[1]);
				int width = int(pdata[2]);
				int height = int(pdata[3]);

				// ����߽������Ͻ�����
				int left = centerX - width / 2;
				int top = centerY - height / 2;

				// ���߽�������������Ϣ�洢����
				classIds.push_back(classIdPoint.x);
				confidences.push_back(max_class_score);
				boxes.push_back(cv::Rect(left, top, width, height));
			}
		}
		pdata += net_width; // �ƶ�����һ��
	}


	//ִ�з�����������������нϵ����Ŷȵ������ص���NMS��
	std::vector<int> nms_result;
	cv::dnn::NMSBoxes(boxes, confidences, _classThreshold, _nmsThreshold, nms_result);
	for (int i = 0; i < nms_result.size(); i++) {
		int idx = nms_result[i];
		Output result;
		result.id = classIds[idx];
		result.confidence = confidences[idx];
		result.box = boxes[idx];
		output.push_back(result);
	}
	if (output.size())
		return true;
	else
		return false;
}

void Yolov5::drawPred(cv::Mat& img, std::vector<Output> result, std::vector<cv::Scalar> color) {

 

	cv::namedWindow("result", cv::WINDOW_AUTOSIZE);
	for (int i = 0; i < result.size(); i++) {
		int left, top;
		left = result[i].box.x;
		top = result[i].box.y;
		int color_num = i;
		cv::rectangle(img, result[i].box, color[result[i].id], 2, 8);

		std::string label = _className[result[i].id] + ":" + std::to_string(result[i].confidence);

		int baseLine;
		cv::Size labelSize = cv::getTextSize(label, cv::FONT_HERSHEY_SIMPLEX, 0.5, 1, &baseLine);
		top = std::max(top, labelSize.height);
		//rectangle(img, cv::Point(left, top - int(1.5 * labelSize.height)), cv::Point(left + int(1.5 * labelSize.width), top + baseLine), cv::Scalar(0, 255, 0), cv::FILLED);
		cv::putText(img, label, cv::Point(left, top), cv::FONT_HERSHEY_SIMPLEX, 1, color[result[i].id], 2);
	}
	cv::imshow("result", img);
	cv::waitKey(0);
	img.release();
 
}