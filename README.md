# snes-mod-player

Exactly what it sounds like. You need [WLA-DX](https://www.villehelin.com/wla.html) to assemble the SPC bit and [64tass](http://tass64.sourceforge.net/) to assemble the SNES bit.

Keep in mind that the compiled MODs should fit into the 64k SPC memory- a good guideline is, no MODs larger than 100 kilobytes.

## Unsupported effects

* `E6x` and `Dxx` where `xx` is nonzero, because they don't really play nice with the packed patterns.
* `E0x` and `EFx`, because they're useless on SNES anyway.
* `9xx`, because SNES samples are read from a fixed directory table.
* `E3x`, because it's a pain.