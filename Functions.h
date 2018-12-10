#pragma once
#include "opencv2/opencv.hpp"
#include <cmath>
#include <algorithm>
#include <vector>


/**
PARAMETROS:
DEBUG 				:= visualizar la segmentacion y etiqutado
MAS_INFO 			:= visualizar mas informacion (numeros en la etiquetas de los blobs...)

FONDO_PASOS_SEG 	:= cada cuanto se quiere que se actualize el fondo
FONDO_MERGE			:= alfa de mezclado entre el frame actual y el que estaba (fondo)
MOSTRAR_FONDO		:= muestra los pasos intermedios del calculo del fondo
FONDO_NOMBRE		:= nombre del archivo donde se guarda el fondo

TAM_MIN_OBJETOS 	:= tamanio minimo (en pixeles) que deben de tener los objetos para participar como eventos en la escena
TAM_MAX_OBJETOS 	:= tamanio maximo (en pixeles) que deben de tener los objetos para participar como eventos en la escena
TASA_APRENDIZAJE	:= tasa de aprendizaje para el MOG2, con cuanto se quiere que reconozca el fondo
SEG_ROJO_AZUL 		:= dejar solo los objetos rojos (true) o los objetos azules (false)

DELAY_EVENTOS		:= espera (en frames) de visualizacion para agregar nuevos eventos a la escena
MOSTRAR_RESULTADO	:= para ver el resultado de la compilacion en video final
CANTIDAD_EVENTOS 	:= cantidad de objetos a visualizar juntos en la escena final

ENTRADA_NOMBRE		:= nombre del video de entrada (.AVI)
SALIDA_NOMBRE		:= nombre del video de salida (.AVI)
RATIO 				:= factor de escala para achicar el video o salida. (=2.0 mas rapido; =1.0 mas lento)
**/
//
#define DEBUG false
#define MAS_INFO false

// fondo
#define FONDO_PASOS_SEG 1
#define FONDO_MERGE 0.90
#define FONDO_MOSTRAR true
#define FONDO_NOMBRE "FONDO.jpg"

// segmentacion
#define TAM_MAX_OBJETOS 5000  		// estaba en 30000
#define TAM_MIN_OBJETOS 40  		// estaba en 110
#define TASA_APRENDIZAJE 0.001f
#define SEG_ROJO_AZUL true

// mostrar
#define DELAY_EVENTOS 10
#define MOSTRAR_RESULTADO true
#define CANTIDAD_EVENTOS 30
#define EVENTOS_SEG false
#define EVENTOS_SEGUNDOS true

// videos
#define ENTRADA_NOMBRE "../video2.avi"
//#define ENTRADA_NOMBRE "video_casa.mp4"
#define SALIDA_NOMBRE "salida2.avi"
#define RATIO 2
//#define FPS 30
//
/// Funciones, dejar abajo de los define

using namespace cv;
using namespace std;

struct BlobCenter{
	vector<Point2i> blob;
	Point2i center;	
	int id;
	int frame; // en que frame aparece
};

void ConvexHull(const Mat maskMog2,Mat src,const Mat frame2, Mat &output, Mat &threshold_output);
void FindBlobs(const cv::Mat &binary, std::vector < std::vector<cv::Point2i> > &blobs);
void Img2Blob(const cv::Mat &img, vector< BlobCenter > &blobs_out, int frame );
void verificar_etiqueta(int id1,int id2, double min_dist_centers, vector< vector<double> > &etiquetas);
void correspondencias_id(vector < BlobCenter > &b1, vector < BlobCenter > &b2, vector< vector<double> > &etiquetas);
void reEtiquetado(vector< BlobCenter > &blobs, vector< vector<double> > &etiquetas, int &next_id);
void paintBlobs(const vector < BlobCenter > &blobs, Mat &output, double time);
void FillObjects(const Mat &src, vector< BlobCenter > &blobs, vector <pair <vector<vector <int> > , vector<Mat> > > &Objects);
void FillObjects2(const Mat &src, vector< BlobCenter > &blobs, vector< vector< Mat > > &Objects);
void reemplaza(Mat &fondo, Mat &imagen);
void mostrar(const vector <pair <vector<vector <int> > , vector<Mat> > > &Objects2, const Mat &fondo, int ventana, VideoWriter &outputVideo, int FPS);
void extraerFondo(const Mat &frame_actual, const Mat &mascara, const Mat &fondo_temporal,Mat &salida);
void seleccionar(const vector <pair <vector<vector <int> > , vector<Mat> > > &Objects,vector <pair <vector<vector <int> > , vector<Mat> > > &Objects_aux);

