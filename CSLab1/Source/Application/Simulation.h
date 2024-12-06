#pragma once
#include "Processor.h"
#include <vector>

namespace CSL1
{
	class Simulation
	{
	public:
		Simulation();
		~Simulation();

		void Update();
		void UpdateGUI();


	private:
		/* Logic */
		void GenerateTasks();
		void RunFIFOScheduler();

	private:
		/* GUI */
		void PowerOptions();
		void SlowestProcessorDisplay();
		void ProbabilityOption();
		void ComplexityOptions();
		void GenerateTasksOption();
		void FIFOSchedulerOption();

	private:
		std::vector<Processor> m_Processors;
		Processor* m_SlowestProcessor;
		std::vector<Task> m_Tasks;
		uint32_t m_Probability;
		const uint32_t m_LowerBoundUnscaled;
		const uint32_t m_HigherBoundUnscaled;
		uint32_t m_LowerBound;
		uint32_t m_HigherBound;
		uint32_t m_TaskCount;

		bool m_bRunning;
	};
}