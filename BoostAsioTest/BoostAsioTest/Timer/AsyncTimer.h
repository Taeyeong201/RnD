#pragma once


#include <boost/asio.hpp>
#include <boost/asio/steady_timer.hpp>
#include <boost/bind.hpp>
#include <memory>
#include <iostream>

class AsyncTimer
{
public:
	AsyncTimer(int timeout_seconds = 600);
	~AsyncTimer();

	void StartTimer();
	void ResetTimer();
	void CloseTimer();
private:
	void run_timer(std::size_t current_id, const boost::system::error_code& error);
	void cancel_timer();
	void restart_timer();

	const int timeout_seconds;

	boost::asio::io_context io_ctx;
	std::unique_ptr<boost::asio::io_context::work> work;
	std::thread run_thread;

	boost::asio::steady_timer timer;
	std::size_t timer_id = 0;
};

