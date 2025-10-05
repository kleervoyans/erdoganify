# Erdoganify

Turn any picture into a portrait of Recep Tayyip Erdoğan. Inspired by [Spu7Nix/obamify](https://github.com/Spu7Nix/obamify), this single-page web app takes an uploaded image, shuffles every pixel, and gradually reorganises it into an Erdoğan likeness.

## Getting started

1. Open `index.html` in your favourite browser.
2. Click **Choose an image** and select a local picture.
3. Watch the pixels migrate until the target portrait is complete.

## How it works

- The canvas is rendered at 480×480 pixels and sampled in a 4×4 grid.
- Your uploaded image is resized to cover the canvas so no gaps appear.
- Each pixel block becomes an animated particle. Their starting positions come from your photo, while their destinations and colours are taken from a reference photo of Erdoğan.
- Particles tween across the canvas with easing and staggered delays, creating the illusion that the original image dissolves and reforms.

## Credits

- Recep Tayyip Erdoğan photo: [Recep Tayyip Erdoğan 2015](https://commons.wikimedia.org/wiki/File:Recep_Tayyip_Erdo%C4%9Fan_2015.jpg) by <span title="Tasnim News Agency">Tasnim News Agency</span>, licensed under [CC BY 4.0](https://creativecommons.org/licenses/by/4.0/deed.en). The image is redistributed here at a reduced resolution under the same licence.
