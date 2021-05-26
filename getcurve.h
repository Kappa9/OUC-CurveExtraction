#ifndef GETCURVE_H
#define GETCURVE_H
//一个在PC端实现提取论文中的曲线的程序
#include <opencv2\opencv.hpp>
#include <iostream>
#include <vector>
#include <fstream>

using namespace cv;
using namespace std;
class GetCurve{
public:
    bool flag = false;
    bool isauto = true;
    CvPoint2D32f ROIpoint[4];//src
    CvPoint2D32f imgpoint[4];//dst
    int pointnum = 0;
    int imgH, imgW;
    Mat image, image1, image2;
    Mat element = getStructuringElement(MORPH_RECT,Size(3, 2));//闭运算参数

    bool findNextPoint(vector<Point>& _neighbor_points, Mat& _image, Point _inpoint, int flag, Point& _outpoint, int& _outflag);
    //寻找图像上的第一个点
    bool findFirstPoint(Mat& _inputimg, Point& _outputpoint);
    //寻找曲线
    void findLines(Mat& _inputimg, vector<deque<Point>>& _outputlines);
    //随机取色 用于画线的时候
    Scalar random_color(RNG& _rng);

    void autoProcess(Mat src);
    //仿射变换实现
    Mat PicetureFix(Mat& img);
    void mousecallback(int event, int x, int y, int flags, void* xcalue );
    //void denoisebinfilter(Mat& srcimage);
    //单通道、二值化后的图像
    void chao_thinimage(Mat& srcimage);
    //图像校正后提取曲线的核心代码
    void getcurve(Mat gray);
    int curvemain(int argc, char* argv[]);

    //使用接口
    Mat AutoFix(Mat src);
    Mat ManualFix(Mat src, Point2i lu,Point2i lb,Point2i rb);
    vector<Point2f> AutoGetCurve(Mat gray, bool isLinesptm);
};
#endif // GETCURVE_H
