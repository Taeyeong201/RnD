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

		// Job �� �߰��Ѵ�.
		template <class F, class... Args>
		std::future<typename std::result_of<F(Args...)>::type> EnqueueJob(F&& f, Args&&... args);

	private:
		size_t num_threads_; // �� Worker �������� ����, �̴� worker_threads_.size()�� ����.
		bool stop_all; // ��� �����尡 ���� �Ǿ�����

		std::vector<std::thread> worker_threads_; // Worker �����带 �����ϴ� ����

		//�ش� ť�� ��� �۾� ������鿡�� ���� ������ ť�̴�. 
		//����, ������Ǯ ����ڵ鵵 �۾����� ���� �ٸ� ������鿡�� ������Ǯ�� �߰��� �� �� �ִ�. 

		std::queue<std::function<void()>> jobs_; // �۾���(���ϵ���) �����ϴ� job ť,

		//��� queue �� ��Ƽ ������ ȯ�濡�� �������� �ʱ� ������ �� queue �� race condition ���� ��ȣ�� ���� ��ġ���� �ʿ��ϴ�.
		//cv_job_q_ �� m_job_q_ �� ������-�Һ��� ������ ������ �� ���ȴ�.
		//���⼭ ������ ������ ������Ǯ�� ����ϴ� ����ڵ��̰�(jobs_ �� �۾��� �߰��ϴ� �����), �Һ��ڵ��� Worker ��������̴�.
		std::condition_variable cv_job_q_; // ���� job ť�� ���� ���� ���� cv
		std::mutex m_job_q_; // ���� job ť�� ���� ���ؽ� m

		void WorkerThread(); // Worker ������
	};

	//������Ǯ ������
	ThreadPool::ThreadPool(size_t num_threads)
		: num_threads_(num_threads), stop_all(false) {
		worker_threads_.reserve(num_threads_); //num_threads_ ���� �����带 ����

		//�ܺο��� ��� �Լ��� �����ϱ� ���ؼ��� mem_fn ���� ���ΰų�, ���� �Լ��� �̿��ϸ� �ȴ�.
		//��� �Լ��� ����Ѵ�. ���� �ȿ��� ��� �Լ��� �����ϱ� ���ؼ��� this �� ��������� �Ѵ�.
		//���� ���� �Լ� �ȿ��� this->WorkerThread() �� ���� ��� �Լ��� ������ �� �ִ�.
		for (size_t i = 0; i < num_threads_; ++i) {
			worker_threads_.emplace_back([this]() { this->WorkerThread(); });
		}
	}

	//������Ǯ �⺻ ������
	//jobs_ �� �۾��� �߰��� �� ���� ����ϰ� �ִٰ�, �۾��� �߰��Ǹ� �޾Ƽ� ó���ϸ� �ȴ�. 
	void ThreadPool::WorkerThread() {
		while (true) {
			std::unique_lock<std::mutex> lock(m_job_q_);

			//���� ���� cv_job_q_ ���� jobs_ �� ���Ұ� �ְų�, stop_all �� �����ɶ� ���� ��ٸ���.
			cv_job_q_.wait(lock, [this]() { return !this->jobs_.empty() || stop_all; });

			//���࿡ ��� �۾����� �����Ǿ� �ְ� jobs_ �� ������� �۾��� ���� �� ��μ� �����带 �����ϰ� �ȴ�.
			if (stop_all && this->jobs_.empty()) { return; }

			// ó���� ���� �ִٸ�, �� ���� job(������)�� ����.
			// C++ ���� �����ϴ� queue �� ��� pop �� �ϸ� �� ���� ���Ҹ� ���������� �ش� ���Ҹ� �������� �ʴ´�. 
			// ����, �ش� ���ҿ� �����ϱ� ���ؼ��� front �� ȣ���ؾ� �Ѵ�.
			std::function<void()> job = std::move(jobs_.front()); //�ش� �����带 �����ؼ� job���� �̵���Ų��.
			jobs_.pop();
			lock.unlock();

			job();// �ش� job(������)�� �����Ѵ�.
		}
	}

	//������Ǯ �Ҹ���
	//stop_all �� ������ �ڿ�, ��� Worker ������鿡 �˷��ش�. �� �� ��� ��������� join �ϸ� �ȴ�.
	ThreadPool::~ThreadPool() {
		stop_all = true;
		cv_job_q_.notify_all();

		for (auto& t : worker_threads_) { t.join(); }
	}

	// job �� �߰��Ѵ�.
	//class... �� ���� ���� ���ø����� ������ ������ ���ڵ��� ���� �� �ִ�. 
	//EnqueueJob(func, 1, 2, 3); �� ���, ù ��° ������ �Լ� func �� f �� ���� �ǰ�, ������ 1, 2, 3 �� args... �κп� ���� �ȴ�.
	template <class F, class... Args>
	std::future<typename std::result_of<F(Args...)>::type> ThreadPool::EnqueueJob(F&& f, Args&&... args) {
		if (stop_all) {
			throw std::runtime_error("ThreadPool ��� ������");
		}

		//EnqueueJob �Լ��� ���޹��� �Լ� f �� ���ϰ��� ������ future �� �����ؾ� �Ѵ�.
		//�Լ� F �� ���ϰ��� std::result_of �� ����ϸ� �� �� �ִ�.
		using return_type = typename std::result_of<F(Args...)>::type; //result_of�� C++17���� deprecated ��, ��ſ� ������ ���
		//using return_type = typename std:: invoke_result_t<F, Args...> 
		auto job = std::make_shared<std::packaged_task<return_type()>>(
			std::bind(std::forward<F>(f), std::forward<Args>(args)...));

		std::future<return_type> job_result_future = job->get_future(); //job �� ���� ����� �����ϴ� job_result_future �� ����
		{
			std::lock_guard<std::mutex> lock(m_job_q_);
			jobs_.push([job]() { (*job)(); }); //jobs_ �� job �� �����ϴ� ���� �Լ��� �߰�
		}
		cv_job_q_.notify_one();

		//job �� ����ȴٸ�, f �� ���ϰ��� job_result_future �� ���� �ǰ�, �̴� ������Ǯ ����ڰ� ������ �� �ְ� �ȴ�.
		return job_result_future;
	}
}  // namespace ThreadPool