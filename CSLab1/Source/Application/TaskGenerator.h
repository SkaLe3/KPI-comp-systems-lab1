#pragma once

namespace CSL1
{
	class Task
	{

	};

	class TaskGenerator
	{
	public:
		TaskGenerator(uint32_t procsCount, std::pair<uint32_t, uint32_t> range, uint32_t probability);


	private:
		Task GenerateTask();
		std::vector<std::vector<uint32_t>> CreateSet();

	private:
		uint32_t m_Count;
		uint32_t m_Probability;
		std::pair<uint32_t, uint32_t> m_Range;

		std::vector<std::vector<uint32_t>> processorsSet;
	};
}