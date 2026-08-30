{ pkgs ? import <nixpkgs> {
    overlays = [
      (self: super: {
        opencv4 = super.opencv4.override { enableGtk3 = true; };
      })
    ];
  }
}:
pkgs.mkShell {
  buildInputs = [
    pkgs.opencv4
    pkgs.dlib
    pkgs.cmake
    pkgs.pkg-config
    pkgs.gdb
    pkgs.valgrind
  ];
}
