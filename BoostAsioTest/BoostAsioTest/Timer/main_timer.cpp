#include <cstddef>
#include <iostream>
#include <boost/asio.hpp>
#include <boost/asio/steady_timer.hpp>
#include <memory>

int main()
{
	using namespace boost::asio;

	io_context io_ctx;
	std::unique_ptr<boost::asio::io_context::work> work
		= std::make_unique<boost::asio::io_context::work>(io_ctx);
	std::thread run_thread = std::thread([&]() {
		io_ctx.run();
	});

	steady_timer timer{ io_ctx, std::chrono::seconds{10} };
	std::size_t timer_id = 0;

	timer.async_wait([&, current_id = timer_id](boost::system::error_code ec) {
		if (ec || current_id != timer_id) {
			std::cout << "timer is cancelled - " << ec << std::endl;
		}
		else {
			std::cout << "timer is not canncelled" << std::endl;
		}
	});

	getchar();
	io_ctx.post([&] {
		std::cout << "start work" << std::endl;
		++timer_id;
		timer.cancel();
		std::cout << "cancel timer" << std::endl;
	});


	io_ctx.stop();
	run_thread.join();
	work.reset();

	std::cout << "main  end" << std::endl;
}