# SPDX-FileCopyrightText: 2026 Yiyu Zhou <yiyu@yiyuzhou.io>
#
# SPDX-License-Identifier: GPL-3.0-or-later

{ inputs, ... }:
{
  imports = [ inputs.treefmt-nix.flakeModule ];

  perSystem =
    {
      config,
      lib,
      pkgs,
      riscv64,
      ...
    }:
    {
      checks = {
        reuse = pkgs.runCommandLocal "reuse-lint" { nativeBuildInputs = [ pkgs.reuse ]; } ''
          reuse --root ${../.} lint
          touch "$out"
        '';
        # Through qemu-riscv64 on systems other than RISC-V
        agree = pkgs.runCommandLocal "rv64_emu-agree" { } ''
          sh ${./agree.sh} ${riscv64.stdenv.hostPlatform.emulator pkgs} ${lib.getExe config.packages.rv64_emu}
          touch "$out"
        '';
      };

      treefmt = {
        projectRootFile = "flake.nix";
        programs = {
          nixfmt.enable = true;
          rumdl-format.enable = true;
          shfmt = {
            enable = true;
            indent_size = 0; # Tabs, as in the C sources
          };
          taplo.enable = true;
          yamlfmt = {
            enable = true;
            settings.formatter.retain_line_breaks_single = true; # Blank line after the license
          };
        };
      };
    };
}
