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
    let
      # qemu-riscv64 on systems other than RISC-V
      emulator = riscv64.stdenv.hostPlatform.emulator pkgs;
      rv64_emu = lib.getExe config.packages.rv64_emu;
    in
    {
      checks = {
        reuse = pkgs.runCommandLocal "reuse-lint" { nativeBuildInputs = [ pkgs.reuse ]; } ''
          reuse --root ${../.} lint
          touch "$out"
        '';
        agree = pkgs.runCommandLocal "rv64_emu-agree" { } ''
          sh ${./agree.sh} ${emulator} ${rv64_emu}
          touch "$out"
        '';
        tests = pkgs.runCommandLocal "rv64_emu-tests" { } ''
          RUN=${emulator} sh ${../tests}/run.sh ${rv64_emu}
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
