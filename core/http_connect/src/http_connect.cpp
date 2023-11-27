#include "../include/http_connect.h"

HttpConnection::HttpConnection() {
    curl_global_init(CURL_GLOBAL_ALL);
    curl_ = curl_easy_init();
}

HttpConnection::~HttpConnection() {
    curl_easy_cleanup(curl_);
    curl_global_cleanup();
}

//保存响应数据
size_t HttpConnection::WriteCallback(void* contents, size_t size, size_t nmemb, void* userp) {
    size_t realsize = size * nmemb;
    string* str = static_cast<string*>(userp);
    str->append(static_cast<char*>(contents), realsize);
    return realsize;
}

//回调函数，用于处理HTTP响应头
size_t HttpConnection::HeaderCallback(void* contents, size_t size, size_t nmemb, void* userp) {
    const size_t totalSize = size * nmemb;
    const size_t headerSize = totalSize - 2; // 去除换行符

    // 将响应头的数据存储到字符串中
    std::string header(static_cast<char*>(contents), headerSize);
    std::string filename;

    // 从响应头中提取文件名
    const std::string prefix = "Content-Disposition: attachment; filename=";
    size_t pos = header.find(prefix);
    if (pos != std::string::npos) {
        pos += prefix.size();
        size_t endPos = header.size();
        if (endPos != std::string::npos) {
            filename = header.substr(pos, endPos - pos);
            // 将文件名存储到用户数据中
            std::string* filenamePtr = static_cast<std::string*>(userp);
            *filenamePtr = filename;
        }
    }
    return totalSize;
}

bool HttpConnection::Post(const string& url, const string& data, string& response) {
    spdlog::info("enter Post! url = {}, post_data = {}", url, data);
    Logger::daily_logger->info("enter Post! url = {}, post_data = {}", url, data);
    lock_guard<mutex> lock(curl_mutex);
    if (!curl_) {
        spdlog::error("curl_ is null!!!");
        Logger::daily_logger->error("curl_ is null!!!");
        return false;
    }

    curl_easy_setopt(curl_, CURLOPT_URL, url.c_str());//设置请求url
    curl_easy_setopt(curl_, CURLOPT_POST, 1L);//设置请求方法为post
    struct curl_slist *headers = NULL;
    headers = curl_slist_append(headers, "Content-Type: application/json");
    curl_easy_setopt(curl_, CURLOPT_HTTPHEADER, headers);//设置请求头
    curl_easy_setopt(curl_, CURLOPT_POSTFIELDS, data.c_str());//设置请求数据
    curl_easy_setopt(curl_, CURLOPT_WRITEFUNCTION, &WriteCallback);//设置响应数据处理回调函数
    curl_easy_setopt(curl_, CURLOPT_WRITEDATA, &response);

    CURLcode res = curl_easy_perform(curl_);//执行请求
    spdlog::debug("response = {}", response);
    Logger::daily_logger->debug("response = {}", response);
    spdlog::info("leave Post!");
    Logger::daily_logger->info("leave Post!");
    return (res == CURLE_OK);
}

bool HttpConnection::Get(const string& url, string& response) {
    spdlog::info("enter Get! url = {}", url);
    Logger::daily_logger->info("enter Get! url = {}", url);
    lock_guard<mutex> lock(curl_mutex);
    if (!curl_) {
        spdlog::error("curl_ is null!!!");
        Logger::daily_logger->error("curl_ is null!!!");
        return false;
    }

    curl_easy_setopt(curl_, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl_, CURLOPT_WRITEFUNCTION, &WriteCallback);
    curl_easy_setopt(curl_, CURLOPT_WRITEDATA, &response);

    struct curl_slist *headers = NULL;
    headers = curl_slist_append(headers, "Accept: application/json");
    curl_easy_setopt(curl_, CURLOPT_HTTPHEADER, headers);

    CURLcode res = curl_easy_perform(curl_);
    spdlog::debug("response = {}", response);
    Logger::daily_logger->debug("response = {}", response);
    spdlog::info("leave Get!");
    Logger::daily_logger->info("leave Get!");
    return (res == CURLE_OK);
}

bool HttpConnection::Put(const string& url, const string& data, string& response) {
    spdlog::info("enter Put! url = {}, put_data = {}", url, data);
    Logger::daily_logger->info("enter Put! url = {}, put_data = {}", url, data);
    lock_guard<mutex> lock(curl_mutex);
    if (!curl_) {
        spdlog::error("curl_ is null!!!");
        Logger::daily_logger->error("curl_ is null!!!");
        return false;
    }

    curl_easy_setopt(curl_, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl_, CURLOPT_CUSTOMREQUEST, "PUT");
    struct curl_slist *headers = NULL;
    headers = curl_slist_append(headers, "Content-Type: application/json");
    curl_easy_setopt(curl_, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(curl_, CURLOPT_POSTFIELDS, data.c_str());
    curl_easy_setopt(curl_, CURLOPT_WRITEFUNCTION, &WriteCallback);
    curl_easy_setopt(curl_, CURLOPT_WRITEDATA, &response);

    CURLcode res = curl_easy_perform(curl_);
    spdlog::debug("response = {}", response);
    Logger::daily_logger->debug("response = {}", response);
    spdlog::info("leave Put!");
    Logger::daily_logger->info("leave Put!");
    return (res == CURLE_OK);
}

bool HttpConnection::Download(const std::string &url, const string& path, std::string &response) {
    spdlog::info("enter Download! url = {}, path = {}", url, path);
    Logger::daily_logger->info("enter Download! url = {}, path = {}", url, path);
    lock_guard<mutex> lock(curl_mutex);
    if (!curl_) {
        spdlog::error("curl_ is null!!!");
        Logger::daily_logger->error("curl_ is null!!!");
        return false;
    }
    string filename;
    string path_filename = path;

    // 设置回调函数，用于处理HTTP响应头
    curl_easy_setopt(curl_, CURLOPT_HEADERFUNCTION, HeaderCallback);
    curl_easy_setopt(curl_, CURLOPT_HEADERDATA, &filename);

    // 设置回调函数，用于保存下载的文件内容
    //std::string fileContent;
    curl_easy_setopt(curl_, CURLOPT_WRITEFUNCTION, WriteCallback);
    //curl_easy_setopt(curl_, CURLOPT_WRITEDATA, &fileContent);
    curl_easy_setopt(curl_, CURLOPT_WRITEDATA, &response);

    // 发送HTTP GET请求
    curl_easy_setopt(curl_, CURLOPT_URL, url.c_str());
    CURLcode res = curl_easy_perform(curl_);

    if (res == CURLE_OK) {
        // 保存文件到磁盘（或者进行其他处理）
        path_filename = path_filename.append(filename);
        std::ofstream file(path_filename, std::ios::binary);
        file.write(response.c_str(), response.length());
        file.close();
        spdlog::debug("response = {}", response);
        Logger::daily_logger->debug("response = {}", response);
        spdlog::info("leave Download! File downloaded and saved in: {}", path_filename);
        Logger::daily_logger->info("leave Download! File downloaded and saved in: {}", path_filename);
        return true;
    }

    spdlog::error("Failed to download file: {}", curl_easy_strerror(res));
    Logger::daily_logger->error("Failed to download file: {}", curl_easy_strerror(res));
    return false;

}


void HttpConnection::Reset() {
    lock_guard<mutex> lock(curl_mutex);
    curl_easy_reset(curl_);
}


bool HttpConnection::PostForm(const string& url, const vector<FormField>& data, string& response){
    spdlog::info("enter PostForm! url = {}", url);
    Logger::daily_logger->info("enter PostForm! url = {}", url);
    for(auto& field : data){
        spdlog::info("Form field: key = {}, value = {}", field.key, field.value);
        Logger::daily_logger->info("Form field: key = {}, value = {}", field.key, field.value);
    }
    lock_guard<mutex> lock(curl_mutex);
    if (!curl_) {
        spdlog::error("curl_ is null!!!");
        Logger::daily_logger->error("curl_ is null!!!");
        return false;
    }

    // 创建一个新表单
    curl_httppost* post = NULL;
    curl_httppost* last = NULL;

    // 遍历字段并添加到表单中
    for (const FormField& item : data) {
        curl_formadd(&post, &last, CURLFORM_COPYNAME, item.key.c_str(), CURLFORM_COPYCONTENTS, item.value.c_str(), CURLFORM_END);
    }

    curl_easy_setopt(curl_, CURLOPT_URL, url.c_str());
    // 设置表单数据
    curl_easy_setopt(curl_, CURLOPT_HTTPPOST, post);
    curl_easy_setopt(curl_, CURLOPT_WRITEFUNCTION, &WriteCallback);
    curl_easy_setopt(curl_, CURLOPT_WRITEDATA, &response);

    CURLcode res = curl_easy_perform(curl_);
    spdlog::debug("response = {}", response);
    Logger::daily_logger->debug("response = {}", response);
    spdlog::info("leave PostForm!");
    Logger::daily_logger->info("leave PostForm!");
    return (res == CURLE_OK);
}


bool HttpConnection::PostFile(const string& url, const string& file_path, string& response){
    spdlog::info("enter PostFile! url = {}， file_path = {}", url, file_path);
    Logger::daily_logger->info("enter PostFile! url = {}， file_path = {}", url, file_path);
    lock_guard<mutex> lock(curl_mutex);
    if (!curl_) {
        spdlog::error("curl_ is null!!!");
        Logger::daily_logger->error("curl_ is null!!!");
        return false;
    }

    // 创建一个新表单
    struct curl_httppost* formpost = nullptr;
    struct curl_httppost* lastptr = nullptr;

    // 添加文件字段
    curl_formadd(&formpost, &lastptr,
                 CURLFORM_COPYNAME, "file",
                 CURLFORM_FILE, file_path.c_str(),
                 CURLFORM_END);

    curl_easy_setopt(curl_, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl_, CURLOPT_POST, 1L);
    // 设置表单数据
    curl_easy_setopt(curl_, CURLOPT_HTTPPOST, formpost);
    curl_easy_setopt(curl_, CURLOPT_WRITEFUNCTION, &WriteCallback);
    curl_easy_setopt(curl_, CURLOPT_WRITEDATA, &response);

    CURLcode res = curl_easy_perform(curl_);
    spdlog::debug("response = {}", response);
    Logger::daily_logger->debug("response = {}", response);
    spdlog::info("leave PostFile!");
    Logger::daily_logger->info("leave PostFile!");
    return (res == CURLE_OK);
}

bool HttpConnection::PutFile(const string& url, const string& file_path, string& response){
    spdlog::info("enter PutFile! url = {}， file_path = {}", url, file_path);
    Logger::daily_logger->info("enter PutFile! url = {}， file_path = {}", url, file_path);
    lock_guard<mutex> lock(curl_mutex);
    if (!curl_) {
        spdlog::error("curl_ is null!!!");
        Logger::daily_logger->error("curl_ is null!!!");
        return false;
    }

    FILE* localFile = fopen(file_path.c_str(), "rb");
    if (!localFile) {
        spdlog::error("Failed to open local file!!!");
        Logger::daily_logger->error("Failed to open local file!!!");
        return false;
    }

    // 创建一个新的CURLFORM
    curl_mime *mime = curl_mime_init(curl_);

    // 添加文件
    curl_mimepart *part = curl_mime_addpart(mime);
    curl_mime_name(part, "file");  // 此处设置表单字段名
    curl_mime_filedata(part, file_path.c_str());

    // 设置libcurl选项
    curl_easy_setopt(curl_, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl_, CURLOPT_CUSTOMREQUEST, "PUT");
    curl_easy_setopt(curl_, CURLOPT_MIMEPOST, mime);  // 设置上传的表单数据
    curl_easy_setopt(curl_, CURLOPT_WRITEFUNCTION, &WriteCallback);
    curl_easy_setopt(curl_, CURLOPT_WRITEDATA, &response);

    CURLcode res = curl_easy_perform(curl_);

    curl_mime_free(mime);  // 释放CURLFORM对象

    fclose(localFile);
    spdlog::debug("response = {}", response);
    Logger::daily_logger->debug("response = {}", response);
    spdlog::info("leave PostFile!");
    Logger::daily_logger->info("leave PostFile!");
    return (res == CURLE_OK);
}

int HttpConnection::ProcessCallback(char *progress_data, double t, /* dltotal */ double d, /* dlnow */ double ultotal, double ulnow)
{
    if (t > 0) {
        printf("%s %g / %g (%g %%)\n", progress_data, d, t, d * 100.0 / t);
    }
    return 0;
}

bool HttpConnection::Download_With_Process(const string& url, const string& path, string& response){
    spdlog::info("enter Download_With_Process! url = {}， path = {}", url, path);
    Logger::daily_logger->info("enter Download_With_Process! url = {}， path = {}", url, path);
    lock_guard<mutex> lock(curl_mutex);
    if(!curl_){
        spdlog::error("curl_ is null!!!");
        Logger::daily_logger->error("curl_ is null!!!");
        return false;
    }
    string filename;
    string path_filename = path;

    // 设置回调函数，用于处理HTTP响应头
    curl_easy_setopt(curl_, CURLOPT_HEADERFUNCTION, HeaderCallback);
    curl_easy_setopt(curl_, CURLOPT_HEADERDATA, &filename);

    // 设置回调函数，用于保存下载的文件内容
    curl_easy_setopt(curl_, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curl_, CURLOPT_WRITEDATA, &response);

    //显示下载进度
    string progress_data = "* ";
    curl_easy_setopt(curl_, CURLOPT_NOPROGRESS, false);
    curl_easy_setopt(curl_, CURLOPT_PROGRESSFUNCTION, &ProcessCallback);
    curl_easy_setopt(curl_, CURLOPT_PROGRESSDATA, progress_data.c_str());

    // 发送HTTP GET请求
    curl_easy_setopt(curl_, CURLOPT_URL, url.c_str());
    CURLcode res = curl_easy_perform(curl_);

    if (res == CURLE_OK) {
        // 保存文件到磁盘（或者进行其他处理）
        path_filename = path_filename.append(filename);
        std::ofstream file(path_filename, std::ios::binary);
        file.write(response.c_str(), response.length());
        file.close();
        spdlog::debug("response = {}", response);
        Logger::daily_logger->debug("response = {}", response);
        spdlog::info("leave Download_With_Process! File downloaded and saved in: {}", path_filename);
        Logger::daily_logger->info("leave Download_With_Process! File downloaded and saved in: {}", path_filename);
        return true;
    }

    spdlog::error("Failed to download file: {}", curl_easy_strerror(res));
    Logger::daily_logger->error("Failed to download file: {}", curl_easy_strerror(res));
    return false;
}

bool HttpConnection::Download_With_Process_And_Pause(const string& url, const string& path, string& response) {
    spdlog::info("enter Download_With_Process_And_Pause! url = {}， path = {}", url, path);
    Logger::daily_logger->info("enter Download_With_Process_And_Pause! url = {}， path = {}", url, path);
    lock_guard<mutex> lock(curl_mutex);
    if(!curl_) {
        spdlog::error("curl_ is null!!!");
        Logger::daily_logger->error("curl_ is null!!!");
        return false;
    }

    string filename;
    string path_filename = path;

    // 从文件中加载下载状态
    DownloadState state = LoadDownloadState();

    // 设置回调函数，用于处理HTTP响应头
    curl_easy_setopt(curl_, CURLOPT_HEADERFUNCTION, HeaderCallback);
    curl_easy_setopt(curl_, CURLOPT_HEADERDATA, &filename);

    // 设置回调函数，用于保存下载的文件内容
    curl_easy_setopt(curl_, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curl_, CURLOPT_WRITEDATA, &response);

    // 设置断点续传
    if (!state.file_size.empty()) {
        spdlog::debug("Start resume download");
        Logger::daily_logger->debug("Start resume download");
        curl_easy_setopt(curl_, CURLOPT_RESUME_FROM, std::stoll(state.file_size));
    }

    // 显示下载进度
    string progress_data = "* ";
    curl_easy_setopt(curl_, CURLOPT_NOPROGRESS, false);
    curl_easy_setopt(curl_, CURLOPT_PROGRESSFUNCTION, &ProcessCallback);
    curl_easy_setopt(curl_, CURLOPT_PROGRESSDATA, progress_data.c_str());

    // 发送HTTP GET请求
    curl_easy_setopt(curl_, CURLOPT_URL, url.c_str());

    CURLcode res = curl_easy_perform(curl_);

    //system("read -p 'Press Enter to continue...' var");

    if (res == CURLE_OK) {
        // 保存文件到磁盘（或者进行其他处理）
        path_filename = path_filename.append(filename);
        std::ofstream file(path_filename, std::ios::binary | std::ios::app);  // 追加模式
        file.write(response.c_str(), response.length());
        file.close();

        // 删除下载状态文件
        //std::remove("download_state.txt");
        spdlog::debug("response = {}", response);
        Logger::daily_logger->debug("response = {}", response);
        spdlog::info("leave Download_With_Process_And_Pause! File downloaded and saved in: {}", path_filename);
        Logger::daily_logger->info("leave Download_With_Process_And_Pause! File downloaded and saved in: {}", path_filename);
        return true;
    } else {
        // 保存下载状态到文件
        state.filename = filename;
        state.path = path;
        state.url = url;
        state.file_size = std::to_string(std::stoll(state.file_size) + response.length());  // 更新文件大小
        SaveDownloadState(state);
        spdlog::debug("save download state!");
        Logger::daily_logger->debug("save download state!");
    }

    spdlog::error("Failed to download file: {}", curl_easy_strerror(res));
    Logger::daily_logger->error("Failed to download file: {}", curl_easy_strerror(res));
    return false;
}


void HttpConnection::SaveDownloadState(const DownloadState& state) {
    std::ofstream state_file("download_state.txt");
    state_file << state.filename << std::endl;
    state_file << state.path << std::endl;
    state_file << state.url << std::endl;
    state_file << state.file_size << std::endl; // 保存为字符串
}

DownloadState HttpConnection::LoadDownloadState() {
    DownloadState state;
    std::ifstream state_file("download_state.txt");
    if (state_file.is_open()) {
        std::getline(state_file, state.filename);
        std::getline(state_file, state.path);
        std::getline(state_file, state.url);
        // 读取为字符串后再转换为 std::streampos
        std::string file_size_str;
        std::getline(state_file, file_size_str);
        state.file_size = file_size_str;
    }
    return state;
}

