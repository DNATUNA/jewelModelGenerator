#pragma once

class ETF;
class dwLIC;
class imatrix;

class dwVec2D
{
public:
	dwVec2D() {};
	dwVec2D(float x, float y) { this->x = x; this->y = y; };
	~dwVec2D() {};
	void	normalize();
	float	x;
	float	y;
};

class dwETF
{
	IplImage *	m_pSrcImage;
	int			m_nSrcWidth;
	int			m_nSrcHeight;
	ETF	*		m_pETF;
	dwLIC *		m_pLIC;
	imatrix	*	m_pIMG;
public:
	dwETF() { m_pSrcImage = NULL; m_pETF = NULL; m_pLIC = NULL; m_pIMG = NULL; };
	dwETF(IplImage * pImage) { m_pSrcImage = NULL; m_pETF = NULL; m_pLIC = NULL; m_pIMG = NULL; setSourceImage(pImage);};
	~dwETF();

	void		setSourceImage(IplImage * pImage);
	void		smooth(int nSize, int nCount);
	bool		getVisualisedFlowMap(IplImage * pImage);
	bool		getEdgeImage(float tao, float threshold, IplImage * pImage);
};

