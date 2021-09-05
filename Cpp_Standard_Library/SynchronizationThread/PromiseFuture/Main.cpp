#include <boost/thread.hpp>
#include <boost/thread/future.hpp>
#include <boost/random.hpp>
#include <iostream>
#include <boost/thread/condition_variable.hpp>


class PromiseFutureTest {
public:
	boost::random::mt19937 rng;
	boost::random::uniform_int_distribution<> randomGen{ 10, 100 };

	boost::promise<int> p1;
	boost::promise<int> p2;
	boost::promise<int> p3;
	boost::promise<int> p4;

	boost::shared_future<int> f1;
	boost::shared_future<int> f2;
	boost::shared_future<int> f3;
	boost::shared_future<int> f4;


	void threadFunc() {
		int wait1 = 0;
		int wait2 = 0;
		int wait3 = 0;
		int wait4 = 0;
		int loop = 4;
		int asdasD = 0;
		bool first = false;
		std::thread cap([&]() {
			while (1)
			{
				if (first) {
					f4 = p4.get_future();
					asdasD = f4.get();
				}
				else first = true;
				if (asdasD == -1)  break;
				wait1 = randomGen(rng);
				std::cerr << "cap num > " << std::this_thread::get_id()
					<< "\twait1 for :" << wait1 << std::endl;
				Sleep(wait1);
				p1.set_value(wait1);
				p1 = boost::promise<int>{};

			}
			std::cerr << "cap end" << std::endl;
			p1.set_value(-1);
		});
		std::thread cvt([&]() {
			while (1) {
				f1 = p1.get_future();
				wait2 = f1.get();
				std::cerr << "cvt num > " << std::this_thread::get_id()
					<< "\twait2 for :" << wait2 << std::endl;
				if (wait2 > 0) Sleep(wait2);
				else break;
				p2.set_value(wait2);
				p2 = boost::promise<int>{};
			}
			std::cerr << "cvt end" << std::endl;
			p2.set_value(wait2);
		});
		std::thread encode([&]() {
			while (1) {
				f2 = p2.get_future();
				wait3 = f2.get();
				std::cerr << "encode num > " << std::this_thread::get_id()
					<< "\twait3 for :" << wait3 << std::endl;
				if (wait3 > 0) Sleep(wait3);
				else break;
				p3.set_value(wait3);
				p3 = boost::promise<int>{};
			}
			std::cerr << "encode end" << std::endl;
			p3.set_value(wait3);
		});
		std::thread send([&]() {
			while (1) {
				f3 = p3.get_future();
				wait4 = f3.get();
				std::cerr << "send num > " << std::this_thread::get_id()
					<< "\twait4 for :" << wait4 << std::endl;
				if (loop > 0) {
					Sleep(wait4);
					loop--;
				}
				else break;
				p4.set_value(wait4);
				p4 = boost::promise<int>{};
			}
			std::cerr << "send end" << std::endl;
			p4.set_value(-1);
		});

		cap.join();
		cvt.join();
		encode.join();
		send.join();

	}



	boost::mutex mCap;
	boost::mutex mCvt;
	boost::mutex mEncode;
	boost::mutex mSend;
	boost::condition_variable cvCap;
	boost::condition_variable cvCvt;
	boost::condition_variable cvEncode;
	boost::condition_variable cvSend;
	bool bCap = false, bCvt = false, bEncode = false, bSend = false;

	void threadFunc1() {
		int wait1 = 0;
		int wait2 = 0;
		int wait3 = 0;
		int wait4 = 0;
		int loop = 4;
		int asdasD = 0;
		bool first = false;
		

		std::thread cap([&]() {
			while (1)
			{
				if (loop < 0)  break;
				if(!first)
				{
					boost::lock_guard<boost::mutex> lk(mCap);
					wait1 = randomGen(rng);
					std::cerr << "cap num > " << std::this_thread::get_id()
						<< "\t\twait1 for :" << wait1 << std::endl;
					Sleep(wait1);
					bCap = true;
					cvCap.notify_one();
					first = true;
				}
				else {
					boost::unique_lock<boost::mutex> lk(mSend);
					cvSend.wait(lk, [&] {return bSend; });
					{
						bSend = false;
						boost::lock_guard<boost::mutex> lk(mCap);
						wait1 = randomGen(rng);
						std::cerr << "cap num > " << std::this_thread::get_id()
							<< "\t\twait1 for :" << wait1 << std::endl;
						Sleep(wait1);
						bCap = true;
						cvCap.notify_one();
					}
				}
				if (loop < 0)  break;
				

			}
			std::cerr << "cap end" << std::endl;
			
		});
		std::thread cvt([&]() {
			while (1) {
				if (loop < 0)  break;
				{
					boost::unique_lock<boost::mutex> lk(mCap);
					cvCap.wait(lk, [&] {return bCap; });
					{
						bCap = false;
						wait1 = randomGen(rng);
						boost::lock_guard<boost::mutex> cvtLk(mCvt);
						std::cerr << "cvt num > " << std::this_thread::get_id()
							<< "\t\twait2 for :" << wait1 << std::endl;
						Sleep(wait1);
						bCvt = true;
						cvCvt.notify_one();
					}
				}
				
				if (loop < 0)  break;
			}
			std::cerr << "cvt end" << std::endl;
	

		});
		std::thread encode([&]() {

			while (1) {
				if (loop < 0)  break;
				{
					boost::unique_lock<boost::mutex> lk(mCvt);
					cvCvt.wait(lk, [&] {return bCvt; });
					{
						bCvt = false;
						wait1 = randomGen(rng);
						boost::lock_guard<boost::mutex> enLk(mEncode);
						std::cerr << "encode num > " << std::this_thread::get_id()
							<< "\twait3 for :" << wait1 << std::endl;
						Sleep(wait1);
						bEncode = true;
						cvEncode.notify_one();
					}
				}
				
				
				
	
			}
			std::cerr << "encode end" << std::endl;

		});
		std::thread send([&]() {
			while (1) {
				--loop;
				if (loop < 0)  break;
				{
					boost::unique_lock<boost::mutex> lk(mEncode);
					cvEncode.wait(lk, [&] {return bEncode; });
					{
						bEncode = false;
						wait1 = randomGen(rng);
						boost::lock_guard<boost::mutex> seLk(mSend);
						std::cerr << "send num > " << std::this_thread::get_id()
							<< "\twait4 for :" << wait1 << std::endl;
						Sleep(wait1);
						bSend = true;
						cvSend.notify_one();
					}
					
				}
				
				
			}
			std::cerr << "send end" << std::endl;

		});

		cap.join();
		cvt.join();
		encode.join();
		send.join();

	}
};

int main() {
	PromiseFutureTest test;
	test.threadFunc1();
}
