#include "imgwarp_piecewiseaffine.h"
#include "delaunay.h"

#include "highgui.h"

using cv::Point2d;

ImgWarp_PieceWiseAffine::ImgWarp_PieceWiseAffine(void) {
    backGroundFillAlg = BGNone;
}

ImgWarp_PieceWiseAffine::~ImgWarp_PieceWiseAffine(void) {}

Point_<double> ImgWarp_PieceWiseAffine::getMLSDelta(int x, int y) {
    static Point_<double> swq, qstar, newP, tmpP;
    double sw;

    static vector<double> w;
    w.resize(n_point);

    static Point_<double> swp, pstar, curV, curVJ, Pi, PiJ;
    double miu_s;

    int i = x;
    int j = y;
    int k;

    sw = 0;
    swp.x = swp.y = 0;
    swq.x = swq.y = 0;
    newP.x = newP.y = 0;
    curV.x = i;
    curV.y = j;
    for (k = 0; k < n_point; k++) {
        if ((i == oldDotL[k].x) && j == oldDotL[k].y) break;
        /* w[k] = pow((i-oldDotL[k].x)*(i-oldDotL[k].x)+
                 (j-oldDotL[k].y)*(j-oldDotL[k].y), -alpha);*/
        w[k] = 1 / ((i - oldDotL[k].x) * (i - oldDotL[k].x) +
                    (j - oldDotL[k].y) * (j - oldDotL[k].y));
        sw = sw + w[k];
        swp = swp + w[k] * oldDotL[k];
        swq = swq + w[k] * newDotL[k];
    }
    if (k == n_point) {
        pstar = (1 / sw) * swp;
        qstar = 1 / sw * swq;

        // Calc miu_s
        miu_s = 0;
        for (k = 0; k < n_point; k++) {
            if (i == oldDotL[k].x && j == oldDotL[k].y) continue;

            Pi = oldDotL[k] - pstar;
            miu_s += w[k] * Pi.dot(Pi);
        }

        curV -= pstar;
        curVJ.x = -curV.y, curVJ.y = curV.x;

        for (k = 0; k < n_point; k++) {
            if (i == oldDotL[k].x && j == oldDotL[k].y) continue;

            Pi = oldDotL[k] - pstar;
            PiJ.x = -Pi.y, PiJ.y = Pi.x;

            tmpP.x = Pi.dot(curV) * newDotL[k].x - PiJ.dot(curV) * newDotL[k].y;
            tmpP.y = -Pi.dot(curVJ) * newDotL[k].x + PiJ.dot(curVJ) * newDotL[k].y;
            tmpP *= w[k] / miu_s;
            newP += tmpP;
        }
        newP += qstar;
    } else {
        newP = newDotL[k];
    }

    newP.x -= i;
    newP.y -= j;
    return newP;
}

void ImgWarp_PieceWiseAffine::calcDelta() {
    Mat_<int> imgLabel = Mat_<int>::zeros(tar_h, tar_w);

    delta_x = delta_x.zeros(tar_h, tar_w);
    delta_y = delta_y.zeros(tar_h, tar_w);
    for (int i = 0; i < this->n_point; i++) {
        //! Ignore points outside the target image
        if (oldDotL[i].x < 0) oldDotL[i].x = 0;
        if (oldDotL[i].y < 0) oldDotL[i].y = 0;
        if (oldDotL[i].x >= tar_w) oldDotL[i].x = tar_w - 1;
        if (oldDotL[i].y >= tar_h) oldDotL[i].y = tar_h - 1;

        delta_x(oldDotL[i]) = newDotL[i].x - oldDotL[i].x;
        delta_y(oldDotL[i]) = newDotL[i].y - oldDotL[i].y;
    }
    delta_x(0, 0) = delta_y(0, 0) = 0;
    delta_x(tar_h - 1, 0) = delta_y(0, tar_w - 1) = 0;
    delta_y(tar_h - 1, 0) = delta_y(tar_h - 1, tar_w - 1) = src_h - tar_h;
    delta_x(0, tar_w - 1) = delta_x(tar_h - 1, tar_w - 1) = src_w - tar_w;

    vector<Triangle> V;
    vector<Triangle>::iterator it;
    cv::Rect_<int> boundRect(0, 0, tar_w, tar_h);
    vector<Point_<double> > oL1 = oldDotL;
    if (backGroundFillAlg == BGPieceWise) {
        oL1.push_back(Point2d(0, 0));
        oL1.push_back(Point2d(0, tar_h - 1));
        oL1.push_back(Point2d(tar_w - 1, 0));
        oL1.push_back(Point2d(tar_w - 1, tar_h - 1));
    }
    // In order preserv the background
    V = ::delaunayDiv(oL1, boundRect);

    //     vector< TriangleInID > Vt;
    // //     vector< Triangle >::iterator it;
    // //     cv::Rect_<int> boundRect(0, 0, tar_w, tar_h);
    //     Vt = ::delaunayDivInID(oldDotL, boundRect);
    Mat_<uchar> imgTmp = Mat_<uchar>::zeros(tar_h, tar_w);
    for (it = V.begin(); it != V.end(); it++) {
        cv::line(imgTmp, it->v[0], it->v[1], 255, 1, CV_AA);
        cv::line(imgTmp, it->v[0], it->v[2], 255, 1, CV_AA);
        cv::line(imgTmp, it->v[2], it->v[1], 255, 1, CV_AA);

        // Not interested in points outside the region.
        if (!(it->v[0].inside(boundRect) && it->v[1].inside(boundRect) &&
              it->v[2].inside(boundRect)))
            continue;

        cv::fillConvexPoly(imgLabel, t->v, 3,
                           cv::Scalar_<int>(it - V.begin() + 1));
    }
    // imshow("imgTmp", imgTmp);
    // cvWaitKey(10);

    int i, j;

    Point_<int> v1, v2, curV;

    for (i = 0;; i += grid_size) {
        if (i >= tar_w && i < tar_w + grid_size - 1)
            i = tar_w - 1;
        else if (i >= tar_w)
            break;
        for (j = 0;; j += grid_size) {
            if (j >= tar_h && j < tar_h + grid_size - 1)
                j = tar_h - 1;
            else if (j >= tar_h)
                break;
            int tId = imgLabel(j, i) - 1;
            if (tId < 0) {
                if (backGroundFillAlg == BGMLS) {
                    Point_<double> dV = getMLSDelta(i, j);
                    delta_x(j, i) = dV.x;
                    delta_y(j, i) = dV.y;
                } else {
                    delta_x(j, i) = -i;
                    delta_y(j, i) = -j;
                }
                continue;
            }
            v1 = V[tId].v[1] - V[tId].v[0];
            v2 = V[tId].v[2] - V[tId].v[0];
            curV.x = i, curV.y = j;
            curV -= V[tId].v[0];

            double d0, d1, d2;
            d2 = double(v1.x * curV.y - curV.x * v1.y) /
                 (v1.x * v2.y - v2.x * v1.y);
            d1 = double(v2.x * curV.y - curV.x * v2.y) /
                 (v2.x * v1.y - v1.x * v2.y);
            d0 = 1 - d1 - d2;
            delta_x(j, i) = d0 * delta_x(V[tId].v[0]) + d1 * delta_x(V[tId].v[1]) +
                        d2 * delta_x(V[tId].v[2]);
            delta_y(j, i) = d0 * delta_y(V[tId].v[0]) + d1 * delta_y(V[tId].v[1]) +
                        d2 * delta_y(V[tId].v[2]);
        }
    }
}
