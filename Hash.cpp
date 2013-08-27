/*
	Ronan Murphy 2013
	These functions are desiged for perceptually hashing an image.
	Application of the hash is to be investigated, with areas such as Copyright Inringement and keeping a Database of Useful Source Information in mind.

	To Do:
		Verify math properly, and test to see how much attacks (color changes, brightness, size etc. ) alter the hash in reality.
		Add compatiblity with more image types, (Just .BMP right now).
*/
#include "CImg.h"
#include <string>
#include <iostream>

using namespace cimg_library;

unsigned long int Hash(std::string p_filename);
unsigned int DifferenceBetweenTwoImages(std::string p_file1, std::string p_file2);


int main()
{
	
	unsigned long int hash;

	std::cout << "---------------------" << std::endl;
	std::cout << "Generating Hashes For Images" << std::endl;
	std::cout << "---------------------" << std::endl;
	

	hash = Hash("Assets/julia.bmp");
	std::cout << "julia.bmp Hash \t: " << hash << std::endl;
	hash = Hash("Assets/lisa.bmp");
	std::cout << "lisa.bmp Hash \t: " << hash << std::endl;
	

	unsigned int difs;

	std::cout << std::endl;
	std::cout << "---------------------" << std::endl;
	std::cout << "Comparison of images" << std::endl;
	std::cout << "---------------------" << std::endl;

	std::cout << "Difference of 0 means high chance of same image." << std::endl;
	std::cout << "Difference of 1-5 means chance of same image with minor alterations or attacks on it." << std::endl;
	std::cout << "Difference of 10+ means high chance of being a different image.\n" << std::endl;
	std::cout << "-" << std::endl;

	//should be no difference														Result = 0
	difs = DifferenceBetweenTwoImages("Assets/julia.bmp", "Assets/julia.bmp");
	std::cout << "Same image \t\t\t\t: " << difs << std::endl;

	//should be no difference														Result = 0
	difs = DifferenceBetweenTwoImages("Assets/julia.bmp", "Assets/juliaSmaller.bmp");
	std::cout << "Scaled down image \t\t\t: " << difs << std::endl;

	//should be no difference														Result = 0
	difs = DifferenceBetweenTwoImages("Assets/julia.bmp", "Assets/juliaLarger.bmp");
	std::cout << "Scaled up image \t\t\t: " << difs << std::endl;

	//should a small difference, if any												Result = 0
	difs = DifferenceBetweenTwoImages("Assets/julia.bmp", "Assets/juliaHueChanged.bmp");
	std::cout << "Hue changed on image \t\t\t: " << difs << std::endl;

	//should be small difference, if any (Depending on degree of attack)			Result = 1
	difs = DifferenceBetweenTwoImages("Assets/julia.bmp", "Assets/juliaBrightness.bmp");
	std::cout << "Brightness turned up on image, 62pts \t: " << difs << std::endl;

	//should be smallDifference, less than 5										Result = 2
	difs = DifferenceBetweenTwoImages("Assets/julia.bmp", "Assets/juliaCropped.bmp");
	std::cout << "Image cropped slightly \t\t\t: " << difs << std::endl;

	//Totally different images, number of differences should be 10+					Result = 17
	difs = DifferenceBetweenTwoImages("Assets/julia.bmp", "Assets/lisa.bmp");
	std::cout << "Two completely different images \t: " << difs << std::endl;

	std::cin.ignore();
	return 0;
}


/*
	Name:	Hash
	Desc:	Hashes the image perceptually, based on how it actually appears.
	Args:	p_filename, The name of the file you want to hash.
	Return:	unsigned long int, the figure the image was hashed to.
*/
unsigned long int Hash(std::string p_filename)
{
	CImg<float> image(p_filename.c_str());
	
	//Reduce size to 8x8
	image.resize(8, 8, -100, -100, 1, 0);

	//Convert to greyscale
	CImg<float> grey(image.width(), image.height(), 1, 1, 0);
	grey = image.RGBtoHSI().get_channel(2);

	//Now have 64 pixels, each with a different color.
	//Find Average

	float average = 0;
	int count = 0;

	for(int i = 0; i < 8; i++)
	{
		for(int j = 0; j < 8; j++)
		{
			average = average + grey(i,j);
			count++;
		}
	}
	average = average / count;

	//Compute Bits.
	//If it is less than the average it is 0.
	//Greater or equal to, is 1

	byte holder[64];
	int index = 0;

	for(int i = 0; i < 8; i++)
	{
		for(int j = 0; j < 8; j++)
		{
				if(grey(i, j) < average)
					holder[index] = 0;
				else
					holder[index] = 1;
			index++;
		}
	}

	//Now have an array of 64 bits, 0 or 1
	//convert to int
	unsigned int result = 0;
	for(int i = 0; i < 64; i++)
		result = result * 2 + holder[i];

	return result;
}

/*
	Name:	DifferenceBetweenTwoImages  (Could use a change)
	Desc:	Compares two images perceptually, based on how they appear, and returns a number between 0-64, based on how different it is.
			0	- Nearly identical
			1-5 - Probably the same image with some sort of attack put on it.
			10+ - A different image completely
			These guidelines are an aproximation and can be altered to suit your needs.
	Args:	std::string p_file1, the first file to compare
			std::string p_file2, the second file to compare.
	Return:	unsigned int, the amount of differences between the two files, (ranging from 0-64)
*/
unsigned int DifferenceBetweenTwoImages(std::string p_file1, std::string p_file2)
{
	//repeating everything twice from the previous function.
	//should probably put them in an array of strings and just loop everything.

	//Want to end up with 2 arrays of the bit values.

	CImg<float> img1(p_file1.c_str());
	CImg<float> img2(p_file2.c_str());

	img1.resize(8, 8, -100, -100, 1, 0);
	img2.resize(8, 8, -100, -100, 1, 0);

	CImg<float> grey1(img1.width(), img1.height(), 1, 1, 0);
	grey1 = img1.RGBtoHSI().get_channel(2);

	CImg<float> grey2(img2.width(), img2.height(), 1, 1, 0);
	grey2 = img2.RGBtoHSI().get_channel(2);

	//two arrays to hold images bits.
	byte holder1[64];
	byte holder2[64];

	//FIRST IMG
	float average = 0;
	int count = 0;
	for(int i = 0; i < 8; i++)
	{
		for(int j = 0; j < 8; j++)
		{
			average = average + grey1(i,j);
			count++;
		}
	}
	average = average / count;
	int index = 0;
	for(int i = 0; i < 8; i++)
	{
		for(int j = 0; j < 8; j++)
		{
				if(grey1(i, j) < average)
					holder1[index] = 0;
				else
					holder1[index] = 1;
			index++;
		}
	}

	//SECOND IMG
	average = 0;
	count = 0;
	for(int i = 0; i < 8; i++)
	{
		for(int j = 0; j < 8; j++)
		{
			average = average + grey2(i,j);
			count++;
		}
	}
	average = average / count;
	index = 0;
	for(int i = 0; i < 8; i++)
	{
		for(int j = 0; j < 8; j++)
		{
				if(grey2(i, j) < average)
					holder2[index] = 0;
				else
					holder2[index] = 1;
			index++;
		}
	}

	unsigned int numberOfDifferences = 0;

	for(int i = 0; i < 64; i++)
	{
		if(holder1[i] != holder2[i])
			numberOfDifferences++;
	}

	return numberOfDifferences;
}

