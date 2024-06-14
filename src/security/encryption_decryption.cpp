#include <boost/beast.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <string>
#include <nlohmann/json.hpp>
#include <iostream>

#include "../files/utils.hpp"
#include "../system/log.hpp"
#include "utils.hpp"
#include "encryption_decryption.hpp"

using json = nlohmann::json;
namespace beast = boost::beast;
namespace http = beast::http;
using tcp = boost::asio::ip::tcp;

std::string privateKeyStr = readFile("./config/privateKey.pem");
std::string publicKeyStr = readFile("./config/publicKey.pem");

void encryptData(tcp::socket &&socket, http::request<http::string_body> &&req)
{
    try
    {
        initializeOpenSSLErrors();

        const std::string &body = req.body();

        json parsedJson = json::parse(body);
        std::string data = parsedJson.at("data").get<std::string>();

        std::string encryptedEncodedData = rsaEncrypt(data, publicKeyStr);

        cleanupOpenSSLErrors();

        http::response<http::string_body> res{http::status::ok, req.version()};
        res.set(http::field::server, "c++");
        res.set(http::field::content_type, "text/plain");
        res.body() = encryptedEncodedData;
        res.prepare_payload();
        http::write(socket, res);
    }
    catch (const json::parse_error &e)
    {
        std::string errorMsg = "JSON Parsing error: " + std::string(e.what());
        saveLog("ERROR", errorMsg);
        http::response<http::string_body> res{http::status::bad_request, req.version()};
        res.set(http::field::server, "c++");
        res.set(http::field::content_type, "text/plain");
        res.body() = errorMsg;
        res.prepare_payload();
        http::write(socket, res);
    }
    catch (const json::type_error &e)
    {
        std::string errorMsg = "JSON Data access error: " + std::string(e.what());
        saveLog("ERROR", errorMsg);
        http::response<http::string_body> res{http::status::bad_request, req.version()};
        res.set(http::field::server, "c++");
        res.set(http::field::content_type, "text/plain");
        res.body() = errorMsg;
        res.prepare_payload();
        http::write(socket, res);
    }
    catch (const std::exception &e)
    {
        std::string errorMsg = "Server error: " + std::string(e.what());
        saveLog("ERROR", errorMsg);
        http::response<http::string_body> res{http::status::internal_server_error, req.version()};
        res.set(http::field::server, "c++");
        res.set(http::field::content_type, "text/plain");
        res.body() = errorMsg;
        res.prepare_payload();
        http::write(socket, res);
    }
}

void decryptData(tcp::socket &&socket, http::request<http::string_body> &&req)
{
    try
    {
        initializeOpenSSLErrors();

        const std::string &body = req.body();

        json parsedJson = json::parse(body);
        std::string data = parsedJson.at("data").get<std::string>();

        std::string decryptedData = rsaDecrypt(data, privateKeyStr);

        cleanupOpenSSLErrors();

        http::response<http::string_body> res{http::status::ok, req.version()};
        res.set(http::field::server, "c++");
        res.set(http::field::content_type, "text/plain");
        res.body() = decryptedData;
        res.prepare_payload();
        http::write(socket, res);
    }
    catch (const json::parse_error &e)
    {
        std::string errorMsg = "JSON Parsing error: " + std::string(e.what());
        saveLog("ERROR", errorMsg);
        http::response<http::string_body> res{http::status::bad_request, req.version()};
        res.set(http::field::server, "c++");
        res.set(http::field::content_type, "text/plain");
        res.body() = errorMsg;
        res.prepare_payload();
        http::write(socket, res);
    }
    catch (const json::type_error &e)
    {
        std::string errorMsg = "JSON Data access error: " + std::string(e.what());
        saveLog("ERROR", errorMsg);
        http::response<http::string_body> res{http::status::bad_request, req.version()};
        res.set(http::field::server, "c++");
        res.set(http::field::content_type, "text/plain");
        res.body() = errorMsg;
        res.prepare_payload();
        http::write(socket, res);
    }
    catch (const std::exception &e)
    {
        std::string errorMsg = "Server error: " + std::string(e.what());
        saveLog("ERROR", errorMsg);
        http::response<http::string_body> res{http::status::internal_server_error, req.version()};
        res.set(http::field::server, "c++");
        res.set(http::field::content_type, "text/plain");
        res.body() = errorMsg;
        res.prepare_payload();
        http::write(socket, res);
    }
}
