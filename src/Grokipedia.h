#ifndef GROKIPEDIA_H
#define GROKIPEDIA_H

#include <cpprest/http_client.h>
#include <cpprest/json.h>
#include <pplx/pplx.h>
#include <iostream>
#include <string>
#include <vector>

using namespace web;
using namespace web::http;
using namespace web::http::client;

class Grokipedia {
private:
    std::string api_base = "https://grokipedia.com/api";
    http_client_config client_config;
    http_request create_request(const std::string& endpoint, const std::string& method, const std::string& data = "") {
        http_request request;
        
        if (method == "GET") {
            request.set_method(methods::GET);
        } else if (method == "POST") {
            request.set_method(methods::POST);
        } else if (method == "PUT") {
            request.set_method(methods::PUT);
        } else if (method == "DEL") {
            request.set_method(methods::DEL);
        }
        
        request.set_request_uri(utility::conversions::to_string_t(endpoint));
        
        // Set headers
        request.headers().add(U("Host"), U("grokipedia.com"));
        request.headers().add(U("User-Agent"), U("Mozilla/5.0 (X11; Linux x86_64; rv:109.0) Gecko/20100101 Firefox/115.0"));
        
        if (!data.empty() && (method == "POST" || method == "PUT")) {
            request.set_body(data);
        }
        
        return request;
    }
    
    pplx::task<json::value> make_api_call(const std::string& endpoint, const std::string& method, const std::string& data = "") {
        http_client client(utility::conversions::to_string_t(api_base), client_config);
        auto request = create_request(endpoint, method, data);

        return client.request(request)
            .then([](http_response response) {
                if (response.status_code() == status_codes::OK) {
                    return response.extract_json();
                } else {
                    json::value error_obj;
                    error_obj[U("error")] = json::value::string(
                        U("HTTP Error: ") + utility::conversions::to_string_t(std::to_string(response.status_code())));
                    error_obj[U("success")] = json::value::boolean(false);
                    return pplx::task_from_result(error_obj);
                }
            })
            .then([](pplx::task<json::value> previousTask) {
                try {
                    return previousTask.get();
                } catch (const std::exception& e) {
                    json::value error_obj;
                    error_obj[U("error")] = json::value::string(
                        U("Exception: ") + utility::conversions::to_string_t(e.what()));
                    error_obj[U("success")] = json::value::boolean(false);
                    return error_obj;
                }
            });
    }

public:
    Grokipedia(){
        client_config.set_validate_certificates(false);
    }

    // Areas
    pplx::task<json::value> search(const std::string& query,int limit=20,int offset=0) {
        return make_api_call("/full-text-search?query=" + utility::conversions::to_utf8string(web::uri::encode_data_string(utility::conversions::to_string_t(query))) + "&limit=" + std::to_string(limit) + "&offset=" + std::to_string(offset),"GET");
    }

    pplx::task<json::value> get_page(const std::string& slug,bool includeContent=true,bool validateLinks=true) {
        return make_api_call("/page?slug=" + utility::conversions::to_utf8string(web::uri::encode_data_string(utility::conversions::to_string_t(slug))) + "&includeContent=" + utility::conversions::to_utf8string(web::uri::encode_data_string(utility::conversions::to_string_t(includeContent ? "true" : "false"))) + "&validateLinks=" + utility::conversions::to_utf8string(web::uri::encode_data_string(utility::conversions::to_string_t(validateLinks ? "true" : "false"))),"GET");
    }
};

#endif
