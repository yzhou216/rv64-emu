/*
 * SPDX-FileCopyrightText: 2024 Yiyu Zhou <yiyu@yiyuzhou.io>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include <stdint.h>

[[nodiscard]] uint32_t get_bits(uint64_t value, uint32_t start,
				uint32_t count) [[unsequenced]];
[[nodiscard]] int64_t sign_extend(uint64_t value,
				  uint32_t width) [[unsequenced]];
[[nodiscard]] bool get_bit(uint64_t value, uint32_t which) [[unsequenced]];
