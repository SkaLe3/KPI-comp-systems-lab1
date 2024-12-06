/* Processor.h */
#pragma once

#include "Task.h"
#include <queue>

namespace CSL1
{
	struct ProcessorStatistics
	{
		uint32_t CompletedOperations = 0;
		uint32_t CompletedTasks = 0;
		uint32_t WorkTimeDedicated = 0;
		uint32_t WorkTimeScheduler = 0;
	};

	class Processor
	{
	public:
		Processor();

		const uint32_t ID() const;

		const uint32_t GetPower() const;
		void SetPower(uint32_t power);

		bool IsAcceptable(Task& task) const;
		bool IsQueueEmpty() const;
		
		void AddTask(const Task& task);
		void Tick();

		uint32_t GetSchedulerUptime() const;
		void RunScheduler();
		void StopScheduler();
		bool IsSchedulerRunning() const ;

		void Reset();
	private:
		void ResetStats();

	public:
		ProcessorStatistics Stats;

	private:
		uint32_t m_ID;
		uint32_t m_Power;
		std::queue<Task> m_Queue;
		
		uint32_t m_LastScheduleTime;

		bool m_bRunsScheduler;
		uint32_t m_SchedulerUptime;

	private:
		inline static int32_t NextAvailableID = 0;

	};
}