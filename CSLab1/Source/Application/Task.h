#pragma once
#include <vector>

namespace CSL1
{
	class Task
	{
	public:
		Task(const std::vector<uint32_t>& suiatableProcessors, uint32_t complexity);

		std::vector<uint32_t>& GetProcessors();
		int32_t GetRemaining() const;
		uint32_t GetComplexity() const;
		bool Tick(uint32_t power);

	private:
		uint32_t m_Complexity;
		std::vector<uint32_t> m_Processors;
		bool m_bCompleted;
		int32_t m_Remaining;
	};
}