#include "services/mail.hpp"

#include <curl/curl.h>

#include <cstring>
#include <iostream>
#include <algorithm>

namespace {

struct UploadPayload {
    std::string data;
    size_t offset{};
};

size_t readPayload(char* buffer, size_t size, size_t count, void* userdata) {
    // libcurl 通过回调分段读取待发送的邮件正文。
    auto* payload = static_cast<UploadPayload*>(userdata);
    const size_t capacity = size * count;
    const size_t remaining = payload->data.size() - payload->offset;
    const size_t amount = std::min(capacity, remaining);

    if (amount > 0) {
        std::memcpy(buffer, payload->data.data() + payload->offset, amount);
        payload->offset += amount;
    }
    return amount;
}

}  // namespace

bool sendVerificationMail(const Config& config,
                          const std::string& recipient,
                          const std::string& code) {
    // email.enabled=false 时进入开发模式，只在服务端输出验证码。
    if (configValue(config, "email.enabled", "false") != "true") {
        std::cout << "[DEV] verification code for " << recipient << ": " << code << '\n';
        return true;
    }

    CURL* curl = curl_easy_init();
    if (!curl) {
        std::cerr << "[MAIL] curl init failed\n";
        return false;
    }

    const auto from = configValue(config, "email.from", "");
    const auto url = "smtps://" + configValue(config, "email.smtp_host", "smtp.qq.com") +
                     ":" + configValue(config, "email.smtp_port", "465");
    const auto fromBox = "<" + from + ">";
    const auto recipientBox = "<" + recipient + ">";

    curl_slist* recipients = nullptr;
    recipients = curl_slist_append(recipients, recipientBox.c_str());

    UploadPayload payload{
        "From: " + from + "\r\nTo: " + recipient +
            "\r\nSubject: Tetris verification code\r\n"
            "MIME-Version: 1.0\r\nContent-Type: text/plain; charset=utf-8\r\n\r\n"
            "Your verification code is: " + code + "\r\n",
        0};

    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_USERNAME, from.c_str());
    curl_easy_setopt(curl, CURLOPT_PASSWORD,
                    configValue(config, "email.password", "").c_str());
    curl_easy_setopt(curl, CURLOPT_LOGIN_OPTIONS, "AUTH=LOGIN");
    curl_easy_setopt(curl, CURLOPT_MAIL_FROM, fromBox.c_str());
    curl_easy_setopt(curl, CURLOPT_MAIL_RCPT, recipients);
    curl_easy_setopt(curl, CURLOPT_READFUNCTION, readPayload);
    curl_easy_setopt(curl, CURLOPT_READDATA, &payload);
    curl_easy_setopt(curl, CURLOPT_UPLOAD, 1L);
    curl_easy_setopt(curl, CURLOPT_USE_SSL, CURLUSESSL_ALL);
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 1L);
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 2L);
    curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 15L);
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 30L);

    const CURLcode result = curl_easy_perform(curl);
    if (result != CURLE_OK) {
        std::cerr << "[MAIL] " << curl_easy_strerror(result) << '\n';
    }

    curl_slist_free_all(recipients);
    curl_easy_cleanup(curl);
    return result == CURLE_OK;
}
