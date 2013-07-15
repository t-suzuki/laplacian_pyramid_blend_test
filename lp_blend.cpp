#include <iostream>
#include <cv.h>
#include <highgui.h>

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

int main(int argc, char* argv[]) {
  cv::Mat org_8u = cv::imread("lena.jpg");
  cv::Mat org;
  org_8u.convertTo(org, CV_32FC1, 1/255.0f);

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
