#include "Concurrent.h"

namespace Khellendros
{
	inline namespace Concurrent
	{
		ThreadGuard::~ThreadGuard()
		{
			//��ֹ�߳���joinable״̬������
			if (_thread.joinable()) 
				_action == DtorAction::join ? _thread.join() : _thread.detach();
		}
	}
}
