#include <iostream>
#include <vector>
#include <cv.h>
#include <highgui.h>

bool GaussianPyramid(const cv::Mat& src, std::vector<cv::Mat>& result, int level) {
  result.clear();
  cv::Mat tmp = src;
  cv::Mat down;
  for (int i=0; i<level; ++i) {
    result.push_back(tmp);
    if (tmp.cols % 2 != 0 || !(tmp.cols / 2 >= 1)
        || tmp.rows % 2 != 0 || !(tmp.rows / 2 >= 1))
      return false;
    cv::pyrDown(tmp, down, cv::Size(tmp.cols/2, tmp.rows/2));
    tmp = down;
  }
  result.push_back(down);
  return true;
}

bool LaplacianPyramid(const cv::Mat& src, std::vector<cv::Mat>& result, int level) {
  result.clear();
  cv::Mat tmp = src;
  cv::Mat down, down_up;
  for (int i=0; i<level; ++i) {
    if (tmp.cols % 2 != 0 || !(tmp.cols / 2 >= 1)
        || tmp.rows % 2 != 0 || !(tmp.rows / 2 >= 1))
      return false;
    cv::pyrDown(tmp, down, cv::Size(tmp.cols/2, tmp.rows/2));
    cv::pyrUp(down, down_up, cv::Size(tmp.cols, tmp.rows));
    cv::Mat lap = tmp - down_up;
    result.push_back(lap);
    tmp = down;
  }
  result.push_back(down);
  return true;
}

bool InverseLaplacianPyramid(std::vector<cv::Mat>& src, cv::Mat& result) {
  if (src.size() == 0)
    return false;

  cv::Mat up;
  cv::Mat reconstruct = src[src.size()-1];
  for (unsigned int i=src.size()-1; i>=1; --i) {
    cv::pyrUp(reconstruct, up, cv::Size(src[i].cols*2, src[i].rows*2));
    reconstruct = src[i-1] + up;
  }
  result = reconstruct;

  return true;
}

cv::Mat imread_32f(const std::string& name, int flag=1) {
  cv::Mat org_8u = cv::imread(name, flag);
  cv::Mat org;
  org_8u.convertTo(org, CV_32FC1, 1/255.0f);
  return org;
}

int test_laplacian_pyramid(int argc, char* argv[]) {
  cv::Mat org = imread_32f(argv[1]);

  int div = 2;
  cv::Mat pic(org.rows/div, org.cols/div, CV_32FC3);
  cv::resize(org, pic, cv::Size(pic.cols, pic.rows));

  std::vector<cv::Mat> result;
  LaplacianPyramid(pic, result, 5);

  cv::imshow("in", pic);
  for (unsigned int i=0; i<result.size(); ++i) {
    cv::imshow("out", result[i]);
    cv::waitKey(0);
  }

  cv::Mat reconstruct;
  InverseLaplacianPyramid(result, reconstruct);
  cv::imshow("reconstruct", reconstruct);
  cv::waitKey(0);

  return 0;
}

int test_blend(int argc, char* argv[], bool verbose) {
  // see the lecture note:
  // http://graphics.cs.cmu.edu/courses/15-463/2005_fall/www/Lectures/Pyramids.pdf

  cv::Mat im1 = imread_32f(argv[1]);
  cv::Mat im2 = imread_32f(argv[2]);
  cv::Mat blend = imread_32f(argv[3]);

  int level = 4;
  std::vector<cv::Mat> lap1, lap2, gaussian_blend;
  LaplacianPyramid(im1, lap1, level);
  LaplacianPyramid(im2, lap2, level);
  GaussianPyramid(blend, gaussian_blend, level);

  std::vector<cv::Mat> lap_blended;
  for (int i=0; i<level+1; ++i) {
    cv::Mat res = lap1[i].mul(cv::Scalar(1.0f, 1.0f, 1.0f) - gaussian_blend[i]) + lap2[i].mul(gaussian_blend[i]);
    lap_blended.push_back(res);
    if (verbose) {
      cv::imshow("lap1", lap1[i]);
      cv::imshow("lap2", lap2[i]);
      cv::imshow("gaussian_blend", gaussian_blend[i]);
      cv::imshow("res", res);
      cv::waitKey(0);
    }
  }

  cv::Mat blended;
  InverseLaplacianPyramid(lap_blended, blended);

  cv::imshow("blended", blended);
  cv::waitKey(0);

  return 0;
}

int main(int argc, char* argv[]) {
  using namespace std;

  cout 
    << "Laplacian Pyramid Blending test." << endl
    << "usage: ./lp_blend demo_mode ... " << endl
    << "demo_mode:" << endl
    << "  0 - ./lp_blend 0 image_file : test Laplacian pyramid" << endl
    << "  1 - ./lp_blend 1 image_file1 image_file2 blend_file : blend two images" << endl
    ;

  bool verbose = false;
  int demo_mode = 0;
  if (argc >= 1)
    demo_mode = atoi(argv[1]);

  switch (demo_mode) {
    case 0: test_laplacian_pyramid(argc-1, argv+1); break;
    case 1: test_blend(argc-1, argv+1, verbose); break;
  }

  return 0;
}
