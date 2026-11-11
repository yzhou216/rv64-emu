# SPDX-FileCopyrightText: 2024 Yiyu Zhou <yiyu@yiyuzhou.io>
#
# SPDX-License-Identifier: GPL-3.0-or-later

.align 4
.global midpoint_s

midpoint_s:
	add a0, a0, a1
	li t0, 2
	div a0, a0, t0          # rounds toward zero, as C division does
	ret
