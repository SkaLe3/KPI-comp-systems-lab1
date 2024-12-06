/* Schedulers.cpp */
#include "Schedulers.h"
#include <algorithm>

namespace CSL1
{
	namespace Schedulers
	{


		void FIFO::Run(std::vector<Processor>& processors, std::vector<Task>& tasks, uint32_t timeLeft)
		{
			while (!tasks.empty() && timeLeft > 0)
			{
				while (TryAddTask(processors, tasks));
				for (auto& processor : processors)
				{
					processor.Tick();
				}
				timeLeft--;
			}
			while (timeLeft > 0)
			{
				uint32_t procLeft = processors.size();
				for (auto& processor : processors)
				{
					if (processor.IsQueueEmpty())
					{
						procLeft--;
						continue;
					}
					processor.Tick();
				}
				timeLeft--;
				if (procLeft == 0)
					break;
			}
		}

		bool FIFO::TryAddTask(std::vector<Processor>& processors, std::vector<Task>& tasks)
		{
			if (tasks.empty()) return false;
			Task& currentTask = tasks.front();
			Processor* processor = nullptr;
			auto it = std::find_if(processors.begin(), processors.end(), [&currentTask](const Processor& proc)
								   {
									   return  proc.IsAcceptable(currentTask) && proc.IsQueueEmpty();
								   });
			if (it != processors.end())
			{
				processor = &*it;
			}
			if (processor)
			{
				processor->AddTask(currentTask);
				tasks.erase(tasks.begin());
				return true;
			}
			return false;
		}

		Task* FindDesiredTask(std::vector<Task>& tasks, Processor& proc)
		{
			std::vector<Task*> filteredTasks;
			for (auto& task : tasks)
			{
				if (std::find(task.GetProcessors().begin(), task.GetProcessors().end(), proc.ID()) != task.GetProcessors().end())
				{
					filteredTasks.push_back(&task);
				}
			}

			if (filteredTasks.empty())
			{
				return nullptr;
			}

			std::sort(filteredTasks.begin(), filteredTasks.end(), [](Task* a, Task* b)
					  {
						  return a->GetProcessors().size() < b->GetProcessors().size();
					  });

			return filteredTasks.front();
		}

		void Dedicated::Run(std::vector<Processor>& processors, std::vector<Task>& tasks, uint32_t timeLeft)
		{
			auto minIt = std::min_element(processors.begin(), processors.end(),
										  [](const Processor& a, const Processor& b)
										  {
											  return a.GetPower() < b.GetPower();
										  });
			Processor* slowest = &(*minIt);
			slowest->RunScheduler();

			while (!tasks.empty() && timeLeft > 0)
			{
				std::vector<Processor*> availableProcessors;
				for (Processor& processor : processors)
				{
					if (!processor.IsSchedulerRunning() && processor.IsQueueEmpty())
					{
						availableProcessors.push_back(&processor);
					}
				}
				for (Processor* processor : availableProcessors)
				{
					Task* desiredTask = FindDesiredTask(tasks, *processor);
					if (desiredTask)
					{
						processor->AddTask(*desiredTask);
						tasks.erase(std::remove_if(tasks.begin(), tasks.end(),
												   [&](const Task& t) { return &t == desiredTask; }), tasks.end());
					}
				}
				for (auto& processor : processors)
				{
					processor.Tick();
				}
				timeLeft--;

			}
		}



		void Interruptible::Run(std::vector<Processor>& processors, std::vector<Task>& tasks, uint32_t timeLeft)
		{
			Processor* fastest = FindFastest(processors);
			fastest->RunScheduler();

			while (!tasks.empty() && timeLeft > 0)
			{
				std::vector<Processor*> availableProcessors;
				for (Processor& processor : processors)
				{
					if (processor.IsQueueEmpty())
					{
						availableProcessors.push_back(&processor);
					}
				}

				if (fastest->IsSchedulerRunning())
				{
					for (Processor* processor : availableProcessors)
					{
						Task* desiredTask = FindDesiredTask(tasks, *processor);
						if (desiredTask)
						{
							processor->AddTask(*desiredTask);
							tasks.erase(std::remove_if(tasks.begin(), tasks.end(),
													   [&](const Task& t) { return &t == desiredTask; }), tasks.end());
						}
					}
				}
				for (auto& processor : processors)
				{
					processor.Tick();
				}
				timeLeft--;
				
				if (fastest->GetSchedulerUptime() >= 4)
					fastest->StopScheduler();
				if (!availableProcessors.empty())
					fastest->RunScheduler();
			}
		}

		Processor* Interruptible::FindFastest(std::vector<Processor>& processors)
		{
			auto maxIt = std::max_element(processors.begin(), processors.end(),
									 [](const Processor& a, const Processor& b)
									 {
										 return a.GetPower() < b.GetPower();
									 });
			return &(*maxIt);
		}

	}
}

