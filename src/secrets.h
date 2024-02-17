#ifndef SECRETS_H
#define SECRETS_H

#undef signals // Qt uses a "signals" macro and GLib has a variable with the same name
#include <libsecret/secret.h>
#define signals

void storeToken(const gchar *username, const gchar *token);
gchar *getTokenSync(const gchar *username, GError **error);
void deleteToken(const gchar *username);

#endif //SECRETS_H
