#include "Simulation.h"
#include "TaskGenerator.h"
#include "Schedulers.h"

#include <imgui.h>
#include <string>
#include <thread>
#include <print>


namespace CSL1
{

	Simulation::Simulation()
		: m_LowerBoundUnscaled(10), m_HigherBoundUnscaled(200), m_bRunning(false), m_ElapsedTime(10000)
	{
		m_Processors = std::vector<Processor>(5);
		m_SlowestProcessor = nullptr;
		m_Probability = 80;
		m_LowerBound = m_LowerBoundUnscaled;
		m_HigherBound = m_HigherBoundUnscaled;
		m_TaskCount = 10000;
	}

	Simulation::~Simulation()
	{

	}

	void Simulation::Update()
	{
		int slowestProcessorPower = std::numeric_limits<int>::max();

		for (auto& processor : m_Processors)
		{
			int power = processor.GetPower();  // Assuming get_power() returns the processor's power
			if (power < slowestProcessorPower)
			{
				slowestProcessorPower = power;
				m_SlowestProcessor = &processor;

			}
		}
	}

	void Simulation::UpdateGUI()
	{
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(20, 5));
		ImGui::Begin("Simulation Settings");
		ImGui::SeparatorText("Processors");

		PowerOptions();
		SlowestProcessorDisplay();

		ImGui::SeparatorText("System Parameters");

		ProbabilityOption();
		ComplexityOptions();

		ImGui::End();

		ImGui::Begin("Simulation Controls");

		ImGui::SeparatorText("Status");

		SimualationStatusOption();


		ImGui::SeparatorText("Controls");

		GenerateTasksOption();
		RunSchedulerOption<Schedulers::FIFO>("FIFO", m_FifoState);
		RunSchedulerOption<Schedulers::Dedicated>("Dedicated", m_DedicatedState);
		RunSchedulerOption<Schedulers::Interruptible>("Interruptible", m_InterruptibleState);

		ImGui::End();




		ImGui::PopStyleVar();

		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(5, 5));
		ImGui::Begin("Simulation Output");
		{
			std::lock_guard<std::mutex> lock(m_SimulationResultsMutex);
			OutputSimulationResults();
		}

		ImGui::PopStyleVar();
		ImGui::End();
	}



	void Simulation::GenerateTasks()
	{
		float probability = (float)m_Probability / 100.f;
		TaskGenerator generator = TaskGenerator(m_Processors.size(), std::make_pair(m_LowerBound, m_HigherBound), m_Probability);
		m_Tasks = generator.GenerateTasks(m_TaskCount);
	}


	void Simulation::CalculateResults(const char* schedulerName)
	{
		std::vector<int32_t> processorTasks;

		for (int i = 0; i < 5; i++)
			processorTasks.push_back(m_Processors[i].Stats.CompletedTasks);

		uint32_t completedTasks = 0;
		for (auto& proc : m_Processors)
			completedTasks += proc.Stats.CompletedTasks;

		uint32_t completedOperations = 0;
		for (auto& proc : m_Processors)
			completedOperations += proc.Stats.CompletedOperations;

		uint32_t potentialOperations = 0;
		for (auto& proc : m_Processors)
			potentialOperations += (proc.GetPower() * m_ElapsedTime);

		float efficiency = static_cast<float>(completedOperations) / static_cast<float>(potentialOperations);

		SimulationResult results = {
			.OwningScheduler = schedulerName,
			.CompletedTasks = (int32_t)completedTasks,
			.ProcessorTasks = processorTasks,
			.CompletedOperations = (int32_t)completedOperations,
			.PotentialOperations = (int32_t)potentialOperations,
			.Efficiency = efficiency
		};

		{
			std::lock_guard<std::mutex> lock(m_SimulationResultsMutex);
			m_SimulationResults.push_back(results);
		}

		for (auto& proc : m_Processors)
			proc.Reset();
	}

	void Simulation::PowerOptions()
	{
		for (auto& processor : m_Processors)
		{
			std::string label = "Processor " + std::to_string(processor.ID()) + " Power";

			ImGui::PushItemWidth(100);
			int currentPower = processor.GetPower();
			if (ImGui::SliderInt(label.c_str(), &currentPower, 1, 10))
			{
				processor.SetPower(currentPower);
			}
			ImGui::PopItemWidth();
		}
	}

	void Simulation::SlowestProcessorDisplay()
	{
		ImGuiTableFlags flags = ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg
			| ImGuiTableFlags_SizingFixedFit | ImGuiTableFlags_NoHostExtendX;
		if (ImGui::BeginTable("ProcessorInfoTable", 2, flags))
		{
			ImGui::TableSetupColumn("ID", ImGuiTableColumnFlags_WidthFixed, 40);
			ImGui::TableSetupColumn("Power", ImGuiTableColumnFlags_WidthFixed, 40);
			ImGui::TableHeadersRow();


			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0);
			ImGui::Text("%d", m_SlowestProcessor->ID());

			ImGui::TableSetColumnIndex(1);
			ImGui::Text("%d", m_SlowestProcessor->GetPower());

			ImGui::EndTable();
		}
		ImGui::SameLine();
		ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 10);
		ImGui::Text("Slowest Processor");
	}

	void Simulation::ProbabilityOption()
	{
		ImGui::PushItemWidth(150);
		int currentProbability = m_Probability;
		if (ImGui::SliderInt("Probability", &currentProbability, 1, 100))
		{
			m_Probability = currentProbability;
		}
		ImGui::PopItemWidth();
	}

	void Simulation::ComplexityOptions()
	{
		ImGui::PushItemWidth(150);
		uint32_t lowerBound = m_LowerBoundUnscaled * m_SlowestProcessor->GetPower();
		uint32_t higherBound = m_HigherBoundUnscaled * m_SlowestProcessor->GetPower();
		static int range[2] = { lowerBound, higherBound };
		if (ImGui::SliderInt2("Complexity (ms)", range, lowerBound, higherBound))
		{
			if (range[0] > range[1])
			{
				std::swap(range[0], range[1]);
			}
		}
		range[0] = std::max(lowerBound, (uint32_t)range[0]);
		range[1] = std::min(higherBound, (uint32_t)range[1]);
		ImGui::PopItemWidth();

		m_LowerBound = range[0];
		m_HigherBound = range[1];
	}

	void Simulation::SimualationStatusOption()
	{
		ImGui::BeginDisabled();
		ImGui::Checkbox("Scheduler Running", &m_bRunning);
		static int32_t tasksLeft = 0;
		tasksLeft = m_Tasks.size();
		ImGui::PushItemWidth(50);
		ImGui::DragInt("Tasks Left", &tasksLeft);
		ImGui::PopItemWidth();
		ImGui::EndDisabled();
		ImGui::PushItemWidth(100);
		ImGui::SliderInt("Running Time", &m_ElapsedTime, 5000, 300000);
		ImGui::PopItemWidth();
	}

	void Simulation::GenerateTasksOption()
	{
		static std::chrono::steady_clock::time_point clickTime;
		static bool clicked = false;
		static uint32_t tasksCount = 0;
		if (ImGui::Button("Generate Tasks", ImVec2(150, 20)))
		{
			GenerateTasks();
			tasksCount = m_Tasks.size();
			clicked = true;
			clickTime = std::chrono::steady_clock::now();
		}
		if (clicked)
		{
			ImGui::SameLine();
			ImGui::Text("Generated %d Tasks", tasksCount);

			long displayTime = 2;
			auto elapsed = std::chrono::steady_clock::now() - clickTime;
			if (elapsed > std::chrono::seconds(displayTime))
			{
				clicked = false;
			}
		}

	}

	

	void Simulation::OutputSimulationResults()
	{
		for (auto& res : m_SimulationResults)
		{
			char buffer[128];
			snprintf(buffer, sizeof(buffer), "%s Scheduler", res.OwningScheduler);
			ImGui::SeparatorText(buffer);

			ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0.0f, 0.8f, 0.2f, 1.0f));
			ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0.2f, 0.2f, 0.2f, 1.f));
			ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 5.0f);
			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(10.0f, 5.0f));
			ImGui::PushItemWidth(40);
			ImGui::InputInt("Completed Tasks", &res.CompletedTasks, 0, 0, ImGuiInputTextFlags_ReadOnly);
			ImGui::PopItemWidth();
			ImGui::PopStyleVar(2);
			ImGui::PopStyleColor(2);
			ImGui::Separator();

			for (int i = 0; i < res.ProcessorTasks.size(); ++i)
			{
				float progress = static_cast<float>(res.ProcessorTasks[i]) / static_cast<float>(res.CompletedTasks);
				ImGui::ProgressBar(progress, ImVec2(200.0f, 22.0f), ""); 
				ImGui::SameLine();
				ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0.0f, 0.8f, 0.2f, 1.0f));
				ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0.2f, 0.2f, 0.2f, 1.f));
				ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 5.0f);
				ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(10.0f, 5.0f));
				ImGui::PushItemWidth(40);
				ImGui::InputInt("##CompletedP", &res.ProcessorTasks[i], 0, 0, ImGuiInputTextFlags_ReadOnly);
				ImGui::PopItemWidth();
				ImGui::PopStyleVar(2);
				ImGui::PopStyleColor(2);
				ImGui::SameLine();
				ImGui::Text("Processor %d", i);

				ImGui::Spacing();
			}

			ImGui::Separator();
			ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0.0f, 0.8f, 0.2f, 1.0f));
			ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0.2f, 0.2f, 0.2f, 1.f));
			ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 5.0f);
			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(10.0f, 5.0f));
			ImGui::PushItemWidth(60);
			ImGui::InputInt("##NumberT", &res.PotentialOperations, 0, 0, ImGuiInputTextFlags_ReadOnly);
			ImGui::SameLine();
			ImGui::Text("Best case operations");
			ImGui::InputInt("##NumberR", &res.CompletedOperations, 0, 0, ImGuiInputTextFlags_ReadOnly);
			ImGui::SameLine();
			ImGui::Text("Completed operations");
			ImGui::PopItemWidth();
			ImGui::PopStyleVar(2);
			ImGui::PopStyleColor(2);

			ImGui::Separator();
			ImGui::ProgressBar(res.Efficiency, ImVec2(200.0f, 22.0f));
			ImGui::SameLine();
			ImGui::Text("Efficiency");
			ImGui::SeparatorText("");
		}
	}

}

