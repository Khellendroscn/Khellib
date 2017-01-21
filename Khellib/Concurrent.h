#pragma once
#include <thread>
#include <future>

namespace Khellendros
{
	inline namespace Concurrent
	{
		//����ִ������
		template<typename FuncT,typename...ArgsT>
		inline auto asyncExec(FuncT&& task, ArgsT&&... args)
		{
			return std::async(
				std::launch::async,
				std::forward<FuncT>(task),
				std::forward<ArgsT>(args)...
				);
		}
		//�ں�ִ̨������
		template<typename FuncT,typename...ArgsT>
		auto detachExec(FuncT&& task, ArgsT&&... args)
		{
			using Ret = std::result_of_t<FuncT(ArgsT...)>;
			auto taskadpt = std::bind(std::forward<FuncT>(task), std::forward<ArgsT>(args)...);
			std::packaged_task<Ret()> taskpkg(std::move(taskadpt));
			auto fut = taskpkg.get_future();
			std::thread t(std::move(taskpkg));
			t.detach();
			return fut;
		}
		//RAII�߳�
		//! ����̱߳����������ʹ�߳��޷�����ȷ����
		class ThreadGuard
		{
		public:
			enum class DtorAction{join,detach};
			ThreadGuard(std::thread&& t, DtorAction dtorAction) :
				_action(dtorAction), _thread(std::move(t)) {}
			ThreadGuard(ThreadGuard&&) = default;
			ThreadGuard& operator=(ThreadGuard&&) = default;
			~ThreadGuard();
			std::thread& get() { return _thread; }
			
		private:
			DtorAction _action;
			std::thread _thread;
		};
	}
}