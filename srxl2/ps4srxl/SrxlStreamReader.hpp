#ifndef SrxlStreamReader_H
#define SrxlStreamReader_H

#include "Arduino.h"
#include "usings.h"
#include "ByteBuffer.hpp"
#include "SrxlCommon.hpp"
#include "crc.hpp"

enum SrxlStreamReaderState {
	WAITING_FOR_HEADER,
	WAITING_FOR_PACK_TYPE,
	WAITING_FOR_PACK_LEN,
	READING_DATA,
	READING_CRC,
	MESSAGE_READY,
};
class SrxlStreamReader {
private:
	CycledByteBuffer<100> in_msg;
	uint16_t crc_buffer;
	ByteBuffer crc_buffer_writer = ByteBuffer(this->crc_buffer);
	SrxlStreamReaderState state = WAITING_FOR_HEADER;
	byte p_type;
	byte p_len;
	byte data_read;
	Crc16XModem crc;
	void reset_state() {
		state = WAITING_FOR_HEADER;
		crc.reset();
	}
	bool process(byte data) {
		if (state != READING_CRC) {
			crc.add(data);
		}
		switch (state) {
		case WAITING_FOR_HEADER:
			if (data == SRXL_HEADER_START) {
				state = WAITING_FOR_PACK_TYPE;
			}
			else {
				return false;
			}
			break;
		case WAITING_FOR_PACK_TYPE:
			p_type = data;
			state = WAITING_FOR_PACK_LEN;
			break;
		case WAITING_FOR_PACK_LEN:
			p_len = data;
			if (p_len > 80) {
				return false;
			}
			else {
				if (p_len > 0) {
					state = READING_DATA;
				}
				else {
					state = READING_CRC;
				}
				data_read = 0;
			}
			break;
		case READING_DATA:
			data_read++;
			if (data_read == p_len - 5) {
				state = READING_CRC;
				data_read = 0;
				crc_buffer_writer.clear();
			}
			break;
		case READING_CRC:
			data_read++;
			crc_buffer_writer.write(data);
			if (data_read == 2) {
				if (check_crc()) {
					state = MESSAGE_READY;
				}
				else {
					return false;
				}
			}
			break;
		}
		return true;
	}
	bool playback() {
		if (in_msg.len() > 0) {
			byte remove_me = in_msg.read();
			reset_state();
			while (in_msg.len() > 0 && !process(in_msg.peek())) {
				byte remove_me = in_msg.read();
			}
			reset_state();  // reset the state, cause process(in_msg.peek()) moved the state to the next step
			auto iter = in_msg.iterator();
			while (iter.has_next()) {
				byte data = iter.next();
				if (!process(data)) {
					state = WAITING_FOR_HEADER;
					break;
				}
				else if (state == MESSAGE_READY) {
					return true;
				}
			}
		}
		return false;
	}
	bool check_crc() {
		uint16_t expected = crc.calc();
		little2big_endian(expected);
		return crc_buffer == expected;
	}

public:
	SrxlStreamReader() {}
	void append(byte data) {
		in_msg.write(data);
		if (!process(data)) {
			playback();
		}
	}
	bool msg_ready() {
		return state == MESSAGE_READY;
	}
	void read_msg_into(SrxlGenericPack& msg) {
		if (msg_ready()) {
			byte A6 = in_msg.read();
			byte type = in_msg.read();
			byte len = in_msg.read();
			for (int i = 0; i < len - 5; i++) {
				byte dat = in_msg.read();
				msg.data[i] = dat;
			}
			uint16_t crc = in_msg.read();
			msg.crc = crc;
		}
	}
};
#endif  // SrxlStreamReader_H
