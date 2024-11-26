FROM archlinux:latest

RUN pacman -Syu --needed --noconfirm \
  make \
  cmake \
  ninja \
  clang \
  gcc \
  raylib \
  xorg

WORKDIR /work

COPY . .

# -G Ninja
RUN cmake -B ./build -DCMAKE_BUILD_TYPE=Release
RUN cmake --build ./build --config Release

# COPY meson.build meson.build
# RUN meson setup build
# RUN meson compile -C build

ENTRYPOINT ["/work/build/Release/mandelbrot"]
