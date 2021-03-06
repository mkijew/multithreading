#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>

#include <cmath>
#include <iostream>
#include <iomanip>
#include <mutex>
#include <future>
#include <thread>

void display_mat( const cv::Mat& mat )
{
    std::cout << "rows: " << mat.rows << std::endl;
    std::cout << "cols: " << mat.cols << std::endl;

    for ( int i = 0; i < mat.rows; ++i )
    {
        std::cout << "[" << std::setw(2) << i << "] > ";

        for ( int k = 0; k < mat.cols; ++k )
        {
            // std::cout << (int)(mat.data[i]) << " ";
            std::cout << (int)(mat.at<unsigned char>(i,k)) << " ";
        }

        std::cout << std::endl;
    }
}

cv::Mat createMatLineAlpha( const cv::Size& size, int iAlpha )
{
    cv::Mat mat(size, CV_8U);
    cv::Point origin( size.width / 2, size.height / 2 );
    double dRad = ( iAlpha * M_PI ) / 180.0;

    std::cout << "origin x " << origin.x << std::endl;
    std::cout << "origin y " << origin.y << std::endl;

    if ( iAlpha != 90 )
    {
        for( int iCol = 0; iCol < size.width; ++iCol )
        {
            double dValue = std::tan(dRad) * ( iCol - origin.x );
            int iPos = origin.y - std::lround(dValue);
            std::cout << "iCol " << iCol << " dValue " << dValue << " std::lround((dValue) " << std::lround(dValue) << " iPos " << iPos << std::endl;

            if ( iPos >= 0 && iPos < size.height )
            {
                mat.at<unsigned char>( iPos , iCol) = 1;
            }
        }
    }
    else
    {
        for ( int iRow = 0; iRow < size.height; ++iRow )
        {
            mat.at<unsigned char>(iRow, origin.x) = 1;
        }
    }

    return mat;
}

cv::Mat createMatLineAngle( const cv::Size& size, double angle )
{
    std::cout << "createMatLineAngle()" << std::endl;

    cv::Mat src(size, CV_8U);
    cv::Mat dst(size, CV_8U);

    cv::Point origin( size.width / 2, size.height / 2 );

    std::cout << "origin x " << origin.x << std::endl;
    std::cout << "origin y " << origin.y << std::endl;

    // draw line on src
    cv::line( src,
            cv::Point( 0, size.height / 2 ),
            cv::Point( size.width - 1, size.height / 2 ),
            1,
            1
            );
    // cv::Mat line = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(size.width, 1));

    display_mat(src);

    cv::Mat rot = cv::getRotationMatrix2D(origin, angle, 1.0 );
    cv::warpAffine( src, dst, rot, size );
    return dst;
}

int main( int argc, char** argv )
{
    int horizontal_cols = 1024;
    int horizontal_size_value = horizontal_cols / 30;
    cv::Size horSize(horizontal_size_value, 1);

    std::cout << "height: " << horSize.height << std::endl;
    std::cout << "width: " << horSize.width << std::endl;

    cv::Mat imgStructure = cv::getStructuringElement(cv::MORPH_RECT, horSize);
    display_mat(imgStructure);

    cv::Mat imgStructure2 = cv::getStructuringElement(cv::MORPH_CROSS, cv::Size(7, 7));
    display_mat(imgStructure2);

    std::cout << "------------------------" << std::endl;

    cv::Mat imgLine = createMatLineAlpha( cv::Size(7, 7), 45 );
    display_mat(imgLine);

    cv::Mat imgLine2 = createMatLineAngle( cv::Size(7, 7), 45.0 );
    display_mat(imgLine2);

    std::cout << "------------------------" << std::endl;

    cv::Mat imgLine3 = createMatLineAlpha( cv::Size(21, 21), 135 );
    display_mat(imgLine3);

    cv::Mat imgLine4 = createMatLineAngle( cv::Size(7, 7), 45.0 );
    display_mat(imgLine4);

    return 0;
}