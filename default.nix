{ stdenv, platformio, runCommand, lib, git, cacert, strace }:

let
  deps = runCommand "pio-deps" {
    nativeBuildInputs = [ platformio git strace ];
    buildInputs = [ cacert ];
    outputHashMode = "recursive";
    outputHash = "sha256-pQpattmS8VmO3ZIQUFn66az8GSmB4IvYhTTCFn6SUmo=";
    outputHashAlgo = "sha256";
    src = ./.;
  } ''
    unpackPhase
    set -x
    cd $sourceRoot

    mkdir $NIX_BUILD_TOP/out
    export PLATFORMIO_CORE_DIR=$NIX_BUILD_TOP/out

    pio pkg install
    mv -v $NIX_BUILD_TOP/out $out
  '';
in
stdenv.mkDerivation {
  name = "RP2040-HOTAS";
  nativeBuildInputs = [ platformio ];
  src = ./.;
  buildPhase = ''
    export core_dir=${deps}
    pio run
  '';
}
