#pragma once
#ifdef SPH_THREADSRV
#else
#define SPH_THREADSRV
namespace Hubris::Core {
	class ThreaddingServer {
		ThreaddingServer() = default;
	public:
		virtual ~ThreaddingServer() = 0;
		virtual void Initialize(unsigned int Tcount) = 0;
		virtual void Shutdown() = 0;
	};
}
#endif
