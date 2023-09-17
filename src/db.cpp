#include <fstream>
#include <sstream>

#include "spdlog/spdlog.h"
#include "nlohmann/json.hpp"

#include "aumdb/db.h"
#include "aumdb/core/engine.h"

using json = nlohmann::json;

static AumDb::Core::Engine* engine = nullptr;

AumDb::Result AumDb::start() {    
    if (engine != nullptr) {
        return AumDb::Result(AumDb::ErrorCode::DBAlreadyStarted, "DB already started");
    }
    spdlog::info("Starting DB");

    std::ifstream config_stream("config.json");
    json config_json;
    config_stream >> config_json;

    spdlog::info("Parsed config as {}", config_json.dump());

    engine = &AumDb::Core::Engine::get_instance();

    spdlog::info("Starting engine");
    engine->start(config_json);

    return AumDb::Result();
}

void AumDb::stop() {
    if (engine == nullptr) {
        return;
    }
    spdlog::info("Stopping engine");
    engine->stop();
    engine = nullptr;
}

AumDb::Result AumDb::executeQuery(const std::string& query) {
    if (engine == nullptr) {
        return AumDb::Result(AumDb::ErrorCode::DBNotStarted, "DB not started");
    }
    return engine->executeQuery(query);
}

AumDb::Result::Result() {
}

AumDb::Result::Result(ErrorCode ec, std::string e): 
    error_code_(std::make_unique<AumDb::ErrorCode>(ec)), 
    err_msg_(std::make_unique<std::string>(std::move(e))){}






