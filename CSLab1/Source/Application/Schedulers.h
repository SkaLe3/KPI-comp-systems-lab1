/* Schedulers.h */
#pragma once
#include "Processor.h"
#include "Task.h"

#include <vector>
#include <optional>

namespace CSL1
{
	namespace Schedulers
	{
		class FIFO
		{
		public: 
			void Run(std::vector<Processor>& processors, std::vector<Task>& tasks, uint32_t timeLeft);
		private:
			bool TryAddTask(std::vector<Processor>& processors, std::vector<Task>& tasks);
		};


		class Dedicated
		{
		public:
			void Run(std::vector<Processor>& processors, std::vector<Task>& tasks, uint32_t timeLeft);

		};

		class Interruptible
		{
		public:
			void Run(std::vector<Processor>& processors, std::vector<Task>& tasks, uint32_t timeLeft);

		private:
			Processor* FindFastest(std::vector<Processor>& processors);
		};
	}
}