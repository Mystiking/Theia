# Theia

An openGL graphics application.

## Build Instructions

    1. apt-get install libglew-dev
    2. apt-get install libglfw3-dev
    3. apt-get install libglm-dev
    4. apt-get install freeglut3-dev
    5. apt-get install libglu1-mesa-dev
    6. apt-get install mesa-common-dev
    7. a) sudo add-apt-repository ppa:ubuntu-toolchain-r/test
       b) sudo apt update
       c) sudo apt install g++-7
       d) sudo update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-7 60 \
                         --slave /usr/bin/g++ g++ /usr/bin/g++-7
       e) sudo update-alternatives --config gcc
    8. make demo
    9. ./demo
