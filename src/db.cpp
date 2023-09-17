#include <fstream>

#include "spdlog/spdlog.h"
#include "nlohmann/json.hpp"

#include "aumdb/db.h"

using json = nlohmann::json;

void AumDb::init() {
    std::ifstream config_stream("config.json");
    json configJson;
    config_stream >> configJson;

    spdlog::info("Parsed config as {}", configJson.dump());

    
}
