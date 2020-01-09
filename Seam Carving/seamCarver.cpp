#include <stdio.h>
#include <iostream>
#include <vector>
#include <thread>
#include <pthread.h>
#include <time.h>
#include <assert.h>
#include <string>
#include "CImg.h"

using namespace std;
using namespace cimg_library;

vector<vector<vector<unsigned short>>> img;
vector<vector<vector<unsigned short>>> timg;

unsigned getValidValue(unsigned** gmap, int r, int c, int rows, int cols) {
	if (r < 0 || r >= rows || c < 0 || c >= cols)
		return UINT_MAX;
	return gmap[r][c];
}

void highlight(int r, int c) {
	img[r][c].clear();
	img[r][c].push_back(255);
	img[r][c].push_back(0);
	img[r][c].push_back(0);
}
void highlightT(int r, int c) {
	timg[r][c].clear();
	timg[r][c].push_back(255);
	timg[r][c].push_back(0);
	timg[r][c].push_back(0);
}

void carveSeam(int newWidth, int newHeight) {
	clock_t begin = clock();

	int width = img[0].size();
	int height = img.size();

	// Width first, then height
	// Shrink height
	int rows = height;
	while (width > newWidth) {
		int cols = width;

		// creat
		unsigned** gmap = (unsigned**) malloc(rows * sizeof(unsigned*));
		for (int r = 0; r < rows; r++) 
			gmap[r] = (unsigned*) malloc((cols+1) * sizeof(unsigned));

		gmap[0][0] = UINT_MAX;

		// This will hit everything except the first row
		for (int r = 1; r < rows; r++)
			for (int c = 0; c < cols; c++)
				gmap[r][c] = (unsigned) abs(img[r-1][c][3] - img[r][c][3]);

		// No averaging the first row (because it wasn't hit)
		for (int c = 1; c < cols; c++)
			gmap[0][c] = (unsigned) abs(img[0][c-1][3] - img[0][c][3]);

		// This will hit everything except the first column
		for (int r = 0; r < rows; r++)
			for (int c = 1; c < cols; c++)
				gmap[r][c] = (gmap[r][c] + (unsigned) abs(img[r][c-1][3] - img[r][c][3])) >> 2;

		// Build map from top to bottom
		for (int r = 1; r < rows; r++)
			for (int c = 0; c < cols; c++)
				gmap[r][c] += min(min(getValidValue(gmap,r-1,c-1,rows,cols), 
									  getValidValue(gmap,r-1,c,rows,cols)), 
									  getValidValue(gmap,r-1,c+1,rows,cols));

		// Find shortest bottom-to-top path and erase it
		int smallestIndex = 0;
		unsigned smallestVal = gmap[rows-1][0];
		for (int c = 1; c < cols; c++)
			if (smallestVal >= gmap[rows-1][c]) {
				smallestVal = gmap[rows-1][c];
				smallestIndex = c;
			}
		img[rows-1].erase(img[rows-1].begin() + smallestIndex);
		for (int r = rows-2; r >= 0; r--) {
			unsigned left = getValidValue(gmap,r,smallestIndex-1,rows,cols);
			unsigned middle = getValidValue(gmap,r,smallestIndex,rows,cols);
			unsigned right = getValidValue(gmap,r,smallestIndex+1,rows,cols);
			int minIndex = smallestIndex;
			unsigned minVal = middle;
			if (left < minVal) {
				minIndex = smallestIndex-1;
				minVal = left;
			}
			if (right < minVal)
				minIndex = smallestIndex+1;

			img[r].erase(img[r].begin() + minIndex);
			smallestIndex = minIndex;
		}

		for (int r = 0; r < rows; r++)
			free(gmap[r]);
		free(gmap);

		width--;
	}

	//Flip rows/columns (this doesnt take long at all)
	cout << "Flipping" << endl;
	timg.clear();
	timg.resize(width);
	for (int w = 0; w < width; w++) {
		vector<vector<unsigned short>> rows;
		rows.resize(height);
		for (int h = 0; h < height; h++) {
			vector<unsigned short> values;
			values.resize(4);
			values[0] = img[h][w][0]; //red
			values[1] = img[h][w][1]; //green
			values[2] = img[h][w][2]; //blue
			values[3] = img[h][w][3]; //energy
			rows[h] = values;
		}
		timg[w] = rows;
	}
	width = timg[0].size();
	height = timg.size();

	// Shrink height
	rows = height;
	while (width > newHeight) {
		int cols = width;
		unsigned** gmap = (unsigned**) malloc(rows * sizeof(unsigned*));
		for (int r = 0; r < rows; r++) 
			gmap[r] = (unsigned*) malloc((cols+1) * sizeof(unsigned));

		gmap[0][0] = UINT_MAX;

		// This will hit everything except the first row
		for (int r = 1; r < rows; r++)
			for (int c = 0; c < cols; c++)
				gmap[r][c] = (unsigned) abs(timg[r-1][c][3] - timg[r][c][3]);

		// No averaging the first row (because it wasn't hit)
		for (int c = 1; c < cols; c++)
			gmap[0][c] = (unsigned) abs(timg[0][c-1][3] - timg[0][c][3]);

		// This will hit everything except the first column
		for (int r = 0; r < rows; r++)
			for (int c = 1; c < cols; c++)
				gmap[r][c] = (gmap[r][c] + (unsigned) abs(timg[r][c-1][3] - timg[r][c][3])) >> 2;

		// Build map from top to bottom
		for (int r = 1; r < rows; r++)
			for (int c = 0; c < cols; c++)
				gmap[r][c] += min(min(getValidValue(gmap,r-1,c-1,rows,cols), 
									  getValidValue(gmap,r-1,c,rows,cols)), 
									  getValidValue(gmap,r-1,c+1,rows,cols));

		// Find shortest bottom-to-top path and erase it
		int smallestIndex = 0;
		unsigned smallestVal = gmap[rows-1][0];
		for (int c = 1; c < cols; c++)
			if (smallestVal >= gmap[rows-1][c]) {
				smallestVal = gmap[rows-1][c];
				smallestIndex = c;
			}
		timg[rows-1].erase(timg[rows-1].begin() + smallestIndex);
		for (int r = rows-2; r >= 0; r--) {
			unsigned left = getValidValue(gmap,r,smallestIndex-1,rows,cols);
			unsigned middle = getValidValue(gmap,r,smallestIndex,rows,cols);
			unsigned right = getValidValue(gmap,r,smallestIndex+1,rows,cols);
			int minIndex = smallestIndex;
			unsigned minVal = middle;
			if (left < minVal) {
				minIndex = smallestIndex-1;
				minVal = left;
			}
			if (right < minVal)
				minIndex = smallestIndex+1;

			timg[r].erase(timg[r].begin() + minIndex);
			smallestIndex = minIndex;
		}

		for (int r = 0; r < rows; r++)
			free(gmap[r]);
		free(gmap);

		width--;
	}

	clock_t end = clock();
	cout << "Time took to carve all seams: " << (double)(end-begin) / CLOCKS_PER_SEC << endl;
}

void convertToVecArr(CImg<unsigned char> src) {
	clock_t begin = clock();
	img.clear();
	img.resize(src.height());
	for (int h = 0; h < src.height(); h++) {
		vector<vector<unsigned short>> cols;
		cols.resize(src.width());
		for (int w = 0; w < src.width(); w++) {
			vector<unsigned short> values;
			values.resize(4);
			values[0] = (unsigned short) src(w,h,0,0); //red
			values[1] = (unsigned short) src(w,h,0,1); //green
			values[2] = (unsigned short) src(w,h,0,2); //blue
			values[3] = (unsigned short) values[0]+values[1]+values[2]; //energy
			cols[w] = values;
		}
		img[h] = cols;
	}
	clock_t end = clock();
	cout << "Time took to build array from CImg: " << (double)(end-begin) / CLOCKS_PER_SEC << endl;
}

void* displayImg(void* arg1) {
	cout << "Noice" << endl;
	int* tab = (int*)arg1;
	cout << *tab << endl;
	CImg<unsigned char> test(tab, 256, 256, 1, 1, true);
	CImgDisplay disp(test);
	cout << disp.is_closed() << endl;
	while (!disp.is_closed())
		disp.wait();
	pthread_exit(NULL);
}

void* hey(void* data) {
	pthread_exit(NULL);
}

int main(int argc, char **argv) {
	// Right now, I only have command-line functionality. I want to change this later.
	if (argc < 2) {
		cerr << "Usage: ./seamCarver <IMG_FILE> " << endl;
		return -1;
	}

	// Load the image into CImg
	CImg<unsigned char> src(argv[1]);
	int srcWidth = src.width();
	int srcHeight = src.height();
	if (srcWidth < 1 || srcHeight < 1) {
		cerr << "One of the dimensions of the image is 0 pixels." << endl;
		return -1;
	} else if (srcWidth > 10000 || srcHeight > 10000) {
		// This is in place so that the demo doesn't take forever
		cerr << "One of the dimensions of the image is over 10,000 pixels." << endl;
		return -1;
	}
	printf("Old Width: %d\n", srcWidth);
	printf("Old Height: %d\n", srcHeight);

	// Load the image into a 3d vector array (x,y,colors)
	convertToVecArr(src);

	// Prompt the user to enter new dimensions of the image
	cout << "Enter new dimensions (<WIDTH> <HEIGHT>): ";
	int newWidth = 0;
	int newHeight = 0;
	cin >> newWidth;
	cin >> newHeight;
	if (newWidth <= 0 || newHeight <= 0) {
		cout << "Cannot have a non-positive dimension!" << endl;
		return -1;
	} else if (newWidth > 10000 || newHeight > 10000) {
		// This is in place so that the demo doesn't take forever
		cerr << "For the purpose of speeding up the demo, please do not use images above 10,000 pixels on either dimension." << endl;
		return -1;
	}
	cout << "New dimensions: " << newWidth << ", " << newHeight << endl;

	string filename;
	getline(cin,filename);
	cout << "Enter file to save result as: ";
	getline(cin,filename);

	carveSeam(newWidth, newHeight);

	// Create the new image
	clock_t begin = clock();
	CImg<unsigned char> finalImage(newWidth, newHeight,1,3);
	cimg_forXY(finalImage,x,y) {
		finalImage(x,y,0,0) = timg[x][y][0];
		finalImage(x,y,0,1) = timg[x][y][1];
		finalImage(x,y,0,2) = timg[x][y][2];
	}
	clock_t end = clock();
	cout << "Time took to build final CImg: " << (double)(end-begin) / CLOCKS_PER_SEC << endl;

	// Save image
	string currentDirectory = "FinishedScenes/";
	string saveDir = currentDirectory + filename;
	char file[saveDir.length() + 1];
	strcpy(file, saveDir.c_str());
	strcpy(file, saveDir.c_str());
	finalImage.save_bmp(file);

	// Display image
	CImgDisplay disp(finalImage);
	cout << disp.is_closed() << endl;
	while (!disp.is_closed())
		disp.wait();

	return 0;
}