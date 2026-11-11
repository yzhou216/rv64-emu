# SPDX-FileCopyrightText: 2026 Yiyu Zhou <yiyu@yiyuzhou.io>
#
# SPDX-License-Identifier: GPL-3.0-or-later

{
  description = "64-bit RISC-V emulator with instruction cache support";

  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs/nixpkgs-unstable";
    flake-parts = {
      url = "github:hercules-ci/flake-parts";
      inputs.nixpkgs-lib.follows = "nixpkgs";
    };
    treefmt-nix = {
      url = "github:numtide/treefmt-nix";
      inputs.nixpkgs.follows = "nixpkgs";
    };
  };

  outputs =
    inputs@{ flake-parts, ... }:
    flake-parts.lib.mkFlake { inherit inputs; } {
      imports = [
        ./flake/common.nix
        ./flake/package.nix
        ./flake/devshell.nix
        ./flake/checks.nix
      ];
      # Systems that cross-build rv64_emu, and the one that builds it natively
      systems = [
        "x86_64-linux"
        "aarch64-linux"
        "riscv64-linux"
      ];
    };
}
