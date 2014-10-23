/*
 * This file is part of the libsumo - a reverse-engineered library for
 * controlling Parrot's connected toy: Jumping Sumo
 *
 * Copyright (C) 2014 I. Loreen
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License
 * as published by the Free Software Foundation; either version 2.1 of
 * the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
 * 02110-1301 USA
 */
#ifndef SUMO_PROTOCOL
#define SUMO_PROTOCOL

#include <cstdint>

#define ACK   1
#define SYNC  2
#define IMAGE 3
#define IOCTL 4

namespace sumo
{

struct header
{
	uint8_t type;
	uint8_t ext;
	uint8_t seqno;
	uint16_t size;
	uint16_t unk;
} __attribute__((packed));

struct packet
{
	struct header head;

	packet(uint8_t t, uint8_t e, uint8_t s, uint16_t size) : head( { t, e, s, size, 0 } )
	{ }

} __attribute__((packed));

struct sync : public packet
{
	uint32_t seconds;
	uint32_t nanoseconds;

	sync(uint8_t seq, uint32_t sec, uint32_t nsec) :
		packet(SYNC, 0, seq, sizeof(*this)),
		seconds(sec), nanoseconds(nsec)
	{ }

} __attribute__((packed));

struct move : public packet
{
	uint8_t b[4];
	uint8_t active;
	int8_t  speed;
	int8_t  turn;

	move(uint8_t seq, uint8_t a, int8_t s, int8_t t) :
		packet(SYNC, 10, seq, sizeof(*this)),
		b{0x03,0,0,0}, active(a), speed(s), turn(t)
	{ }

} __attribute__((packed));

struct ack : public packet
{
	uint8_t seqno;

	ack(uint8_t ext, uint8_t seq, uint8_t confirm) :
		packet(ACK, ext, seq, sizeof(*this)),
		seqno(confirm)
	{}
} __attribute__((packed));


struct ioctl_packet : public packet
{
	uint8_t flags;
	uint8_t type;
	uint8_t func;
	uint8_t unk;

	ioctl_packet(uint8_t seq, uint16_t size, uint8_t t, uint8_t fu, uint8_t flags = 0) :
		packet(IOCTL, 11, seq, size),
		flags(flags), type(t), func(fu), unk(0)
	{}

} __attribute__((packed));

template<typename T>
struct ioctl : public ioctl_packet
{
	T param;

	ioctl(uint8_t seq, uint8_t t, uint8_t fu, uint8_t flags = 0) :
		ioctl_packet(seq, sizeof(*this), t, fu, flags),
		param()
	{ }
} __attribute__((packed));

struct date : public ioctl<char[11]>
{
	date(uint8_t seq) :
		ioctl(seq, 4, 1)
	{ }
} __attribute__((packed));

struct time : public ioctl<char[13]>
{
	time(uint8_t seq) :
		ioctl(seq, 4, 2)
	{ }
} __attribute__((packed));

struct turn : public ioctl<float>
{
	turn(uint8_t seq, float an) :
		ioctl(seq, 3, 1, 0x83)
	{
		param = an;
	}
} __attribute__((packed));

struct highjump : public ioctl<uint32_t>
{
	highjump(uint8_t seq) : ioctl(seq, 2, 3, 0x03)
	{
		param = 1;
	}
};

struct image : public packet
{
	uint16_t frame_number;
	uint16_t unk0;
	uint8_t unk1;
	/* image data JPEG */
} __attribute__((packed));

}

#endif