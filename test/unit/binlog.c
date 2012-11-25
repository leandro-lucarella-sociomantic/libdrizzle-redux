/*  vim:expandtab:shiftwidth=2:tabstop=2:smarttab: 
 *
 *  Drizzle Client & Protocol Library
 *
 * Copyright (C) 2012 Andrew Hutchings (andrew@linuxjedi.co.uk)
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *
 *     * Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 *
 *     * Redistributions in binary form must reproduce the above
 * copyright notice, this list of conditions and the following disclaimer
 * in the documentation and/or other materials provided with the
 * distribution.
 *
 *     * The names of its contributors may not be used to endorse or
 * promote products derived from this software without specific prior
 * written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */

#include <libdrizzle/drizzle_client.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{
  (void) argc;
  (void) argv;
  drizzle_st *drizzle;
  drizzle_con_st *con;
  drizzle_return_t ret;
  drizzle_result_st *result;
  drizzle_binlog_st *binlog_event;

  drizzle = drizzle_create();
  if (drizzle == NULL)
  {
    printf("Drizzle object creation error\n");
    return EXIT_FAILURE;
  }
  con = drizzle_con_create(drizzle);
  if (con == NULL)
  {
    printf("Drizzle connection object creation error\n");
    return EXIT_FAILURE;
  }
  drizzle_con_set_tcp(con, "localhost", 3306);
  drizzle_con_set_auth(con, "root", "");
  ret = drizzle_con_connect(con);
  if (ret != DRIZZLE_RETURN_OK)
  {
    printf("Drizzle connection failure\n");
    return EXIT_FAILURE;
  }

  result= drizzle_start_binlog(con, 0, "", 0, &ret);
  if (ret != DRIZZLE_RETURN_OK)
  {
    printf("Drizzle binlog start failure\n");
    return EXIT_FAILURE;
  }

  while (ret == DRIZZLE_RETURN_OK)
  {
    uint32_t i;
    binlog_event= drizzle_binlog_get_event(result, &ret);
    if (ret != DRIZZLE_RETURN_OK)
      break;
    printf("Timestamp: %"PRIu32"\n", binlog_event->timestamp);
    printf("Type: %"PRIu8"\n", binlog_event->type);
    printf("Server-id: %"PRIu32"\n", binlog_event->server_id);
    printf("Next-pos: %"PRIu32"\n", binlog_event->next_pos);
    printf("Length: %"PRIu32"\n", binlog_event->length);
    printf("Data: 0x");
    for (i=0; i<binlog_event->length; i++)
      printf("%02X ", binlog_event->data[i]);
    printf("\n\n");
  }

  drizzle_con_free(con);
  drizzle_free(drizzle);
  if (ret != DRIZZLE_RETURN_EOF)
    return EXIT_FAILURE;
  return EXIT_SUCCESS;
}