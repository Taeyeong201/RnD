//https://github.com/progschj/ThreadPool

#pragma once

#include <chrono>
#include <condition_variable>
#include <cstdio>
#include <functional>
#include <future>
#include <mutex>
#include <queue>
#include <thread>
#include <vector>

namespace ThreadPool {
	class ThreadPool {
	public:
		ThreadPool(size_t num_threads);
		~ThreadPool();

		// Job 을 추가한다.
		template <class F, class... Args>
		std::future<typename std::result_of<F(Args...)>::type> EnqueueJob(F&& f, Args&&... args);

	private:
		size_t num_threads_; // 총 Worker 쓰레드의 개수, 이는 worker_threads_.size()와 같다.
		bool stop_all; // 모든 쓰레드가 종료 되었는지

		std::vector<std::thread> worker_threads_; // Worker 쓰레드를 보관하는 벡터

		//해당 큐는 모든 작업 쓰레드들에서 접근 가능한 큐이다. 
		//또한, 쓰레드풀 사용자들도 작업들을 각기 다른 쓰레드들에서 쓰레드풀에 추가할 수 도 있다. 

		std::queue<std::function<void()>> jobs_; // 작업을(할일들을) 보관하는 job 큐,

		//상기 queue 는 멀티 쓰레드 환경에서 안전하지 않기 때문에 이 queue 를 race condition 에서 보호할 다음 장치들이 필요하다.
		//cv_job_q_ 와 m_job_q_ 는 생산자-소비자 패턴을 구현할 때 사용된다.
		//여기서 생산자 역할은 쓰레드풀을 사용하는 사용자들이고(jobs_ 에 작업을 추가하는 사람들), 소비자들은 Worker 쓰레드들이다.
		std::condition_variable cv_job_q_; // 위의 job 큐를 위한 조건 변수 cv
		std::mutex m_job_q_; // 위의 job 큐를 위한 뮤텍스 m

		void WorkerThread(); // Worker 쓰레드
	};

	//쓰레드풀 생성자
	ThreadPool::ThreadPool(size_t num_threads)
		: num_threads_(num_threads), stop_all(false) {
		worker_threads_.reserve(num_threads_); //num_threads_ 개의 쓰레드를 생성

		//외부에서 멤버 함수에 접근하기 위해서는 mem_fn 으로 감싸거나, 람다 함수를 이용하면 된다.
		//멤버 함수를 사용한다. 람다 안에서 멤버 함수에 접근하기 위해서는 this 를 전달해줘야 한다.
		//또한 람다 함수 안에서 this->WorkerThread() 를 통해 멤버 함수를 실행할 수 있다.
		for (size_t i = 0; i < num_threads_; ++i) {
			worker_threads_.emplace_back([this]() { this->WorkerThread(); });
		}
	}

	//쓰레드풀 기본 생성자
	//jobs_ 에 작업이 추가될 때 까지 대기하고 있다가, 작업이 추가되면 받아서 처리하면 된다. 
	void ThreadPool::WorkerThread() {
		while (true) {
			std::unique_lock<std::mutex> lock(m_job_q_);

			//조건 변수 cv_job_q_ 에서 jobs_ 에 원소가 있거나, stop_all 이 설정될때 까지 기다린다.
			cv_job_q_.wait(lock, [this]() { return !this->jobs_.empty() || stop_all; });

			//만약에 모든 작업들이 설정되어 있고 jobs_ 에 대기중인 작업이 없을 때 비로소 쓰레드를 종료하게 된다.
			if (stop_all && this->jobs_.empty()) { return; }

			// 처리할 일이 있다면, 맨 앞의 job(쓰레드)을 뺀다.
			// C++ 에서 제공하는 queue 의 경우 pop 을 하면 맨 앞의 원소를 제거하지만 해당 원소를 리턴하지 않는다. 
			// 따라서, 해당 원소에 접근하기 위해서는 front 를 호출해야 한다.
			std::function<void()> job = std::move(jobs_.front()); //해당 쓰레드를 접근해서 job으로 이동시킨다.
			jobs_.pop();
			lock.unlock();

			job();// 해당 job(쓰레드)를 수행한다.
		}
	}

	//쓰레드풀 소멸자
	//stop_all 을 설정한 뒤에, 모든 Worker 쓰레드들에 알려준다. 그 후 모든 쓰레드들을 join 하면 된다.
	ThreadPool::~ThreadPool() {
		stop_all = true;
		cv_job_q_.notify_all();

		for (auto& t : worker_threads_) { t.join(); }
	}

	// job 을 추가한다.
	//class... 은 가변 길이 템플릿으로 임의의 길이의 인자들을 받을 수 있다. 
	//EnqueueJob(func, 1, 2, 3); 인 경우, 첫 번째 인자인 함수 func 는 f 에 들어가게 되고, 나머지 1, 2, 3 이 args... 부분에 들어가게 된다.
	template <class F, class... Args>
	std::future<typename std::result_of<F(Args...)>::type> ThreadPool::EnqueueJob(F&& f, Args&&... args) {
		if (stop_all) {
			throw std::runtime_error("ThreadPool 사용 중지됨");
		}

		//EnqueueJob 함수는 전달받은 함수 f 의 리턴값을 가지는 future 를 리턴해야 한다.
		//함수 F 의 리턴값은 std::result_of 를 사용하면 알 수 있다.
		using return_type = typename std::result_of<F(Args...)>::type; //result_of는 C++17에서 deprecated 됨, 대신에 다음을 사용
		//using return_type = typename std:: invoke_result_t<F, Args...> 
		auto job = std::make_shared<std::packaged_task<return_type()>>(
			std::bind(std::forward<F>(f), std::forward<Args>(args)...));

		std::future<return_type> job_result_future = job->get_future(); //job 의 실행 결과를 보관하는 job_result_future 를 정의
		{
			std::lock_guard<std::mutex> lock(m_job_q_);
			jobs_.push([job]() { (*job)(); }); //jobs_ 에 job 을 실행하는 람다 함수를 추가
		}
		cv_job_q_.notify_one();

		//job 이 실행된다면, f 의 리턴값이 job_result_future 에 들어가게 되고, 이는 쓰레드풀 사용자가 접근할 수 있게 된다.
		return job_result_future;
	}
}  // namespace ThreadPool