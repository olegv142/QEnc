#pragma once

#include <Arduino.h>

/*
 * This is yet another library for interfacing to quadrature rotary encoder.
 * The goal is keep the code as simple and as fast as possible. So it can be
 * called from timer ISR with sub-milliseconds period even on slow CPU.
 */

class QEnc {
protected:
	uint8_t                 m_pinA;
	uint8_t                 m_pinB;
	volatile uint8_t const *m_portA;
	volatile uint8_t const *m_portB;
	uint8_t                 m_bitA;
	uint8_t                 m_bitB;
	uint8_t                 m_lastA;
	uint8_t                 m_lastB;
	volatile int32_t        m_cnt;

public:
	QEnc(uint8_t pinA, uint8_t pinB)
		// resolve pins to ports for faster subsequent access
		: m_pinA(pinA), m_pinB(pinB)
		, m_portA(portInputRegister(digitalPinToPort(pinA)))
		, m_portB(portInputRegister(digitalPinToPort(pinB)))
		, m_bitA(digitalPinToBitMask(pinA))
		, m_bitB(digitalPinToBitMask(pinB))
		, m_lastA(~0), m_lastB(~0)
		, m_cnt(0)
		{}

	// Configure IO ports
	void begin()
	{
		pinMode(m_pinA, INPUT_PULLUP);
		pinMode(m_pinB, INPUT_PULLUP);
	}

	// Poll IO ports and update counter if necessary.
	// Typically called from timer ISR with period ~500 usec.
	void poll()
	{
		uint8_t const A = *m_portA & m_bitA;
		uint8_t const B = *m_portB & m_bitB;
		int8_t d = !A == !B ? 1 : -1;
		if (m_lastA != A) {
			if (m_lastB == B)
				m_cnt -= d;
		} else if (m_lastB != B)
			m_cnt += d;
		m_lastA = A;
		m_lastB = B;
	}

	// Returns the current counter value
	int32_t value()
	{
		return m_cnt / 4;
	}
};
