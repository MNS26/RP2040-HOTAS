{
  outputs = { self, nixpkgs }:
  {
    packages.x86_64-linux.default = nixpkgs.legacyPackages.x86_64-linux.callPackage ./default.nix {};
    devShells.x86_64-linux.default = nixpkgs.legacyPackages.x86_64-linux.stdenv.mkDerivation {
      name = "shell";
      nativeBuildInputs = [ nixpkgs.legacyPackages.x86_64-linux.platformio ];
    };
  };
}
