#pragma once
#include "opencv2/opencv.hpp"
#include <cmath>
#include <algorithm>
#include <vector>

// #define MAS_INFO false

// background
#define BACKGROUND_CALC_FRAME 1
#define BACKGROUND_FUSION 0.90
#define BACKGROUND_INITIAL "../background.bmp"

// segment
#define MAX_Area 15000  //前景物体大小阈值
#define MIN_Area 1000		
#define LEARNING_RATE 0.005f
// #define SEG_ROJO_AZUL true


// // mostrar
// #define DELAY_EVENTOS 10
// #define MOSTRAR_RESULTADO true
// #define CANTIDAD_EVENTOS 30
// #define EVENTOS_SEGUNDOS true


// videos
#define SOURCE_VIDEO "../vtest.avi"
#define RESULT_VIDEO "result.avi"
#define RATIO 2
#define FPS 30

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

// void FillObjects2(const Mat &src, vector< BlobCenter > &blobs, vector< vector< Mat > > &Objects);
// void reemplaza(Mat &fondo, Mat &imagen);
// void mostrar(const vector <pair <vector<vector <int> > , vector<Mat> > > &Objects2, const Mat &fondo, int ventana, VideoWriter &outputVideo, int FPS);
// void seleccionar(const vector <pair <vector<vector <int> > , vector<Mat> > > &Objects,vector <pair <vector<vector <int> > , vector<Mat> > > &Objects_aux);

