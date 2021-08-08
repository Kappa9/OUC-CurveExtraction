#include<getcurve.h>

//一个在PC端实现提取论文中的曲线的程序
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

vector<Point2f> GetCurve::AutoGetCurve(Mat gray, bool isLinesptm) //图像校正后提取曲线的核心代码
{
    Mat src = gray;
    Mat dst;
    Point2i linepointU, linepointD;

    int PoiNumInacol, startrow;
    int stepsize;//设置步长
    int linewidth = 0;
    int col = src.cols;
    int row = src.rows;

    vector<Point2f> rawDate;//存放线上点数据
    float midPos1, midPos2;//存放中点数据
    vector<Point2f> diff;//存放差分数据

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
                midPos1=startrow;
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
        while ((!GetPoints) && (pointScaned <col))
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

                midPos2=(linepointD.y + linepointU.y)/2;
                if (isLinesptm&&( midPos2 < midPos1))
                {
                    rawDate.push_back(Point2f(i, linepointU.y));
                    midPos1 = midPos2;
                }
                if (isLinesptm && (midPos2 > midPos1))
                {
                    rawDate.push_back(Point2f(i, linepointD.y));
                    midPos1 = midPos2;
                }
                if (!isLinesptm)
                {
                    rawDate.push_back(Point2f(i, linepointU.y));
                }

                break;
            }

            pointScaned += 1;
        }
        pointScaned = 0; k = l = 0;
    }

    if (isLinesptm)
    {
        return rawDate;
    }
    float min = 100; float temp; int id = 0;

    diff.push_back(Point2f(rawDate[0].x, 0));//给第一个位置的差分赋0值
    for (int i = 0; i < rawDate.size()-1; i++)//寻找差分最小处对应的x坐标（id
    {

        temp =abs( rawDate[i + 1].y - rawDate[i].y);
        diff.push_back(Point2f(rawDate[i+1].x, temp));//给第一个位置的差分赋0值
        if (temp <= min)
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
    for (int i = 1; i < rawDate.size(); i++)//给原始数据加上线宽
    {
        rawDate[i].y = rawDate[i].y-0.5 + 0.5*linewidth*sqrt((diff[i].x- diff[i-1].x) * (diff[i].x - diff[i - 1].x) +diff[i].y* diff[i].y)/ (diff[i].x - diff[i - 1].x);
    }
    return rawDate;
}
