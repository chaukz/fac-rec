# fac-rec

A C++ face recognition scanner, built from scratch as a learning project — the goal isn't just "get a camera to recognize faces," it's to actually earn the pointers, dynamic memory, and struct ownership that OpenCV and dlib would otherwise hide behind convenient containers.

Full architecture notes (pipeline diagram, component contracts, milestone breakdown) live outside this repo in an Obsidian vault — this README tracks what's actually built and how to run it.

## Status

- [x] **Milestone 1 — Toolchain.** CMake finds OpenCV via nix-shell, builds and links a trivial `cv::Mat` test.
- [x] **Milestone 2 — Live capture.** `Capture` class wraps `cv::VideoCapture`; live webcam window, clean exit on ESC. (Required rebuilding nixpkgs' `opencv4` with `enableGtk3 = true` — it ships headless by default.)
- [x] **Milestone 3 — Face detection.** `Detector` contract drafted (`include/Detector.h`); Haar cascade implementation next.
- [x] **Milestone 4 — `FaceRecord` + hand-rolled `FaceDatabase`.** Struct owning a heap-allocated embedding array; custom dynamic array (no `std::vector`) to force real pointer/memory practice.
- [x] **Milestone 5 — Landmarks + embeddings.** dlib's 68-point shape predictor + 128D ResNet face descriptor.
- [x] **Milestone 6 — Persistence.** Binary file I/O to save/load enrolled faces between runs.
- [x] **Milestone 7 — Recognition logic.** Distance matching, granted/denied state machine, event log as a hand-built linked list.
- [x] **Milestone 8 — Threading.** Split capture and inference across `std::thread`s.

## Build & Run

### Docker (recommended — no dependencies required)

    git clone https://github.com/chaukz/fac-rec.git
    cd fac-rec

Before building, download the model files into `models/`:

- [`shape_predictor_68_face_landmarks.dat`](http://dlib.net/files/shape_predictor_68_face_landmarks.dat.bz2)
- [`dlib_face_recognition_resnet_model_v1.dat`](http://dlib.net/files/dlib_face_recognition_resnet_model_v1.dat.bz2)

Both are `.bz2` archives — extract them into `models/` before running `docker build`.

    docker build -t fac-rec .

On Linux, run with webcam and display forwarded:

    xhost +local:docker
    docker run --device /dev/video0 \
      -e DISPLAY=$DISPLAY \
      -v /tmp/.X11-unix:/tmp/.X11-unix \
      -v $(pwd)/data:/app/data \
      fac-rec

`--device /dev/video0` passes your webcam through. Adjust the device path if yours differs (e.g. `/dev/video1`). The `data/` volume keeps your enrolled faces between runs.

> macOS / Windows: camera passthrough in Docker Desktop is limited. Use the Nix path below or run Linux in a VM.

### Nix (for development)

Requires [Nix](https://nixos.org/) (developed on NixOS). All dependencies — OpenCV, dlib, CMake, gdb, valgrind — are declared in `shell.nix`, nothing is installed globally.

    git clone https://github.com/chaukz/fac-rec.git
    cd fac-rec
    nix-shell
    mkdir -p build && cd build
    cmake ..
    cmake --build .
    ./fac_rec

## Project Structure

    fac-rec/
    ├── CMakeLists.txt
    ├── shell.nix
    ├── models/          # trained model files (.dat/.xml) — gitignored, downloaded separately
    ├── data/             # enrolled face database — gitignored, contains real biometric data
    ├── include/          # public contracts (.h)
    ├── src/              # implementations (.cpp)
    └── tests/

## Why This Architecture

Library types (`cv::Mat`, dlib's types) stop at the `Detector`/`Encoder` boundary. Everything past that point — the database, the matcher, the event log — works on plain `float*` arrays and hand-written structs. That boundary is deliberate: it's what forces real ownership and memory-management code to exist, instead of OpenCV's reference counting quietly doing the work.

## Data & Consent

`data/faces.db` is gitignored and never committed. Only faces of people who've explicitly consented are enrolled — no scraped photos, no faces captured without the person's knowledge.
