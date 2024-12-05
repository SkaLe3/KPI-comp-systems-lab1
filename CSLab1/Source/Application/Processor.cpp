#include "Processor.h"

namespace CSL1
{

	Processor::Processor()
		: m_ID(NextAvailableID), m_Power(1)
	{
		NextAvailableID++;
	}


	const uint32_t Processor::ID() const
	{
	   return m_ID;
	}

	const uint32_t Processor::GetPower() const
	{
		return m_Power;
	}

	void Processor::SetPower(uint32_t power)
	{
		m_Power = power;
	}

}

