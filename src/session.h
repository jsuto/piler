/*
 * session.h, SJ
 */

#include "defs.h"

void handle_smtp_session(int new_sd, struct __data *data, struct __config *cfg);
void initSessionData(struct session_data *sdata);
void killChild();

