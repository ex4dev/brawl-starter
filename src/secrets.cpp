#include "secrets.h"

#include <iostream>
#include <QMessageBox>
#include <qstringliteral.h>

#include "constants.h"

static constexpr SecretSchema schema = {
    "dev.tswanson.kocity-qt", SECRET_SCHEMA_NONE,
    {
            {"username", SECRET_SCHEMA_ATTRIBUTE_STRING},
        }
};

void storeToken(const gchar *username, const gchar *token) {
    secret_password_store(
        &schema,
        SECRET_COLLECTION_DEFAULT,
        "Auth Token for kocity.xyz",
        token,
        nullptr,
        [](GObject *, GAsyncResult *res, gpointer) {
            GError *error = nullptr;
            secret_password_store_finish(res, &error);
            if (error != nullptr) {
                QMessageBox::critical(nullptr, constants::STR_ERROR, QStringLiteral("Failed to save auth token: ") + error->message);
                g_error_free(error);
            }
        },
        nullptr,
        "username", username, nullptr
    );
}

gchar *getTokenSync(const gchar *username, GError **error) {
    return secret_password_lookup_sync(
        &schema,
        nullptr,
        error,
        "username", username, nullptr
    );
}

void deleteToken(const gchar *username) {
    secret_password_clear(
        &schema,
        nullptr,
        [](GObject *, GAsyncResult *res, gpointer) {
            GError *error = nullptr;
            secret_password_clear_finish(res, &error);
            if (error != nullptr) {
                QMessageBox::critical(nullptr, constants::STR_ERROR, QStringLiteral("Failed to delete authentication token: ") + error->message);
                g_error_free(error);
            }
        },
        nullptr,
        "username", username, nullptr
    );
}