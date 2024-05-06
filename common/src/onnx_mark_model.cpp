#include <utility>

#include "../include/onnx_mark_model.h"

// ��ʼ��
YOLOv5::YOLOv5(const Configuration &config, vector<string> classes, bool isCuda = false) {
    this->confThreshold = config.confThreshold;
    this->nmsThreshold = config.nmsThreshold;
    this->objThreshold = config.objThreshold;

    this->net = readNet(config.modelpath);  // ����ģ��onnxȨ�ء�dnn.hpp

    this->classes = std::move(classes);
    //cuda // https://blog.csdn.net/cxyhjl/article/details/125383555
    if (isCuda) {
        net.setPreferableBackend(cv::dnn::DNN_BACKEND_CUDA);
        net.setPreferableTarget(cv::dnn::DNN_TARGET_CUDA);
        cout << "cuda" << endl;
    }
        //cpu
    else {
        net.setPreferableBackend(cv::dnn::DNN_BACKEND_DEFAULT);
        net.setPreferableTarget(cv::dnn::DNN_TARGET_CPU);
    }
    this->num_classes = static_cast<int>(this->classes.size());  // �������
    this->inpHeight = 640;
    this->inpWidth = 640;
}

Mat YOLOv5::resize_image(Mat srcimg, int *newh, int *neww, int *top, int *left) const {
    int srch = srcimg.rows, srcw = srcimg.cols;  // ����߿�
    *newh = this->inpHeight;    // ָ�����ָ������yoloģ�͵Ŀ��
    *neww = this->inpWidth;
    Mat dstimg;                 // ����һ��Ŀ��Դ
    if (this->keep_ratio && srch != srcw) {   // �߿���
        float hw_scale = (float) srch / srcw; // �������
        if (hw_scale > 1) {     // ����yolov5��Ԥ������д���
            *newh = this->inpHeight;
            *neww = int(this->inpWidth / hw_scale); //
            resize(srcimg, dstimg, Size(*neww, *newh), INTER_AREA);
            *left = int((this->inpWidth - *neww) * 0.5);
            // ��yolov5�Ĵ����Ӧ,û�н���32��ȡģ����,�������114������䵽(640,640)��,������뻹��640,640
            copyMakeBorder(dstimg, dstimg, 0, 0, *left, this->inpWidth - *neww - *left, BORDER_CONSTANT, 114);
        } else {
            *newh = (int) this->inpHeight * hw_scale;
            *neww = this->inpWidth;
            resize(srcimg, dstimg, Size(*neww, *newh), INTER_AREA);
            *top = (int) (this->inpHeight - *newh) * 0.5;
            copyMakeBorder(dstimg, dstimg, *top, this->inpHeight - *newh - *top, 0, 0, BORDER_CONSTANT, 114);
        }
    } else {
        resize(srcimg, dstimg, Size(*neww, *newh), INTER_AREA);
    }
    return dstimg;
}

// Draw the predicted bounding box
void YOLOv5::drawPred(float conf, int left, int top, int right, int bottom, Mat &frame, int classid) {
    //Draw a rectangle displaying the bounding box
    rectangle(frame, Point(left, top), Point(right, bottom), Scalar(0, 0, 255), 2);

    //Get the label for the class name and its confidence
    string label = format("%.2f", conf);
    label = this->classes[classid] + ":" + label;

    //Display the label at the top of the bounding box
    int baseLine;
    Size labelSize = getTextSize(label, FONT_HERSHEY_SIMPLEX, 0.5, 1, &baseLine);
    top = max(top, labelSize.height);
    //rectangle(frame, Point(left, top - int(1.5 * labelSize.height)), Point(left + int(1.5 * labelSize.width), top + baseLine), Scalar(0, 255, 0), FILLED);
    putText(frame, label, Point(left, top), FONT_HERSHEY_SIMPLEX, 0.75, Scalar(0, 255, 0), 1);

    //    cout << "��ǩid: " << classid << ", ��ǩ: " << this->classes[classid] << ", ���Ŷ�: " << conf << ", Left: " << left
//         << ", Top: " << top << ", Right: " << right << ", Bottom: " << bottom << endl;
}

// Ԥ��
vector<DetectLabel> YOLOv5::detect(Mat frame) {
    int newh = 0, neww = 0, padh = 0, padw = 0;
    Mat dstimg = this->resize_image(frame, &newh, &neww, &padh, &padw);  // Ԥ����
    Mat blob = blobFromImage(dstimg, 1 / 255.0, Size(this->inpWidth, this->inpHeight), Scalar(0, 0, 0), true,
                             false); // return:4-dimensional Mat with NCHW dimensions order.
    this->net.setInput(blob);    // �������
    vector<Mat> outs;   // Ҫ���յ���һ��
    this->net.forward(outs, this->net.getUnconnectedOutLayersNames());  // [b,num_pre,(5+classes)]

    int num_proposal = outs[0].size[1]; // 25200
    int out_dim2 = outs[0].size[2];  //
    if (outs[0].dims > 2) {
        outs[0] = outs[0].reshape(0, num_proposal);  // һ�㶼����ڶ�ά�ģ�����չƽ��ά[b,num_pre*(5+classes)]
    }
    /////generate proposals
    vector<float> confidences;
    vector<Rect> boxes;    //  opencv�ﱣ��box��
    vector<int> classIds;
    float ratioh = (float) frame.rows / newh, ratiow = (float) frame.cols / neww;
    float *pdata = (float *) outs[0].data;  // ���帡����ָ�룬
    for (int i = 0; i < num_proposal; ++i) // �������е�num_pre_boxes
    {
        int index = i * out_dim2;      // prob[b*num_pred_boxes*(classes+5)]
        float obj_conf = pdata[index + 4];  // ���Ŷȷ���
        if (obj_conf > this->objThreshold)  // ������ֵ
        {
            //Mat scores = outs[0].row(row_ind).colRange(5, nout); // �൱��python�����Ƭ������ÿ���Ԥ��������
            Mat scores(1, this->num_classes, CV_32FC1, pdata + index + 5);     // ���������������
            Point classIdPoint; //�����
            double max_class_socre; // ����һ��double���͵ı�������Ԥ�������������ֵ
            // Get the value and location of the maximum score
            minMaxLoc(scores, 0, &max_class_socre, 0, &classIdPoint);  // ��ÿ������������ֵ������
            max_class_socre *= obj_conf;   // ����������*���Ŷ�
            if (max_class_socre > this->confThreshold) // �ٴ�ɸѡ
            {
                const int class_idx = classIdPoint.x;  // �������,��yolo����Ǳ�ʾ�ڼ���
                // float cx = (pdata[0] * 2.f - 0.5f + j) * stride;  ///cx,ӳ���ԭͼ����Ӧyolov5�������
                // float cy = (pdata[1] * 2.f - 0.5f + i) * stride;   ///cy���������ڵ�yolov5������onnx�Ѿ�������������������
                // float w = powf(pdata[2] * 2.f, 2.f) * anchor_w;   ///w���������ﲻ��Ҫ����һ���ˣ�ֱ��ȡ�������
                // float h = powf(pdata[3] * 2.f, 2.f) * anchor_h;  ///h

                // ���������ֻ��Ҫֱ��ȡ����
                float cx = pdata[index];  //x
                float cy = pdata[index + 1];  //y
                float w = pdata[index + 2];  //w
                float h = pdata[index + 3];  //h

                int left = int((cx - padw - 0.5 * w) * ratiow);  // *ratiow�����ԭͼ�ߴ�
                int top = int((cy - padh - 0.5 * h) * ratioh);

                confidences.push_back((float) max_class_socre);
                boxes.push_back(Rect(left, top, (int) (w * ratiow), (int) (h * ratioh)));  //��x,y,w,h��
                classIds.push_back(class_idx);  //
            }
        }

    }

    vector<int> indices;
    vector<DetectLabel> detect_labels;
    dnn::NMSBoxes(boxes, confidences, this->confThreshold, this->nmsThreshold, indices);
    for (size_t i = 0; i < indices.size(); ++i) {
        int idx = indices[i];
        Rect box = boxes[idx];
        this->drawPred(confidences[idx], box.x, box.y,
                       box.x + box.width, box.y + box.height, frame, classIds[idx]);

        DetectLabel label = {
                classIds[idx],
                this->classes[label.classid],
                confidences[idx],
                box.x,
                box.y,
                box.x + box.width,
                box.y + box.height
        };
//        label.classid = classIds[idx];
//        label.label = this->classes[label.classid];
//        label.conf = confidences[idx];
//        label.left = box.x;
//        label.right = box.x + box.width;
//        label.top = box.y;
//        label.bottom = box.y + box.height;
        detect_labels.push_back(label);
    }
    return detect_labels;
}

//int main() {
//    auto timeStart = (double) getTickCount();
//    clock_t startTime, endTime; //����ʱ��
//    // �Լ������yoloһЩ���ó���
//    Configuration yolo_nets = {0.3, 0.5, 0.3, "F:\\advertising_interface_yolov5.onnx"};
//    YOLOv5 yolo_model(yolo_nets, false);
//    string imgpath = "F:\\20240424_223804.jpg";
//    Mat srcimg = imread(imgpath);
//
//    double nTime = ((double) getTickCount() - timeStart) / getTickFrequency();
//    startTime = clock();//��ʱ��ʼ
//    yolo_model.detect(srcimg);
//    endTime = clock();//��ʱ����
//    cout << "clock_running time is:" << (double) (endTime - startTime) / CLOCKS_PER_SEC << "s" << endl;
//    cout << "The whole run time is:" << (double) clock() / CLOCKS_PER_SEC << "s" << endl;
//    cout << "getTickCount_running time :" << nTime << "s" << endl;
//
//    static const string kWinName = "Deep learning object detection in OpenCV";
//    namedWindow(kWinName, WINDOW_NORMAL);  // ����Ӧ���ڴ��ڴ�С
//    imwrite("restult_cpu.jpg", srcimg);
//    imshow(kWinName, srcimg);
//    float width = 271;
//    float height = 603;
//    float a = 1.25;
//    // �������ڴ�С
//    resizeWindow(kWinName, static_cast<int>(width * a), static_cast<int>(height * a));
//    waitKey(0);
//    destroyAllWindows();
//    return 0;
//}

/**
 * @brief ������ͼ�����Ŀ���⣬�����ؼ�⵽�ı�ǩ������ʱ�䡣
 *
 * @param yolo_nets YOLO ģ�͵����ò�����
 * @param label_class ��ǩ����б�
 * @param input_img_path ����ͼ���·����
 * @param output_img_path ���ͼ���·������ѡ����
 * @param is_cude �Ƿ�ʹ�� GPU ����ģ�ͣ�Ĭ��Ϊ false��
 * @return һ�� pair��������⵽�ı�ǩ�б������ʱ�䣨��λ���룩��
 */
pair<vector<DetectLabel>, double> detectImage(
        const Configuration &yolo_nets,
        const vector<string> &label_class,
        const string &input_img_path,
        const string &output_img_path,
        bool is_cude
) {
    clock_t startTime, endTime; //����ʱ��

    // �Լ������yoloһЩ���ó���
    YOLOv5 yolo_model(yolo_nets, label_class, is_cude);
    Mat srcimg = imread(input_img_path);

    startTime = clock();                                            // ��ʱ��ʼ
    vector<DetectLabel> detect_labels = yolo_model.detect(srcimg);  // ִ������
    endTime = clock();                                              // ��ʱ����
    double clock_running = (double) (endTime - startTime) / CLOCKS_PER_SEC;

    // ���ͼƬ��ָ��·��
    if (!output_img_path.empty()) {
        imwrite(output_img_path, srcimg);
    }

    return make_pair(detect_labels, clock_running);
}

pair<vector<DetectLabel>, Mat> reDetectImage(YOLOv5 *yolo_model, Mat srcimg) {

    vector<DetectLabel> detect_labels = yolo_model->detect(srcimg);  // ִ������

    return make_pair(detect_labels, srcimg);
}
