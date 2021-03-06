#include "stdafx.h"
#include "CPUThreadManager.h"
#include "Emu\Cell\PPUThread.h"
#include "Emu\Cell\SPUThread.h"
#include "Emu\Cell\RawSPUThread.h"
#include "Emu\ARMv7\ARMv7Thread.h"

CPUThreadManager::CPUThreadManager()
	: m_raw_spu_num(0)
{
}

CPUThreadManager::~CPUThreadManager()
{
	Close();
}

void CPUThreadManager::Close()
{
	m_raw_spu_num = 0;
	while(m_threads.GetCount()) RemoveThread(m_threads[0].GetId());
}

CPUThread& CPUThreadManager::AddThread(CPUThreadType type)
{
	std::lock_guard<std::mutex> lock(m_mtx_thread);

	CPUThread* new_thread;

	switch(type)
	{
	case CPU_THREAD_PPU:		new_thread = new PPUThread(); break;
	case CPU_THREAD_SPU:		new_thread = new SPUThread(); break;
	case CPU_THREAD_RAW_SPU:	new_thread = new RawSPUThread(m_raw_spu_num++); break;
	case CPU_THREAD_ARMv7:		new_thread = new ARMv7Thread(); break;
	default: assert(0);
	}
	
	new_thread->SetId(Emu.GetIdManager().GetNewID(wxString::Format("%s Thread", new_thread->GetTypeString()), new_thread));

	m_threads.Add(new_thread);
	wxGetApp().SendDbgCommand(DID_CREATE_THREAD, new_thread);

	return *new_thread;
}

void CPUThreadManager::RemoveThread(const u32 id)
{
	std::lock_guard<std::mutex> lock(m_mtx_thread);

	for(u32 i=0; i<m_threads.GetCount(); ++i)
	{
		if(m_threads[i].m_wait_thread_id == id)
		{
			m_threads[i].Wait(false);
			m_threads[i].m_wait_thread_id = -1;
		}

		if(m_threads[i].GetId() != id) continue;

		CPUThread* thr = &m_threads[i];
		wxGetApp().SendDbgCommand(DID_REMOVE_THREAD, thr);
		if(thr->IsAlive())
		{
			thr->Close();
		}
		else
		{
			thr->Close();
			delete thr;
		}


		m_threads.RemoveFAt(i);
		i--;
	}

	Emu.GetIdManager().RemoveID(id, false);
	Emu.CheckStatus();
}

s32 CPUThreadManager::GetThreadNumById(CPUThreadType type, u32 id)
{
	s32 num = 0;

	for(u32 i=0; i<m_threads.GetCount(); ++i)
	{
		if(m_threads[i].GetId() == id) return num;
		if(m_threads[i].GetType() == type) num++;
	}

	return -1;
}

CPUThread* CPUThreadManager::GetThread(u32 id)
{
	for(u32 i=0; i<m_threads.GetCount(); ++i)
	{
		if(m_threads[i].GetId() == id) return &m_threads[i];
	}

	return nullptr;
}

void CPUThreadManager::Exec()
{
	for(u32 i=0; i<m_threads.GetCount(); ++i)
	{
		m_threads[i].Exec();
	}
}