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

	}
}

