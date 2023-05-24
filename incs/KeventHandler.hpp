#pragma once

#include "Color.hpp"
#include <iostream>
#include <unistd.h>
#include <vector>
#include <sys/event.h>
#include <sys/time.h>

class KeventHandler
{
private:
	int kq_; // TODO: kqueue_fd_로 바꾸기

	std::vector<struct kevent> change_list_;																																				// TODO: event_list_로 바꾸기
	void kevent_init_(uintptr_t ident, int16_t filter, uint16_t flags, uint32_t fflags, int64_t data, void *udata); // TODO: initialize_kevent으로 바꾸기
public:
	KeventHandler();
	~KeventHandler();

	std::vector<struct kevent> set_monitor(const bool &end_signal);
	void set_read(uintptr_t ident);		// TODO: add_read_event로 바꾸기
	void set_server(uintptr_t ident); // TODO: add_server_event로 바꾸기
	void set_write(uintptr_t ident);	// TODO: add_write_event로 바꾸기
	void set_exit(uintptr_t ident);		// TODO: add_exit_event로 바꾸기
	void delete_event(const struct kevent &event);
	void delete_server(uintptr_t serv_sock);
};
