// WhmIO.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//
#include "../common/stdinc.h"
#include "rscio.h"

int main(int argc, char* argv[])
{
    if (argc == 2)
    {
        auto uncompressed = rscio::read_rsc(argv[1]);

        std::cout << uncompressed.size() << std::endl;
    }

    return 0;
}
