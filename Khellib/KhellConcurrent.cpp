#include "Concurrent.h"

namespace Khellendros
{
	inline namespace Concurrent
	{
		ThreadGuard::~ThreadGuard()
		{
			//防止线程在joinable状态下析构
			if (_thread.joinable()) 
				_action == DtorAction::join ? _thread.join() : _thread.detach();
		}
	}
}
