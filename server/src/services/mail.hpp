#pragma once

#include "config/config.hpp"

#include <string>

// 邮件服务层：发送注册验证码，开发模式下输出到控制台。
bool sendVerificationMail(const Config& config,
                          const std::string& recipient,
                          const std::string& code);
