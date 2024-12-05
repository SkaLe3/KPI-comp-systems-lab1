#pragma once
#include <cstdint>

namespace CSL1
{
	class Processor
	{
	public:
		Processor();

		const uint32_t ID() const;

		const uint32_t GetPower() const;
		void SetPower(uint32_t power);
		



	private:
		uint32_t m_ID;
		uint32_t m_Power;

	private:
		inline static int32_t NextAvailableID = 0;

	};
}