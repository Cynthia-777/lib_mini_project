#include <gtest/gtest.h>
#include "../core/http_connect/include/http_connect.h"
#include <filesystem>
#include "../core/thread_pool/thread_pool.h"

class HttpConnectionTest : public testing::Test{
protected:
    void SetUp() override {
        //cout << "enter test!" << endl;
    }

    void TearDown() override {
        //cout << "leave test!" << endl;
    }
};

HttpConnection httpConnection;
string response;

ThreadPool pool(8);

TEST_F(HttpConnectionTest, TestGet) {
string get_url = "http://127.0.0.1:5000/get";
auto result = pool.enqueue(&HttpConnection::Get, &httpConnection, ref(get_url), ref(response));
bool res = result.get();
EXPECT_TRUE(res);
httpConnection.Reset();
}

TEST_F(HttpConnectionTest, TestPut) {
string put_url = "http://127.0.0.1:5000/put";
string put_data = "put_data";
auto result = pool.enqueue(&HttpConnection::Put, &httpConnection, ref(put_url), ref(put_data), ref(response));
bool res = result.get();
EXPECT_TRUE(res);
httpConnection.Reset();
}

TEST_F(HttpConnectionTest, TestPost) {
string post_url = "http://127.0.0.1:5000/post_data";
string post_data = "post_data";
auto result = pool.enqueue(&HttpConnection::Post, &httpConnection, ref(post_url), ref(post_data), ref(response));
bool res = result.get();
EXPECT_TRUE(res);
httpConnection.Reset();
}

TEST_F(HttpConnectionTest, TestPostForm) {
string post_form_url = "http://127.0.0.1:5000/post_form";
vector<FormField> form_data;
form_data.push_back({"content", "post_form_data_value"});
auto result = pool.enqueue(&HttpConnection::PostForm, &httpConnection, ref(post_form_url), ref(form_data), ref(response));
bool res = result.get();
EXPECT_TRUE(res);
httpConnection.Reset();
}

TEST_F(HttpConnectionTest, TestPostFile) {
string post_file_url = "http://127.0.0.1:5000/post_file";
string file_path = "../CMakeLists.txt";
auto result = pool.enqueue(&HttpConnection::PostFile, &httpConnection, ref(post_file_url), ref(file_path), ref(response));
bool res = result.get();
EXPECT_TRUE(res);
httpConnection.Reset();
}

TEST_F(HttpConnectionTest, TestPutFile) {
string put_file_url = "http://127.0.0.1:5000/put_file";
string file_path = "../CMakeLists.txt";
auto result = pool.enqueue(&HttpConnection::PutFile, &httpConnection, ref(put_file_url), ref(file_path), ref(response));
bool res = result.get();
EXPECT_TRUE(res);
httpConnection.Reset();
}

TEST_F(HttpConnectionTest, TestDownload) {
string download_path = "../download/";
string download_url = "http://127.0.0.1:5000/download";
auto result = pool.enqueue(&HttpConnection::Download, &httpConnection, ref(download_url), ref(download_path), ref(response));
bool res = result.get();
EXPECT_TRUE(res);
httpConnection.Reset();
}

TEST_F(HttpConnectionTest, TestDownloadWithProcess) {
string download_path = "../download/";
string download_url = "http://127.0.0.1:5000/download";
auto result = pool.enqueue(&HttpConnection::Download_With_Process, &httpConnection, ref(download_url), ref(download_path), ref(response));
bool res = result.get();
EXPECT_TRUE(res);
httpConnection.Reset();
}

TEST_F(HttpConnectionTest, TestDownloadWithProcessAndPause) {
string download_path = "../download/";
string download_url = "http://127.0.0.1:5000/download";
auto result = pool.enqueue(&HttpConnection::Download_With_Process_And_Pause, &httpConnection, ref(download_url), ref(download_path), ref(response));
bool res = result.get();
EXPECT_TRUE(res);
httpConnection.Reset();
}