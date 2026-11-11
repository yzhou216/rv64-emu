# SPDX-FileCopyrightText: 2026 Yiyu Zhou <yiyu@yiyuzhou.io>
#
# SPDX-License-Identifier: GPL-3.0-or-later

# rv64_emu for the system of the Nixpkgs whose callPackage calls this, which
# must be riscv64 GNU/Linux, since rv64_emu calls its RISC-V test programs
{ lib, stdenv }:
stdenv.mkDerivation {
  name = "rv64_emu";

  src = lib.fileset.toSource {
    root = ../.;
    fileset = lib.fileset.unions [
      ../Makefile
      ../src
      ../progs
    ];
  };

  enableParallelBuilding = true;

  installPhase = ''
    runHook preInstall
    install -D rv64_emu "$out/bin/rv64_emu"
    runHook postInstall
  '';

  meta = {
    description = "64-bit RISC-V emulator with instruction cache support";
    license = lib.licenses.gpl3Plus;
    mainProgram = "rv64_emu";
    platforms = [ "riscv64-linux" ];
  };
}
