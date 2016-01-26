#pragma once

int
message_getHandler(message_handler_t* handler, int id);

int
message_handle(int id, message_handler_t func);

int 
message_send(thread_h tid, int id, void* data);
