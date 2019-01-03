#pragma once
#include "opencv2/opencv.hpp"
#include <cmath>
#include <algorithm>
#include <vector>

#define MAS_INFO false

// background
#define BACKGROUND_CALC_FRAME 1
#define BACKGROUND_FUSION 0.90
#define BACKGROUND_INITIAL "../background.bmp"

// segment
#define MAX_AREA 15000  //前景物体大小阈值
#define MIN_AREA 20		
#define LEARNING_RATE 0.005f

// synthesis
#define DELAY_EVENTS 20
#define EVENTS_NUM 30

// videos
#define SOURCE_VIDEO "../video.mp4"
#define RESULT_VIDEO "result.avi"
#define RATIO 2

using namespace cv;
using namespace std;

struct BlobCenter
{
	vector<Point2i> blob;
	Point2i center;	
	int id;
	int frame;
};

void findConvexHull(const Mat front_mask, const Mat frame, Mat &output, Mat &hull_mask);
void extractBackground(const Mat src_frame, const Mat front_mask, Mat &background);
void Blob(const Mat img, vector<BlobCenter> &blobs_out, int frame);
void FindBlobs(const Mat binary, vector<vector<Point2i>> &blobs);

void generateLableVec(const int id1, const int id2, const double min_dist_centers, vector<vector<double>> &label_vec);
void findCorrespondence(const vector<BlobCenter> b1, const vector<BlobCenter> b2, vector<vector<double>> &label_vec);
void addLabelToObject(vector<BlobCenter> &blobs, vector<vector<double>> &label_vec, int &next_id);
void paintBlobs(const vector <BlobCenter> blobs, Mat &output, double time);
void FillObjects(const Mat &src, vector< BlobCenter > &blobs, vector <pair <vector<vector <int> > , vector<Mat> > > &Objects);

void select(const vector<pair<vector<vector<int>>, vector<Mat>>> Objects, vector<pair<vector<vector<int>>, vector<Mat>>> &Objects_aux);
void relocate(Mat &background, Mat &image);
void synthesis(vector<pair<vector<vector<int>>, vector<Mat>>> Objects, const Mat backgroud, VideoWriter &outputVideo, const int FPS);


