# 2110452-power-grid

Instructions given by [this document](https://cdn.discordapp.com/attachments/945538048127541281/1229371279849558057/166_2023-TermProject.pdf?ex=662f7039&is=661cfb39&hm=1524274b047c6eb5ac5cd84d5dcea6c29db14a234d763a85dcbbac6748fa4deb&).

## Usage

Run `bash runner.sh` to test against all test data in `data/input/`.

## Compilation and Running

Everything is given in the Dockerfile, so one can `docker build` directly. However, if one wants to compile and run outside docker, use

```
<CC> -std=c++17 -O3 src/main.cpp src/util.cpp src/solve.cpp -DMAX_THREADS=<NUM> -o main
```

where `<CC>` is the compiler (usually `g++`), and `<NUM>` is the maximum number of threads (omitting `-DMAX_THREADS=<NUM>` gives the default value as 12). Call `./main <INPUTFILE> <OUTPUTFILE>` to run the program.

## GitHub Actions

Unfortunately, GitHub-hosted runner gives only 4 cores maximum, so the author assumes that the maximum number of threads would be 8. The action is triggered automatically upon all pushes to `deploy` branch. One can get the results of the builds by going to the `Actions` tab on GitHub, and then go to one of the finished builds. Once inside, one can see the `Artifacts` tab at the bottom of the page. Currently there should be three of them:

- `run-report`: Report of the run, i.e. time usage.
- `run-log`: Log of the run. This includes the standard error output from Docker and `./main`.
- `run-output`: Output of the run. This is intended to be used to check for correctness.
