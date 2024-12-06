#pragma once
#include "Task.h"
#include <functional>
#include <random>

namespace CSL1
{

	class TaskGenerator
	{
	public:
		TaskGenerator(uint32_t procsCount, std::pair<uint32_t, uint32_t> range, uint32_t probability);

		std::vector<Task> GenerateTasks(uint32_t tasksCount);

	private:
		std::vector<std::vector<uint32_t>> CreateSet();
	private:
		std::function<Task(void)> GenerateTask;

	private:
		uint32_t m_Count;
		float m_Probability;
		std::pair<uint32_t, uint32_t> m_Range;

		std::vector<std::vector<uint32_t>> processorsSet;

		std::random_device rd;
		std::mt19937 gen;
		std::uniform_int_distribution<uint32_t> distributionProcessors;
		std::uniform_int_distribution<uint32_t> distributionComplexity;

	};
}