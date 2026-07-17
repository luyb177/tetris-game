#include "repositories/email_code_repository.hpp"

EmailCodeRepository::EmailCodeRepository(Database& database) : database_(database) {}

void EmailCodeRepository::create(const std::string& email, const std::string& code) {
    database_.execute(
        "INSERT INTO email_codes(email,code,expires_at) VALUES('" +
        database_.escape(email) + "','" + database_.escape(code) +
        "',DATE_ADD(NOW(), INTERVAL 5 MINUTE))");
}

bool EmailCodeRepository::consumeValid(const std::string& email,
                                       const std::string& code) {
    const auto rows = database_.query(
        "SELECT id FROM email_codes WHERE email='" + database_.escape(email) +
        "' AND code='" + database_.escape(code) +
        "' AND used=0 AND expires_at>NOW() ORDER BY id DESC LIMIT 1");
    if (rows.empty()) return false;
    database_.execute("UPDATE email_codes SET used=1 WHERE id=" + rows[0][0]);
    return true;
}
