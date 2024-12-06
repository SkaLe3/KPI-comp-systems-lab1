/* TaskGenerator.cpp */
#include "TaskGenerator.h"


namespace CSL1
{

	TaskGenerator::TaskGenerator(uint32_t procsCount, std::pair<uint32_t, uint32_t> range, uint32_t probability)
		: m_Count(procsCount),
		m_Probability(static_cast<float>(probability) / 100.0f),
		m_Range(range),
		gen(rd())
	{
		processorsSet = CreateSet();
		distributionProcessors = std::uniform_int_distribution<uint32_t>(0, processorsSet.size() - 1);
		distributionComplexity = std::uniform_int_distribution<uint32_t>(m_Range.first, m_Range.second);

		GenerateTask = [this]() -> Task
			{
				uint32_t pset = distributionProcessors(gen);
				uint32_t comp = distributionComplexity(gen);
				return Task(processorsSet[pset], comp);
			};
	}


	std::vector<Task> TaskGenerator::GenerateTasks(uint32_t tasksCount)
	{
		std::bernoulli_distribution disGenerate(m_Probability);
		std::vector<Task> result;

		for (uint32_t i = 0; i < tasksCount; i++)
		{
			if (disGenerate(gen))
			{
				result.push_back(GenerateTask());
			}
		}
		return result;
	}

	std::vector<std::vector<uint32_t>> TaskGenerator::CreateSet()
	{
		return {
			{1, 2, 3, 4, 0}, {2, 3, 4, 0}, {1, 3, 4, 0}, {1, 2, 4, 0}, {1, 2, 3, 0}, {1, 2, 3, 4},
			{1, 2, 3}, {1, 2, 4}, {1, 2, 0}, {1, 3, 4}, {1, 3, 0}, {1, 4, 0}, {2, 3, 4}, {2, 3, 0},
			{2, 4, 0}, {3, 4, 0}, {1, 2}, {1, 3}, {1, 4}, {1, 0}, {2, 3}, {2, 4}, {2, 0},
			{3, 4}, {3, 0}, {4, 0}, {1}, {2}, {3}, {4}, {0}
		};

	}

}