#ifndef GETCOLOR_H
#define GETCOLOR_H
#include <opencv2/opencv.hpp>
#include <iostream>
#include <fstream>
#include <opencv/highgui.h>
#include <QDebug>
#include <QImage>
#include <QPixmap>
using namespace cv;
using namespace std;

class Getcolor{
public:
    bool flag = false;
    int xvalue = 0;
    int yvalue = 0;
    Mat image, image1, image2;
    void mousecallback(int event, int x, int y, int flags, void* userdata);
    int colormain(int argc, char* argv[]);
    Mat GetPointColor(Mat src, Point2i pos);
};

#endif // GETCOLOR_H
//Mat AutoFix(Mat rawImg);
//    用于自动校正，输入的是原图像，返回的是校正后的图像，两图像分辨率会有出入
//Mat ManualFix(Max rawImg,Point lu,Point lb,Point rb);
//    用于手动矫正，传入原图像，左上坐标leftup(lu)，左下坐标(lb)，右下坐标(rb)，返回矫正后图像，会有出入。

//Mat GetColor(Mat img,Point pos);pos：点击的坐标
//    用于选取图片颜色，传入之前校正后的图像，以及选到的点的rgb；返回一个只有选取的颜色出现在画面中的新图像
//vector<point> AutoGetCurve(Mat img);
//    用于自动提取曲线数据，传入图像（可能是不经过选取颜色的，也可能是选取过的），之后把对应点在原图中显示。
//    struct Point{
//    Float x;float y;
//像素坐标→物理坐标转换关系式
//    Xmin:x轴最小值,col:图片显示的横向像素数
//    x=xmin+(xmax−xmin)∗(point.x)/col
//    y=ymin+(ymax−ymin)∗(row−point.y)/row
//Void output(vector<Point>);
