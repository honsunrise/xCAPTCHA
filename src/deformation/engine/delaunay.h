#include <opencv2/opencv.hpp>
#include <algorithm>
#include <cstdlib>
#include <map>
#include <set>
#include <vector>
using cv::Point2i;
using cv::Point_;
using cv::Vec6f;
using std::vector;
using std::set;

struct Triangle {
  Point2i v[3];
};

inline bool operator<(const Triangle &t1, const Triangle &t2) {
  for (int i = 0; i < 3; i++) {
    if ((t1.v[i].x < t2.v[i].x) || (t1.v[i].x == t2.v[i].x && t1.v[i].y < t2.v[i].y))
      return true;
    if (t1.v[i].x > t2.v[i].x)
      return false;
    if (t1.v[i].x == t2.v[i].x && t1.v[i].y > t2.v[i].y)
      return false;
  }
  return false;
}

template<typename T>
vector<Triangle> delaunayDiv(const vector<Point_<T> > &vP, const cv::Rect &boundRect) {
  cv::Subdiv2D subdiv(boundRect);

  for (size_t e = 0; e < vP.size(); e++) {
    subdiv.insert(vP[e]);
  }

  set<Triangle> V;
  vector<Vec6f> triangleList;
  subdiv.getTriangleList(triangleList);

  for (const auto &t : triangleList) {
    Triangle tr;
    tr.v[0] = Point(cvRound(t[0]), cvRound(t[1]));
    tr.v[1] = Point(cvRound(t[2]), cvRound(t[3]));
    tr.v[2] = Point(cvRound(t[4]), cvRound(t[5]));
    V.insert(tr);
  }

  vector<Triangle> ans;
  ans.resize(V.size());
  std::copy(V.begin(), V.end(), ans.begin());
  return ans;
}