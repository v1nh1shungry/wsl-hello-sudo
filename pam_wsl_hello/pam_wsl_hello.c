#define PAM_SM_ACCOUNT
#define PAM_SM_AUTH
#define PAM_SM_PASSWORD
#define PAM_SM_SESSION

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <security/pam_appl.h>
#include <security/pam_modules.h>

int pam_sm_open_session(pam_handle_t *pamh, int flags, int argc, const char **argv) {
  return PAM_IGNORE;
}

int pam_sm_close_session(pam_handle_t *pamh, int flags, int argc, const char **argv) {
  return PAM_IGNORE;
}

int pam_sm_acct_mgmt(pam_handle_t *pamh, int flags, int argc, const char **argv) {
  return PAM_IGNORE;
}

int pam_sm_setcred(pam_handle_t *pamh, int flags, int argc, const char **argv) {
  return PAM_IGNORE;
}

int pam_sm_chauthtok(pam_handle_t *pamh, int flags, int argc, const char **argv) {
  return PAM_IGNORE;
}

int pam_sm_authenticate(pam_handle_t *pamh, int flags, int argc, const char **argv) {
  const char *user = NULL;
  int pgu_ret = pam_get_user(pamh, &user, NULL);
  if (pgu_ret != PAM_SUCCESS || user == NULL) {
    return PAM_AUTH_ERR;
  }
  const char *path = "WindowsHelloService";
  unsigned long path_length = strlen(path);
  const char *subcommand = "authenticate";
  unsigned long subcommand_length = strlen(subcommand);
  unsigned long user_length = strlen(user);
  unsigned long command_length = path_length + 1 + subcommand_length + user_length + 1;
  char *command = (char *)malloc(command_length);
  char *ptr = command;
  strncpy(ptr, path, path_length);
  ptr += path_length;
  *ptr = ' ';
  ++ptr;
  strcpy(ptr, subcommand);
  ptr += subcommand_length;
  *ptr = ' ';
  ++ptr;
  strncpy(ptr, user, user_length);
  ptr += user_length;
  *ptr = 0;
  printf("[sudo] Windows Hello for %s:\n", user);
  int result = system(command);
  free(command);
  if (result == 0) {
    return PAM_SUCCESS;
  }
  fprintf(stderr, "sudo: failed to authenticate %s with Windows Hello\n", user);
  return PAM_AUTH_ERR;
}

