#pragma once
#include "Processor.h"
#include <vector>
#include <functional>
#include <chrono>
#include <mutex>


namespace CSL1
{
	struct ShedulerButtonState
	{
		std::chrono::steady_clock::time_point ClickTime;
		bool ClickedSuccess = false;
		bool ClickedFailure = false;
	};

	struct SimulationResult
	{
		const char* OwningScheduler;
		int32_t CompletedTasks;
		std::vector<int32_t> ProcessorTasks;
		int32_t CompletedOperations;
		int32_t PotentialOperations;
		float Efficiency;
	};

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
		void RunDedicatedScheduler();
		void RunInterruptibleScheduler();
		void CalculateResults(const char* schedulerName);

	private:
		/* GUI */
		void PowerOptions();
		void SlowestProcessorDisplay();
		void ProbabilityOption();
		void ComplexityOptions();
		void SimualationStatusOption();
		void GenerateTasksOption();
		void RunSchedulerOption(std::function<void()> schedulerFunction, const char* schedulerName, ShedulerButtonState& state);

		void OutputSimulationResults();

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
		int32_t m_ElapsedTime;

		bool m_bRunning;

		std::vector<SimulationResult> m_SimulationResults;
		std::mutex m_SimulationResultsMutex;

		ShedulerButtonState m_FifoState;
		ShedulerButtonState	m_DedicatedState;
		ShedulerButtonState	m_InterruptibleState;
	};
}