# SPDX-FileCopyrightText: 2026 Yiyu Zhou <yiyu@yiyuzhou.io>
#
# SPDX-License-Identifier: GPL-3.0-or-later

{
  perSystem =
    { pkgs, ... }:
    {
      # Nixpkgs of riscv64 GNU/Linux, the one system that rv64_emu runs on,
      # with which the other systems cross-build it
      _module.args.riscv64 = if pkgs.stdenv.hostPlatform.isRiscV64 then pkgs else pkgs.pkgsCross.riscv64;
    };
}
