#include "TaskGenerator.h"

#include <random>


namespace CSL1
{

	TaskGenerator::TaskGenerator(uint32_t procsCount, std::pair<uint32_t, uint32_t> range, uint32_t probability)
		: m_Count(procsCount), m_Probability(probability), m_Range(range)
	{
		processorsSet = CreateSet();
	}

	Task TaskGenerator::GenerateTask()
	{
		std::random_device rd;
		std::mt19937 gen(rd());
		std::uniform_int_distribution<> dis(0, processorsSet.size() - 1);

		return 
	}

	std::vector<std::vector<uint32_t>> TaskGenerator::CreateSet()
	{
		return { {1, 2, 3, 4, 0}, {2, 3, 4, 0}, {1, 3, 4, 0}, {1, 2, 4, 0}, {1, 2, 3, 0}, {1, 2, 3, 4},
				 {1, 2, 3}, {1, 2, 4}, {1, 2, 0}, {1, 3, 4}, {1, 3, 0}, {1, 4, 0}, {2, 3, 4}, {2, 3, 0}, {2, 4, 0}, {3, 4, 0},
				 {1, 2}, {1, 3}, {1, 4}, {1, 0}, {2, 3}, {2, 4}, {2, 0}, {3, 4}, {3, 0}, {4, 0},
				 {1}, {2}, {3}, {4}, {0} }
	}

}