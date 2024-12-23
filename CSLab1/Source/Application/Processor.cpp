/* Processor.cpp */
#include "Processor.h"

namespace CSL1
{

	Processor::Processor()
		: m_ID(NextAvailableID), m_Power(1), m_LastScheduleTime(0), m_bRunsScheduler(false), m_SchedulerUptime(0)
	{
		NextAvailableID++;
		ResetStats();
	}


	const uint32_t Processor::ID() const
	{
		return m_ID;
	}


	const uint32_t Processor::GetPower() const
	{
		return m_Power;
	}

	void Processor::SetPower(uint32_t power)
	{
		m_Power = power;
	}

	bool Processor::IsAcceptable(Task& task)  const
	{
		auto Contains = [](const std::vector<uint32_t>& vec, uint32_t number)
			{
				return std::find(vec.begin(), vec.end(), number) != vec.end();
			};
		return  Contains(task.GetProcessors(), m_ID);
	}

	bool Processor::IsQueueEmpty()  const
	{
		return m_Queue.empty();
	}


	void Processor::AddTask(const Task& task)
	{
		m_Queue.push(task);
		m_LastScheduleTime = task.GetComplexity();
	}

	void Processor::Tick()
	{
		if (m_bRunsScheduler)
		{
			Stats.WorkTimeScheduler++;
			m_SchedulerUptime++;
			return;
		}
		Stats.WorkTimeDedicated++;
		if (!m_Queue.empty())
		{
			Stats.CompletedOperations += m_Power;
			if (m_Queue.front().Tick(m_Power))
			{
				Stats.CompletedOperations += m_Queue.front().GetRemaining();
				m_Queue.pop();
				Stats.CompletedTasks++;
			}
		}
	}

	uint32_t Processor::GetSchedulerUptime() const
	{
		return m_SchedulerUptime;
	}

	void Processor::RunScheduler()
	{
		 m_bRunsScheduler = true;
	}

	void Processor::StopScheduler()
	{
		m_bRunsScheduler = false;
		m_SchedulerUptime = 0;
	}

	bool Processor::IsSchedulerRunning() const
	{
		return m_bRunsScheduler;
	}


	void Processor::Reset()
	{
		ResetStats();
		m_LastScheduleTime = 0;	
		m_bRunsScheduler = false;
		m_Queue = std::queue<Task>();
	}

	void Processor::ResetStats()
	{
	   Stats = ProcessorStatistics();
	}

}

