#pragma once

#include "shared.h"

class dwVec3D
{
public:
	float x = 0.f;
	float y = 0.f;
	float z = 0.f;
	dwVec3D() {}
	dwVec3D(float x, float y, float z) {
		this->x = x;
		this->y = y;
		this->z = z;
	}
	dwVec3D crossProduct(const dwVec3D& vec) {
		dwVec3D ret;
		ret.x = y * vec.z - z * vec.y;
		ret.y = z * vec.x - x * vec.z;
		ret.z = x * vec.y - y * vec.x;
		return ret;
	}
	inline void makeUnitVec();
};

class dwSeed
{
	int x;
	int y;
	float maxDist = 0.f;
public:
	dwSeed(int x, int y) { 
		this->x = x; this->y = y; 
	}
	int getX() const { return x; }
	int getY() const { return y; }
	double getMaxDist() const { return maxDist; }

	float calcAndUpdateDistance (int x, int y) {
		float dist = sqrt(pow(this->x - x, 2) + pow(this->y - y, 2));
		if (dist > maxDist)
			maxDist = dist;
		return dist;
	}
};

class modelGen
{
	int m_nWidth = 0;
	int m_nHeight = 0;
	int m_nMaxDiagonal = 0;

	IplImage* m_pMaskImage = NULL;
	IplImage* m_pOriginalImage = NULL;
	IplImage* m_pEdgeImage = NULL;

	bool* m_pForeground = NULL;
	float* m_pHeights = NULL;

	std::vector<dwSeed> m_seedList;
	int m_nNumSeeds = 0;

	void setDimension(int nWidth, int nHeight);
	void setDiagonal();
	void setPixels();
public:
	GLubyte* m_pPixels = NULL;
	int* m_pIndexMap = NULL;
	float* m_pDistances = NULL;
	float* m_pRelativeBrightness = NULL;

	modelGen() {};
	void setMask(std::string& strFileName);
	void setImage(std::string& strFileName);
	void setEdge(IplImage* pImage);
	void generateSeeds();
	void setRelativeBrightness();
	bool exportModel(std::string& strFileName, float fHeightFactor, float fDistanceFactor);

	int getWidth() const { return m_nWidth; }
	int getHeight() const { return m_nHeight; }
	int getNumSeeds() const { return m_nNumSeeds; }
	int getMaxDiagonal() const { return m_nMaxDiagonal; }
	dwSeed& operator[](int i) { return m_seedList[i]; }

	void encodeColor(int index, unsigned char& r, unsigned char& g, unsigned char& b) {
		r = (index / 65536); g = ((index % 65536) / 256); b = (index % 256); 
	}
	int decodeColor(unsigned char r, unsigned char g, unsigned char b) {
		return (r * 65536 + g * 256 + b); 
	}
};

