FROM ubuntu:20.04

RUN echo "Start Install opencv3.4.7!!!"
RUN apt-get update && apt-get upgrade -y
RUN apt-get install -y software-properties-common
RUN add-apt-repository 'deb http://security.ubuntu.com/ubuntu xenial-security main'
RUN apt update && apt upgrade -y

RUN apt install -y build-essential cmake pkg-config git \
    libjasper1 libjpeg-dev libtiff5-dev libpng-dev libjasper-dev \
    libavcodec-dev libavformat-dev libswscale-dev libdc1394-22-dev libxvidcore-dev libx264-dev libxine2-dev libv4l-dev v4l-utils libgstreamer1.0-dev libgstreamer-plugins-base1.0-dev \
    libgtk-3-dev libatlas-base-dev libeigen3-dev gfortran \
    python3-dev python3-numpy libtbb2 libtbb-dev
RUN apt-get install -y qt5-default qtbase5-dev qtdeclarative5-dev
RUN curl -s https://packagecloud.io/install/repositories/github/git-lfs/script.deb.sh | bash
RUN apt-get install -y git-lfs unzip

RUN echo "Clone project..."
RUN cd ~ && git clone https://github.com/DNATUNA/jewelModelGenerator.git
RUN cd ~/jewelModelGenerator && git lfs pull

RUN echo "build opencv..."
RUN cd ~/jewelModelGenerator && unzip opencv.zip

RUN echo "cmake..."
RUN mkdir ~/jewelModelGenerator/opencv/opencv-3.4.7/build && cd ~/jewelModelGenerator/opencv/opencv-3.4.7/build && cmake -D CMAKE_BUILD_TYPE=DEBUG -D CMAKE_INSTALL_PREFIX=/usr/local -D WITH_TBB=OFF -D WITH_IPP=OFF -D WITH_1394=OFF -D BUILD_WITH_DEBUG_INFO=OFF -D BUILD_DOCS=OFF -D INSTALL_C_EXAMPLES=ON -D INSTALL_PYTHON_EXAMPLES=ON -D BUILD_EXAMPLES=OFF -D BUILD_TESTS=OFF -D BUILD_PERF_TESTS=OFF -D WITH_QT=ON -D WITH_GTK=OFF -D WITH_OPENGL=ON -D OPENCV_EXTRA_MODULES_PATH=../../opencv_contrib-3.4.7/modules -D WITH_V4L=ON -D WITH_FFMPEG=ON -D WITH_XINE=ON -D BUILD_NEW_PYTHON_SUPPORT=ON -D OPENCV_GENERATE_PKGCONFIG=ON ..

RUN echo "make -j..."
RUN cd ~/jewelModelGenerator/opencv/opencv-3.4.7/build && make -j"$(nproc)"

RUN echo "install opencv..."
RUN cd ~/jewelModelGenerator/opencv/opencv-3.4.7/build && make install
RUN echo pkg-config --modversion opencv
RUN echo "Success Install opencv3.4.7!!!"

RUN echo "Start Install opengl!!!"
RUN apt-get install -y build-essential \
    freeglut3-dev libglu1-mesa-dev mesa-common-dev
RUN echo "Success Install opengl!!!"

CMD /bin/bash
