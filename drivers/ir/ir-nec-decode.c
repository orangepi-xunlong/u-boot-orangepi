/* ir-rc5-decoder.c - handle RC5(x) IR Pulse/Space protocol
 *
 * Copyright (C) 2010 by Mauro Carvalho Chehab <mchehab@redhat.com>
 *
 *  GNU General Public License for more details.
 */

/*
 * This code handles 14 bits RC5 protocols and 20 bits RC5x protocols.
 * There are other variants that use a different number of bits.
 * This is currently unsupported.
 * It considers a carrier of 36 kHz, with a total of 14/20 bits, where
 * the first two bits are start bits, and a third one is a filing bit
 */

#include <common.h>
#include "bitrev.h"
#include "sunxi-ir.h"

#define NEC_NBITS (32)
#define NEC_UNIT (562500) /* ns */
#define NEC_HEADER_PULSE (16 * NEC_UNIT)
#define NECX_HEADER_PULSE (8 * NEC_UNIT) /* Less common NEC variant */
#define NEC_HEADER_SPACE (8 * NEC_UNIT)
#define NEC_REPEAT_SPACE (4 * NEC_UNIT)
#define NEC_BIT_PULSE (1 * NEC_UNIT)
#define NEC_BIT_0_SPACE (1 * NEC_UNIT)
#define NEC_BIT_1_SPACE (3 * NEC_UNIT)
#define NEC_TRAILER_PULSE (1 * NEC_UNIT)
#define NEC_TRAILER_SPACE (10 * NEC_UNIT) /* even longer in reality */
#define NECX_REPEAT_BITS 1

enum nec_state {
	STATE_INACTIVE,
	STATE_HEADER_SPACE,
	STATE_BIT_PULSE,
	STATE_BIT_SPACE,
	STATE_TRAILER_PULSE,
	STATE_TRAILER_SPACE,
};

int ir_nec_decode(struct ir_raw_buffer *ir_raw, struct ir_raw_event ev)
{
	struct nec_dec *data = &(ir_raw->nec);
	u32 scancode;
	u8 address, not_address, command, not_command;
	bool send_32bits = false;
	/*
	if (!(dev->enabled_protocols & RC_BIT_NEC))
		return 0;
*/
	if (!is_timing_event(ev)) {
		if (ev.reset)
			data->state = STATE_INACTIVE;
		return 0;
	}

	debug("line:%d NEC decode started at state %d (%uus %s)\n", __LINE__,
	      data->state, TO_US(ev.duration), TO_STR(ev.pulse));

	switch (data->state) {
	case STATE_INACTIVE:
		if (!ev.pulse)
			break;

		if (eq_margin(ev.duration, NEC_HEADER_PULSE, NEC_UNIT * 2)) {
			data->is_nec_x    = false;
			data->necx_repeat = false;
		} else if (eq_margin(ev.duration, NECX_HEADER_PULSE,
				     NEC_UNIT / 2))
			data->is_nec_x = true;
		else
			break;

		data->count = 0;
		data->state = STATE_HEADER_SPACE;
		return 0;

	case STATE_HEADER_SPACE:
		if (ev.pulse)
			break;

		if (eq_margin(ev.duration, NEC_HEADER_SPACE, NEC_UNIT)) {
			data->state = STATE_BIT_PULSE;
			return 0;
		} else if (eq_margin(ev.duration, NEC_REPEAT_SPACE,
				     NEC_UNIT / 2)) {
			/* //if (!dev->keypressed) { */
			/* //	pr_notice("Discarding last key repeat: event after key up\n"); */
			/* //} else { */
			/* //	rc_repeat(dev); */
			pr_notice("line:%d Repeat last key\n", __LINE__);
			data->state       = STATE_TRAILER_PULSE;
			data->curt_repeat = true;
			/* //} */
			return 0;
		}

		break;

	case STATE_BIT_PULSE:
		if (!ev.pulse)
			break;

		if (!eq_margin(ev.duration, NEC_BIT_PULSE, NEC_UNIT / 2))
			break;

		data->state       = STATE_BIT_SPACE;
		data->curt_repeat = false;
		return 0;

	case STATE_BIT_SPACE:
		if (ev.pulse)
			break;

		if (data->necx_repeat && data->count == NECX_REPEAT_BITS &&
		    geq_margin(ev.duration, NEC_TRAILER_SPACE, NEC_UNIT / 2)) {
			pr_notice("line:%d Repeat last key\n", __LINE__);
			/* //rc_repeat(dev); */
			data->state = STATE_INACTIVE;
			return 0;

		} else if (data->count > NECX_REPEAT_BITS)
			data->necx_repeat = false;

		data->bits <<= 1;
		if (eq_margin(ev.duration, NEC_BIT_1_SPACE, NEC_UNIT / 2))
			data->bits |= 1;
		else if (!eq_margin(ev.duration, NEC_BIT_0_SPACE, NEC_UNIT / 2))
			break;
		data->count++;

		if (data->count == NEC_NBITS)
			data->state = STATE_TRAILER_PULSE;
		else
			data->state = STATE_BIT_PULSE;

		return 0;

	case STATE_TRAILER_PULSE:
		if (!ev.pulse)
			break;

		if (!eq_margin(ev.duration, NEC_TRAILER_PULSE, NEC_UNIT / 2))
			break;

		data->state = STATE_TRAILER_SPACE;
		return 0;

	case STATE_TRAILER_SPACE:
		if (ev.pulse)
			break;

		if (!geq_margin(ev.duration, NEC_TRAILER_SPACE, NEC_UNIT / 2))
			break;

		address     = bitrev8((data->bits >> 24) & 0xff);
		not_address = bitrev8((data->bits >> 16) & 0xff);
		command     = bitrev8((data->bits >> 8) & 0xff);
		not_command = bitrev8((data->bits >> 0) & 0xff);

		if ((command ^ not_command) != 0xff) {
			pr_notice("NEC checksum error: received 0x%08x\n",
				  data->bits);
			send_32bits = true;
		}

		if (send_32bits) {
			/* NEC transport, but modified protocol, used by at
			 * least Apple and TiVo remotes */
			scancode = data->bits;
			pr_notice("NEC (modified) scancode 0x%08x\n", scancode);
		} else {
			/* Extended NEC */
			scancode = address << 8 | not_address << 16 | command;
			pr_notice("NEC scancode 0x%06x\n", scancode);
		}

		if (data->is_nec_x)
			data->necx_repeat = true;

		pr_notice("scancode key down 0x%06x\n", scancode);
		rc_keydown(ir_raw, scancode, 0);
		data->state = STATE_INACTIVE;
		return 0;
	}

	debug("NEC decode failed at count %d state %d (%uus %s)\n", data->count,
	      data->state, TO_US(ev.duration), TO_STR(ev.pulse));
	data->state = STATE_INACTIVE;
	return -1;
}
