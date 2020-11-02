#include "modelGen.h"
#include<cmath>

inline void dwVec3D::makeUnitVec() {
	float len = sqrt(x * x + y * y + z * z);
	x /= len;
	y /= len;
	z /= len;
}

void modelGen::setMask(std::string& strFileName) {
	if (m_pMaskImage)
		cvReleaseImage(&m_pMaskImage);

	m_pMaskImage = cvLoadImage(strFileName.c_str(), CV_LOAD_IMAGE_GRAYSCALE);

	if (m_pMaskImage)
	{
		m_pForeground = new bool[m_nWidth * m_nHeight];

		int index = 0;
		for (int j = 0; j < m_nHeight; ++j)
		{
			for (int i = 0; i < m_nWidth; ++i)
			{
				if ((unsigned char)(m_pMaskImage->imageData[j * m_pMaskImage->widthStep + i]) < 128)
					m_pForeground[index++] = true;
				else
					m_pForeground[index++] = false;
			}
		}
	}
}

void modelGen::setImage(std::string& strFileName) {
	if (m_pOriginalImage)
		cvReleaseImage(&m_pOriginalImage);
	m_pOriginalImage = cvLoadImage(strFileName.c_str(), CV_LOAD_IMAGE_GRAYSCALE);
	setDimension(m_pOriginalImage->width, m_pOriginalImage->height);
	setRelativeBrightness();
}

void modelGen::setEdge(IplImage* pImage) {
	if (m_pEdgeImage)
		cvReleaseImage(&m_pEdgeImage);
	m_pEdgeImage = pImage;
}

void modelGen::setDimension(int nWidth, int nHeight) { 
	m_nWidth = nWidth;
	m_nHeight = nHeight;
	setDiagonal(); 
	setPixels(); 
}

void modelGen::setDiagonal() { 
	m_nMaxDiagonal = sqrt(m_nWidth * m_nWidth + m_nHeight * m_nHeight) + 1; 
}

void modelGen::setPixels() {
	m_pPixels = new GLubyte[m_nWidth * m_nHeight * 4];
	m_pIndexMap = new int[m_nWidth * m_nHeight];
	m_pDistances = new float[m_nWidth * m_nHeight];
	m_pHeights = new float[m_nWidth * m_nHeight];
}

void modelGen::setRelativeBrightness() {
	IplImage* pSmoothImage = cvCreateImage(cvSize(m_nWidth, m_nHeight), IPL_DEPTH_8U, 1);
	cvSmooth(m_pOriginalImage, pSmoothImage, CV_GAUSSIAN, 15, 15);

	IplImage* pSmoothImage2 = cvCreateImage(cvSize(m_nWidth, m_nHeight), IPL_DEPTH_8U, 1);
	cvSmooth(m_pOriginalImage, pSmoothImage2, CV_GAUSSIAN, 5, 5);

	m_pRelativeBrightness = new float[m_nWidth * m_nHeight];

	int index = 0;
	for (int j = 0; j < m_nHeight; ++j)
	{
		for (int i = 0; i < m_nWidth; ++i)
		{
			m_pRelativeBrightness[index++] = (unsigned char)(pSmoothImage2->imageData[pSmoothImage2->widthStep * j + i]) - (unsigned char)(pSmoothImage->imageData[pSmoothImage->widthStep * j + i]);
		}
	}
}

void modelGen::generateSeeds() {
	if (m_pEdgeImage->nChannels == 1)
	{
		for (int y = 0; y < m_nHeight; ++y)
		{
			for (int x = 0; x < m_nWidth; ++x)
			{
				if ((unsigned char)(m_pEdgeImage->imageData[x + y * m_pEdgeImage->widthStep]) < 128)
				{
					m_seedList.push_back(dwSeed(x, y));
					m_nNumSeeds++;
				}
			}
		}
	}
	else
	{
		for (int y = 0; y < m_nHeight; ++y)
		{
			for (int x = 0; x < m_nWidth; ++x)
			{
				int index = y * m_pEdgeImage->widthStep + x * 3;
				int b = (unsigned char)(m_pEdgeImage->imageData[index]);
				int g = (unsigned char)(m_pEdgeImage->imageData[++index]);
				int r = (unsigned char)(m_pEdgeImage->imageData[++index]);

				if ((b + g + r) < 384)
				{
					m_seedList.push_back(dwSeed(x, y));
					m_nNumSeeds++;
				}
			}
		}
	}
}

bool modelGen::exportModel(std::string& strFileName, float fHeightFactor, float fDistanceFactor) {
	int nNumTriangles = (m_nWidth - 1) * (m_nHeight - 1) * 2;
	int nFileSize = 80 + 4 + (12 * 4 + 2) * nNumTriangles;

	int indexMem = 80;
	char* pMem = new char[nFileSize];
	*(unsigned int *)(pMem + indexMem) = nNumTriangles;
	indexMem += 4;

	for (int j = 0; j < m_nHeight - 1; ++j)
	{
		for (int i = 0; i < m_nWidth - 1; ++i)
		{
			float height00, height10, height01, height11;
			int index = m_nWidth * (m_nHeight - j - 1) + i;

			if (m_pForeground && !m_pForeground[index])
				height00 = 0.f;
			else
			{
				height00 = fHeightFactor * atan(fDistanceFactor * m_pDistances[index]);
				height00 += (0.05 * m_pRelativeBrightness[index]);
			}

			if (m_pForeground && !m_pForeground[index + 1])
				height10 = 0.f;
			else
			{
				height10 = fHeightFactor * atan(fDistanceFactor * m_pDistances[index + 1]);
				height10 += (0.05 * m_pRelativeBrightness[index + 1]);
			}

			index -= m_nWidth;
			if (m_pForeground && !m_pForeground[index])
				height01 = 0.f;
			else
			{
				height01 = fHeightFactor * atan(fDistanceFactor * m_pDistances[index]);
				height01 += (0.05 * m_pRelativeBrightness[index]);
			}

			if (m_pForeground && !m_pForeground[index + 1])
				height11 = 0.f;
			else
			{
				height11 = fHeightFactor * atan(fDistanceFactor * m_pDistances[index + 1]);
				height11 += (0.05 * m_pRelativeBrightness[index + 1]);
			}

			dwVec3D vec1(0.0f, -1.0f, height00 - height01);
			dwVec3D vec2(1.0f, 0.0f, height10 - height00);
			dwVec3D vec3(0.0f, 1.0f, height11 - height10);
			dwVec3D vec4(-1.0f, 0.0f, height01 - height11);
			vec1.makeUnitVec();
			vec2.makeUnitVec();
			vec3.makeUnitVec();
			vec4.makeUnitVec();

			dwVec3D normal1 = vec1.crossProduct(vec2);
			normal1.makeUnitVec();
			dwVec3D normal2 = vec3.crossProduct(vec4);
			normal2.makeUnitVec();


			glNormal3f(normal1.x, normal1.y, normal1.z);
			glVertex3f(i, j, height00);
			glVertex3f(i + 1, j, height10);
			glVertex3f(i, j + 1, height01);

			glNormal3f(normal2.x, normal2.y, normal2.z);
			glVertex3f(i + 1, j, height10);
			glVertex3f(i + 1, j + 1, height11);
			glVertex3f(i, j + 1, height01);
			
			//triangle1
			*(float*)(pMem + indexMem) = normal1.x;
			indexMem += 4;
			*(float*)(pMem + indexMem) = normal1.y;
			indexMem += 4;
			*(float*)(pMem + indexMem) = normal1.z;
			indexMem += 4;

			*(float*)(pMem + indexMem) = i;
			indexMem += 4;
			*(float*)(pMem + indexMem) = j;
			indexMem += 4;
			*(float*)(pMem + indexMem) = height00;
			indexMem += 4;

			*(float*)(pMem + indexMem) = i+1;
			indexMem += 4;
			*(float*)(pMem + indexMem) = j;
			indexMem += 4;
			*(float*)(pMem + indexMem) = height10;
			indexMem += 4;

			*(float*)(pMem + indexMem) = i;
			indexMem += 4;
			*(float*)(pMem + indexMem) = j + 1;
			indexMem += 4;
			*(float*)(pMem + indexMem) = height01;
			indexMem += 4;

			*(unsigned short*)(pMem + indexMem) = 0;
			indexMem += 2;

			//triangle2
			*(float*)(pMem + indexMem) = normal2.x;
			indexMem += 4;
			*(float*)(pMem + indexMem) = normal2.y;
			indexMem += 4;
			*(float*)(pMem + indexMem) = normal2.z;
			indexMem += 4;

			*(float*)(pMem + indexMem) = i + 1;
			indexMem += 4;
			*(float*)(pMem + indexMem) = j;
			indexMem += 4;
			*(float*)(pMem + indexMem) = height10;
			indexMem += 4;

			*(float*)(pMem + indexMem) = i + 1;
			indexMem += 4;
			*(float*)(pMem + indexMem) = j + 1;
			indexMem += 4;
			*(float*)(pMem + indexMem) = height11;
			indexMem += 4;

			*(float*)(pMem + indexMem) = i;
			indexMem += 4;
			*(float*)(pMem + indexMem) = j + 1;
			indexMem += 4;
			*(float*)(pMem + indexMem) = height01;
			indexMem += 4;

			*(unsigned short*)(pMem + indexMem) = 0;
			indexMem += 2;
		}
	}
	FILE* pFile = fopen(strFileName.c_str(), "wb");
	if (pFile == NULL)
		return false;

	fwrite(pMem, sizeof(char), nFileSize, pFile);
	fclose(pFile);

	return true;
}
