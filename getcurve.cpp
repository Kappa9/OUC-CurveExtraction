#include<getcurve.h>

//一个在PC端实现提取论文中的曲线的程序
bool GetCurve::findNextPoint(vector<Point>& _neighbor_points, Mat& _image, Point _inpoint, int flag, Point& _outpoint, int& _outflag)
{
    int i = flag;
    int count = 1;
    bool success = false;

    while (count <= 7)
    {
        Point tmppoint = _inpoint + _neighbor_points[i];
        if (tmppoint.x > 0 && tmppoint.y > 0 && tmppoint.x < _image.cols && tmppoint.y < _image.rows)
        {
            if (_image.at<uchar>(tmppoint) == 255)
            {
                _outpoint = tmppoint;
                _outflag = i;
                success = true;
                _image.at<uchar>(tmppoint) = 0;
                break;
            }
        }
        if (count % 2)
        {
            i += count;
            if (i > 7)
            {
                i -= 8;
            }
        }
        else
        {
            i += -count;
            if (i < 0)
            {
                i += 8;
            }
        }
        count++;
    }
    return success;
}
//寻找图像上的第一个点
bool GetCurve::findFirstPoint(Mat& _inputimg, Point& _outputpoint)
{
    bool success = false;
    for (int i = 0; i < _inputimg.rows; i++)
    {
        uchar* data = _inputimg.ptr<uchar>(i);
        for (int j = 0; j < _inputimg.cols; j++)
        {
            if (data[j] == 255)
            {
                success = true;
                _outputpoint.x = j;
                _outputpoint.y = i;
                data[j] = 0;
                break;
            }
        }
        if (success)
            break;
    }
    return success;
}
//寻找曲线
void GetCurve::findLines(Mat& _inputimg, vector<deque<Point>>& _outputlines)
{
    vector<Point> neighbor_points = { Point(-1,-1),Point(0,-1),Point(1,-1),Point(1,0),Point(1,1),Point(0,1),Point(-1,1),Point(-1,0) };
    Point first_point;
    while (findFirstPoint(_inputimg, first_point))
    {
        deque<Point> line;
        line.push_back(first_point);
        //由于第一个点不一定是线段的起始位置，双向找
        Point this_point = first_point;
        int this_flag = 0;
        Point next_point;
        int next_flag;
        while (findNextPoint(neighbor_points, _inputimg, this_point, this_flag, next_point, next_flag))
        {
            line.push_back(next_point);
            this_point = next_point;
            this_flag = next_flag;
        }
        //找另一边
        this_point = first_point;
        this_flag = 0;
        //cout << "flag:" << this_flag << endl;
        while (findNextPoint(neighbor_points, _inputimg, this_point, this_flag, next_point, next_flag))
        {
            line.push_front(next_point);
            this_point = next_point;
            this_flag = next_flag;
        }
        if (line.size() > 10)
        {
            _outputlines.push_back(line);
        }
    }
}
//随机取色 用于画线的时候
Scalar GetCurve::random_color(RNG& _rng)
{
    int icolor = (unsigned)_rng;
    return Scalar(icolor & 0xFF, (icolor >> 8) & 0xFF, (icolor >> 16) & 0xFF);
}
int GetCurve::curvemain(int argc, char* argv[])
{
    Mat image = imread("D:\\data.jpg", 1);
    if (!image.data)
    {
        cout << "the image is error" << endl;
        return 0;
    }
    imshow("imageshow", image);
    image.copyTo(image1);
    cout << "Auto or not:" << endl;
    //cin >> isauto;
    isauto = 0;
    //cout  << isauto << endl;
    if (!isauto)
    {
        cout << "请点击曲线图的左上/右上/左下/右下角：" << endl;
        //cv::setMouseCallback("imageshow", mousecallback, 0);
    }
    //autoProcess(image1);
    //adaptiveThreshold(image1, image1, 255, ADAPTIVE_THRESH_MEAN_C, THRESH_BINARY, 5, -2);
    //imshow("image1", image1);
    //Mat gray,atnois;
    //cvtColor(~image1, gray, CV_BGR2GRAY);
    //bilateralFilter(gray, atnois, 9, 20, 3);
    //adaptiveThreshold(atnois, gray, 255, ADAPTIVE_THRESH_MEAN_C, THRESH_BINARY, 15, -2);
    //imshow("thinimg", gray);
    //imwrite("D:/test_bin.png", gray);
    //chao_thinimage(gray);
    //暂时
    //getcurve(image1);
    waitKey(0);
    return 0;
}
void GetCurve::autoProcess(Mat src)
{
    Mat dst;
    Mat structreElement1 = getStructuringElement(MORPH_RECT, Size(3, 3), Point(-1, -1));
    Mat structreElement2 = getStructuringElement(MORPH_RECT, Size(2, 2), Point(-1, -1));

    Mat gray_src, antiNoise;
    cvtColor(~src, gray_src, CV_BGR2GRAY);
    //imshow(" grayImg", gray_src); //imwrite("D:\graysrc.png", gray_src);
    bilateralFilter(gray_src, antiNoise, 9, 20, 3);
    //imshow(" antiImg", antiNoise); //imwrite("D:\wanti.png", antiNoise);
    Mat binImg;
    adaptiveThreshold(antiNoise, binImg, 255, ADAPTIVE_THRESH_MEAN_C, THRESH_BINARY, 15, -2);

    //imshow(" binImg", binImg); imwrite("D:\wbin4.png", binImg);

    //霍夫变换
    Mat whiteBoard = Mat::zeros(binImg.size(), CV_8UC3);
    Mat whiteBoard2 = whiteBoard;
    vector<Vec4f> plines;
    HoughLinesP(binImg, plines, 1, CV_PI / 180.0, 200, binImg.rows / 1.5, 3);
    Scalar color = Scalar(0, 0, 255);
    for (size_t i = 0; i < plines.size(); i++)
    {
        Vec4f hline = plines[i];
        line(whiteBoard, Point(hline[0], hline[1]), Point(hline[2], hline[3]), color, 1, LINE_AA);
    }

    cvtColor(whiteBoard, dst, CV_BGR2GRAY);
    dilate(dst, dst, structreElement2, Point(-1, -1), 1);
    erode(dst, dst, structreElement1, Point(-1, -1), 1); //erode(dst, dst, structreElement1, Point(-1, -1), 1);

    imshow("HL1 Image", dst); //imwrite("D:\huofu.png", dst);

    vector<vector<Point>> contours;
    //储存轮廓
    vector<Vec4i> hierarchy;
    findContours(dst, contours, hierarchy, CV_RETR_LIST, CV_CHAIN_APPROX_NONE);
    //获取轮廓
    Mat linePic = Mat::zeros(binImg.rows, binImg.cols, CV_8UC3);
    for (int index = 0; index < contours.size(); index++) {
        drawContours(linePic, contours, index, Scalar(rand() & 255, rand() & 255, rand() & 255), 1, 8/*, hierarchy*/);
    }
    //imshow("Counters Image", linePic);

    //寻找面积第二大轮廓
    vector<vector<Point>> polyContours(contours.size());
    Point centerpoint;

    int maxArea = 0, secID = 0, secAR = 0;
    for (int index = 0; index < contours.size(); index++) {
        if (contourArea(contours[index]) > contourArea(contours[maxArea]))
        {
            secID = maxArea, maxArea = index;
        }
        if (contourArea(contours[secID]) > contourArea(contours[secAR]))
        {
            secAR = secID;
        }
        approxPolyDP(contours[index], polyContours[index], 10, true);
    }
    //Mat polyPic = Mat::zeros(binImg.size(), CV_8UC3);
    //drawContours(polyPic, polyContours, secAR, Scalar(0, 0, 255/*rand() & 255, rand() & 255, rand() & 255*/), 1);



    vector<int>  hull;
    convexHull(polyContours[secAR], hull, false);    //检测该轮廓的凸包
    cout << polyContours[secAR] << endl;
    //将四个点按照左右上下的顺序排列
    centerpoint = (polyContours[secAR][1] + polyContours[secAR][2] + polyContours[secAR][3] + polyContours[secAR][0]) / 4;
    cout << centerpoint << endl;
    Point temppoint;
    for (int i = 0; i < 4; i++)
    {
        if ((polyContours[secAR][i].x < centerpoint.x) && (polyContours[secAR][i].y < centerpoint.y))
        {
            temppoint = polyContours[secAR][0];
            polyContours[secAR][0] = polyContours[secAR][i];
            polyContours[secAR][i] = temppoint;
        }
    }
    for (int i = 1; i < 4; i++)
    {
        if ((polyContours[secAR][i].x > centerpoint.x) && (polyContours[secAR][i].y < centerpoint.y))
        {
            temppoint = polyContours[secAR][1];
            polyContours[secAR][1] = polyContours[secAR][i];
            polyContours[secAR][i] = temppoint;
        }

    }
    for (int i = 2; i < 4; i++)
    {
        if ((polyContours[secAR][i].x < centerpoint.x) && (polyContours[secAR][i].y > centerpoint.y))
        {
            temppoint = polyContours[secAR][2];
            polyContours[secAR][2] = polyContours[secAR][i];
            polyContours[secAR][i] = temppoint;
        }
    }
    cout << polyContours[secAR] << endl;
    for (int i = 0; i < 4; i++)
    {
        ROIpoint[i].x = polyContours[secAR][i].x;
        ROIpoint[i].y = polyContours[secAR][i].y;
    }
    image1 = PicetureFix(binImg);
    //adaptiveThreshold(fixedimg, fixedimg, 255, ADAPTIVE_THRESH_MEAN_C, THRESH_BINARY, 5, -2);
    //chao_thinimage(fixedimg);
    imshow("fixed", image1); //imwrite("D:\dwfixed.png", image1);
}
Mat GetCurve::PicetureFix(Mat& img) //仿射变换实现
{
    imgH = img.rows; imgW = img.cols;
    imgpoint[0].x = 0;             //dst top left
    imgpoint[0].y = 0;
    imgpoint[1].x = imgW;  //dst top right
    imgpoint[1].y = 0;
    imgpoint[2].x = 0;             //dst buttom left
    imgpoint[2].y = imgH;
    imgpoint[3].x = imgW;   //dst buttom right
    imgpoint[3].y = imgH;
    CvMat* H = cvCreateMat(3, 3, CV_32F);
    cvGetPerspectiveTransform(ROIpoint, imgpoint, H);
    //对图象进行校正
    //IplImage* srcImg = &IplImage(img);
    IplImage temp = IplImage(img);
    IplImage* srcImg = &temp;
    IplImage* dstImg = cvCloneImage(srcImg);
    cvWarpPerspective(srcImg, dstImg, H, CV_INTER_LINEAR, cvScalarAll(255));
    Mat M = cvarrToMat(dstImg);
    imshow("dst", M);
    return M;
}
void GetCurve::mousecallback(int event, int x, int y, int flags, void* xcalue )
{
    image1.copyTo(image2);
    switch (event)
    {
    case EVENT_LBUTTONDOWN:
    {
        flag = true;
    }
    break;
    case EVENT_LBUTTONUP:
    {
        if (flag)
        {

            ROIpoint[pointnum].x = x;
            ROIpoint[pointnum].y = y;
            cout << ROIpoint[pointnum].x << endl;
            cout << ROIpoint[pointnum].y << endl;
            flag = 0;
            pointnum += 1;

        }
        if (pointnum == 4)
        {
            imgH = image1.rows; imgW = image1.cols;
            imgpoint[0].x = 0;             //dst top left
            imgpoint[0].y = 0;
            imgpoint[1].x = imgW;  //dst top right
            imgpoint[1].y = 0;
            imgpoint[2].x = 0;             //dst buttom left
            imgpoint[2].y = imgH;
            imgpoint[3].x = imgW;   //dst buttom right
            imgpoint[3].y = imgH;
            CvMat* H = cvCreateMat(3, 3, CV_32F);
            cvGetPerspectiveTransform(ROIpoint, imgpoint, H);
            //对图象进行校正
            //IplImage* srcImg = &IplImage(image1);
            IplImage temp = IplImage(image1);
            IplImage* srcImg = &temp;
            IplImage* dstImg = cvCloneImage(srcImg);
            cvWarpPerspective(srcImg, dstImg, H, CV_INTER_LINEAR, cvScalarAll(255));
            cvShowImage("dst", dstImg);
            Mat M= cvarrToMat(dstImg);
            //二值化&反色
            //denoisebinfilter(M);
            //骨架提取
            //chao_thinimage(M);
            imshow("thinImage", M);
            imwrite("D:/data2.jpg", M);
        }
        break;
    }
    }
    ////用户调整部分
    //float offsetx = 0;
    //float offsety = 0;
    //float xmin = 0, xmax = 1, ymin = 0, ymax = 1;
    //int ptmd = 0;//点输出密度
    //cout << "请输入横坐标(x)偏移像素数：" << endl;
    //cin >> offsetx;
    //cout << "请输入纵坐标(y)偏移像素数：" << endl;
    //cin >> offsety;
    //cout << "请输入横坐标(x)最小值：" << endl;
    //cin >> xmin;
    //cout << "请输入横坐标(x)最大值：" << endl;
    //cin >> xmax;
    //cout << "请输入纵坐标(y)最小值：" << endl;
    //cin >> ymin;
    //cout << "请输入纵坐标(y)最大值：" << endl;
    //cin >> ymax;
    //cout << "请输入点密度：" << endl;
    //cin >> ptmd;

    //if (ymax <= ymin || xmax <= xmin)
    //{
    //	cout << "参数有错误" << endl;
    //	return;
    //}

    //for (int i = 0; i < pt.size(); i++)
    //{
    //	Point2f newpt;
    //	newpt.x = pt[i].y;
    //	newpt.y = bin.rows - pt[i].x - 1;
    //	newpt.x = (xmax - xmin) * (newpt.x + offsetx) / bin.cols + xmin;
    //	newpt.y = (ymax - ymin) * (newpt.y + offsety) / bin.rows + ymin;
    //	pt2.push_back(newpt);
    //}
    //cout << "计算结束,输出数据" << endl;

    ////csv文件写入部分
    //ofstream oFile;  //定义文件输出流

    //oFile.open("坐标.csv", ios::out | ios::trunc);    //打开要输出的文件     这样就很容易的输出一个需要的excel 文件
    ////输出保存数据
    //for (int i = 0; i < pt2.size(); i++)
    //{
    //	oFile << pt2[i].x << "," << pt2[i].y << endl;
    //	i += ptmd;
    //}
}
//void GetCurve::denoisebinfilter(Mat& srcimage)
//{
//    Mat gray, antiNoi,dsti;
//    cvtColor(~srcimage, gray, CV_BGR2GRAY);
//
//    bilateralFilter(gray, antiNoi, 9, 20, 3);
//    //Mat kernel = getStructuringElement(MORPH_RECT, Size(1, 1), Point(-1, -1));
//    //morphologyEx(gray_src, antiNoise, CV_MOP_OPEN, kernel);
//
//    Mat bin;
//    adaptiveThreshold(antiNoi, bin, 255, ADAPTIVE_THRESH_MEAN_C, THRESH_BINARY, 15, -2);
//
//
//    /*Mat hline = getStructuringElement(MORPH_RECT, Size(srcimage.cols / 4, 1), Point(-1, -1));
//    Mat vline = getStructuringElement(MORPH_RECT, Size(1, srcimage.rows / 4), Point(-1, -1));
//
//    Mat temp;
//    erode(binImg, temp, hline);
//    dilate(temp, dst, hline);
//
//    Mat temp2, dst2;
//    erode(binImg, temp2, vline);
//    dilate(temp2, dst2, vline);
//    add(temp2, temp, temp);*/
//    srcimage = bin;
//}
void GetCurve::chao_thinimage(Mat& srcimage)//单通道、二值化后的图像
{
    vector<Point> deletelist1;
    int Zhangmude[9];
    int nl = srcimage.rows;
    int nc = srcimage.cols;
    while (true)
    {
        for (int j = 1; j < (nl - 1); j++)
        {
            uchar* data_last = srcimage.ptr<uchar>(j - 1);
            uchar* data = srcimage.ptr<uchar>(j);
            uchar* data_next = srcimage.ptr<uchar>(j + 1);
            for (int i = 1; i < (nc - 1); i++)
            {
                if (data[i] == 255)
                {
                    Zhangmude[0] = 1;
                    if (data_last[i] == 255) Zhangmude[1] = 1;
                    else  Zhangmude[1] = 0;
                    if (data_last[i + 1] == 255) Zhangmude[2] = 1;
                    else  Zhangmude[2] = 0;
                    if (data[i + 1] == 255) Zhangmude[3] = 1;
                    else  Zhangmude[3] = 0;
                    if (data_next[i + 1] == 255) Zhangmude[4] = 1;
                    else  Zhangmude[4] = 0;
                    if (data_next[i] == 255) Zhangmude[5] = 1;
                    else  Zhangmude[5] = 0;
                    if (data_next[i - 1] == 255) Zhangmude[6] = 1;
                    else  Zhangmude[6] = 0;
                    if (data[i - 1] == 255) Zhangmude[7] = 1;
                    else  Zhangmude[7] = 0;
                    if (data_last[i - 1] == 255) Zhangmude[8] = 1;
                    else  Zhangmude[8] = 0;
                    int whitepointtotal = 0;
                    for (int k = 1; k < 9; k++)
                    {
                        whitepointtotal = whitepointtotal + Zhangmude[k];
                    }
                    if ((whitepointtotal >= 2) && (whitepointtotal <= 6))
                    {
                        int ap = 0;
                        if ((Zhangmude[1] == 0) && (Zhangmude[2] == 1)) ap++;
                        if ((Zhangmude[2] == 0) && (Zhangmude[3] == 1)) ap++;
                        if ((Zhangmude[3] == 0) && (Zhangmude[4] == 1)) ap++;
                        if ((Zhangmude[4] == 0) && (Zhangmude[5] == 1)) ap++;
                        if ((Zhangmude[5] == 0) && (Zhangmude[6] == 1)) ap++;
                        if ((Zhangmude[6] == 0) && (Zhangmude[7] == 1)) ap++;
                        if ((Zhangmude[7] == 0) && (Zhangmude[8] == 1)) ap++;
                        if ((Zhangmude[8] == 0) && (Zhangmude[1] == 1)) ap++;
                        if (ap == 1)
                        {
                            if ((Zhangmude[1] * Zhangmude[7] * Zhangmude[5] == 0) && (Zhangmude[3] * Zhangmude[5] * Zhangmude[7] == 0))
                            {
                                deletelist1.push_back(Point(i, j));
                            }
                        }
                    }
                }
            }
        }
        if (deletelist1.size() == 0) break;
        for (size_t i = 0; i < deletelist1.size(); i++)
        {
            Point tem;
            tem = deletelist1[i];
            uchar* data = srcimage.ptr<uchar>(tem.y);
            data[tem.x] = 0;
        }
        deletelist1.clear();

        for (int j = 1; j < (nl - 1); j++)
        {
            uchar* data_last = srcimage.ptr<uchar>(j - 1);
            uchar* data = srcimage.ptr<uchar>(j);
            uchar* data_next = srcimage.ptr<uchar>(j + 1);
            for (int i = 1; i < (nc - 1); i++)
            {
                if (data[i] == 255)
                {
                    Zhangmude[0] = 1;
                    if (data_last[i] == 255) Zhangmude[1] = 1;
                    else  Zhangmude[1] = 0;
                    if (data_last[i + 1] == 255) Zhangmude[2] = 1;
                    else  Zhangmude[2] = 0;
                    if (data[i + 1] == 255) Zhangmude[3] = 1;
                    else  Zhangmude[3] = 0;
                    if (data_next[i + 1] == 255) Zhangmude[4] = 1;
                    else  Zhangmude[4] = 0;
                    if (data_next[i] == 255) Zhangmude[5] = 1;
                    else  Zhangmude[5] = 0;
                    if (data_next[i - 1] == 255) Zhangmude[6] = 1;
                    else  Zhangmude[6] = 0;
                    if (data[i - 1] == 255) Zhangmude[7] = 1;
                    else  Zhangmude[7] = 0;
                    if (data_last[i - 1] == 255) Zhangmude[8] = 1;
                    else  Zhangmude[8] = 0;
                    int whitepointtotal = 0;
                    for (int k = 1; k < 9; k++)
                    {
                        whitepointtotal = whitepointtotal + Zhangmude[k];
                    }
                    if ((whitepointtotal >= 2) && (whitepointtotal <= 6))
                    {
                        int ap = 0;
                        if ((Zhangmude[1] == 0) && (Zhangmude[2] == 1)) ap++;
                        if ((Zhangmude[2] == 0) && (Zhangmude[3] == 1)) ap++;
                        if ((Zhangmude[3] == 0) && (Zhangmude[4] == 1)) ap++;
                        if ((Zhangmude[4] == 0) && (Zhangmude[5] == 1)) ap++;
                        if ((Zhangmude[5] == 0) && (Zhangmude[6] == 1)) ap++;
                        if ((Zhangmude[6] == 0) && (Zhangmude[7] == 1)) ap++;
                        if ((Zhangmude[7] == 0) && (Zhangmude[8] == 1)) ap++;
                        if ((Zhangmude[8] == 0) && (Zhangmude[1] == 1)) ap++;
                        if (ap == 1)
                        {
                            if ((Zhangmude[1] * Zhangmude[3] * Zhangmude[5] == 0) && (Zhangmude[3] * Zhangmude[1] * Zhangmude[7] == 0))
                            {
                                deletelist1.push_back(Point(i, j));
                            }
                        }
                    }
                }
            }
        }
        if (deletelist1.size() == 0) break;
        for (size_t i = 0; i < deletelist1.size(); i++)
        {
            Point tem;
            tem = deletelist1[i];
            uchar* data = srcimage.ptr<uchar>(tem.y);
            data[tem.x] = 0;
        }
        deletelist1.clear();
    }
}
void GetCurve::getcurve(Mat gray) //图像校正后提取曲线的核心代码
{
    Mat src = gray;
    imshow("Input Image", src);
    ofstream outp;
    Mat dst;
    Point linepointU, linepointD;

    int PoiNumInacol, startrow;
    int stepsize;//设置步长
    int linewidth;
    int col = src.cols;
    int row = src.rows;

    float xmin, xmax, ymin, ymax;
    /*cout << "请输入横坐标最小值" << endl;
    cin >> xmin;
    cout << "请输入横坐标最大值" << endl;
    cin >> xmax;
    cout << "请输入纵坐标最小值" << endl;
    cin >> ymin;
    cout << "请输入纵坐标最大值" << endl;
    cin >> ymax;*/
    xmin = 337.5; xmax = 450; ymin = 0; ymax = 1.1;
    vector<float> rawDate;
    bool arvCurve = false;
    bool GetPoints = false;
    bool outCurve1 = false; bool outCurve2 = false;
    for (int j = 0; j < row - 1; j++)//寻找到曲线最左端中间的像素点
    {
        if ((arvCurve == 0) && (src.at<uchar>(j, 0) == 255))
        {
            linepointU = Point(j, 0);
            cout << linepointU << endl;
            if (arvCurve == 0)
            {
                PoiNumInacol = j;
            }
            arvCurve = 1;
        }
        if ((arvCurve == 1) && (src.at<uchar>(j, 0) == 0))
        {
            arvCurve = 0;
            linewidth = j - PoiNumInacol;
            rawDate.push_back(PoiNumInacol + float(linewidth) / 2);
            startrow = PoiNumInacol + (j - PoiNumInacol) / 2;
            //cout << "啊这" << endl;
            //cout << startrow << endl;
        }
    }
    for (int i = 1; i < col ; i++)//二值化图像的at<Vec3b>会怎么表示？
    {
        int k = 0; int l = 0;
        GetPoints = 0;
        while (!GetPoints)
        {
            if ((arvCurve == 0) && ((src.at<uchar>(startrow + k, i) == 255) || (src.at<uchar>(startrow - l, i) == 255)))
            {
                if (src.at<uchar>(startrow + k, i) == 255)
                {
                    startrow = startrow + k + 1;
                }
                if (src.at<uchar>(startrow - l, i) == 255)
                {
                    startrow = startrow + l - 1;
                }
                arvCurve = 1; k = l = 1;
                //cout << "在线上" << endl;
            }
            if ((outCurve1 == 0) && (arvCurve == 1) && ((src.at<uchar>(startrow + k, i) == 0)))
            {
                linepointD = Point(startrow + k - 1, i);
                //cout << "到底下了" << endl;
                //cout << linepointD.x << endl;
                outCurve1 = 1;
            }
            else if (outCurve1 == 0)
            {
                k += 1;
            }

            if ((outCurve2 == 0) && (arvCurve == 1) && ((src.at<uchar>(startrow - l, i) == 0)))
            {
                linepointU = Point(startrow - l + 1, i);
                //cout << "到上边了" << endl;
                //cout << linepointU.x << endl;
                outCurve2 = 1;
            }
            else if (outCurve2 == 0)
            {
                l += 1;
            }

            if ((arvCurve == 1) && (outCurve2 == 1) && (outCurve1 == 1))
            {
                arvCurve = 0; outCurve1 = 0; outCurve2 = 0; k = l = 0;
                GetPoints = 1;
            }
        }
        rawDate.push_back(linepointU.x + float(linewidth) / 4);
        startrow = linepointU.x + linewidth / 2;
    }

    //cout << row << endl;

    outp.open("D://output2.csv", ios::out | ios::trunc);
    outp << "横坐标" << "," << "纵坐标" << "," << endl;
    for (int i = 0; i < col; i++)
    {
        outp << (xmin + (xmax - xmin) * ((float(i) + 1) / float(col))) << "," << (ymin + (ymax - ymin) * ((row - rawDate[i]) / row)) << "," << endl;
    }
    outp.close();
}

Mat GetCurve::AutoFix(Mat src)
{
    Mat dst;
    Mat structreElement1 = getStructuringElement(MORPH_RECT, Size(3, 3), Point(-1, -1));
    Mat structreElement2 = getStructuringElement(MORPH_RECT, Size(2, 2), Point(-1, -1));
    CvPoint2D32f ROIpoint[4];//src
    Mat gray_src, antiNoise;
    src.copyTo(image1);
    cvtColor(~src, gray_src, CV_RGB2GRAY);
    bilateralFilter(gray_src, antiNoise, 9, 20, 3);
    Mat binImg;
    adaptiveThreshold(antiNoise, binImg, 255, ADAPTIVE_THRESH_MEAN_C, THRESH_BINARY, 15, -2);

    //霍夫变换
    Mat whiteBoard = Mat::zeros(binImg.size(), CV_8UC3);
    Mat whiteBoard2 = whiteBoard;
    vector<Vec4f> plines;
    HoughLinesP(binImg, plines, 1, CV_PI / 180.0, 200, binImg.rows / 1.5, 3);
    Scalar color = Scalar(0, 0, 255);
    for (size_t i = 0; i < plines.size(); i++)
    {
        Vec4f hline = plines[i];
        line(whiteBoard, Point(hline[0], hline[1]), Point(hline[2], hline[3]), color, 1, LINE_AA);
    }

    cvtColor(whiteBoard, dst, CV_RGB2GRAY);
    dilate(dst, dst, structreElement2, Point(-1, -1), 1);
    erode(dst, dst, structreElement1, Point(-1, -1), 1); //erode(dst, dst, structreElement1, Point(-1, -1), 1);

    vector<vector<Point>> contours;
    //储存轮廓
    vector<Vec4i> hierarchy;
    findContours(dst, contours, hierarchy, CV_RETR_LIST, CV_CHAIN_APPROX_NONE);
    //获取轮廓
    Mat linePic = Mat::zeros(binImg.rows, binImg.cols, CV_8UC3);
    for (int index = 0; index < contours.size(); index++) {
        drawContours(linePic, contours, index, Scalar(rand() & 255, rand() & 255, rand() & 255), 1, 8/*, hierarchy*/);
    }
    //寻找面积第二大轮廓
    vector<vector<Point>> polyContours(contours.size());
    Point centerpoint;
    int maxArea = 0, secID = 0, secAR = 0;
    for (int index = 0; index < contours.size(); index++) {
        if (contourArea(contours[index]) > contourArea(contours[maxArea]))
        {
            secID = maxArea, maxArea = index;
        }
        if (contourArea(contours[secID]) > contourArea(contours[secAR]))
        {
            secAR = secID;
        }
        approxPolyDP(contours[index], polyContours[index], 10, true);
    }

    vector<int>  hull;
    convexHull(polyContours[secAR], hull, false);    //检测该轮廓的凸包
    cout <<"polyContours[secAR]:" << polyContours[secAR] << endl;
    //将四个点按照左右上下的顺序排列
    centerpoint = (polyContours[secAR][1] + polyContours[secAR][2] + polyContours[secAR][3] + polyContours[secAR][0]) / 4;
    cout <<"centerpoint:" << centerpoint << endl;
    Point temppoint;
    for (int i = 0; i < 4; i++)
    {
        if ((polyContours[secAR][i].x < centerpoint.x) && (polyContours[secAR][i].y < centerpoint.y))
        {
            temppoint = polyContours[secAR][0];
            polyContours[secAR][0] = polyContours[secAR][i];
            polyContours[secAR][i] = temppoint;
        }
    }
    for (int i = 1; i < 4; i++)
    {
        if ((polyContours[secAR][i].x > centerpoint.x) && (polyContours[secAR][i].y < centerpoint.y))
        {
            temppoint = polyContours[secAR][1];
            polyContours[secAR][1] = polyContours[secAR][i];
            polyContours[secAR][i] = temppoint;
        }

    }
    for (int i = 2; i < 4; i++)
    {
        if ((polyContours[secAR][i].x < centerpoint.x) && (polyContours[secAR][i].y > centerpoint.y))
        {
            temppoint = polyContours[secAR][2];
            polyContours[secAR][2] = polyContours[secAR][i];
            polyContours[secAR][i] = temppoint;
        }
    }
    cout <<"polyContours[secAR]:" << polyContours[secAR] << endl;
    for (int i = 0; i < 4; i++)
    {
        ROIpoint[i].x = polyContours[secAR][i].x;
        ROIpoint[i].y = polyContours[secAR][i].y;
    }
    CvPoint2D32f imgpoint[4];
    imgH = image1.rows; imgW = image1.cols;
    imgpoint[0].x = 0;             //dst top left
    imgpoint[0].y = 0;
    imgpoint[1].x = imgW;  //dst top right
    imgpoint[1].y = 0;
    imgpoint[2].x = 0;             //dst buttom left
    imgpoint[2].y = imgH;
    imgpoint[3].x = imgW;   //dst buttom right
    imgpoint[3].y = imgH;
    CvMat* H = cvCreateMat(3, 3, CV_32F);
    cvGetPerspectiveTransform(ROIpoint, imgpoint, H);
    //对图象进行校正
    IplImage temp = IplImage(image1);
    IplImage* srcImg = &temp;
    IplImage* dstImg = cvCloneImage(srcImg);

    cvWarpPerspective(srcImg, dstImg, H, CV_INTER_LINEAR, cvScalarAll(255));
    Mat M = cvarrToMat(dstImg);
    return M;
}

Mat GetCurve::ManualFix(Mat src, Point2i lu,Point2i lb,Point2i rb)
{
    CvPoint2D32f ROIpoint[4];//src
    CvPoint2D32f imgpoint[4];//dst
    int pointnum = 0;
    int imgH, imgW;
    Mat image1;
    src.copyTo(image1);
    ROIpoint[0].x = lu.x; ROIpoint[2].x = lb.x; ROIpoint[3].x = rb.x; ROIpoint[1].x = rb.x-lb.x+lu.x;
    ROIpoint[0].y = lu.y; ROIpoint[2].y = lb.y; ROIpoint[3].y = rb.y; ROIpoint[1].y = rb.y - lb.y + lu.y;

    imgH = image1.rows; imgW = image1.cols;
    imgpoint[0].x = 0;             //dst top left
    imgpoint[0].y = 0;
    imgpoint[1].x = imgW;  //dst top right
    imgpoint[1].y = 0;
    imgpoint[2].x = 0;             //dst buttom left
    imgpoint[2].y = imgH;
    imgpoint[3].x = imgW;   //dst buttom right
    imgpoint[3].y = imgH;
    CvMat* H = cvCreateMat(3, 3, CV_32F);
    cvGetPerspectiveTransform(ROIpoint, imgpoint, H);
    //对图象进行校正
    IplImage temp = IplImage(image1);
    IplImage* srcImg = &temp;
    IplImage* dstImg = cvCloneImage(srcImg);
    cvWarpPerspective(srcImg, dstImg, H, CV_INTER_LINEAR, cvScalarAll(255));
    Mat M = cvarrToMat(dstImg);
    return M;
}

vector<Point2f> GetCurve::AutoGetCurve(Mat gray) //图像校正后提取曲线的核心代码
{
    Mat src = gray;
    //imshow("Input Image", src);
    Mat dst;
    Point2i linepointU, linepointD;

    int PoiNumInacol, startrow;
    int stepsize;//设置步长
    int linewidth=0;
    int col = src.cols;
    int row = src.rows;

    vector<Point2f> rawDate;
    bool arvCurve = false;
    bool getLeft = false;
    int leftStart = 1; int pointScaned = 0;
    bool GetPoints = false;
    bool outCurve1 = false; bool outCurve2 = false;
    while (!getLeft)
    {
        for (int j = 0; j < row - 1; j++)//寻找到曲线最左端中间的像素点
        {
            if ((arvCurve == 0) && (src.at<uchar>(j, leftStart) == 255))
            {
                linepointU = Point(j, leftStart);
                if (arvCurve == 0)
                {
                    PoiNumInacol = j;
                }
                arvCurve = 1;
            }
            if ((arvCurve == 1) && (src.at<uchar>(j, leftStart) == 0))
            {
                arvCurve = 0;
                getLeft = 1;
                rawDate.push_back(Point2f(leftStart, (PoiNumInacol + float(linewidth) / 2)));
                startrow = PoiNumInacol + (j - PoiNumInacol) / 2;
                //cout << "啊这" << endl;
            }
        }
        leftStart += 1;
    }
    //cout << "找到了左起点" <<","<< rawDate[0]<<","<<linewidth<< endl;

    for (int i = leftStart; i < col; i++)//从最左端开始向右寻找曲线上的点
    {
        if (i < 200) {
            cout << i << "," << startrow << endl;
        }
        int k = 0; int l = 0;
        GetPoints = 0;
        while ((!GetPoints) && (pointScaned <col/3))
        {
            if ((arvCurve == 0) && ((src.at<uchar>(startrow + k, i) == 255) || (src.at<uchar>(startrow - l, i) == 255)))//搜索起始点，起始点下k/上l点是否在线上
            {
                int setStart = 0;
                if (src.at<uchar>(startrow + k, i) == 255)//如果在线上，则将该点设置为起始点
                {
                    startrow = startrow + k; setStart = 1;
                }
                if (!setStart&&( src.at<uchar>(startrow - l, i) == 255))
                {
                    startrow = startrow - l ;
                }
                arvCurve = 1; k = l = 1;
                //cout << "在线上" <<","<< startrow<< endl;
            }

            if ((outCurve1 == 0) && (arvCurve == 1) && ((src.at<uchar>(startrow + k, i) == 0)))
            {
                linepointD = Point(i, startrow + k - 1);
                //cout << "到底下了" << endl;
                outCurve1 = 1;
            }
            else if (outCurve1 == 0)
            {
                k += 1;
            }

            if ((outCurve2 == 0) && (arvCurve == 1) && ((src.at<uchar>(startrow - l, i) == 0)))
            {
                linepointU = Point(i, startrow - l + 1);
                //cout << "到上边了" << endl;
                outCurve2 = 1;
            }
            else if (outCurve2 == 0)
            {
                l += 1;
            }

            if ((startrow + k) >= row-1)
            {
                k = row - startrow-1;
            }
            if ((startrow - l) <= 0+1)
            {
                l = startrow - 1;
            }

            if ((arvCurve == 1) && (outCurve2 == 1) && (outCurve1 == 1))
            {
                arvCurve = 0; outCurve1 = 0; outCurve2 = 0; k = l = 0;
                GetPoints = 1;
                startrow = linepointU.y;
                rawDate.push_back(Point2f(i, linepointU.y));
                break;
            }

            pointScaned += 1;
        }
        pointScaned = 0; k = l = 0;
    }

    float min = 100; float temp; int id = 0;
    for (int i = 0; i < rawDate.size()-1; i++)//寻找差分最小处对应的x坐标（id
    {
        temp =abs( rawDate[i + 1].y - rawDate[i].y);
        if (temp < min)
        {
            min = temp; id = i;
        }
    }
    for (int j = 0; j < row - 1; j++)//获取线宽
    {
        if ((arvCurve == 0) && (src.at<uchar>(j, id) == 255))
        {
            if (arvCurve == 0)
            {
                PoiNumInacol = j;
            }
            arvCurve = 1;
        }
        if ((arvCurve == 1) && (src.at<uchar>(j, id) == 0))
        {
            arvCurve = 0;
            linewidth = j- PoiNumInacol;
        }
    }
    cout <<id<<","<<linewidth << endl;
    for (int i = 0; i < rawDate.size(); i++)//给原始数据加上线宽
    {
        rawDate[i].y = rawDate[i].y + linewidth / 2;
    }
    return rawDate;
}
