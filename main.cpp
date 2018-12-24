#include "opencv2/opencv.hpp"
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <sstream>
#include "Functions.h"

using namespace cv;
using namespace std;

int main()
{		
	//取出源视频参数，写入结果视频 
	char fileName[100] = SOURCE_VIDEO;
	VideoCapture stream(fileName);
	Size video_size = Size((int)stream.get(CV_CAP_PROP_FRAME_WIDTH) / RATIO, (int)stream.get(CV_CAP_PROP_FRAME_HEIGHT)  /RATIO);
	int codec = static_cast<int>(stream.get(CV_CAP_PROP_FOURCC));
	double frame_rate = stream.get(CV_CAP_PROP_FPS);
	VideoWriter outputVideo(RESULT_VIDEO, codec, frame_rate, video_size, true);
	 
	Ptr< BackgroundSubtractor> pMOG2;
	pMOG2 = createBackgroundSubtractorMOG2();
	Mat frame, src_frame, preprocessed_frame;

	Mat front_mask;			//前景的mask
	Mat morpho_element;		//形态学算子
	int morph_size = 2;		//设置形态学算子大小
	Mat image_convexhull;	//移动物体的凸包在src_frame上的显示
	
	int background_frames_for_calc = frame_rate * BACKGROUND_CALC_FRAME; //计算背景所用的帧数
	int frame_num = 0;		//经过的帧数
	Mat background;			//背景

	vector<BlobCenter> blobs, blobs_old;	//前景物体集合
	int ratio = 2, next_id = 1;
	bool flag = true;
	vector<vector<double>> etiquetas;
	vector <pair<vector<vector<int>>, vector<Mat>>> Objects, Objects_copia;	

	Mat colored_output; 
	
	while (true) 
	{   
		stream.read(frame);
		if(frame.empty())
		{
			std::cout<<"no video frame"<<std::endl;
			break;
		}
		src_frame = frame.clone();
		imshow("src", src_frame);
		
		//preprocess
		GaussianBlur(src_frame, preprocessed_frame, Size(9, 9), 0, 0);
		
		//bgs
		pMOG2->apply(preprocessed_frame, front_mask, LEARNING_RATE);
		GaussianBlur(front_mask, front_mask, Size(9, 9), 0, 0);
		threshold(front_mask, front_mask, 200, 255,0 );
		morpho_element = getStructuringElement(cv::MORPH_RECT, Size( 2 * morph_size + 1, 2 * morph_size + 1));
		morphologyEx(front_mask, front_mask, cv::MORPH_OPEN, morpho_element);
		morphologyEx(front_mask, front_mask, cv::MORPH_CLOSE, morpho_element);
		// imshow("mask", front_mask);

		//找凸包
		Mat hull_mask = Mat::zeros(src_frame.size(), CV_8UC1);
		findConvexHull(front_mask, src_frame, image_convexhull, hull_mask);
		// imshow("hull", image_convexhull);
		imshow("hull_mask", hull_mask);

		//提取出移动物体，并筛选出目标物体存储在blobs中, 物体所在的帧数为frame_size
		/*blobs是vector<BlobCenter>， BlobCenter结构如下
		BlobCenter
		{
			vector<Point2i> blob;
			Point2i center;	
			int id;
			int frame;
		}
		*/
		Blob(hull_mask, blobs, frame_num);

		//get background（有点小问题）
		// if (!(frame_num % background_frames_for_calc))
		// {
		// 	if(frame_num == 0) background = Mat::zeros(src_frame.size(), CV_8UC3);
		// 	else extractBackground(src_frame, hull_mask, background);
		// }
		// imshow("background", background_temp);

		/*****************************************利用物体轨迹进行轨迹重组**********************************/
		//效果特别差，等待完善！！！
		// if(flag && (blobs.size() > 0) )
		// {
		// 	flag = false;
		// 	for(int t=0; t < blobs.size(); t++)
		// 	{
		// 		blobs[t].id = next_id;
		// 		next_id++;
		// 	}
		// }
		// else if(blobs.size() > 0)
		// {
		// 	correspondencias_id(blobs_old, blobs, etiquetas);
		// 	reEtiquetado(blobs, etiquetas, next_id);

		// 	colored_output = Mat::zeros(src_frame.size(), CV_8UC3);
		// 	double time = frame_num / frame_rate;
		// 	paintBlobs(blobs, colored_output, time);
		// 	imshow("colored_out", colored_output);
		// }

		// if(blobs.size() > 0)
		// {
		// 	FillObjects(src_frame, blobs, Objects);
		// }
		// imshow("blobs", src_frame);
		
		// blobs_old = blobs;
		// blobs.clear();
		// etiquetas.clear();
		// frame_num++;
		if(cvWaitKey(30) > 0) break;
	}

	/************************************暂时不知道具体作用************************************/			
	/*
	Objects_copia = Objects;		
	Objects.clear();				
	for(int i=0; i<Objects_copia.size(); i++)
	{
		if(Objects_copia[i].second.size() > 10)
		{
			Objects.push_back(Objects_copia[i]);
			
		}
	}
	Mat fondo = imread(FONDO_NOMBRE,3);

#if EVENTOS_SEG
	vector <pair < vector<vector <int> > , vector<Mat> > > Objects_aux;
	seleccionar(Objects,Objects_aux);
	Objects = Objects_aux;
#endif
	mostrar(Objects,fondo,CANTIDAD_EVENTOS, outputVideo,frame_rate);
	*/
	return 0;
}


