#include <getcolor.h>

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
    //Mat kernelp = getStructuringElement(MORPH_RECT, Size(7, 7), Point(-1, -1));
    //morphologyEx(maskp, maskp, CV_MOP_CLOSE, kernelp);
    return maskp;
}
