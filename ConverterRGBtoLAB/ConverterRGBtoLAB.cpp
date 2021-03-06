// ConverterRGBtoLAB.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"


/**
 * Converte uma nuvem de pontos em RGB para CIELab usando a OpenCV. Os valores
 * são alvos na esturura RGB da PCL, porém estão em LAB.
 */
pcl::PointCloud<pcl::PointXYZRGB>::Ptr 
convert_rgb_to_lab_opencv_fake_rgb(pcl::PointCloud<pcl::PointXYZRGB>::Ptr cloud)
{
	pcl::PointCloud <pcl::PointXYZRGB>::Ptr cloud_lab(new pcl::PointCloud <pcl::PointXYZRGB>);

	cloud_lab->height = cloud->height;
	cloud_lab->width = cloud->width;

	for (pcl::PointCloud<pcl::PointXYZRGB>::iterator it = cloud->begin(); it != cloud->end(); it++) {
		// Color conversion
		cv::Mat pixel(1, 1, CV_8UC3, cv::Scalar(it->b, it->g, it->r));
		cv::Mat temp;
		cv::cvtColor(pixel, temp, CV_BGR2Lab);

		pcl::PointXYZRGB point;

		point.x = it->x;
		point.y = it->y;
		point.z = it->z;

		point.b = temp.at<uchar>(0, 0);
		point.g = temp.at<uchar>(0, 1);
		point.r = temp.at<uchar>(0, 2);


		cloud_lab->push_back(point);

	}

	return cloud_lab;
}


/**
* Converte uma nuvem de pontos em RGB para CIELab usando a OpenCV. Os valores
* são alvos na esturura CIELAB da PCL.
*/
pcl::PointCloud<pcl::PointXYZLAB>::Ptr convert_rgb_to_lab_opencv(pcl::PointCloud<pcl::PointXYZRGB>::Ptr cloud)
{
	pcl::PointCloud <pcl::PointXYZLAB>::Ptr cloud_lab(new pcl::PointCloud <pcl::PointXYZLAB>);

	cloud_lab->height = cloud->height;
	cloud_lab->width = cloud->width;
	
	for (pcl::PointCloud<pcl::PointXYZRGB>::iterator it = cloud->begin(); it != cloud->end(); it++) {
		// Color conversion
		cv::Mat pixel(1, 1, CV_8UC3, cv::Scalar(it->r, it->g, it->b));
		cv::Mat temp;
		cv::cvtColor(pixel, temp, CV_BGR2Lab);

		pcl::PointXYZLAB point;
		
		point.x = it->x;
		point.y = it->y;
		point.z = it->z;
		
		point.L = temp.at<uchar>(0, 0);
		point.a = temp.at<uchar>(0, 1);
		point.b = temp.at<uchar>(0, 2);
		

		cloud_lab->push_back(point);

	}

	return cloud_lab;
}


/**
* Converte uma nuvem de pontos em RGB para CIELab usando o algoritmo de conversão tradicional. Os valores
* são alvos na esturura CIELAB da PCL.
*/
pcl::PointCloud<pcl::PointXYZLAB>::Ptr convert_rgb_to_lab(pcl::PointCloud<pcl::PointXYZRGB>::Ptr cloud)
{
	pcl::PointCloud <pcl::PointXYZLAB>::Ptr cloud_lab (new pcl::PointCloud <pcl::PointXYZLAB>);

	cloud_lab->height = cloud->height;
	cloud_lab->width = cloud->width;

	for (pcl::PointCloud<pcl::PointXYZRGB>::iterator it = cloud->begin(); it != cloud->end(); it++) {


		double R, G, B, X, Y, Z;

		// map sRGB values to [0, 1]
		R = it->r / 255.0;
		G = it->g / 255.0;
		B = it->b / 255.0;

		// linearize sRGB values
		if (R > 0.04045)
			R = pow((R + 0.055) / 1.055, 2.4);
		else
			R = R / 12.92;

		if (G > 0.04045)
			G = pow((G + 0.055) / 1.055, 2.4);
		else
			G = G / 12.92;

		if (B > 0.04045)
			B = pow((B + 0.055) / 1.055, 2.4);
		else
			B = B / 12.92;

		// postponed:
		//    R *= 100.0;
		//    G *= 100.0;
		//    B *= 100.0;

		// linear sRGB -> CIEXYZ
		X = R * 0.4124 + G * 0.3576 + B * 0.1805;
		Y = R * 0.2126 + G * 0.7152 + B * 0.0722;
		Z = R * 0.0193 + G * 0.1192 + B * 0.9505;

		// *= 100.0 including:
		X /= 0.95047;  //95.047;
					   //    Y /= 1;//100.000;
		Z /= 1.08883;  //108.883;

		
					   
		
		// CIEXYZ -> CIELAB
		if (X > 0.008856)
			X = pow(X, 1.0 / 3.0);
		else
			X = 7.787 * X + 16.0 / 116.0;

		if (Y > 0.008856)
			Y = pow(Y, 1.0 / 3.0);
		else
			Y = 7.787 * Y + 16.0 / 116.0;

		if (Z > 0.008856)
			Z = pow(Z, 1.0 / 3.0);
		else
			Z = 7.787 * Z + 16.0 / 116.0;

		Eigen::Vector3f colorLab;
		colorLab[0] = static_cast<float> (116.0 * Y - 16.0);
		colorLab[1] = static_cast<float> (500.0 * (X - Y));
		colorLab[2] = static_cast<float> (200.0 * (Y - Z));
		


		pcl::PointXYZLAB point_lab;
		point_lab.x = it->x;
		point_lab.y = it->y;
		point_lab.z = it->z;

		point_lab.L = colorLab[0];
		point_lab.a = colorLab[1];
		point_lab.b = colorLab[2];

		cloud_lab->push_back(point_lab);
	}


	return cloud_lab;
}



int main()
{
	// Declarando a nuvem de pontos do paciente.
	pcl::PointCloud <pcl::PointXYZRGB>::Ptr cloud_rgb (new pcl::PointCloud <pcl::PointXYZRGB>);

	// Declarando a nuvem de pontos do paciente.
	pcl::PointCloud <pcl::PointXYZLAB>::Ptr cloud_lab (new pcl::PointCloud <pcl::PointXYZLAB>);

	// Declarando a nuvem de pontos do paciente.
	pcl::PointCloud <pcl::PointXYZRGB>::Ptr cloud_lab_fake_rgb(new pcl::PointCloud <pcl::PointXYZRGB>);


	std::cout << "Iniciando Leitura da Nuvem de Pontos do Paciente... ";
	if (pcl::io::loadPCDFile <pcl::PointXYZRGB>("paciente.pcd", *cloud_rgb) == -1)
	{
		std::cout << "Cloud reading failed." << std::endl;

	}
	std::cout << "OK!" << std::endl << std::endl;





	std::cout << "___________________________________________________________" << std::endl << std::endl;

	std::cout << "Convertendo a nuvem de pontos do paciente de RGB para LAB... ";
	//pcl::copyPointCloud(*cloud_rgba, *cloud_lab);
	
	cloud_lab_fake_rgb = convert_rgb_to_lab_opencv_fake_rgb(cloud_rgb);
	
	std::cout << "OK!" << std::endl << std::endl;

	std::cout << "___________________________________________________________" << std::endl << std::endl;

	

	std::cout << "Salvando arquivo PCD... ";
	pcl::io::savePCDFile("paciente_lab_opencv_aprox_fake_bgr2.pcd", *cloud_lab_fake_rgb);
	std::cout << "OK!" << std::endl << std::endl;

	std::cout << "Salvando arquivo PLY... ";
	pcl::io::savePLYFile("paciente_lab_opencv_aprox_fake_bgr2.ply", *cloud_lab_fake_rgb);
	std::cout << "OK!" << std::endl << std::endl;

	
	
	getchar();
    return 0;
}

