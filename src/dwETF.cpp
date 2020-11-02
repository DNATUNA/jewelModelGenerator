#include "shared.h"
#include "dwETF.h"

#include "imatrix.h"
#include "myvec.h"
#include "ETF.h"
#include "fdog.h"
#include "dwLIC.h"
#include "dwColors.h"

void dwVec2D::normalize()
{
	float fMag = sqrt(x*x + y*y);
	x /= fMag;
	y /= fMag;
}

dwETF::~dwETF()
{
// 	if (m_pSrcImage)
// 		cvReleaseImage(&m_pSrcImage);

	if (m_pLIC)
	{
		delete m_pLIC;
	}

	if (m_pETF)
	{
		delete m_pETF;
	}

	if (m_pIMG)
	{
		delete m_pIMG;
	}
}

void dwETF::setSourceImage(IplImage * pImage)
{
	dwColors color;
	
	if (m_pSrcImage)
		cvReleaseImage(&m_pSrcImage);

		

	m_pSrcImage = pImage;
	m_nSrcWidth = m_pSrcImage->width;
	m_nSrcHeight = m_pSrcImage->height;

	m_pIMG = new imatrix(m_nSrcWidth, m_nSrcHeight);

	for (int j = 0; j < m_nSrcHeight; j++)
	{
		for (int i = 0; i < m_nSrcWidth; i++)
		{
			int B = (unsigned char)(m_pSrcImage->imageData[j*m_pSrcImage->widthStep + i * 3 + 0]);
			int G = (unsigned char)(m_pSrcImage->imageData[j*m_pSrcImage->widthStep + i * 3 + 1]);
			int R = (unsigned char)(m_pSrcImage->imageData[j*m_pSrcImage->widthStep + i * 3 + 2]);

			float L, a, b;
			color.RGB2Lab(R, G, B, L, a, b);

			int nLuminance = int(L * 2.55f + 0.5f);
			if (nLuminance > 255)
				nLuminance = 255;
			else if (nLuminance < 0)
				nLuminance = 0;

			(*m_pIMG)[i][j] = (unsigned char)(nLuminance);
		}
	}

	if (m_pETF)
	{
		delete m_pETF;
	}
	m_pETF = new ETF;
	m_pETF->init(m_nSrcWidth, m_nSrcHeight);
	m_pETF->set(*m_pIMG);
}

void dwETF::smooth(int nSize, int nCount)
{
	m_pETF->Smooth(nSize, nCount);
}

bool dwETF::getVisualisedFlowMap(IplImage * pImage)
{
	if (pImage)
	{
		if ((pImage->width == m_nSrcWidth) && (pImage->height == m_nSrcHeight) && (pImage->depth == m_pSrcImage->depth) && (pImage->nChannels == 1))
		{
			m_pLIC = new dwLIC(m_nSrcWidth, m_nSrcHeight);
			dwVec2D * tempMap = new dwVec2D[m_nSrcWidth * m_nSrcHeight];

			for (int j = 0; j < m_nSrcHeight; j++)
			{
				for (int i = 0; i < m_nSrcWidth; i++)
				{
					Vect temp = m_pETF->get(i, j);
					tempMap[j*m_nSrcWidth + i].x = temp.tx;
					tempMap[j*m_nSrcWidth + i].y = temp.ty;
					tempMap[j*m_nSrcWidth + i].normalize();

					m_pLIC->pVectr[(m_nSrcHeight - j - 1)*m_nSrcWidth * 2 + i * 2] = temp.tx;
					m_pLIC->pVectr[(m_nSrcHeight - j - 1)*m_nSrcWidth * 2 + i * 2 + 1] = -temp.ty;
				}
			}

			delete[] tempMap;

			m_pLIC->FlowImagingLIC();
			IplImage * pTempImage = m_pLIC->GetLICimage();
			for (int j = 0; j < m_nSrcHeight; j++)
			{
				for (int i = 0; i < m_nSrcWidth; i++)
				{
					pImage->imageData[j * pImage->widthStep + i] = pTempImage->imageData[j * pTempImage->widthStep + i];
				}
			}
			return true;
		}
		else
			return false;
	}
	else
		return false;
}


bool dwETF::getEdgeImage(float tao, float threshold, IplImage * pImage)
{
	if (pImage)
	{
		GetFDoG(*m_pIMG, *m_pETF, 1.0, 3.0, tao);
		GrayThresholding(*m_pIMG, threshold);

		if (pImage->nChannels == 1)
		{
			for (int j = 0; j < m_nSrcHeight; j++)
			{
				for (int i = 0; i < m_nSrcWidth; i++)
				{
					pImage->imageData[j*pImage->widthStep + i] = (*m_pIMG)[i][j];
				}
			}
		}
		else if (pImage->nChannels == 3)
		{
			for (int j = 0; j < m_nSrcHeight; j++)
			{
				for (int i = 0; i < m_nSrcWidth; i++)
				{
					pImage->imageData[j*pImage->widthStep + i * 3 + 0] = pImage->imageData[j*pImage->widthStep + i * 3 + 1] = 
						pImage->imageData[j*pImage->widthStep + i * 3 + 2] = (*m_pIMG)[i][j];
				}
			}
		}
		else
		{
			return false;
		}
		return true;
	}
	else
		return false;
}
