#ifndef IMGTRANSPIECEWISEAFFINE_H
#define IMGTRANSPIECEWISEAFFINE_H

#include "deformation/image_deformation_mls.h"

class ImgWarp_PieceWiseAffine : public image_deformation_mls {
 public:
  enum BGFill {
    BGNone,
    BGMLS,
    BGPieceWise
  };

  ImgWarp_PieceWiseAffine(void);
  ~ImgWarp_PieceWiseAffine(void);

  void calcDelta();
  BGFill backGroundFillAlg;
 private:
  Point_<double> getMLSDelta(int x, int y);
};

#endif //IMGTRANSPIECEWISEAFFINE_H