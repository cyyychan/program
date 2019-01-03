#include"../Functions.h"
#include <iostream>
#include <string>

void findConvexHull(const Mat front_mask, const Mat src_frame, Mat &output, Mat &hull_mask)
{
	vector<vector<Point>> contours;
	vector<Vec4i> hierarchy;
		
	// Find contours
	findContours(front_mask, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(0, 0));
	
	// Find the convex hull object for each contour
	vector<vector<Point>> hull(contours.size());
	for( int i = 0; i < contours.size(); i++ )
		convexHull(Mat(contours[i]), hull[i], false);
	
	/*******************************检查convex数量**********************************/
	//将图像front_mask复制给mask_3C(将一维的图像变为三维图像, 三个通道数值一样)
	//RNG rng(12345);
	// Mat mask_3C = Mat::zeros(front_mask.size(), CV_8UC3);
	// for(int i = 0; i < 3; i++)
	// 	for (int x=0; x<mask_3C.cols; x++)
	// 		for (int y=0; y<mask_3C.rows; y++)
	// 			mask_3C.at<Vec3b>(y,x)[i] = front_mask.at<uchar>(y,x);
	// for( int i = 0; i< contours.size(); i++ )
	// {
	// 	Scalar color = Scalar(rng.uniform(0, 255), rng.uniform(0,255), rng.uniform(0,255));
	// 	drawContours(mask_3C, hull, i, color, -1);
	// }
	// imshow("mask", mask_3C);
	/******************************************************************************/

	//draw convex
	for( int i = 0; i< contours.size(); i++ )
	{
		Scalar color = Scalar(255);
		drawContours(hull_mask, hull, i, color, -1);
	}
	output = src_frame.clone();
	for(int i = 0; i < 3; i++)
		for (int x=0; x<output.cols; x++)
			for (int y=0; y<output.rows; y++)
				output.at<cv::Vec3b>(y,x)[i] = output.at<cv::Vec3b>(y,x)[i] * (hull_mask.at<uchar>(y,x) / 255);
}

void extractBackground(const Mat src_frame, const Mat hull_mask, Mat &background)
{
	double alpha = BACKGROUND_FUSION;
	double beta = 1.0 - alpha;
	
	for (int i = 0; i < src_frame.cols; i++)
	{
		for (int j = 0;j < src_frame.rows; j++)
		{
			if((!(hull_mask.at<uchar>(j, i))))
			{
				background.at<Vec3b>(j, i)[0] = (alpha * background.at<Vec3b>(j, i)[0]) + (beta * src_frame.at<Vec3b>(j, i)[0]); 
				background.at<Vec3b>(j, i)[1] = (alpha * background.at<Vec3b>(j, i)[1]) + (beta * src_frame.at<Vec3b>(j, i)[1]); 
				background.at<Vec3b>(j, i)[2] = (alpha * background.at<Vec3b>(j, i)[2]) + (beta * src_frame.at<Vec3b>(j, i)[2]); 
			}
		}
	}
}

//区域生长找到连通域，单个连通域的点集用blob(vector<Point2i>)存储， 然后放入blobs(vector<vector<Point2i>>)中
void FindBlobs(const Mat binary, vector<vector<Point2i>> &blobs)
{
	// Fill the label_image with the blobs
	// 0  - background
	// 1  - unlabelled foreground
	// 2+ - labelled foreground
	
	cv::Mat label_image;
	binary.convertTo(label_image, CV_32SC1);
	
	int label_count = 2;
	
	for(int y = 0; y < label_image.rows; y++) 
	{
		int *row = (int*)label_image.ptr(y);
		for(int x = 0; x < label_image.cols; x++) 
		{
			if(row[x] != 1) 
			{
				continue;
			}

			cv::Rect rect;
			cv::floodFill(label_image, cv::Point(x,y), label_count, &rect);
			
			std::vector<cv::Point2i> blob;
			
			for(int i = rect.y; i < (rect.y + rect.height); i++)
			{
				int *row2 = (int*)label_image.ptr(i);
				for(int j = rect.x; j < (rect.x + rect.width); j++) 
				{
					if(row2[j] != label_count) 
					{
						continue;
					}
					blob.push_back(cv::Point2i(j,i));
				}
			}
			if(blob.size() > MIN_AREA && blob.size() < MAX_AREA)
				blobs.push_back(blob);
			label_count++;
		}
	}
}

//提取blobs中单个blob的特征，point2i，center， id， frame存储在blobs_out中
void  Blob(const cv::Mat img, vector<BlobCenter> &blobs_out, int frame)
{
	Mat binary;
	vector<vector<Point2i>> blobs;
	threshold(img, binary, 0.0, 1.0, cv::THRESH_BINARY);
	FindBlobs(binary, blobs);
	
	int blob_size;
	int id_blob = 1;
	BlobCenter blob_temp;
	
	for(int i=0; i < blobs.size(); i++) 
	{
		blob_temp.blob = blobs[i];
		blob_size = blob_temp.blob.size(); 
		for(int j=0; j < blob_size; j++) 
		{
			blob_temp.center.x += (double)blob_temp.blob[j].x;
			blob_temp.center.y += (double)blob_temp.blob[j].y;
		}
		
		blob_temp.center.x /= blob_size;
		blob_temp.center.y /= blob_size;
		
		blob_temp.id = id_blob;
		blob_temp.frame = frame;
		id_blob++;
		blobs_out.push_back(blob_temp);			
	}
}

void generateLableVec(const int min_dist_id, const int id, const double min_dist_centers, vector<vector<double>> &label_vec)
{
	bool flag = true;
	if(min_dist_id != -1)
	{
		for(int i = 0; i < label_vec.size(); i++)
		{
			if(label_vec[i][0] == (double)min_dist_id)
			{
				if(min_dist_centers < label_vec[i][2])
				{
					label_vec[i][1] = id;
					label_vec[i][2] = min_dist_centers; 
				}
				flag = false;
				break;
			}
		}
		if(flag)
		{
			vector<double> aux;
			aux.push_back(min_dist_id);
			aux.push_back(id);
			aux.push_back(min_dist_centers);
			label_vec.push_back(aux);
		}
	}
}

//寻找每一帧blobs与前一帧blobs的关系
void findCorrespondence(const vector<BlobCenter> b1, const vector<BlobCenter> b2, vector<vector<double>> &label_vec)
{	
	bool is_first_blob;
	double dist_centers, min_dist_centers;
	int min_dist_id;
	
	for (int i = 0; i < b2.size(); i++)
	{
		min_dist_centers = 0;
		min_dist_id = -1;
		is_first_blob = true;
		
		for (int j = 0; j < b1.size(); j++)
		{
			dist_centers = sqrt(pow(((double)b2[i].center.x - (double)b1[j].center.x), 2) + pow(((double)b2[i].center.y - (double)b1[j].center.y), 2));
			if(is_first_blob)
			{
				is_first_blob = false;
				min_dist_centers = dist_centers;
				min_dist_id = b1[j].id;
			}
			else if(dist_centers < min_dist_centers)
			{
				min_dist_centers = dist_centers;
				min_dist_id = b1[j].id;
			}
		}
		generateLableVec(min_dist_id, b2[i].id, min_dist_centers, label_vec);
	}	
}


void addLabelToObject(vector<BlobCenter> &blobs, vector<vector<double>> &label_vec, int &next_id)
{
	bool found; 
	for(int i = 0; i < blobs.size(); i++)
	{
		found = false;
		for(int j = 0; j < label_vec.size(); j++)
		{
			if(blobs[i].id == label_vec[j][1])
			{
				found = true;
				blobs[i].id = label_vec[j][0];
				break;
			}
		}
		if(!found)
		{
			blobs[i].id = next_id;
			next_id = next_id + 1;
		}
	}
}

void paintBlobs(const vector<BlobCenter> blobs, Mat &output, double time)
{
	unsigned char r;
	unsigned char g;
	unsigned char b;
	
	for(size_t i=0; i < blobs.size(); i++) 
	{
		vector<Point2i> blob = blobs[i].blob;
		int label = blobs[i].id;
		
		r = label*100/1.0;
		g = label*100/10.0;
		b = label*100/100.0;
		
		for(int j=0; j<blob.size(); j++)
		{
			output.at<cv::Vec3b>(blob[j].y,blob[j].x)[0] = b;
			output.at<cv::Vec3b>(blob[j].y,blob[j].x)[1] = g;
			output.at<cv::Vec3b>(blob[j].y,blob[j].x)[2] = r;
		}

		string text = to_string(label);
		// string text = to_string(time);
		double escala = 0.3f;
		double grosor = 1;
		putText(output, text, cvPoint(blobs[i].center.x,blobs[i].center.y), FONT_HERSHEY_SCRIPT_SIMPLEX, escala, cvScalar(255,255,255), grosor);	
	}
}


void FillObjects(const Mat &src, vector<BlobCenter> &blobs, vector<pair<vector<vector<int>>, vector<Mat>>> &Objects)
{
	for(int i=0; i<blobs.size(); i++)
	{
		Mat obj_img = Mat::zeros(src.size(), CV_8UC3);
		vector<Point2i> blob = blobs[i].blob;
		int id = blobs[i].id;
		
		for(int j=0; j<blob.size(); j++)
		{
			obj_img.at<Vec3b>(blob[j].y, blob[j].x)[0] = src.at<Vec3b>(blob[j].y, blob[j].x)[0]; //b
			obj_img.at<Vec3b>(blob[j].y, blob[j].x)[1] = src.at<Vec3b>(blob[j].y, blob[j].x)[1]; //g
			obj_img.at<Vec3b>(blob[j].y, blob[j].x)[2] = src.at<Vec3b>(blob[j].y, blob[j].x)[2]; //r
		}
		
		if(id < Objects.size())
		{
			vector <int> aux;
			aux.push_back(blobs[i].center.x);
			aux.push_back(blobs[i].center.y);
			aux.push_back(blobs[i].frame);
			Objects[id].first.push_back(aux);
			Objects[id].second.push_back(obj_img);
		}
		else
		{ 
			vector<Mat> aux;
			vector <int> aux2;
			aux2.push_back(0);
			aux2.push_back(0);
			aux2.push_back(0);

			while(id >= Objects.size())
			{
				pair <vector<vector <int> > , vector<Mat> > otro;
				otro.first.push_back(aux2);
				otro.second = aux;
				Objects.push_back(otro);
			}
			
			vector <int> aux3;
			aux3.push_back(blobs[i].center.x);
			aux3.push_back(blobs[i].center.y);
			aux3.push_back(blobs[i].frame);
			Objects[id].first.push_back(aux3);
			Objects[id].second.push_back(obj_img);
		}
	}
}


void relocate(Mat &background, Mat &image)
{
	for (int i=0;i<background.cols;i++)
	{
		for (int j=0;j<background.rows;j++)
		{
			if(!((image.at<Vec3b>(j, i)[0]==0) && (image.at<Vec3b>(j, i)[1]==0) && (image.at<Vec3b>(j, i)[2]==0)))
			{
				background.at<Vec3b>(j, i)[0] = image.at<Vec3b>(j, i)[0]; //b
				background.at<Vec3b>(j, i)[1] = image.at<Vec3b>(j, i)[1]; //g
				background.at<Vec3b>(j, i)[2] = image.at<Vec3b>(j, i)[2]; //r
			}
		}
	}
	
}


void synthesis(vector<pair<vector<vector<int>>, vector<Mat>>> Objects, const Mat background, VideoWriter &outputVideo, const int FPS)
{
	vector<pair<vector<vector<int>>, vector<Mat>>> window;
	int count = DELAY_EVENTS; 
	reverse(Objects.begin(),Objects.end());

	while(true)
	{
		if (Objects.empty()) 
		{
			cout << "\n no objects" << endl;
			break;
		}

		Mat frame = background.clone();
		int n = 0;
		while((window.size() < EVENTS_NUM || window.size() < Objects.size()) && (count >= DELAY_EVENTS))
		{
			// printf("***%d\n", n++);
			window.insert(window.begin(), *(--Objects.end()));
			Objects.pop_back();
			count = 0; 
		}
		// printf("window: %d\n", window.size());
		// printf("d: %d\n", count);
		
		for(int i = 0 ; i < window.size(); i++)
		{
			relocate(frame, *(window[i].second.begin()));
			
			/**************************************************put text**************************************************/
			vector<vector<int>> aux2 = window[i].first;
			vector<int> aux = aux2[0];
			string text = to_string(aux[2]/FPS / 60) + ":" +  to_string(aux[2] / FPS);

			putText(frame, text, cvPoint(aux[0],aux[1]), FONT_HERSHEY_SCRIPT_SIMPLEX, 0.3f, cvScalar(0,0,255));
			/************************************************************************************************************/

			window[i].first.erase(window[i].first.begin());
			window[i].second.erase(window[i].second.begin());
			if(window[i].second.empty())
			{
				window.erase(window.begin()+i);
			}
		}
		count++;
		imshow("result",frame);
		if (waitKey(50) >= 0)
			break;
		if (!outputVideo.isOpened())
		{
			cout  << "\n fatol error: write to file!! " << endl;
			return;
		}
		else
		{
			outputVideo.write(frame); 
		}
	}
	outputVideo.release();
}

void select(const vector<pair<vector<vector<int>>, vector<Mat>>> Objects, vector<pair<vector<vector<int>>, vector<Mat>>> &Objects_aux)
{
	Objects_aux.clear();
	
	for (int i=0; i<Objects.size(); i++)
	{
		int count = 0;
		vector<Mat> obj = Objects[i].second;

		for (int j=0; j<obj.size(); j++)
		{
			Mat ig;
			inRange(obj[j], Scalar(0,0,0), Scalar(10,10,10), ig); 
			double tam_obj = obj[j].cols * obj[j].rows - countNonZero(ig);
			// printf("tam_obj: %f\n", tam_obj);
			
			if (tam_obj > MIN_AREA*5)	
			{
				// Mat obj_hsv, img1;
				// cvtColor(obj[j], obj_hsv, CV_RGB2HSV); 
				// inRange(obj_hsv, Scalar(50,130,130), Scalar(90,255,255), img1);
				
				// double tam_color = countNonZero(img1);
				// printf("tam_color: %f\n", tam_color);
				// double porcentaje = (tam_color / tam_obj) * 100.0;
				// if (porcentaje > 20.0) 
				count++;
			}
		}
		if (count>1)
			Objects_aux.push_back(Objects[i]);
	}
	cout << "size: " << Objects.size() << "\t" << Objects_aux.size() << endl;
}
