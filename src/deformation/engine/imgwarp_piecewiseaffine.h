#ifndef IMGTRANSPIECEWISEAFFINE_H
#define IMGTRANSPIECEWISEAFFINE_H

#include "deformation/image_deformation_mls.h"

class ImgWarp_PieceWiseAffine :
	public image_deformation_mls
{
public:
    //! How to deal with the background.
    /*!
        BGNone: No background is reserved.
        BGMLS: Use MLS to deal with the background.
        BGPieceWise: Use the same scheme for the background.
    */
    enum BGFill {
			BGNone, //! No background is reserved.
            BGMLS,  //! Use MLS to deal with the background.
			BGPieceWise}; //! Use the same scheme for the background.
    
	ImgWarp_PieceWiseAffine(void);
	~ImgWarp_PieceWiseAffine(void);

    void calcDelta();
    BGFill backGroundFillAlg;
private:
    Point_<double> getMLSDelta(int x, int y);
};

#endif //IMGTRANSPIECEWISEAFFINE_H