#include "Simulation.h"
#include "TaskGenerator.h"
#include "Schedulers.h"

#include <imgui.h>
#include <string>
#include <chrono>
#include <thread>


namespace CSL1
{

	Simulation::Simulation()
		: m_LowerBoundUnscaled(10), m_HigherBoundUnscaled(200), m_bRunning(false)
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

		ImGui::SeparatorText("Controls");
		GenerateTasksOption();
		FIFOSchedulerOption();

		ImGui::End();




		ImGui::PopStyleVar();

		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(5, 5));
		ImGui::Begin("Simulation Output");
		ImGui::Text("====================================");
		ImGui::Text("OutPut");
		ImGui::Text("====================================");
		ImGui::End();
		ImGui::PopStyleVar();
	}



	void Simulation::GenerateTasks()
	{
		float probability = (float)m_Probability / 100.f;
		TaskGenerator generator = TaskGenerator(m_Processors.size(), std::make_pair(m_LowerBound, m_HigherBound), m_Probability);
		m_Tasks = generator.GenerateTasks(m_TaskCount);
	}

	void Simulation::RunFIFOScheduler()
	{
		Schedulers::FIFO fifo;
		fifo.Run(m_Processors, m_Tasks, 10000);
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

	void Simulation::GenerateTasksOption()
	{
		ImGui::PushItemWidth(150);
		static std::chrono::steady_clock::time_point clickTime;
		static bool clicked = false;
		static uint32_t tasksCount = 0;
		if (ImGui::Button("Generate Tasks"))
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

		ImGui::PopItemWidth();
	}

	void Simulation::FIFOSchedulerOption()
	{
		ImGui::PushItemWidth(150);
		static std::chrono::steady_clock::time_point clickTime;
		static bool clickedSuccess = false;
		static bool clickedFailure = false;
		if (ImGui::Button("Run FIFO"))
		{
			if (!m_bRunning)
			{
				std::thread([this]()
							{
								m_bRunning = true;
								RunFIFOScheduler();
								m_bRunning = false;
							}).detach();
				clickedSuccess = true;
				clickTime = std::chrono::steady_clock::now();
			}
			else
			{
				
			}
		}
		if (clickedSuccess)
		{
			clickedFailure = false;
			ImGui::SameLine();
			ImGui::TextColored(ImVec4(0.2f, 0.8f, 0.2f, 1.0f), "FIFO Scheduler Started");

			long displayTime = 1;
			auto elapsed = std::chrono::steady_clock::now() - clickTime;
			if (elapsed > std::chrono::seconds(displayTime))
			{
				clickedSuccess = false;
			}
		}
		if (clickedFailure)
		{
		   clickedSuccess = false;
		   ImGui::SameLine();
		   ImGui::TextColored(ImVec4(0.8f, 0.2f, 0.2f, 1.0f), "Failed To start");

		   long displayTime = 1;
		   auto elapsed = std::chrono::steady_clock::now() - clickTime;
		   if (elapsed > std::chrono::seconds(displayTime))
		   {
			   clickedFailure = false;
		   }
		}

		ImGui::Checkbox("IsRunning", &m_bRunning);

		ImGui::PopItemWidth();
	}

}

