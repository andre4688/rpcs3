#pragma once
#include "CPUThread.h"

class CPUThreadManager
{
	ArrayF<CPUThread> m_threads;
	std::mutex m_mtx_thread;
	wxSemaphore m_sem_task;
	Stack<u32> m_delete_threads;
	u32 m_raw_spu_num;

public:
	CPUThreadManager();
	~CPUThreadManager();

	void Close();

	CPUThread& AddThread(CPUThreadType type);
	void RemoveThread(const u32 id);

	ArrayF<CPUThread>& GetThreads() { return m_threads; }
	s32 GetThreadNumById(CPUThreadType type, u32 id);
	CPUThread* GetThread(u32 id);

	void Exec();
	void Task();
};