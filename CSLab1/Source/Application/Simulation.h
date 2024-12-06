#pragma once
#include "Processor.h"
#include <vector>
#include <functional>
#include <chrono>
#include <mutex>
#include <imgui.h>


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
		void CalculateResults(const char* schedulerName);

		template <typename T>
		void RunScheduler()
		{
			T askedScheduler;
			askedScheduler.Run(m_Processors, m_Tasks, m_ElapsedTime);
		}

	private:
		/* GUI */
		void PowerOptions();
		void SlowestProcessorDisplay();
		void ProbabilityOption();
		void ComplexityOptions();
		void SimualationStatusOption();
		void GenerateTasksOption();
		template<typename T>
		void RunSchedulerOption(const char* schedulerName, ShedulerButtonState& state);

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

	template<typename T>
	void Simulation::RunSchedulerOption(const char* schedulerName, ShedulerButtonState& state)
	{
		ImGui::PushID(schedulerName);
		if (ImGui::Button(("Run " + std::string(schedulerName)).c_str(), ImVec2(150, 20)))
		{
			state.ClickTime = std::chrono::steady_clock::now();
			if (!m_bRunning)
			{
				m_bRunning = true;
				std::thread([=, this]()
							{
								RunScheduler<T>();
								CalculateResults(schedulerName);
								m_bRunning = false;
							}).detach();
				state.ClickedSuccess = true;
				state.ClickedFailure = false;
			}
			else
			{
				state.ClickedFailure = true;
				state.ClickedSuccess = false;
			}
		}
		if (state.ClickedSuccess)
		{
			ImGui::SameLine();
			ImGui::TextColored(ImVec4(0.2f, 0.8f, 0.2f, 1.0f), "%s Scheduler Started", schedulerName);

			long displayTime = 1;
			auto elapsed = std::chrono::steady_clock::now() - state.ClickTime;
			if (elapsed > std::chrono::seconds(displayTime))
			{
				state.ClickedSuccess = false;
			}
		}
		if (state.ClickedFailure)
		{
			ImGui::SameLine();
			ImGui::TextColored(ImVec4(0.8f, 0.2f, 0.2f, 1.0f), "Failed To start %s", schedulerName);

			long displayTime = 1;
			auto elapsed = std::chrono::steady_clock::now() - state.ClickTime;
			if (elapsed > std::chrono::seconds(displayTime))
			{
				state.ClickedFailure = false;
			}
		}
		ImGui::PopID();
	}

}