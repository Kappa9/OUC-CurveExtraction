#include <getcolor.h>

void Getcolor::mousecallback(int event, int x, int y, int flags, void* userdata)
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
            xvalue = x;
            yvalue = y;
            flag = 0;
            int b = image1.at<Vec3b>(yvalue, xvalue)[0];
            int g = image1.at<Vec3b>(yvalue, xvalue)[1];
            int r = image1.at<Vec3b>(yvalue, xvalue)[2];

            cout << "X: " << xvalue << "Y: " << yvalue << " B:" << b << ' ' << "G:" << g << ' ' << "R:" << r << endl;
        }
    }
    break;
    }
}

int Getcolor::colormain(int argc, char* argv[])
{
    namedWindow("imageshow", 0);
    Mat image = imread("E:\GetDATA\GetCurve\test.png", 1);
    if (!image.data)
    {
        cout << "the image is error" << endl;
        return 1;
    }
    imshow("imageshow", image);
    image.copyTo(image1);
    //cv::setMouseCallback("imageshow", mousecallback, 0);

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
    waitKey(0);
    return 0;

}
Mat Getcolor::GetPointColor(Mat src, Point2i pos)
{
    Mat hsvp;
    int h, s, v;
    cvtColor(src, hsvp, COLOR_RGB2HSV);
    h = static_cast<int>(hsvp.at<Vec3b>(pos.y, pos.x)[0]);
    s = static_cast<int>(hsvp.at<Vec3b>(pos.y, pos.x)[1]);
    v = static_cast<int>(hsvp.at<Vec3b>(pos.y, pos.x)[2]);

    Mat dstp = Mat::zeros(hsvp.size(), hsvp.type());
    Mat maskp;
    //避免超出0~255
    int hmin = h - 20;
    if (hmin < 0) { hmin = 0; }
    int hmax = h + 20;
    if (hmax > 255) { hmax = 255; }
    int smin = s - 30;
    if (smin < 0) { smin = 0; }
    int vmin = v - 30;
    if (vmin < 0) { vmin = 0; }
    //
    inRange(hsvp, Scalar(hmin, smin, vmin), Scalar(hmax, 255, 255), maskp);
    for (int r = 0; r < src.rows; r++)
    {
        for (int c = 0; c < src.cols; c++)
        {
            if (maskp.at<uchar>(r, c) == 255)
            {
                dstp.at<Vec3b>(r, c)[0] = src.at<Vec3b>(r, c)[0];
                dstp.at<Vec3b>(r, c)[1] = src.at<Vec3b>(r, c)[1];
                dstp.at<Vec3b>(r, c)[2] = src.at<Vec3b>(r, c)[2];
            }
        }
    }
    Mat kernelp = getStructuringElement(MORPH_RECT, Size(7, 7), Point(-1, -1));
    morphologyEx(maskp, maskp, CV_MOP_CLOSE, kernelp);
    return maskp;
}
