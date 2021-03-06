#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>

#include <iostream>
#include <mutex>
#include <future>
#include <thread>

void show_wait_destroy(const char* winname, const cv::Mat& img) {
    cv::imshow(winname, img);
    cv::moveWindow(winname, 500, 0);
    cv::waitKey(0);
    cv::destroyWindow(winname);
}

cv::Mat createMatLineAlpha( const cv::Size& size, int iAlpha )
{
    cv::Mat mat(size, CV_8U);
    cv::Point origin( size.width / 2, size.height / 2 );
    double dRad = ( iAlpha * M_PI ) / 180.0;

    if ( iAlpha != 90 )
    {
        for( int iCol = 0; iCol < size.width; ++iCol )
        {
            double dValue = std::tan(dRad) * ( iCol - origin.x );
            int iPos = origin.y - std::lround(dValue);

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

auto lineMatchFunction = [](const cv::Mat& bw_image, const cv::Size& size, int alpha)
{
    // Create structure element for extracting lines through morphology operations
    cv::Mat imgStructure = createMatLineAlpha(size, alpha);
    cv::Mat imgLocal = bw_image.clone();

    // Apply morphology operations
    cv::erode(imgLocal, imgLocal, imgStructure, cv::Point(-1, -1));
    cv::dilate(imgLocal, imgLocal, imgStructure, cv::Point(-1, -1));

    return imgLocal;
};

int main( int argc, char** argv )
{
    cv::CommandLineParser parser(argc, argv, "{@input | notes.png | input image}");
    cv::Mat src = cv::imread( cv::samples::findFile( parser.get<cv::String>("@input") ), cv::IMREAD_COLOR);
    if (src.empty())
    {
        std::cout << "Could not open or find the image!\n" << std::endl;
        std::cout << "Usage: " << argv[0] << " <Input image>" << std::endl;
        return -1;
    }

    // Show source image
    cv::imshow("src", src);

    // Transform source image to gray if it is not already
    cv::Mat gray;
    if (src.channels() == 3)
    {
        cvtColor(src, gray, cv::COLOR_BGR2GRAY);
    }
    else
    {
        gray = src;
    }

    // Show gray image
    show_wait_destroy("gray", gray);

    // Apply adaptiveThreshold at the bitwise_not of gray, notice the ~ symbol
    cv::Mat bw;
    cv::adaptiveThreshold(~gray, bw, 255, cv::ADAPTIVE_THRESH_MEAN_C, cv::THRESH_BINARY, 15, -2);
    
    // Show binary image
    show_wait_destroy("binary", bw);

    // Create the images that will use to extract the angle lines
    int angles[] = { 0, 30, 45, 60, 90, 135, 150 };
    const int angCnt = sizeof(angles) / sizeof(angles[0]);

    // Specify size on horizontal and vertical
    int horizontal_size_value = bw.cols / 30;
    int vertical_size_value = bw.rows / 30;
    cv::Size matchSize(horizontal_size_value, vertical_size_value);

    // start lines finding ...
    std::array< std::future<cv::Mat>, angCnt > futureResults;
    for ( int i = 0; i < angCnt; ++i )
    {
        futureResults[i] = std::async( std::launch::async, lineMatchFunction, std::cref(bw), std::cref(matchSize), angles[i]);
    }

    // collect results
    std::array< cv::Mat, angCnt > matResults;
    for ( int i = 0; i < angCnt; ++i )
    {
        matResults[i] = futureResults[i].get();
    }

    // display partial results: 0, 30, 45, 60, 90, 135, 150
    for ( int i = 0; i < angCnt; ++i )
    {
        std::stringstream ss;
        ss << "Angle_" <<  angles[i];
        show_wait_destroy( ss.str().c_str(), matResults[i] );
    }

    // be sure that we have at least 7 lines ...
    assert( matResults.size() >= 7 );

    // display results on one image
    cv::Mat display = src.clone();
    display.setTo(cv::Scalar(0,0,255), matResults[0]); // red
    display.setTo(cv::Scalar(0,255,0), matResults[1]); // green
    display.setTo(cv::Scalar(255,0,0), matResults[2]); // blue

    display.setTo(cv::Scalar(0,255,255), matResults[3]);  // yellow
    display.setTo(cv::Scalar(255,255,0), matResults[4]);  // cyan
    display.setTo(cv::Scalar(255,0,255), matResults[5]); // magenta

    display.setTo(cv::Scalar(128,128,255), matResults[6]); // pink

    show_wait_destroy("result", display);

    return 0;
}