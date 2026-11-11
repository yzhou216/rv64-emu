# SPDX-FileCopyrightText: 2026 Yiyu Zhou <yiyu@yiyuzhou.io>
#
# SPDX-License-Identifier: GPL-3.0-or-later

{
  perSystem =
    { config, riscv64, ... }:
    {
      packages = {
        default = config.packages.rv64_emu;
        rv64_emu = riscv64.callPackage ./rv64_emu.nix { };
      };
    };
}
