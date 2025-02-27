#include "gameboy.h"
#include <fstream>
#include <iostream>

Gameboy::Gameboy(const std::vector<u8>& cartridge_data, Options& options,
                 const std::vector<u8>& save_data)
    : cartridge(get_cartridge(cartridge_data, save_data)),
      cpu(*this, options),
      video(*this, options),
      mmu(*this, options),
      serial(options),
      debugger(*this, options)
{
    if (options.disable_logs) log_set_level(LogLevel::Error);

    log_set_level(options.trace
        ? LogLevel::Trace
        : LogLevel::Info
    );
    cpu.initialise();
}

void Gameboy::button_pressed(GbButton button) {
    input.button_pressed(button);
}

void Gameboy::button_released(GbButton button) {
    input.button_released(button);
}

void Gameboy::debug_toggle_background() {
    video.debug_disable_background = !video.debug_disable_background;
}

void Gameboy::debug_toggle_sprites() {
    video.debug_disable_sprites = !video.debug_disable_sprites;
}

void Gameboy::debug_toggle_window() {
    video.debug_disable_window = !video.debug_disable_window;
}

void Gameboy::run(
    const should_close_callback_t& _should_close_callback,
    const vblank_callback_t& _vblank_callback
) {
    should_close_callback = _should_close_callback;

    video.register_vblank_callback(_vblank_callback);

    while (!should_close_callback()) {
        tick();
    }

    debugger.set_enabled(false);
}

void Gameboy::tick() {
    debugger.cycle();

    auto cycles = cpu.tick();
    elapsed_cycles += cycles.cycles;

    auto& vram = video.get_vram();
    if (std::any_of(vram.begin(), vram.end(), [](u8 byte) {
        return byte != 0;
    })) {
        std::ofstream outfile("/tmp/test", std::ios::out | std::ios::binary); 
        outfile.write(reinterpret_cast<const char*>(&vram[0]), vram.size());
        exit(0);
    }
    video.tick(cycles);
    timer.tick(cycles.cycles);
}

auto Gameboy::get_cartridge_ram() const -> const std::vector<u8>& {
    return cartridge->get_cartridge_ram();
}
