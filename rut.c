#define _XOPEN_SOURCE // for get_pass
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pwd.h>
#include <errno.h>
#include <string.h>
#include <crypt.h>
#include <shadow.h>
#include <stdbool.h>

#define MAX_USERNAME_LENGTH 32

struct auth_user {
	char username[MAX_USERNAME_LENGTH];
	bool no_password; //this is negative because it's inited to 0
} rutters[] = {
#include "hardcoded_users.conf"
};

void error(const char* error) {
	fprintf(stderr, "rut: %s", error);
	if(errno)
		fprintf(stderr, ": ", strerror(errno));

	fprintf(stderr, "\n");
	exit(1);
}

const char* load_rutters_file_to_string() {
	char * buffer = 0;
	long length;
	FILE * f = fopen("/etc/rutters", "r");

	if (!f)
		error("can not open rutters file.");

	//read whole file to buffer, stolen from Nils Pipenbrinck from stackoverflow.
	fseek(f, 0, SEEK_END);
	length = ftell(f);
	fseek(f, 0, SEEK_SET);
	buffer = malloc(length);
	if(buffer)
		fread (buffer, 1, length, f);
	fclose(f);

	return buffer;
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

	size_t user_count = sizeof(rutters) / sizeof(struct auth_user);

	for (size_t i = 0; i < user_count; i++)
	{
		struct auth_user* rutter = &rutters[i];
		if(!rutter->username[0]) break; //if at the end of the array
		//compare the name
		if(strncmp(rutter->username, user_data->pw_name, MAX_USERNAME_LENGTH) == 0) {
			if(rutter->no_password) return true; //nopass
			//the user was found, let's ask for password
			return ask_password(uid, &user_data);
		}
	}

	//if user was not found
	return false;
}

int main(int argc, char const *argv[])
{
	// unready();
	if(argc < 2)
		error("rut -- run command as root");

	if(!auth())
		error("can not auth");

	//set user_id to root's
	if (setuid(0) != 0)
    	error("can not set userid to root's: ");

	//environment, i have no idea why we need this but we do.
    char* envp[] = {getenv("PATH"), NULL};
	//execute
	execvpe(argv[1], &argv[1], envp);
	return 0;
}