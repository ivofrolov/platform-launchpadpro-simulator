# Launchpad Pro Simulator: development platform for [PlatformIO](https://platformio.org/)

**WORK IN PROGRESS** | **ONLY MACOS SUPPORTED CURRENTLY**

Native desktop simulator to simplify firmware development for the [Novation Launchpad Pro](https://novationmusic.com/products/launchpad-pro) grid controller.

See the original [documentation](https://github.com/dvhdr/launchpad-pro) for API and hardware description.

## Getting Started

Install development platform.

    pio platform install https://github.com/ivofrolov/platform-launchpadpro-simulator.git

Create project using `pio init` and adjust platformio.ini like so.

``` ini
[env:launchpadpro-simulator]
platform = launchpadpro-simulator
```

You can combine both simulator and [hardware platform](https://github.com/ivofrolov/platform-launchpadpro) in one project using Platformio environments.

Compile firmware and launch simulator using `pio run -t exec`.

See [example](/examples/simple).

![Screenshot](/assets/screenshot.png)
