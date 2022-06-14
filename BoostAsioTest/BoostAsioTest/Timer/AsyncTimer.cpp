#include "AsyncTimer.h"

using namespace boost::asio;

AsyncTimer::AsyncTimer(int _timeout_seconds)
	: 
	timeout_seconds(_timeout_seconds),
	work(std::make_unique<boost::asio::io_context::work>(io_ctx)),
	timer(io_ctx, std::chrono::seconds{ timeout_seconds })
{
	run_thread = std::thread([&]() {
		io_ctx.run();
		});
}

AsyncTimer::~AsyncTimer()
{
	io_ctx.stop();
	run_thread.join();
	work.reset();
}

void AsyncTimer::StartTimer()
{
	timer.async_wait(boost::bind(
		&AsyncTimer::run_timer, this, timer_id, 
		boost::asio::placeholders::error
	));
}

void AsyncTimer::ResetTimer()
{
	cancel_timer();
	restart_timer();
}

void AsyncTimer::CloseTimer()
{
	cancel_timer();
}

void AsyncTimer::run_timer(std::size_t current_id, const boost::system::error_code& error)
{
	if (error || current_id != timer_id) {
		std::cout << "timer cancel " << current_id << std::endl;
	}
	else {
		std::cout << "time out " << current_id << std::endl;
		restart_timer();
	}
}

void AsyncTimer::cancel_timer()
{
	io_ctx.post([&] {
		timer.cancel();
		std::cout << "cancel " << timer_id << std::endl;
		});
}

void AsyncTimer::restart_timer()
{
	++timer_id;
	std::cout << "run timer " << timer_id << std::endl;
	timer.expires_from_now(std::chrono::seconds(timeout_seconds));
	timer.async_wait(boost::bind(
		&AsyncTimer::run_timer, this, timer_id,
		boost::asio::placeholders::error
	));
}
