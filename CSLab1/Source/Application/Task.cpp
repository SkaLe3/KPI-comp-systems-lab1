/* Task.cpp */
#include "Task.h"

namespace CSL1
{

	Task::Task(const std::vector<uint32_t>& suiatableProcessors, uint32_t complexity)
		: m_Complexity(complexity), m_Processors(suiatableProcessors), m_bCompleted(false), m_Remaining(m_Complexity)
	{

	}

	std::vector<uint32_t>& Task::GetProcessors()
	{
		return m_Processors;
	}

	int32_t Task::GetRemaining() const
	{
		return m_Remaining;
	}

	uint32_t Task::GetComplexity() const
	{
		return m_Complexity;
	}

	bool Task::Tick(uint32_t power)
	{
		m_Remaining -= power;
		if (m_Remaining <= 0)
		{
			m_bCompleted = true;
		}
		return m_bCompleted;
	}

}

