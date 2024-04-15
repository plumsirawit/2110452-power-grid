FROM ubuntu:22.04
RUN DEBIAN_FRONTEND=noninteractive \
    apt-get update \
    && apt-get install -y gcc g++ \
    && rm -rf /var/lib/apt/lists/*
ADD src /src
RUN g++ -std=c++17 -O3 /src/main.cpp /src/util.cpp /src/solve.cpp -DMAX_THREADS=8 -o /main.out
ENTRYPOINT [ "/main.out" ]
