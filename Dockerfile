FROM ubuntu:22.04
RUN DEBIAN_FRONTEND=noninteractive \
    apt-get update \
    && apt-get install -y gcc g++ \
    && rm -rf /var/lib/apt/lists/*
ADD main.cpp /main.cpp
RUN g++ -std=c++17 -O3 /main.cpp -o /main.out
ENTRYPOINT [ "/main.out" ]
