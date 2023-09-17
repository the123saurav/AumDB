#include <iostream>

#include "spdlog/spdlog.h"

#include "aumdb/db.h"

using namespace std;

int main()
{
    spdlog::info("Hello");
    AumDb::init();
}