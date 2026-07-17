#include "repositories/user_repository.hpp"

UserRepository::UserRepository(Database& database) : database_(database) {}

void UserRepository::create(const std::string& email,
                            const std::string& passwordHash,
                            const std::string& nickname) {
    database_.execute(
        "INSERT INTO users(email,password_hash,nickname) VALUES('" +
        database_.escape(email) + "','" + database_.escape(passwordHash) +
        "','" + database_.escape(nickname) + "')");
}

std::optional<User> UserRepository::findByCredentials(
    const std::string& email, const std::string& passwordHash) {
    const auto rows = database_.query(
        "SELECT id,nickname FROM users WHERE email='" + database_.escape(email) +
        "' AND password_hash='" + database_.escape(passwordHash) + "' LIMIT 1");
    if (rows.empty()) return std::nullopt;
    return User{std::stoll(rows[0][0]), rows[0][1]};
}

std::optional<User> UserRepository::findByEmail(const std::string& email) {
    const auto rows = database_.query(
        "SELECT id,nickname FROM users WHERE email='" + database_.escape(email) +
        "' LIMIT 1");
    if (rows.empty()) return std::nullopt;
    return User{std::stoll(rows[0][0]), rows[0][1]};
}
