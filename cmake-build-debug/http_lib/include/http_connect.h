#ifndef HTTP_LIB_HTTP_CONNECT_H
#define HTTP_LIB_HTTP_CONNECT_H

#include <curl/curl.h>
#include <string>
#include <iostream>
#include <fstream>
#include <vector>
#include <thread>
#include <mutex>
#include <spdlog/spdlog.h>
using namespace std;

struct FormField {
    std::string key;
    std::string value;
};

struct DownloadState {
    std::string filename;
    std::string path;
    std::string url;
    std::string file_size;
};

class HttpConnection {
public:
    HttpConnection();

    ~HttpConnection();

    bool Post(const string& url, const string& data, string& response);

    bool Get(const string& url, string& response);

    bool Put(const string& url, const string& data, string& response);

    bool Download(const string& url, const string& path, string& response);

    bool PostForm(const string& url, const vector<FormField>& data, string& response);

    bool PostFile(const string& url, const string& file_path, string& response);

    bool PutFile(const string& url, const string& file_path, string& response);

    void Reset();

    bool Download_With_Process(const string& url, const string& path, string& response);

    bool Download_With_Process_And_Pause(const string& url, const string& path, string& response);

    double Get_File_Size(const string& url);


private:
    CURL* curl_ = nullptr;
    mutex curl_muter;

    static size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userp);

    // 回调函数，用于处理HTTP响应头
    static size_t HeaderCallback(void* contents, size_t size, size_t nmemb, void* userp);

    //回调函数，下载进度显示
    static int ProcessCallback(char *progress_data, double t, /* dltotal */ double d, /* dlnow */ double ultotal, double ulnow);

    // 保存下载状态到文件
    static void SaveDownloadState(const DownloadState& state);

    // 从文件中加载下载状态
    static DownloadState LoadDownloadState();
};


#endif //HTTP_LIB_HTTP_CONNECT_H
