#define _XOPEN_SOURCE // for get_pass
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <pwd.h>
#include <errno.h>
#include <string.h>
#include <crypt.h>
#include <shadow.h>

#define MAX_USERNAME_LENGTH 32

struct auth_user {
	const char *username;
	bool skip_password; 
} rutters[] = {
#include "users.conf"
};

void error(const char* error) {
	fprintf(stderr, "rut: %s", error);
	if(errno)
		fprintf(stderr, ": %s", strerror(errno));

	fputc('\n', stderr);
	exit(1);
}

bool ask_password(__uid_t user_id, struct passwd* user_data) {
	struct spwd *shadow_data = getspnam(user_data->pw_name);
	if (shadow_data == NULL && errno == EACCES)
		error("no permission to read shadow password file");

	//shadow password contains the hashed password
	if (shadow_data)
		user_data->pw_passwd = shadow_data->sp_pwdp; //so it'd be easier to read

	printf("[rut] password for %s: ", user_data->pw_name);
	const char* pass = getpass(""); //ask for the password

	//hash it
	char* encrypted_pass = crypt(pass, user_data->pw_passwd);
	//check if the hash is the same as the shadow.
	return strcmp(encrypted_pass, user_data->pw_passwd) == 0;
}

bool auth() {
	__uid_t uid = getuid(); //get real user id
	if(uid == 0) return true; //if already root
	struct passwd* user_data = getpwuid(uid);
	if(user_data == NULL)
		error("can not get data");

	static size_t user_count = sizeof(rutters) / sizeof(struct auth_user);
	for (size_t i = 0; i < user_count; ++i) {
		struct auth_user* rutter = &rutters[i];
		//compare the name
		if(strncmp(rutter->username, user_data->pw_name, MAX_USERNAME_LENGTH) == 0) {
			if(rutter->skip_password) return true;
			//the user was found, let's ask for password
			return ask_password(uid, user_data);
		}
	}

	return false; //if user was not found
}

int main(int argc, char const *argv[]) {
	if(argc < 2)
		error("runs command as root\n"
			  "usage: rut <program> <args>");

	if(!auth())
		error("can not auth");

	//set user_id to root's
	if (setuid(0) != 0)
    	error("can not set userid to root's");

	//execute, don't fork since it's the exitpoint anyway.
	if(execvp(argv[1], &argv[1]) != 0)
		fprintf(stderr, "%s: command not found.\n", argv[1]);
	return 1; // exit with an error if the exec was not successful,
}