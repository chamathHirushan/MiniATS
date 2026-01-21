#pragma once

#ifndef ASIO_STANDALONE
#define ASIO_STANDALONE
#endif

#ifdef _WIN32
  #ifndef _WIN32_WINNT
    #define _WIN32_WINNT 0x0601
  #endif
#endif

#include <asio.hpp>

// Undefine macros that might conflict with Crow
#ifdef DELETE
#undef DELETE
#endif
#ifdef POST
#undef POST
#endif
#ifdef OPTIONS
#undef OPTIONS
#endif
#ifdef GET
#undef GET
#endif
#ifdef PUT
#undef PUT
#endif
#ifdef HEAD
#undef HEAD
#endif

#include "crow_all.hpp"

// CORS Middleware Definition
struct CORS {
    struct context {};
    
    void before_handle(crow::request& req, crow::response& res, context& ctx) {
        // Handle CORS preflight
        // Using explicit cast to avoid potential enum ambiguity issues
        if (req.method == (crow::HTTPMethod)6) { // 6 is typically OPTIONS in Crow
            res.add_header("Access-Control-Allow-Origin", "*");
            res.add_header("Access-Control-Allow-Methods", "GET, POST, PATCH, PUT, DELETE, OPTIONS");
            res.add_header("Access-Control-Allow-Headers", "Content-Type, Authorization, Accept");
            res.code = 204;
            res.end();
        }
    }
    
    void after_handle(crow::request& req, crow::response& res, context& ctx) {
        res.add_header("Access-Control-Allow-Origin", "*");
        res.add_header("Access-Control-Allow-Methods", "GET, POST, PATCH, PUT, DELETE, OPTIONS");
        res.add_header("Access-Control-Allow-Headers", "Content-Type, Authorization, Accept");
    }
};
