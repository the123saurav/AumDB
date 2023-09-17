#ifndef AUMDB_DB_H
#define AUMDB_DB_H

#include <string>
#include <vector>
#include <variant>
#include <memory>

namespace AumDb {

    // Enums for error codes
    enum class ErrorCode {
        None,
        BadInput,
        DBNotStarted,
        DBAlreadyStarted,
        ResourceAlreadyExists,

        RuntimeError
    };

    // Cursor class for result traversal
    class Cursor {
    public:
        // Fetches the next row in the result set
        virtual bool fetch_next() = 0;

        // Retrieves the data in the given column index
        virtual std::variant<int> get_column(unsigned short col_idx) = 0;

        virtual ~Cursor() = 0;
    };

    /*
        Base Result class for query execution results.
        A caller can always cast to it if he just wants to know if it was successful
        e.g INSERT/UPDATE/DELETE/CREATE
        OR subclass to types for more info where applicable.
    */ 
    struct Result {
        std::unique_ptr<ErrorCode> error_code_;
        std::unique_ptr<std::string> err_msg_;

        Result();

        // Constructor to initialize error information
        Result(ErrorCode ec, std::string e);
    };

    // Result class for INSERT operations
    struct CreateResult: public Result {
        using Result::Result;
    };

    // Result class for INSERT operations
    struct InsertResult: public Result {
        using Result::Result;
    };

    // Result class for SELECT operations
    struct SelectResult: public Result {
        std::unique_ptr<Cursor> cursor;  // Cursor for result set traversal

        SelectResult(std::unique_ptr<Cursor> c, ErrorCode ec, std::string e);
    };

    // Public API for starting the database
    Result start();

    // Public API for executing a query; returns a unique pointer to a Result object
    Result executeQuery(const std::string& query);

    // Public API for stopping the database
    void stop();

}  // namespace AumDb

#endif  // AUMDB_DB_H