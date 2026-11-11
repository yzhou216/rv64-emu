# SPDX-FileCopyrightText: 2026 Yiyu Zhou <yiyu@yiyuzhou.io>
#
# SPDX-License-Identifier: GPL-3.0-or-later

{
  perSystem =
    {
      config,
      pkgs,
      riscv64,
      ...
    }:
    {
      # Shell in which make builds rv64_emu, which qemu-riscv64 runs on
      # systems other than RISC-V
      devShells.default = riscv64.mkShell {
        inputsFrom = [ config.packages.rv64_emu ];

        packages = with pkgs; [
          qemu-user
          reuse
        ];
      };
    };
}
