// jewelModelGenerator.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include "modelGen.h"
#include "dwETF.h"

using namespace std;
using namespace cv;

//################################ OpenGL callback functions ################################
void init(int argc, char** argv, int nWidth, int nHeight);
void Render4DistanceTransform();
void Reshape4DistanceTransform(int w, int h);


modelGen generator;

int main(int argc, char** argv)
{
    if (argc != 3)
    {
        fprintf(stderr, "insufficient arguments.\n");
        exit(1);
    }

    std::string strImageName = argv[1];
    if(strImageName.length() == 0)
    {
        fprintf(stderr, "wrong image filename.\n");
        exit(2);
    }

    std::string strModelName = argv[2];
    if (strModelName.length() == 0)
    {
        fprintf(stderr, "wrong model filename.\n");
        exit(3);
    }

    //################################ getting a source image ################################
    IplImage* pSourceImage = cvLoadImage(strImageName.c_str(), CV_LOAD_IMAGE_COLOR);
    if (!pSourceImage)
    {
        fprintf(stderr, "Fail to open image.\n");
        exit(4);
    }
    generator.setImage(strImageName);
    fprintf(stdout, "%s\n", strImageName.c_str());
    //################################ generating an edge image ################################
    int nETFsize = 4;
    int nETFiteration = 4;
    dwETF etf(pSourceImage);
    etf.smooth(nETFsize, nETFiteration);;

    float fTao = 0.99f;
    float fThreshold = 1.f;
    IplImage* pEdgeImage = cvCreateImage(cvSize(pSourceImage->width, pSourceImage->height), 8, 1);

    if (!etf.getEdgeImage(fTao, fThreshold, pEdgeImage))
    {
        fprintf(stderr, "Fail to create an edge image.\n");
        exit(5);
    }
    generator.setEdge(pEdgeImage);

    //################################ setting a maske image ################################
    strImageName.erase(strImageName.end() - 4, strImageName.end());
    strImageName += "_mask.jpg";
    generator.setMask(strImageName);

    //################################ distance transform ################################
    generator.generateSeeds();
    
    init(argc, argv, generator.getWidth(), generator.getHeight());
    glutReshapeFunc(Reshape4DistanceTransform);
    glutDisplayFunc(Render4DistanceTransform);
    glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_GLUTMAINLOOP_RETURNS);

    glutMainLoop();
    //std::cout << "Distance-trasformed\n" << std::endl;

    //################################ image refinement ################################
    // implement later

    //################################ generating 3D model ################################
    float fHeightFactor = 3.f;
    float fDistanceFactor = 0.3f;

    if (!generator.exportModel(strModelName, fHeightFactor, fDistanceFactor))
    {
        fprintf(stderr, "export: file IO error.\n");
        exit(6);
    }

    return 0;
}

void init(int argc, char** argv, int nWidth, int nHeight)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
    glutInitWindowSize(nWidth, nHeight);
    glutInitWindowPosition(0, 0);
    glutCreateWindow("Model Renderer");

    glClearColor(1.0, 1.0, 1.0, 1.0);
    glEnable(GL_DEPTH_TEST);
}

void Reshape4DistanceTransform(int w, int h)
{
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, w, 0, h, -100, 100);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

void Render4DistanceTransform()
{
    unsigned char r, g, b;
    int count = 0;

    //OpenGL을 이용해서 콘을 화면에 그리는 부분
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    int nNumSeeds = generator.getNumSeeds();
    for (int i = 0; i < nNumSeeds; i++)
    {
        glPushMatrix();

        int x = generator[i].getX();
        int y = generator[i].getY();
        glTranslated(x, y, 0);

        generator.encodeColor(i, r, g, b);
        glColor3ub(r, g, b);
        glutSolidCone(generator.getMaxDiagonal(), 100, 40, 1);

        glPopMatrix();
    }

    //화면의 픽셀들을 메모리로 읽어오는 부분
    auto pPixels = generator.m_pPixels;
    int nWidth = generator.getWidth();
    int nHeight = generator.getHeight();
    glReadPixels(0, 0, nWidth, nHeight, GL_RGBA, GL_UNSIGNED_BYTE, pPixels);
    glutSwapBuffers();

    int i = 0;
    int ii = 0;
    for (int y = 0; y < nHeight; y++)
    {
        for (int x = 0; x < nWidth; x++)
        {
            unsigned char r = pPixels[i++];
            unsigned char g = pPixels[i++];
            unsigned char b = pPixels[i++];
            i++;
            int index = generator.decodeColor(r, g, b);

            double dist = generator[index].calcAndUpdateDistance(x, y);
            generator.m_pDistances[ii] = dist;
            generator.m_pIndexMap[ii] = index;
            ii++;
        }
    }
    glutLeaveMainLoop();
}